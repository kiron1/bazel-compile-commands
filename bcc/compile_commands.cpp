#include "bcc/compile_commands.hpp"

#include <optional>
#include <sstream>

#include <string_view>
#include <boost/json.hpp>

namespace bcc {

namespace {
std::optional<boost::json::string>
find_argument(boost::json::array const& arguments, std::string_view flag)
{
  bool got_flag = false;
  for (auto const& argument : arguments) {
    auto const& astr = argument.as_string();

    if (got_flag) {
      return astr;
    }
    if (astr == flag) {
      got_flag = true;
    }
  }
  return std::nullopt;
}

/// Join an array of arguments into a commands string.
std::string
join_arguments(boost::json::array const& args)
{
  auto cmd = std::stringstream();
  bool need_sep = false;
  for (const auto& arg : args) {
    if (need_sep) {
      cmd << " ";
    }
    const auto has_space = arg.as_string().find(' ') == std::string_view::npos;
    const auto has_quote = arg.as_string().find('"') == std::string_view::npos;
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
compile_commands_builder::execution_root(boost::filesystem::path value)
{
  execution_root_ = std::move(value);
  return *this;
}

boost::json::array
compile_commands_builder::build(boost::json::value const& analysis) const
{
  // the root element of a compile_commands.json document is an array of objects
  auto json = boost::json::array();

  // The aquery result is an object with an actions element which is an array of
  // objects
  if (const auto actions_object = analysis.if_object(); actions_object) {
    if (const auto actions_value = actions_object->if_contains("actions"); actions_value) {

      const auto actions = actions_value->as_array();

      for (const auto& action_value : actions) {
        if (const auto& action = action_value.if_object(); action) {
          if (const auto& args_value = action->if_contains("arguments"); args_value) {

            auto args = args_value->as_array();
            if (compiler_.has_value()) {
              args[0] = compiler_.value();
            }
            std::transform(std::begin(args), std::end(args), std::begin(args), [&](auto a) {
              return boost::json::string(replacements_.apply(a.as_string().data()));
            });
            const auto cmd = join_arguments(args);
            const auto output = find_argument(args, "-o");

            /// A hack way to get the input file (TODO)
            const auto file = find_argument(args, "-c");
            if (file.has_value()) {
              // one entry in the compile_commands.json document
              auto obj = boost::json::object();
              obj.insert(boost::json::object::value_type{ "directory", execution_root_.native() });
              if (arguments_) {
                obj.insert(boost::json::object::value_type{ "arguments", args });
              }
              obj.insert(boost::json::object::value_type{ "command", cmd });
              obj.insert(boost::json::object::value_type{ "file", file.value() });
              if (output.has_value()) {
                // `output` is optional in `compile_commands.json`
                obj.insert(boost::json::object::value_type{ "output", output.value() });
              }

              json.push_back(obj);
            }
          }
        }
      }
    }
  }
  return json;
}

} // namespace bcc
