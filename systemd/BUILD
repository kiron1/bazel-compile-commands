load("@rules_pkg//:mappings.bzl", "pkg_files")

package(default_visibility = ["//visibility:public"])

pkg_files(
    name = "systemd",
    srcs = [
        "basil-tonic.service",
        "basil-tonic.socket",
    ],
    prefix = "lib/systemd/user",
)
