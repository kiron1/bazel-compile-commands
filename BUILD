load("@bazel_skylib//rules:common_settings.bzl", "string_flag")
load("@rules_pkg//:pkg.bzl", "pkg_deb", "pkg_tar")
load("@mgred_rules_pandoc//pandoc:defs.bzl", "pandoc")
load("@//bazel:pkg_info.bzl", "pkg_variables", "pkg_version")

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

pandoc(
    name = "man",
    out = "bazel-compile-commands.1",
    input = "documentation.md",
    standalone = True,
    title = "bazel-compile-commands(1)",
    to = "man",
)

pkg_tar(
    name = "bin.tar",
    srcs = [":bazel-compile-commands"],
    mode = "0755",
    package_dir = "bin",
)

pkg_tar(
    name = "man.tar",
    srcs = [":man"],
    mode = "0644",
    package_dir = "share/man/man1",
)

pkg_tar(
    name = "usr.tar",
    package_dir = "/usr",
    deps = [
        ":bin.tar",
        ":man.tar",
    ],
)

pkg_deb(
    name = "deb",
    architecture = select({
        "@platforms//cpu:aarch64": "arm64",
        "@platforms//cpu:x86_32": "i386",
        "@platforms//cpu:x86_64": "amd64",
    }),
    built_using = "unzip (6.0.1)",
    data = ":usr.tar",
    description = "Generates compile_commands.json file from a Bazel workspace",
    homepage = "https://github.com/kiron1/bazel-compile-commands",
    maintainer = "Kiron <kiron1@gmail.com>",
    package = "bazel-compile-commands",
    package_file_name = "bazel-compile-commands_{version}_{architecture}.deb",
    package_variables = "@//:variables",
    version_file = "@//:version_file",
)

pkg_variables(
    name = "variables",
    architecture = select({
        "@platforms//cpu:x86_64": "amd64",
        "@platforms//cpu:aarch64": "aarch64",
    }),
    version = "//:version",
)

pkg_version(
    name = "version_file",
    out = "version.txt",
    version = "//:version",
)

string_flag(
    name = "version",
    build_setting_default = "0",
    visibility = ["//visibility:public"],
)
