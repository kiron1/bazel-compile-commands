#include "bcc/options.hpp"
#include "bcc/version.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>

#include <boost/process/search_path.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace bcc {
namespace {

std::optional<std::filesystem::path>
find_bazelccrc()
{
  auto dir = std::filesystem::current_path();
  auto root = dir.root_path();

  while (!dir.empty() || dir == root) {
    auto const rcpath = dir / rc_name;
    if (std::filesystem::exists(rcpath)) {

      return rcpath;
    }
    auto const parent_dir = dir.parent_path();
    if (parent_dir == dir) {
      break;
    }
    dir = parent_dir;
  }
  return std::nullopt;
}
}

char const* const rc_name = ".bazelccrc";

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
    ("version,V", "print version")
    ("verbose,v", po::bool_switch(&result.verbose), "verbose, report more information")
    ("arguments,a", "provide `arguments` array in output (enabled by default)")
    ("command", po::bool_switch(&result.command), "provide `command` string in output")
    ("resolve", po::bool_switch(&result.resolve), "resolve file symlinks when their target is inside the workspace")
    ("bazel-command,B", po::value(&result.bazel_command)->value_name("PATH"), "bazel command")
    ("bazelsupopt,s", po::value(&result.bazel_startup_options)->value_name("OPTION"), "bazel startup options")
    ("bazelopt,b", po::value(&result.bazel_flags)->value_name("OPTION"), "bazel options")
    ("compiler,c", po::value<std::string>()->value_name("PATH"), "use `compiler` as replacement for the bazel compiler wrapper script")
    ("config", po::value(&result.configs)->value_name("NAME"), "Bazel build config to apply")
    ("output,o", po::value(&result.output_path)->value_name("PATH"), "output path for the `compile_commands.json` file")
    ("replace,R", po::value<std::vector<std::string>>()->value_name("KEY=VALUE"), "Replace KEY with VALUE of each compile argument")
    ("targets", po::value<std::vector<std::string>>()->value_name("LABEL"), "Bazel target labels to query for compile commands")
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
    if (ifs) {
      po::store(po::parse_config_file(ifs, cfg), vm);
    }
  }
  // done with command line parsing
  po::notify(vm);

  if (vm.count("help")) {
    std::cerr << desc << '\n';
    std::exit(0);
  }

  if (vm.count("version")) {
    std::cerr << "bazel-compile-commands: " << BCC_VERSION << '\n';
    std::exit(0);
  }

  if (vm.count("compiler")) {
    result.compiler = vm["compiler"].as<std::string>();
  }

  if (vm.count("replace")) {
    auto const rdef = vm["replace"].as<std::vector<std::string>>();
    for (auto const& r : rdef) {
      auto const pos = r.find('=');
      if (pos == 0) {
        // ignore empty keys
      } else if (pos == std::string::npos) {
        // no '=' found
        result.replace.push_back({ r, "" });
      } else {
        result.replace.push_back({ r.substr(0, pos), r.substr(pos + 1) });
      }
    }
  }

  if (vm.count("targets")) {
    result.targets = vm["targets"].as<std::vector<std::string>>();
  }

  if (result.bazel_command.empty()) {
    result.bazel_command = boost::process::search_path("bazelisk");
    if (result.bazel_command.empty()) {
      result.bazel_command = boost::process::search_path("bazel");
    }
  } else {
    if (!result.bazel_command.is_absolute()) {
      result.bazel_command = boost::process::search_path(result.bazel_command);
    }
  }
  if (!result.bazel_command.empty()) {
    result.bazel_command = std::filesystem::canonical(result.bazel_command);
  }
  return result;
}

std::ostream&
options::write(std::ostream& os) const
{
  os << "verbose = " << this->verbose << "\n";
  os << "command = " << this->command << "\n";
  os << "resolve = " << this->resolve << "\n";
  os << "bazel-command = " << this->bazel_command << "\n";
  for (auto const& opt : bazel_startup_options) {
    os << "bazelsupopt = " << opt << "\n";
  }
  for (auto const& opt : bazel_flags) {
    os << "bazelopt = " << opt << "\n";
  }
  if (compiler.has_value()) {
    os << "compiler = " << this->compiler.value() << "\n";
  }
  os << "output = " << this->output_path << "\n";
  for (auto const& r : this->replace) {
    os << "replace = " << r.first << "=" << r.second << "\n";
  }
  for (auto const& c : this->configs) {
    os << "config = " << c << "\n";
  }
  for (auto const& t : this->targets) {
    os << "targets = " << t << "\n";
  }
  return os;
}

} // namespace bcc
