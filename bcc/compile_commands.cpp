#include "bcc/compile_commands.hpp"

#include <iostream>
#include <optional>
#include <sstream>
#include <string_view>
#include <unordered_map>

#include <boost/json.hpp>

#include "bcc/artifacts.hpp"
#include "bcc/dep_set_of_files.hpp"
#include "bcc/path_fragments.hpp"

namespace bcc {

namespace {

/// ends_with for pre C++20
bool
ends_with(std::string_view const& sv, std::string_view const& suffix)
{
  return sv.size() >= suffix.size() && sv.compare(sv.size() - suffix.size(), std::string_view::npos, suffix) == 0;
}

bool
is_cc_suffix(std::string_view const& v)
{
  return ends_with(v, ".C") || ends_with(v, ".c") || ends_with(v, ".cc") || ends_with(v, ".cxx") ||
         ends_with(v, ".c++") || ends_with(v, ".cpp") || ends_with(v, ".m") || ends_with(v, ".mm");
}

/// Join an array of arguments into a commands string.
std::string
join_arguments(boost::json::array const& args)
{
  auto cmd = std::stringstream();
  bool need_sep = false;
  for (auto const& arg : args) {
    if (need_sep) {
      cmd << " ";
    }
    auto const has_space = arg.as_string().find(' ') == std::string_view::npos;
    auto const has_quote = arg.as_string().find('"') == std::string_view::npos;
    if (has_space || has_quote) {
      // no need to quote
      cmd << arg.as_string().c_str();
    } else {
      // json::string will be quoted correctly when streamed
      cmd << arg.as_string();
    }
    need_sep = true;
  }
  return cmd.str();
}
} // namespace

compile_commands_builder&
compile_commands_builder::arguments(bool value)
{
  arguments_ = value;
  return *this;
}

compile_commands_builder&
compile_commands_builder::compiler(std::optional<std::string> value)
{
  compiler_ = std::move(value);
  return *this;
}

compile_commands_builder&
compile_commands_builder::replacements(bcc::replacements value)
{
  replacements_ = std::move(value);
  return *this;
}

compile_commands_builder&
compile_commands_builder::execution_root(std::filesystem::path value)
{
  execution_root_ = std::move(value);
  return *this;
}

boost::json::array
compile_commands_builder::build(analysis::ActionGraphContainer const& action_graph) const
{
  auto const fragements = path_fragments(action_graph.path_fragments());
  auto const art = artifacts(action_graph.artifacts(), fragements);
  auto const dep_set = dep_set_of_files(action_graph.dep_set_of_files(), art);

  // the root element of a compile_commands.json document is an array of objects
  auto json = boost::json::array();

  for (auto const& action : action_graph.actions()) {
    auto const action_args = action.arguments();
    if (!action_args.empty()) {
      // arguments are optional in the action graph
      auto args = boost::json::array();
      auto action_args_begin = std::begin(action_args);
      if (compiler_.has_value()) {
        args.push_back(boost::json::string(compiler_.value()));
        std::advance(action_args_begin, 1); ///< skip over the real compiler
      }
      std::transform(action_args_begin, std::end(action_args), std::back_inserter(args), [&](auto a) {
        return boost::json::string(replacements_.apply(a));
      });
      auto const output = art.path_of_artifact(action.primary_output_id());
      auto file = std::optional<std::string>{};
      for (auto const& k : action.input_dep_set_ids()) {
        auto const set = dep_set.get(k);
        file = set.find_if(is_cc_suffix);
        if (file.has_value()) {
          break;
        }
      }

      // input file is required
      if (file.has_value()) {
        auto obj = boost::json::object();
        obj.insert(boost::json::object::value_type{ "directory", execution_root_.native() });
        if (arguments_) {
          obj.insert(boost::json::object::value_type{ "arguments", args });
        }
        obj.insert(boost::json::object::value_type{ "command", join_arguments(args) });
        obj.insert(boost::json::object::value_type{ "file", file.value() });
        obj.insert(boost::json::object::value_type{ "output", output });

        json.push_back(obj);
      }
    }
  }
  return json;
}

} // namespace bcc
