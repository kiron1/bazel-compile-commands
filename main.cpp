
#include "bcc/bazel.hpp"
#include "bcc/compile_commands.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

/* #include <boost/format.hpp> */

int main(int argc, char **argv) {
  try {
    auto bazel = bcc::bazel::create();
    // std::cout << "bazel_command: " << bazel.command_path() << '\n';
    // std::cout << "workspace: " << bazel.workspace_path() << '\n';
    // std::cout << "execution_root: " << bazel.execution_root() << '\n';

    auto actions = bazel.aquery({"mnemonic('(Objc|Cpp)Compile',deps(//...))"});

    auto compile_commands_json = std::ofstream("compile_commands.json");
    auto compile_commands =
        bcc::compile_commands(actions, bazel.execution_root());
    compile_commands_json << compile_commands;
  } catch (std::exception const &ex) {
    std::cerr << "fatal error: " << ex.what() << '\n';
    return 1;
  }
  return 0;
}
