load("@rules_rust//rust:defs.bzl", "rust_binary", "rust_library", "rust_test")

package(default_visibility = ["//visibility:public"])

rust_library(
    name = "libbasil_tonic",
    srcs = [
        "lib.rs",
        "service.rs",
        "socket.rs",
        "workspace.rs",
    ],
    crate_name = "basil_tonic",
    deps = [
        "@//bazel_proto",
        "@//compile_commands",
        "@crate_index//:futures-util",
        "@crate_index//:prost",
        "@crate_index//:serde",
        "@crate_index//:serde_json",
        "@crate_index//:thiserror",
        "@crate_index//:tokio",
        "@crate_index//:tokio-stream",
        "@crate_index//:tonic",
        "@crate_index//:tracing",
        "@crate_index//:url",
    ] + select({
        "@platforms//os:macos": ["@crate_index//:libc"],
        "//conditions:default": [],
    }),
)

rust_test(
    name = "libbasil_tonic_test",
    size = "small",
    crate = ":libbasil_tonic",
)

rust_binary(
    name = "basil_tonic",
    srcs = ["main.rs"],
    deps = [
        ":libbasil_tonic",
        "@crate_index//:clap",
        "@crate_index//:tokio",
        "@crate_index//:tokio-stream",
        "@crate_index//:tonic",
        "@crate_index//:tracing",
        "@crate_index//:tracing-subscriber",
    ],
)
