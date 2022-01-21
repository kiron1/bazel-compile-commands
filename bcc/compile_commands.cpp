#include "bcc/compile_commands.hpp"

#include <sstream>

#include <boost/io/quoted.hpp>
#include <boost/json.hpp>

namespace bcc {

namespace {
boost::json::string find_file(boost::json::array const &arguments) {
  bool got_c_flag = false;
  for (auto const &argument : arguments) {
    auto const &astr = argument.as_string();

    if (got_c_flag) {
      return astr;
    }
    if (astr == "-c") {
      got_c_flag = true;
    }
  }
  return "file_not_found.cpp";
}
} // namespace

boost::json::value
compile_commands(boost::json::value const &aquery,
                 boost::filesystem::path const &execution_root) {
  // The aqery result is an object with an actions element which is an array of
  // objects
  auto actions = aquery.at("actions").as_array();

  // the root element of a compile_commands.json document is an array of objects
  auto json = boost::json::array();

  for (const auto &action_value : actions) {
    const auto &action = action_value.as_object();
    auto args = action.at("arguments").as_array();
    auto file = find_file(args);
    auto cmd = std::stringstream();
    bool need_sep = false;
    for (const auto &arg : args) {
      if (need_sep) {
        cmd << " ";
      }
      if (arg.as_string().find(' ') == std::string_view::npos &&
          arg.as_string().find('"') == std::string_view::npos) {
        // no need to quote
        cmd << arg.as_string().c_str();
      } else {
        // json::string will be quoted correctly when streamed
        cmd << arg.as_string();
      }
      need_sep = true;
    }

    // one entry in the compile_commands.json document
    auto obj = boost::json::object();
    obj.insert(
        boost::json::object::value_type{"directory", execution_root.native()});
    obj.insert(boost::json::object::value_type{"arguments", args});
    obj.insert(boost::json::object::value_type{"commands", cmd.str()});
    obj.insert(boost::json::object::value_type{"file", file});

    json.push_back(obj);
  }
  return json;
}

} // namespace bcc
