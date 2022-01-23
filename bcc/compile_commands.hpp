#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/json/value.hpp>

namespace bcc {

class compile_commands_builder
{
public:
  /// Set C compiler.
  compile_commands_builder& cc(std::optional<std::string> cc);
  /// Set C++ compiler.
  compile_commands_builder& cxx(std::optional<std::string> cxx);
  /// Set execution_root.
  compile_commands_builder& execution_root(
    boost::filesystem::path execution_root);
  /// Turn actions from a bazel aquery into a compile_commands.json format.
  boost::json::array build(boost::json::value const& analysis) const;

private:
  std::optional<std::string> cc_{};
  std::optional<std::string> cxx_{};
  boost::filesystem::path execution_root_{};
};
} // namespace bcc
