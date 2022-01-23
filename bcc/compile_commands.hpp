#pragma once

#include <string>
#include <optional>
#include <boost/filesystem/path.hpp>
#include <boost/json/value.hpp>

namespace bcc {

class compile_commands_builder
{
public:
  /// Set compiler.
  compile_commands_builder& compiler(std::optional<std::string> value);
  /// Set execution_root.
  compile_commands_builder& execution_root(
    boost::filesystem::path value);
  /// Turn actions from a bazel aquery into a compile_commands.json format.
  boost::json::array build(boost::json::value const& analysis) const;

private:
  std::optional<std::string> compiler_{};
  boost::filesystem::path execution_root_{};
};
} // namespace bcc
