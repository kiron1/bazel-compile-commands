load("@rules_pkg//:pkg.bzl", "pkg_deb", "pkg_tar")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "bazel-compile-commands",
    srcs = ["main.cpp"],
    deps = [
        "//bcc:bazel",
        "//bcc:compile_commands",
        "//bcc:options",
        "//bcc:platform",
        "//bcc:replacements",
    ],
)

pkg_tar(
    name = "bazel-compile-commands-bin",
    srcs = [":bazel-compile-commands"],
    mode = "0755",
    package_dir = "/usr/bin",
)

pkg_deb(
    name = "deb",
    architecture = "amd64",
    built_using = "unzip (6.0.1)",
    data = ":bazel-compile-commands-bin",
    description = "Generates compile_commands.json file from a Bazel workspace",
    homepage = "https://github.com/kiron1/bazel-compile-commands",
    maintainer = "Kiron <kiron1@gmail.com>",
    package = "bazel-compile-commands",
    version = "0.0.1",
)
