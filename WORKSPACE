workspace(name = "bazel-compile-commands")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

_RULES_BOOST_COMMIT = "c649f6cd9cb8ca284d261890cdd9d75ec486861d"

http_archive(
    name = "build_bazel_rules_apple",
    sha256 = "20da675977cb8249919df14d0ce6165d7b00325fb067f0b06696b893b90a55e8",
    urls = [
        "https://github.com/bazelbuild/rules_apple/releases/download/3.0.0/rules_apple.3.0.0.tar.gz",
    ],
)

http_archive(
    name = "io_bazel",
    patches = ["//third_party:bazel.patch"],
    sha256 = "8b9de325bef017ee7a8d9a346e61a76d199e1eb5dc6924753eed4f8152066bab",
    strip_prefix = "bazel-6.3.2",
    urls = [
        "https://github.com/bazelbuild/bazel/archive/refs/tags/6.3.2.tar.gz",
    ],
)

http_archive(
    name = "googleapis",
    build_file = "@io_bazel//third_party/googleapis:BUILD.bazel",
    # patches = ["//third_party:no-build-files.patch"],
    patch_cmds = ["find . -mindepth 2 -name 'BUILD' -o -name 'BUILD.bazel' -exec rm {} +"],
    sha256 = "5bb6b0253ccf64b53d6c7249625a7e3f6c3bc6402abd52d3778bfa48258703a0",
    strip_prefix = "googleapis-2f9af297c84c55c8b871ba4495e01ade42476c92",
    urls = [
        "https://github.com/googleapis/googleapis/archive/2f9af297c84c55c8b871ba4495e01ade42476c92.tar.gz",
    ],
)

# GoogleTest/GoogleMock framework. Used by unit-tests.
http_archive(
    name = "com_google_googletest",
    sha256 = "8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7",
    strip_prefix = "googletest-1.14.0",
    urls = ["https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz"],
)

http_archive(
    name = "com_github_nelhage_rules_boost",
    sha256 = "70304cb09bd7529b8f45195f79a4c4070c1d75fe7dd3cc8f2f263ec2f12c12a3",
    strip_prefix = "rules_boost-{}".format(_RULES_BOOST_COMMIT),
    urls = [
        "https://github.com/nelhage/rules_boost/archive/{}.tar.gz".format(_RULES_BOOST_COMMIT),
    ],
)

http_archive(
    name = "rules_proto",
    sha256 = "dc3fb206a2cb3441b485eb1e423165b231235a1ea9b031b4433cf7bc1fa460dd",
    strip_prefix = "rules_proto-5.3.0-21.7",
    urls = [
        "https://github.com/bazelbuild/rules_proto/archive/refs/tags/5.3.0-21.7.tar.gz",
    ],
)

http_archive(
    name = "rules_pkg",
    sha256 = "8f9ee2dc10c1ae514ee599a8b42ed99fa262b757058f65ad3c384289ff70c4b8",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_pkg/releases/download/0.9.1/rules_pkg-0.9.1.tar.gz",
        "https://github.com/bazelbuild/rules_pkg/releases/download/0.9.1/rules_pkg-0.9.1.tar.gz",
    ],
)

http_archive(
    name = "mgred_rules_pandoc",
    sha256 = "0ee76dc7403d40de7946e6b884f3e868e72f2d7ba172edfc828bfb50fd5baf04",
    strip_prefix = "rules_pandoc-0.2.0",
    url = "https://github.com/mgred/rules_pandoc/releases/download/v0.2.0/rules_pandoc-v0.2.0.tar.gz",
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()

load(
    "@build_bazel_rules_apple//apple:repositories.bzl",
    "apple_rules_dependencies",
)

apple_rules_dependencies()

load(
    "@build_bazel_apple_support//lib:repositories.bzl",
    "apple_support_dependencies",
)

apple_support_dependencies()

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")

rules_proto_dependencies()

rules_proto_toolchains()

# register_toolchains("//toolchains:prost_toolchain")

load("@rules_pkg//:deps.bzl", "rules_pkg_dependencies")

rules_pkg_dependencies()

load("@mgred_rules_pandoc//pandoc:repositories.bzl", "LATEST_PANDOC_VERSION", "pandoc_register_toolchains", "rules_pandoc_dependencies")

rules_pandoc_dependencies()

pandoc_register_toolchains(
    name = "pandoc3_1",
    pandoc_version = LATEST_PANDOC_VERSION,
)
