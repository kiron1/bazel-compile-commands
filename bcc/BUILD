package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "bazel-compile-commands",
    srcs = ["main.cpp"],
    linkopts = select({
        "@platforms//os:windows": ["-DEFAULTLIB:shell32.lib"],
        "//conditions:default": [],
    }),
    deps = [
        ":bazel",
        ":compile_commands",
        ":options",
        ":platform",
        ":replacements",
    ],
)

cc_proto_library(
    name = "analysis",
    deps = ["@io_bazel//src/main/protobuf:analysis_v2_proto"],
)

cc_library(
    name = "artifacts",
    srcs = ["artifacts.cpp"],
    hdrs = ["artifacts.hpp"],
    deps = [
        ":analysis",
        ":path_fragments",
    ],
)

cc_library(
    name = "bazel",
    srcs = ["bazel.cpp"],
    hdrs = ["bazel.hpp"],
    defines = ["BOOST_PROCESS_USE_STD_FS=1"],
    deps = [
        ":analysis",
        "@boost//:json",
        "@boost//:process",
    ],
)

cc_library(
    name = "compile_commands",
    srcs = ["compile_commands.cpp"],
    hdrs = ["compile_commands.hpp"],
    deps = [
        ":analysis",
        ":artifacts",
        ":dep_set_of_files",
        ":path_fragments",
        ":replacements",
        "@boost//:io",
        "@boost//:json",
    ],
)

cc_test(
    name = "compile_commands_test",
    size = "small",
    srcs = ["compile_commands_test.cpp"],
    deps = [
        ":compile_commands",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "dep_set_of_files",
    srcs = ["dep_set_of_files.cpp"],
    hdrs = ["dep_set_of_files.hpp"],
    deps = [
        ":analysis",
        ":artifacts",
        "@boost//:core",
    ],
)

cc_library(
    name = "options",
    srcs = ["options.cpp"],
    hdrs = ["options.hpp"],
    defines = ["BOOST_PROCESS_USE_STD_FS=1"],
    deps = [
        "@boost//:process",
        "@boost//:program_options",
    ],
)

cc_library(
    name = "path_fragments",
    srcs = ["path_fragments.cpp"],
    hdrs = ["path_fragments.hpp"],
    deps = [
        ":analysis",
    ],
)

cc_library(
    name = "platform",
    srcs = ["platform.cpp"],
    hdrs = ["platform.hpp"],
    defines = ["BOOST_PROCESS_USE_STD_FS=1"],
    deps = [
        ":replacements",
        "@boost//:process",
    ],
)

cc_library(
    name = "replacements",
    srcs = ["replacements.cpp"],
    hdrs = ["replacements.hpp"],
)

cc_test(
    name = "replacements_test",
    size = "small",
    srcs = ["replacements_test.cpp"],
    deps = [
        ":replacements",
        "@com_google_googletest//:gtest_main",
    ],
)
