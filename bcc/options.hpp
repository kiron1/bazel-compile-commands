#pragma once

#include <optional>
#include <string>
#include <vector>

namespace bcc {
struct options
{
  static options from_argv(int argc, char* argv[]);

  /// Be verbose.
  bool verbose{ false };
  /// Include `arguments` array in the final `compile_commands.json`
  bool arguments{ false };
  /// Bazel command.
  std::string bazel_commands{ "bazel" };
  /// Replace Bazel compiler with `cc`.
  std::optional<std::string> compiler{};
  /// Bazel startup options.
  std::vector<std::string> bazel_startup_options;
  /// Output path of the `compile_commands.json` file.
  std::string output_path{ "%workspace%/compile_commands.json" };
  /// Targets to consider for the generation of the `compile_commands.json`
  /// file.
  std::vector<std::string> targets{ "//..." };
  /// Flags to be forwarded to the `bazel query` call.
  std::vector<std::string> bazel_flags{};
};
} // namespace bcc
