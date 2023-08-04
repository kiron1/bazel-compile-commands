
#include "bcc/bazel.hpp"
#include "bcc/compile_commands.hpp"
#include "bcc/options.hpp"
#include "bcc/platform.hpp"
#include "bcc/replacements.hpp"

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string_view>

#include <boost/filesystem.hpp>
#include <boost/json.hpp>

#include <unistd.h>

std::string
make_query(std::vector<std::string> const& targets)
{
  auto query = std::stringstream{};
  query << "mnemonic('(Objc|Cpp)Compile',deps(";
  bool need_plus = false;
  for (auto const& target : targets) {
    if (need_plus) {
      query << " + ";
    }
    query << target;
    need_plus = true;
  }
  query << "))";
  return query.str();
}

std::string
replace_workspace_placeholder(std::string value, std::string workspace_location)
{
  bcc::replacements repl;

  repl.add({ "%workspace%", std::move(workspace_location) });

  return repl.apply(std::move(value));
}

int
main(int argc, char** argv)
{
  try {
    // When run via `bazel run ...` change directory to the current workspace
    if (getenv("BUILD_WORKSPACE_DIRECTORY")) {
      chdir(getenv("BUILD_WORKSPACE_DIRECTORY"));
    }

    auto options = bcc::options::from_argv(argc, argv);

    if (options.bazel_command.empty() || !boost::filesystem::exists(options.bazel_command)) {
      std::cerr << "fatal error: bazel or bazelisk command not found, please enure the command is in PATH or use "
                   "`--bazel-command PATH'"
                << std::endl;
      return 1;
    }

    auto bazel = bcc::bazel::create(options.bazel_command, options.bazel_startup_options);

    if (options.write_rc_file) {
      if (!options.rcpath.has_value()) {
        options.rcpath = bazel.workspace_path() / bcc::rc_name;
      }
      auto rcfile = std::ofstream(options.rcpath.value().c_str());
      options.write(rcfile);
    }

    options.output_path = replace_workspace_placeholder(options.output_path, bazel.workspace_path().native());
    if (options.verbose) {
      options.write(std::cerr);
    }

    const auto replacements =
      bcc::platform_replacements(bazel.workspace_path().native(), bazel.execution_root().native());
    if (options.verbose) {
      for (auto const& def : replacements.definitions()) {
        std::cerr << def.first << "=" << def.second << std::endl;
      }
    }

    auto builder = bcc::compile_commands_builder();
    builder.arguments(options.arguments)
      .replacements(replacements)
      .execution_root(bazel.execution_root())
      .compiler(options.compiler);

    const auto query_str = make_query(options.targets);
    if (options.verbose) {
      std::cerr << "Query `" << query_str << '`' << std::endl;
    }
    auto actions = bazel.aquery(query_str, options.bazel_flags, options.configs);
    if (options.verbose) {
      if (const auto actions_object = actions.if_object(); actions_object) {
        if (const auto actions_value = actions_object->if_contains("actions"); actions_value) {
          const auto actions_size = actions_value->is_array() ? actions_value->as_array().size() : 0;
          std::cerr << "Build compile commands from " << actions_size << " actions" << std::endl;
        }
      }
    }
    const auto compile_commands = builder.build(actions);
    {
      if (options.verbose) {
        std::cerr << "Writting " << compile_commands.size() << " commands to `" << options.output_path << "`"
                  << std::endl;
      }
      // Allow `-` as output_path to mean write to stdout instead of a file.
      auto compile_commands_file = std::ofstream{};
      auto* compile_commands_buf = static_cast<std::streambuf*>(nullptr);
      if (options.output_path == "-") {
        compile_commands_buf = std::cout.rdbuf();
      } else {
        compile_commands_file.open(options.output_path.c_str());
        compile_commands_buf = compile_commands_file.rdbuf();
      }

      auto compile_commands_stream = std::ostream(compile_commands_buf);
      compile_commands_stream << compile_commands;
    }
  } catch (std::exception const& ex) {
    std::cerr << "fatal error: " << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
