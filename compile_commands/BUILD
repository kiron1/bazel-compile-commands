load("@rules_rust//rust:defs.bzl", "rust_library", "rust_test")

package(default_visibility = ["//visibility:public"])

rust_library(
    name = "compile_commands",
    srcs = ["lib.rs"],
    deps = [
        "@crate_index//:serde",
    ],
)

rust_test(
    name = "compile_commands_test",
    size = "small",
    crate = ":compile_commands",
    deps = ["@crate_index//:serde_json"],
)
