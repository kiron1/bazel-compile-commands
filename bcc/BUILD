package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bazel",
    srcs = ["bazel.cpp"],
    hdrs = ["bazel.hpp"],
    deps = [
        "@boost//:filesystem",
        "@boost//:json",
        "@boost//:process",
    ],
)

cc_test(
    name = "bazel_test",
    srcs = ["bazel_test.cpp"],
    deps = [
        ":bazel",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "compile_commands",
    srcs = ["compile_commands.cpp"],
    hdrs = ["compile_commands.hpp"],
    deps = [
        "@boost//:filesystem",
        "@boost//:io",
        "@boost//:json",
    ],
)

cc_test(
    name = "compile_commands_test",
    srcs = ["compile_commands_test.cpp"],
    deps = [
        ":compile_commands",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "options",
    srcs = ["options.cpp"],
    hdrs = ["options.hpp"],
    deps = [
        "@boost//:program_options",
    ],
)
