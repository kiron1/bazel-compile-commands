load("@rules_rust//rust:defs.bzl", "rust_library")
load("@rules_rust//proto/prost:defs.bzl", "rust_prost_library")

package(default_visibility = ["//visibility:public"])

rust_prost_library(
    name = "google_devtools_build_v1_build_events_proto",
    proto = "@googleapis//:google_devtools_build_v1_build_events_proto",
)

rust_prost_library(
    name = "google_devtools_build_v1_publish_build_event_proto",
    proto = "@googleapis//:google_devtools_build_v1_publish_build_event_proto",
)

rust_prost_library(
    name = "build_event_stream_proto",
    proto = "@io_bazel//src/main/java/com/google/devtools/build/lib/buildeventstream/proto:build_event_stream_proto",
)

rust_library(
    name = "bazel_proto",
    srcs = ["lib.rs"],
    deps = [
        ":build_event_stream_proto",
        ":google_devtools_build_v1_build_events_proto",
        ":google_devtools_build_v1_publish_build_event_proto",
        "@crate_index//:prost",
        "@crate_index//:prost-types",
        "@crate_index//:tonic",
    ],
)
