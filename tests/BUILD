cc_test(
    name = "self_test",
    size = "small",
    srcs = ["self_test.cpp"],
    data = [
        ":bazel-mock",
        "//bcc:bazel-compile-commands",
    ],
    deps = [
        "@bazel_tools//tools/cpp/runfiles",
        "@boost//:filesystem",
        "@boost//:json",
        "@boost//:process",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_binary(
    name = "bazel-mock",
    srcs = ["bazel_mock.cpp"],
    deps = ["//bcc:analysis"],
)
