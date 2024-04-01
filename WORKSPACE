workspace(name = "bazel-compile-commands")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

_RULES_BOOST_COMMIT = "c649f6cd9cb8ca284d261890cdd9d75ec486861d"

http_archive(
    name = "com_github_nelhage_rules_boost",
    sha256 = "70304cb09bd7529b8f45195f79a4c4070c1d75fe7dd3cc8f2f263ec2f12c12a3",
    strip_prefix = "rules_boost-{}".format(_RULES_BOOST_COMMIT),
    urls = [
        "https://github.com/nelhage/rules_boost/archive/{}.tar.gz".format(_RULES_BOOST_COMMIT),
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

load("@mgred_rules_pandoc//pandoc:repositories.bzl", "LATEST_PANDOC_VERSION", "pandoc_register_toolchains", "rules_pandoc_dependencies")

rules_pandoc_dependencies()

pandoc_register_toolchains(
    name = "pandoc3_1",
    pandoc_version = LATEST_PANDOC_VERSION,
)
