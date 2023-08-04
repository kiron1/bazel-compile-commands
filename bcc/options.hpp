#pragma once

#include <optional>
#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>

namespace bcc {

extern const char* const rc_name;

struct options
{
  static options from_argv(int argc, char* argv[]);

  /// Be verbose.
  bool verbose{ false };
  /// Include `arguments` array in the final `compile_commands.json`
  bool arguments{ false };
  /// Bazel command.
  boost::filesystem::path bazel_command{};
  /// Replace Bazel compiler with `cc`.
  std::optional<std::string> compiler{};
  /// Bazel startup options.
  std::vector<std::string> bazel_startup_options;
  /// Output path of the `compile_commands.json` file.
  std::string output_path{ "%workspace%/compile_commands.json" };
  /// Bazel build configs to apply.
  std::vector<std::string> configs{};
  /// Targets to consider for the generation of the `compile_commands.json`
  /// file.
  std::vector<std::string> targets{ "//..." };
  /// Flags to be forwarded to the `bazel query` call.
  std::vector<std::string> bazel_flags{};
  /// Write current config to file.
  bool write_rc_file{ false };
  /// Path of config file if one is found.
  std::optional<boost::filesystem::path> rcpath;

  std::ostream& write(std::ostream& os) const;
};
} // namespace bcc
