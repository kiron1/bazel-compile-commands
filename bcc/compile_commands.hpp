#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/json/value.hpp>

namespace bcc {
/// Turn actions from a bazel aquery into a compile_commands.json format.
boost::json::value
compile_commands(boost::json::value const &actions,
                 boost::filesystem::path const &execution_root);
} // namespace bcc
