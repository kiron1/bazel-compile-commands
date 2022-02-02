#include "bcc/options.hpp"

#include <cstdlib>
#include <iostream>
#include <optional>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace bcc {
namespace {

std::optional<fs::path>
find_bazelccrc()
{
  const auto rcname = fs::path(".bazelccrc");
  auto dir = fs::current_path();
  while (!dir.empty()) {
    const auto rcpath = dir / rcname;
    if (fs::exists(rcpath)) {
      return rcpath;
    }
    dir = dir.parent_path();
  }
  return std::nullopt;
}
}

const char* const rc_name = ".bazelccrc";

options
options::from_argv(int argc, char* argv[])
{
  options result;
  result.rcpath = find_bazelccrc();

  // Declare the supported options.
  po::options_description cfg;
  // clang-format off
  cfg.add_options()
    ("help,h", "produce help message")
    ("verbose,v", po::bool_switch(&result.verbose), "verbose, report more information")
    ("arguments,a", po::bool_switch(&result.arguments), "include `arguments` array in output")
    ("bazel-command,B", po::value(&result.bazel_command), "bazel command")
    ("bazelsupopt,s", po::value(&result.bazel_startup_options), "bazel startup options")
    ("bazelopt,s", po::value(&result.bazel_flags), "bazel options")
    ("compiler,c", po::value<std::string>(), "use `compiler` as replacement for the bazel compiler wrapper script")
    ("output,o", po::value<std::string>(&result.output_path), "output path for the `compile_commands.json` file")
    ("targets", po::value<std::vector<std::string>>(), "Bazel target labels to query for compile commands")
    ;
  // clang-format on

  po::options_description desc("Usage: bazle-compile-commands [-hvcCBbsw] TARGETS");
  desc.add(cfg);
  // clang-format off
  desc.add_options()
    ("write,w", po::bool_switch(&result.write_rc_file), "Write current supplied settings to a project specific config file")
    ;
  // clang-format on

  // All remaining arguments are considerd as bazel target labels.
  po::positional_options_description targets;
  targets.add("targets", -1);

  po::variables_map vm;
  // parse command line variables
  po::store(po::command_line_parser(argc, argv).options(desc).positional(targets).run(), vm);
  // parse rc file options when found
  if (result.rcpath.has_value()) {
    auto ifs = std::ifstream(result.rcpath.value().c_str());
    po::store(po::parse_config_file(ifs, cfg), vm);
  }
  // done with command line parsing
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

std::ostream&
options::write(std::ostream& os) const
{
  os << "verbose = " << this->verbose << "\n";
  os << "arguments = " << this->arguments << "\n";
  os << "bazel-command = " << this->bazel_command << "\n";
  for (const auto& opt : bazel_startup_options) {
    os << "bazelsupopt = " << opt << "\n";
  }
  for (const auto& opt : bazel_flags) {
    os << "bazelopt = " << opt << "\n";
  }
  if (compiler.has_value()) {
    os << "compiler = " << this->compiler.value() << "\n";
  }
  os << "output = " << this->output_path << "\n";
  for (const auto& t : this->targets) {
    os << "targets = " << t << "\n";
  }
  return os;
}

} // namespace bcc
