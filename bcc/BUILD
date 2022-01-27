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

cc_library(
    name = "compile_commands",
    srcs = ["compile_commands.cpp"],
    hdrs = ["compile_commands.hpp"],
    deps = [
        "@boost//:filesystem",
        "@boost//:io",
        "@boost//:json",
        ":replacements",
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

cc_library(
    name = "platform",
    srcs = ["platform.cpp"],
    hdrs = ["platform.hpp"],
    deps = [
        "@boost//:process",
        ":replacements",
    ],
)

cc_library(
    name = "replacements",
    srcs = ["replacements.cpp"],
    hdrs = ["replacements.hpp"],
)

cc_test(
    name = "replacements_test",
    srcs = ["replacements_test.cpp"],
    deps = [
        ":replacements",
        "@com_google_googletest//:gtest_main",
    ],
)
