load("@rules_rust//rust:defs.bzl", "rust_library_group")
load("@rules_rust//proto/prost:defs.bzl", "rust_prost_toolchain")

rust_library_group(
    name = "prost_runtime",
    deps = [
        "@crate_index//:prost",
    ],
)

rust_library_group(
    name = "tonic_runtime",
    deps = [
        ":prost_runtime",
        "@crate_index//:tonic",
    ],
)

rust_prost_toolchain(
    name = "prost_toolchain_impl",
    prost_plugin = "@crate_index//:protoc-gen-prost__protoc-gen-prost",
    prost_plugin_flag = "--plugin=protoc-gen-prost=%s",
    prost_runtime = ":prost_runtime",
    prost_types = "@crate_index//:prost-types",
    proto_compiler = "@com_google_protobuf//:protoc",
    tonic_plugin = "@crate_index//:protoc-gen-tonic__protoc-gen-tonic",
    tonic_runtime = ":tonic_runtime",
)

toolchain(
    name = "prost_toolchain",
    toolchain = "prost_toolchain_impl",
    toolchain_type = "@rules_rust//proto/prost:toolchain_type",
)
