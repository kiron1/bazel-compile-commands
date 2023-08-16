#pragma once

#include "bcc/replacements.hpp"

#include <filesystem>
#include <optional>
#include <string>

#include <boost/json/value.hpp>

#include "external/io_bazel/src/main/protobuf/analysis_v2.pb.h"

namespace bcc {

class compile_commands_builder
{
public:
  /// Include `arguments` in final compile_commands.json file.
  compile_commands_builder& arguments(bool value);
  /// Set compiler.
  compile_commands_builder& compiler(std::optional<std::string> value);
  /// Set execution_root.
  compile_commands_builder& execution_root(std::filesystem::path value);
  /// Set replacements.
  compile_commands_builder& replacements(bcc::replacements value);
  /// Turn actions from a bazel aquery into a compile_commands.json format.
  boost::json::array build(analysis::ActionGraphContainer const& action_graph) const;

private:
  bool arguments_{ false };
  std::optional<std::string> compiler_{};
  bcc::replacements replacements_{};
  std::filesystem::path execution_root_{};
};
} // namespace bcc
