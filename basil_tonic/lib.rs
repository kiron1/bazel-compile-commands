pub mod service;
pub mod socket;
pub mod workspace;

pub use service::PublishBuildEvent;
pub use workspace::Workspace;

static COMPILE_COMMANDS_JSON: &str = "compile_commands.json";

pub fn make_service() -> bazel_proto::PublishBuildEventServer<service::PublishBuildEvent> {
    bazel_proto::PublishBuildEventServer::new(service::PublishBuildEvent)
}
