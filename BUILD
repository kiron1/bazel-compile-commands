load("@//bazel:pkg_info.bzl", "pkg_variables", "pkg_version")
load("@bazel_skylib//rules:common_settings.bzl", "string_flag")
load("@rules_pkg//:mappings.bzl", "pkg_attributes", "pkg_filegroup", "pkg_files")
load("@rules_pkg//:pkg.bzl", "pkg_deb", "pkg_tar", "pkg_zip")

package(default_visibility = ["//visibility:public"])

platform(
    name = "arm64-apple-darwin",
    constraint_values = [
        "@platforms//cpu:arm64",
        "@platforms//os:macos",
    ],
)

platform(
    name = "x86_64-apple-darwin",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:macos",
    ],
)

filegroup(
    name = "pandoc_macos",
    srcs = select({
        "@platforms//cpu:arm64": ["@pandoc_macos_arm64//:pandoc"],
        "@platforms//cpu:x86_64": ["@pandoc_macos_x86_64//:pandoc"],
    }),
)

filegroup(
    name = "pandoc",
    srcs = select({
        "@platforms//os:linux": ["@pandoc_linux_x86_64//:pandoc"],
        "@platforms//os:macos": [":pandoc_macos"],
        "@platforms//os:windows": ["@pandoc_windowss_x86_64//:pandoc"],
    }),
)

genrule(
    name = "man",
    srcs = ["documentation.md"],
    outs = ["bazel-compile-commands.1"],
    cmd = "$(location :pandoc) -f markdown -t man -o $@ $(location documentation.md)",
    tools = [":pandoc"],
)

pkg_files(
    name = "bin_files",
    srcs = [
        "//bcc:bazel-compile-commands-bin",
    ],
    attributes = pkg_attributes(mode = "0755"),
    prefix = "bin",
)

pkg_files(
    name = "man_files",
    srcs = [":man"],
    prefix = "share/man/man1",
)

pkg_filegroup(
    name = "usr_files",
    srcs = [
        ":bin_files",
        ":man_files",
    ],
    prefix = "/usr",
)

pkg_deb(
    name = "deb",
    architecture = select({
        "@platforms//cpu:aarch64": "arm64",
        "@platforms//cpu:x86_64": "amd64",
    }),
    built_using = "unzip (6.0.1)",
    data = ":tar",
    description = "Generates compile_commands.json file from a Bazel workspace",
    homepage = "https://github.com/kiron1/bazel-compile-commands",
    maintainer = "Kiron <kiron1@gmail.com>",
    package = "bazel-compile-commands",
    package_file_name = "bazel-compile-commands_{version}_{architecture}.deb",
    package_variables = "@//:variables",
    version_file = "@//:version_file",
)

pkg_tar(
    name = "tar",
    srcs = [":usr_files"],
)

pkg_zip(
    name = "zip",
    srcs = [":usr_files"],
    package_file_name = select({
        "@platforms//os:macos": "bazel-compile-commands_{version}_{os}_universal.zip",
        "//conditions:default": "bazel-compile-commands_{version}_{os}_{architecture}.zip",
    }),
    package_variables = "@//:variables",
)

alias(
    name = "pkg",
    actual = select({
        "@platforms//os:linux": ":deb",
        # Build a zip file for any other platforms:
        "//conditions:default": ":zip",
    }),
)

sh_binary(
    name = "copy",
    srcs = ["copy.sh"],
    data = [":pkg"],
    env = {"PKG": "$(location :pkg)"},
    deps = ["@bazel_tools//tools/bash/runfiles"],
)

pkg_variables(
    name = "variables",
    architecture = select({
        "@platforms//cpu:x86_64": "amd64",
        "@platforms//cpu:arm64": "arm64",
    }),
    os = select({
        "@platforms//os:linux": "linux",
        "@platforms//os:macos": "macos",
        "@platforms//os:windows": "windows",
        "//conditions:default": "unknown",
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
