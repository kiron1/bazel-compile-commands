#include "bcc/version.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <boost/process.hpp>
#include <boost/program_options.hpp>

#ifndef _WIN32
#include <errno.h>
#include <string.h>
#include <unistd.h>
#endif

namespace fs = std::filesystem;
namespace bp = boost::process;
namespace po = boost::program_options;

bp::filesystem::path
get_absolute_executable_path(bp::filesystem::path const& path)
{
  if (path.is_absolute()) {
    return path;
  } else {
    return bp::search_path(path);
  }
}

std::optional<std::string>
get_bazel_execution_root(std::string const& bazel_path,
                         std::string const& workspace_dir,
                         std::vector<std::string> const& bazel_startup_options)
{
  bp::ipstream is;
  bp::child c(get_absolute_executable_path(bazel_path),
              bazel_startup_options,
              "info",
              "execution_root",
              bp::std_out > is,
              bp::start_dir(workspace_dir));
  c.wait();

  std::string result;
  std::string line;
  while (std::getline(is, line)) {
    result += line;
  }

  int exit_code = c.exit_code();
  if (exit_code != 0) {
    std::cerr << "Failed to run 'bazel info execution_root'. Exit code: " << exit_code << std::endl;
    return std::nullopt;
  }

  // Remove trailing newline
  if (!result.empty() && result.back() == '\n') {
    result.pop_back();
  }

  return result;
}

fs::path
get_workspace_dir(std::vector<std::string> const& args)
{
  for (std::string const& arg : args) {
    if (arg.rfind("--compile-commands-dir=", 0) == 0) {
      return arg.substr(strlen("--compile-commands-dir="));
    }
  }
  return fs::current_path();
}

int
main(int argc, char* argv[])
{
  po::options_description desc("Usage: bazel-clangd-wrapper OPTIONS [ -- CLANGD_OPTIONS ]");

  // clang-format off
  desc.add_options()
    ("help,h", "Produce help message")
    ("version,V", "print version")
    ("bazel-path", po::value<std::string>()->default_value("bazel"), "path to the bazel executable")
    ("clangd-path", po::value<std::string>()->default_value("clangd"), "path to the clangd executable")
    ("bazelsupopt,s", po::value<std::vector<std::string>>()->value_name("OPTION"), "bazel startup options");
  // clang-format on

  // Arguments before -- are passed to the wrapper, arguments after -- are passed to clangd.
  char** const split_iter = std::find(argv, argv + argc, std::string("--"));
  std::vector<char*> wrapper_args(argv, split_iter);
  std::vector<std::string> clangd_args;
  if (split_iter != argv + argc) {
    clangd_args.assign(split_iter + 1, argv + argc);
  }

  po::variables_map vm;
  po::positional_options_description p;

  try {
    po::store(po::command_line_parser(wrapper_args.size(), wrapper_args.data()).options(desc).positional(p).run(), vm);
    po::notify(vm);
  } catch (po::error const& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 0;
  }

  if (vm.count("version")) {
    std::cerr << "bazel-clangd-wrapper: " << BCC_VERSION << '\n';
    return 0;
  }

  std::string const bazel_path = vm["bazel-path"].as<std::string>();
  std::string const clangd_path = vm["clangd-path"].as<std::string>();
  std::vector<std::string> const bazel_startup_options =
    vm["bazelsupopt"].empty() ? std::vector<std::string>() : vm["bazelsupopt"].as<std::vector<std::string>>();

  fs::path const workspace_dir = get_workspace_dir(clangd_args);

  std::optional<std::string> const execution_root =
    get_bazel_execution_root(bazel_path, workspace_dir.string(), bazel_startup_options);
  if (execution_root) {
    clangd_args.push_back("--path-mappings=" + workspace_dir.string() + "=" + execution_root.value());
  }

#ifdef _WIN32
  try {
    bp::child clangd(get_absolute_executable_path(clangd_path), bp::args(clangd_args));
    clangd.wait();
    return clangd.exit_code();
  } catch (bp::process_error const& e) {
    std::cerr << "fatal error: couldn't launch clangd: " << e.what() << std::endl;
    return 1;
  }
#else
  std::vector<char*> clangd_argv;
  clangd_argv.push_back(const_cast<char*>(clangd_path.c_str()));
  for (auto const& arg : clangd_args) {
    clangd_argv.push_back(const_cast<char*>(arg.c_str()));
  }
  clangd_argv.push_back(nullptr);
  execvp(clangd_path.c_str(), clangd_argv.data());
  std::cerr << "fatal error: couldn't launch clangd: " << strerror(errno) << std::endl;
  return 1;
#endif
}
