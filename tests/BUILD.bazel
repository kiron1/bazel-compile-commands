cc_test(
    name = "self_test",
    size = "small",
    srcs = ["self_test.cpp"],
    data = [
        ":bazel-mock",
        "//bcc:bazel-compile-commands",
    ],
    defines = ["BOOST_PROCESS_USE_STD_FS=1"],
    deps = [
        "//bcc:boost_json",
        "@bazel_tools//tools/cpp/runfiles",
        "@boost.process",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_binary(
    name = "bazel-mock",
    srcs = ["bazel_mock.cpp"],
    deps = ["//bcc:analysis"],
)
