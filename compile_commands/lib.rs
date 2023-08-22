use std::collections::BTreeMap;

#[derive(Debug, Clone)]
pub struct Entry {
    directory: String,
    arguments: Vec<String>,
}

#[derive(Debug, Default)]
pub struct File {
    output: BTreeMap<String, Entry>,
}

#[derive(Debug, Default)]
pub struct Database {
    file: BTreeMap<String, File>,
}

#[derive(Debug)]
pub struct PrepareFile {
    file: String,
}

#[derive(Debug)]
pub struct PrepareOutput {
    file: String,
    output: String,
}

#[derive(Debug)]
pub struct Preparedirectory {
    file: String,
    output: String,
    directory: String,
}
#[derive(Debug)]
pub struct PrepareArguments {
    file: String,
    output: String,
    arguments: Vec<String>,
}

#[derive(Debug)]
pub struct PrepareFinal {
    file: String,
    output: String,
    directory: String,
    arguments: Vec<String>,
}

impl Database {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn prepare(file: impl Into<String>) -> PrepareFile {
        PrepareFile { file: file.into() }
    }

    pub fn file(&self, file: &String) -> Option<&File> {
        self.file.get(file)
    }

    pub fn files(&self) -> impl Iterator<Item = (&String, &File)> {
        self.file.iter()
    }

    pub fn len(&self) -> usize {
        self.file
            .iter()
            .fold(0, |acc, (_k, f)| acc + f.output.len())
    }

    pub fn is_empty(&self) -> bool {
        self.file.is_empty()
    }

    pub fn clear(&mut self) {
        self.file.clear()
    }

    pub fn extend(&mut self, other: Database) {
        for (file_path, file) in other.file.into_iter() {
            for (output_path, entry) in file.output.into_iter() {
                let Entry {
                    directory,
                    arguments,
                } = entry;
                Database::prepare(file_path.clone())
                    .output(output_path)
                    .directory(directory)
                    .arguments(arguments)
                    .commit(self);
            }
        }
    }
}

impl<'de> serde::de::Deserialize<'de> for Database {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: serde::de::Deserializer<'de>,
    {
        struct DatabaseVisitor;

        #[derive(serde::Deserialize)]
        struct RowBuf {
            file: String,
            output: String,
            directory: String,
            arguments: Vec<String>,
        }

        impl<'de> serde::de::Visitor<'de> for DatabaseVisitor {
            type Value = Database;

            fn expecting(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
                formatter.write_str("Expect a compile_command.json format")
            }

            fn visit_seq<A>(self, mut seq: A) -> Result<Self::Value, A::Error>
            where
                A: serde::de::SeqAccess<'de>,
            {
                let mut db = Database::new();

                // While there are entries remaining in the input, add them
                // into our map.
                while let Some(value) = seq.next_element::<RowBuf>()? {
                    Database::prepare(value.file)
                        .output(value.output)
                        .directory(value.directory)
                        .arguments(value.arguments)
                        .commit(&mut db);
                }

                Ok(db)
            }
        }

        deserializer.deserialize_seq(DatabaseVisitor)
    }
}

#[derive(serde::Serialize)]
struct Row<'f, 'o, 'd, 'a> {
    file: &'f str,
    output: &'o str,
    directory: &'d str,
    arguments: &'a [String],
}

impl serde::ser::Serialize for Database {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: serde::ser::Serializer,
    {
        use serde::ser::SerializeSeq;
        let mut seq = serializer.serialize_seq(Some(self.len()))?;
        for (f, o) in &self.file {
            for (o, e) in &o.output {
                let row = Row {
                    file: f,
                    output: o,
                    directory: e.directory(),
                    arguments: e.arguments(),
                };
                seq.serialize_element(&row)?;
            }
        }
        seq.end()
    }
}

impl File {
    pub fn output(&self, output: &String) -> Option<&Entry> {
        self.output.get(output)
    }

    pub fn outputs(&self) -> impl Iterator<Item = (&String, &Entry)> {
        self.output.iter()
    }
}

impl Entry {
    pub fn directory(&self) -> &str {
        &self.directory
    }
    pub fn arguments(&self) -> &[String] {
        &self.arguments
    }
}

impl PrepareFile {
    pub fn output(self, output: impl Into<String>) -> PrepareOutput {
        PrepareOutput {
            file: self.file,
            output: output.into(),
        }
    }
}

impl PrepareOutput {
    pub fn directory(self, directory: impl Into<String>) -> Preparedirectory {
        Preparedirectory {
            file: self.file,
            output: self.output,
            directory: directory.into(),
        }
    }

    pub fn arguments(self, arguments: impl Into<Vec<String>>) -> PrepareArguments {
        PrepareArguments {
            file: self.file,
            output: self.output,
            arguments: arguments.into(),
        }
    }
}

impl PrepareArguments {
    pub fn directory(self, directory: impl Into<String>) -> PrepareFinal {
        PrepareFinal {
            file: self.file,
            output: self.output,
            arguments: self.arguments,
            directory: directory.into(),
        }
    }
}

impl Preparedirectory {
    pub fn arguments(self, arguments: impl Into<Vec<String>>) -> PrepareFinal {
        PrepareFinal {
            file: self.file,
            output: self.output,
            directory: self.directory,
            arguments: arguments.into(),
        }
    }
}

impl PrepareFinal {
    pub fn commit(self, db: &mut Database) {
        let PrepareFinal {
            file,
            output,
            directory,
            arguments,
        } = self;
        let entry = Entry {
            directory,
            arguments,
        };
        db.file
            .entry(file)
            .and_modify({
                let output = output.clone();
                let entry = entry.clone();
                |f| {
                    f.output
                        .entry(output)
                        .and_modify({
                            let entry = entry.clone();
                            |e| *e = entry
                        })
                        .or_insert(entry);
                }
            })
            .or_insert_with(|| {
                let mut f = File::default();
                f.output.insert(output, entry);
                f
            });
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn with_one() -> Database {
        let mut db = Database::new();
        Database::prepare("file.cpp")
            .output("file.o")
            .arguments(vec!["cc".into(), "file.cpp".into()])
            .directory("/tmp")
            .commit(&mut db);
        db
    }

    fn with_two() -> Database {
        let mut db = Database::new();
        Database::prepare("file.cpp")
            .output("release/file.o")
            .arguments(vec![
                String::from("cc"),
                String::from("-c"),
                String::from("-o"),
                String::from("release/file.o"),
                String::from("file.cpp"),
            ])
            .directory("/tmp")
            .commit(&mut db);
        Database::prepare("file.cpp")
            .output("debug/file.o")
            .arguments(vec![
                String::from("cc"),
                String::from("-c"),
                String::from("-o"),
                String::from("debug/file.o"),
                String::from("file.cpp"),
            ])
            .directory("/tmp")
            .commit(&mut db);
        db
    }

    #[test]
    fn it_works() {
        let d = Database::new();
        assert_eq!(d.len(), 0);
    }

    #[test]
    fn insert_one() {
        let mut db = with_one();
        assert_eq!(db.len(), 1);
        db.clear();
        assert_eq!(db.len(), 0);
    }

    #[test]
    fn insert_two() {
        let db = with_two();
        assert_eq!(db.len(), 2);

        assert_eq!(
            db.file(&String::from("file.cpp"))
                .unwrap()
                .output(&String::from("debug/file.o"))
                .unwrap()
                .arguments(),
            vec![
                String::from("cc"),
                String::from("-c"),
                String::from("-o"),
                String::from("debug/file.o"),
                String::from("file.cpp"),
            ]
        );

        assert_eq!(
            db.file(&String::from("file.cpp"))
                .unwrap()
                .output(&String::from("release/file.o"))
                .unwrap()
                .arguments(),
            vec![
                String::from("cc"),
                String::from("-c"),
                String::from("-o"),
                String::from("release/file.o"),
                String::from("file.cpp"),
            ]
        );
    }

    #[test]
    fn serialize_to_json() {
        let db = with_two();
        let json = serde_json::to_string(&db).unwrap();
        let expected = "[{\"file\":\"file.cpp\",\"output\":\"debug/file.o\",\"directory\":\"/tmp\",\"arguments\":[\"cc\",\"-c\",\"-o\",\"debug/file.o\",\"file.cpp\"]},{\"file\":\"file.cpp\",\"output\":\"release/file.o\",\"directory\":\"/tmp\",\"arguments\":[\"cc\",\"-c\",\"-o\",\"release/file.o\",\"file.cpp\"]}]";
        assert_eq!(&json, &expected);
    }
}
