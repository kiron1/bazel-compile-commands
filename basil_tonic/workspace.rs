use std::{
    fs::File,
    io::{BufReader, Write},
    path::PathBuf,
};

use compile_commands::Database;

use crate::COMPILE_COMMANDS_JSON;

#[derive(Debug, thiserror::Error)]
pub enum Error {
    #[error("payload is missing")]
    NoPayload,
    #[error("no workspace path provided")]
    NoWorkspace,
    #[error("json serialization/deserialization error: {0}")]
    Json(
        #[source]
        #[from]
        serde_json::Error,
    ),
    #[error("I/O error: {0}")]
    Io(
        #[source]
        #[from]
        std::io::Error,
    ),
}

#[derive(Debug, Default)]
pub struct Workspace {
    workspace_directory: Option<PathBuf>,
    local_exec_root: Option<PathBuf>,
    compile_commands: compile_commands::Database,
}

fn find_argument(arguments: &[&str], flag: &str) -> Option<String> {
    let mut got_flag = false;
    for arg in arguments {
        if got_flag {
            return Some(arg.to_string());
        }
        if *arg == flag {
            got_flag = true;
        }
    }
    None
}

impl Workspace {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn compile_commands(&self) -> &Database {
        &self.compile_commands
    }

    #[tracing::instrument(skip(self, event))]
    pub fn process(&mut self, event: bazel_proto::BuildEvent) -> Result<(), Error> {
        let payload = event.payload.ok_or(Error::NoPayload)?;
        match payload {
            bazel_proto::build_event::Payload::Action(action) => {
                let is_cpp = matches!(&*action.r#type, "ObjcCompile" | "CppCompile");
                if is_cpp {
                    let output = action
                        .primary_output
                        .and_then(|f| f.file)
                        .and_then(|f| match f {
                            bazel_proto::file::File::Uri(u) => Some(u),
                            _ => None,
                        })
                        .and_then(|s| s.parse::<url::Url>().ok())
                        .and_then(|u| u.to_file_path().ok());
                    if let Some(output) = output {
                        if let Some(ref local_exec_root) = self.local_exec_root {
                            let output = output.strip_prefix(local_exec_root).unwrap_or(&output);
                            let output = output.to_str().expect("output is a valid path");
                            let local_exec_root = local_exec_root
                                .to_str()
                                .expect("local_exec_root is a valid path");
                            let arguments = action
                                .command_line
                                .iter()
                                .map(|x| &**x)
                                .collect::<Vec<&str>>();
                            // TODO: better way to find path of compilation unit
                            let file = find_argument(&arguments, "-c");
                            if let Some(file) = file {
                                tracing::debug!(%file, %output, "add");
                                Database::prepare(file)
                                    .output(output)
                                    .directory(local_exec_root)
                                    .arguments(action.command_line)
                                    .commit(&mut self.compile_commands);
                            }
                        }
                    }
                }
            }
            bazel_proto::build_event::Payload::Started(started) => {
                let workspace_directory = PathBuf::from(&started.workspace_directory);
                let compile_commands_json_path = {
                    let mut p = workspace_directory.clone();
                    p.push(COMPILE_COMMANDS_JSON);
                    p
                };
                tracing::debug!(?compile_commands_json_path, "loading",);
                let file = File::open(&compile_commands_json_path);
                if let Ok(file) = file {
                    let reader = BufReader::new(file);
                    let db: Result<Database, _> = serde_json::from_reader(reader);
                    match db {
                        Ok(db) => self.compile_commands.extend(db),
                        Err(error) => {
                            tracing::error!(%error, "failed to parse {}", COMPILE_COMMANDS_JSON)
                        }
                    }
                }
                self.workspace_directory = Some(workspace_directory);
                tracing::debug!(?self.workspace_directory, "workspace");
            }
            bazel_proto::build_event::Payload::WorkspaceInfo(info) => {
                self.local_exec_root = Some(PathBuf::from(&info.local_exec_root));
                tracing::debug!(?self.local_exec_root, "execution root");
            }

            _ => {}
        }

        Ok(())
    }

    #[tracing::instrument(skip(self))]
    pub fn write(&self) -> Result<(), Error> {
        let json_path = {
            let mut p = self
                .workspace_directory
                .as_ref()
                .map(PathBuf::from)
                .ok_or(Error::NoWorkspace)?;
            p.push(COMPILE_COMMANDS_JSON);
            p
        };
        tracing::info!(?json_path, size = %self.compile_commands.len(), "write {}", COMPILE_COMMANDS_JSON);
        let mut json_file = File::options()
            .write(true)
            .truncate(true)
            .create(true)
            .open(json_path)?;
        let json = serde_json::to_string(&self.compile_commands)?;
        json_file.write_all(json.as_bytes())?;
        Ok(())
    }
}

#[cfg(test)]
mod tests {

    #[test]
    fn it_works() {}
}
