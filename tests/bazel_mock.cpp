#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

#include <google/protobuf/text_format.h>

#include "src/main/protobuf/analysis_v2.pb.h"

namespace {
char const* const aquery_textproto = R"(
rule_classes {
id: 1
name: "cc_binary"
}
targets {
id: 1
label: "//:main"
rule_class_id: 1
}
configuration {
id: 1
mnemonic: "darwin_arm64-fastbuild"
platform_name: "darwin_arm64"
checksum: "f4755fdba2a9adc4be1938f00075ca430c17dfc1e57b306638d8379237e33b52"
}
path_fragments {
id: 3
label: "external"
}
path_fragments {
id: 2
label: "local_config_cc"
parent_id: 3
}
path_fragments {
id: 1
label: "cc_wrapper.sh"
parent_id: 2
}
artifacts {
id: 1
path_fragment_id: 1
}
path_fragments {
id: 4
label: "libtool"
parent_id: 2
}
artifacts {
id: 2
path_fragment_id: 4
}
path_fragments {
id: 5
label: "libtool_check_unique"
parent_id: 2
}
artifacts {
id: 3
path_fragment_id: 5
}
path_fragments {
id: 6
label: "make_hashed_objlist.py"
parent_id: 2
}
artifacts {
id: 4
path_fragment_id: 6
}
path_fragments {
id: 7
label: "wrapped_clang"
parent_id: 2
}
artifacts {
id: 5
path_fragment_id: 7
}
path_fragments {
id: 8
label: "wrapped_clang_pp"
parent_id: 2
}
artifacts {
id: 6
path_fragment_id: 8
}
path_fragments {
id: 9
label: "xcrunwrapper.sh"
parent_id: 2
}
artifacts {
id: 7
path_fragment_id: 9
}
dep_set_of_files {
id: 2
direct_artifact_ids: 1
direct_artifact_ids: 2
direct_artifact_ids: 3
direct_artifact_ids: 4
direct_artifact_ids: 5
direct_artifact_ids: 6
direct_artifact_ids: 7
}
path_fragments {
id: 10
label: "main.cpp"
}
artifacts {
id: 8
path_fragment_id: 10
}
dep_set_of_files {
id: 1
transitive_dep_set_ids: 2
direct_artifact_ids: 8
}
path_fragments {
id: 16
label: "bazel-out"
}
path_fragments {
id: 15
label: "darwin_arm64-fastbuild"
parent_id: 16
}
path_fragments {
id: 14
label: "bin"
parent_id: 15
}
path_fragments {
id: 13
label: "_objs"
parent_id: 14
}
path_fragments {
id: 12
label: "main"
parent_id: 13
}
path_fragments {
id: 11
label: "main.o"
parent_id: 12
}
artifacts {
id: 9
path_fragment_id: 11
}
path_fragments {
id: 17
label: "main.d"
parent_id: 12
}
artifacts {
id: 10
path_fragment_id: 17
}
actions {
target_id: 1
action_key: "7e536f8eda15e04d9292a64bb937d579272c49b8cf88bb77c4fc2f282a6b9ff3"
mnemonic: "CppCompile"
configuration_id: 1
arguments: "cc"
arguments: "-Wall"
arguments: "-c"
arguments: "main.cpp"
arguments: "-o"
arguments: "bazel-out/darwin_arm64-fastbuild/bin/_objs/main/main.o"
environment_variables {
  key: "ZERO_AR_DATE"
  value: "1"
}
input_dep_set_ids: 1
output_ids: 9
output_ids: 10
discovers_inputs: true
execution_info {
  key: "requires-darwin"
}
execution_info {
  key: "supports-xcode-requirements-set"
}
primary_output_id: 9
execution_platform: "@local_config_platform//:host"
}
)";
}

int
main(int argc, char** argv)
{
  std::vector<std::string> arguments(argv + 1, argv + argc);
  auto info_iter = std::find(std::begin(arguments), std::end(arguments), "info");
  auto aquery_iter = std::find(std::begin(arguments), std::end(arguments), "aquery");

  if (info_iter != std::end(arguments)) {
    ++info_iter;
    if (info_iter != std::end(arguments)) {
      if (*info_iter == std::string_view("workspace")) {
        std::cout << "/tmp/workspace\n";
        return 0;
      } else if (*info_iter == std::string_view("execution_root")) {
        std::cout << "/tmp/execroot\n";
        return 0;
      } else {
        std::cerr << "fatal error: invalid argument for workspace: invlaid location\n";
        return 1;
      }
    } else {
      std::cerr << "fatal error: invalid argument for workspace: location required\n";
      return 1;
    }
  } else if (aquery_iter != std::end(arguments)) {
    analysis::ActionGraphContainer agc;
    google::protobuf::TextFormat::ParseFromString(aquery_textproto, &agc);
    agc.SerializePartialToOstream(&std::cout);
    return 0;
  } else {
    std::cerr << "fatal error: invalid argument: unknown sub-command\n";
    return 1;
  }

  return 0;
}
