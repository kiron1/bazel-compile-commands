load("@rules_pkg//pkg:tar.bzl", "pkg_tar")

package(default_visibility = ["//visibility:public"])

pkg_tar(
    name = "systemd",
    srcs = [
        "basil-tonic.service",
        "basil-tonic.socket",
    ],
    package_dir = "lib/systemd/user",
)
