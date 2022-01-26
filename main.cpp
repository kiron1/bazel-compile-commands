
#include "bcc/bazel.hpp"
#include "bcc/compile_commands.hpp"
#include "bcc/options.hpp"
#include "bcc/platform.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/json.hpp>

#include <unistd.h>

std::string make_query(std::vector<std::string> const& targets) {
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

int
main(int argc, char** argv)
{
  try {
    // When run via `bazel run ...` change directory to the current workspace
    if (getenv("BUILD_WORKSPACE_DIRECTORY")) {
      chdir(getenv("BUILD_WORKSPACE_DIRECTORY"));
    }

    auto options = bcc::options::from_argv(argc, argv);
    if (options.verbose) {
      std::cerr << "arguments: " << options.arguments << std::endl;
      if (options.compiler.has_value()) {
        std::cerr << "compiler: " << options.compiler.value() << std::endl;
      }
      std::cerr << "targets: ";
      std::copy(std::begin(options.targets),
                std::end(options.targets),
                std::ostream_iterator<std::string_view>(std::cerr, " "));
      std::cerr << std::endl;
      std::cerr << "config: ";
      std::copy(std::begin(options.bazel_flags),
                std::end(options.bazel_flags),
                std::ostream_iterator<std::string_view>(std::cerr, " "));
      std::cerr << std::endl;
    }

    auto bazel = bcc::bazel::create();
    if (options.verbose) {
      std::cerr << "bazel_command: " << bazel.command_path() << std::endl;
      std::cerr << "workspace: " << bazel.workspace_path() << std::endl;
      std::cerr << "execution_root: " << bazel.execution_root() << std::endl;
    }

    const auto replacements = bcc::platform_replacements(
      bazel.workspace_path().native(), bazel.execution_root().native());
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
    auto actions = bazel.aquery(query_str, options.bazel_flags);
    if (options.verbose) {
      std::cerr << "Build compile commands from "
                << actions.at("actions").as_array().size() << " actions"
                << std::endl;
    }
    const auto compile_actions = builder.build(actions);
    compile_commands_array.insert(std::end(compile_commands_array),
                                  std::begin(compile_actions),
                                  std::end(compile_actions));

    {
      if (options.verbose) {
        std::cerr << "Writting " << compile_commands_array.size()
                  << " commands to `compile_commands.json`" << std::endl;
      }
      auto compile_commands_json = std::ofstream("compile_commands.json");
      compile_commands_json << compile_commands_array;
    }
  } catch (std::exception const& ex) {
    std::cerr << "fatal error: " << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
