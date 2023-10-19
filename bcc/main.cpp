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

class file_error : public std::runtime_error
{
public:
  file_error(std::string_view what, boost::filesystem::path const& path)
    : std::runtime_error([&]() {
      std::stringstream msg;
      msg << "failed to " << what << ": " << path.native();
      return msg.str();
    }())
  {
  }
};

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

    auto const bazel = bcc::bazel::create(options.bazel_command, options.bazel_startup_options);

    if (options.write_rc_file) {
      if (!options.rcpath.has_value()) {
        options.rcpath = bazel.workspace_path() / bcc::rc_name;
      }
      auto rcfile = std::ofstream(options.rcpath.value().c_str());
      if (!rcfile) {
        throw file_error("open", options.rcpath.value());
      }
      options.write(rcfile);
      if (!rcfile) {
        throw file_error("write", options.rcpath.value());
      }
    }

    options.output_path = replace_workspace_placeholder(options.output_path, bazel.workspace_path().native());
    if (options.verbose) {
      options.write(std::cerr);
    }

    auto replacements = bcc::platform_replacements(bazel.workspace_path().native(), bazel.execution_root().native());
    if (options.verbose) {
      for (auto const& def : replacements.definitions()) {
        std::cerr << def.first << "=" << def.second << std::endl;
      }
    }
    replacements.add_all(options.replace);

    auto builder = bcc::compile_commands_builder();
    builder.arguments(options.arguments)
      .replacements(replacements)
      .execution_root(bazel.execution_root())
      .compiler(options.compiler);

    auto const query_str = make_query(options.targets);
    if (options.verbose) {
      std::cerr << "Query `" << query_str << '`' << std::endl;
    }
    auto const agc = bazel.aquery(query_str, options.bazel_flags, options.configs);
    auto const actions = agc.actions();

    if (options.verbose) {
      std::cerr << "Build compile commands from " << agc.actions().size() << " actions" << std::endl;
    }

    auto const compile_commands = builder.build(agc);
    {
      if (options.verbose) {
        std::cerr << "Writing " << compile_commands.size() << " commands to `" << options.output_path << "`"
                  << std::endl;
      }
      // Allow `-` as output_path to mean write to stdout instead of a file.
      auto compile_commands_file = std::ofstream{};
      auto* compile_commands_buf = static_cast<std::streambuf*>(nullptr);
      if (options.output_path == "-") {
        compile_commands_buf = std::cout.rdbuf();
      } else {
        auto const output_path = boost::filesystem::path(options.output_path);
        boost::filesystem::create_directories(output_path.parent_path());
        compile_commands_file.open(output_path.c_str());
        if (!compile_commands_file) {
          throw file_error("open", output_path);
        }
        compile_commands_buf = compile_commands_file.rdbuf();
      }

      auto compile_commands_stream = std::ostream(compile_commands_buf);
      compile_commands_stream << compile_commands;
      if (!compile_commands_stream) {
        throw file_error("write", options.output_path);
      }
    }
  } catch (std::exception const& ex) {
    std::cerr << "fatal error: " << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
