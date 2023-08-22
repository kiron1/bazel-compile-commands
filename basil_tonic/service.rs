use std::pin::Pin;

use prost::Message;
use tokio::sync::mpsc;
use tokio_stream::{wrappers::ReceiverStream, Stream, StreamExt};
use tonic::{Request, Response, Status, Streaming};

use crate::COMPILE_COMMANDS_JSON;

#[derive(Default, Debug)]
pub struct PublishBuildEvent;

type ResponseStream = Pin<
    Box<dyn Stream<Item = Result<bazel_proto::PublishBuildToolEventStreamResponse, Status>> + Send>,
>;

#[tonic::async_trait]
impl bazel_proto::PublishBuildEvent for PublishBuildEvent {
    #[tracing::instrument(skip(self, request))]
    async fn publish_lifecycle_event(
        &self,
        request: Request<bazel_proto::PublishLifecycleEventRequest>,
    ) -> Result<Response<bazel_proto::Empty>, Status> {
        tracing::debug!(remote_addr = ?request.remote_addr(), "client");
        Ok(Response::new(bazel_proto::Empty {}))
    }

    type PublishBuildToolEventStreamStream = ResponseStream;

    #[tracing::instrument(skip(self, request))]
    async fn publish_build_tool_event_stream(
        &self,
        request: Request<Streaming<bazel_proto::PublishBuildToolEventStreamRequest>>,
    ) -> Result<Response<Self::PublishBuildToolEventStreamStream>, Status> {
        tracing::debug!(remote_addr = ?request.remote_addr(), "client");
        let (tx, rx) = mpsc::channel(128);

        let mut request = request.into_inner();

        tokio::spawn(async move {
            let mut ws = crate::Workspace::new();
            while let Some(Ok(item)) = request.next().await {
                if let Some(build_event) = item.ordered_build_event {
                    let event = build_event.event.unwrap().event.unwrap();
                    if let bazel_proto::Event::BazelEvent(build_event) = event {
                        // type_url: "type.googleapis.com/build_event_stream.BuildEvent"
                        let build_event = bazel_proto::BuildEvent::decode(&*build_event.value);
                        match build_event {
                            Ok(build_event) => {
                                if let Err(ref error) = ws.process(build_event) {
                                    tracing::error!(%error, "process event failed");
                                }
                            }
                            Err(error) => {
                                tracing::error!(%error, "failed to decode BuildEvent");
                            }
                        }
                    }
                    let response = bazel_proto::PublishBuildToolEventStreamResponse {
                        stream_id: build_event.stream_id,
                        sequence_number: build_event.sequence_number,
                    };
                    tx.send(Ok(response)).await.ok();
                } else {
                    break;
                }
            }

            if !ws.compile_commands().is_empty() {
                if let Err(ref error) = ws.write() {
                    tracing::error!(%error, "write {} failed", COMPILE_COMMANDS_JSON);
                }
            }
        });

        let output_stream = ReceiverStream::new(rx);

        Ok(Response::new(
            Box::pin(output_stream) as Self::PublishBuildToolEventStreamStream
        ))
    }
}
