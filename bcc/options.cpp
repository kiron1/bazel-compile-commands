#include "bcc/options.hpp"

#include <cstdlib>
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace bcc {
options
options::from_argv(int argc, char* argv[])
{
  auto* argv_end = argc + argv;
  int our_argc = argc;

  options result;

  const auto it = std::find(argv, argv_end, std::string_view("--"));
  if (it != argv_end) {
    our_argc = std::distance(argv, it);
    result.bazel_flags.assign(it + 1, argv_end);
  }

  // Declare the supported options.
  po::options_description desc("Usage: bazle-compile-commands [-hvcC] TARGETS -- CONFIG");
  // clang-format off
  desc.add_options()
    ("help,h", "produce help message")
    ("verbose,v", po::bool_switch(&result.verbose), "verbose, report more information")
    ("arguments,a", po::bool_switch(&result.arguments), "include `arguments` array in output")
    ("bazel-commands,B", po::value(&result.bazel_commands), "bazel command")
    ("bazelsupopt,s", po::value(&result.bazel_startup_options), "bazel startup options")
    ("compiler,c", po::value<std::string>(), "use `compiler` as replacement for the bazel compiler wrapper script")
    ("output,o", po::value<std::string>(&result.output_path), "output path for the `compile_commands.json` file")
    ("targets", po::value<std::vector<std::string>>(), "Bazel target labels to query for compile commands")
    ;
  // clang-format on

  // All remaining arguments are considerd as bazel target labels.
  po::positional_options_description targets;
  targets.add("targets", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(our_argc, argv).options(desc).positional(targets).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cerr << desc << '\n';
    std::exit(0);
  }

  if (vm.count("targets")) {
    result.targets = vm["targets"].as<std::vector<std::string>>();
  }
  return result;
}
} // namespace bcc
