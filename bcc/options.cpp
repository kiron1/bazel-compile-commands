#include "bcc/options.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <boost/filesystem.hpp>
#include <boost/process/search_path.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace bcc {
namespace {

std::optional<fs::path>
find_bazelccrc()
{
  auto dir = fs::current_path();
  while (!dir.empty()) {
    const auto rcpath = dir / rc_name;
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

  if (vm.count("compiler")) {
    result.compiler = vm["compiler"].as<std::string>();
  }

  if (vm.count("replace")) {
    const auto rdef = vm["replace"].as<std::vector<std::string>>();
    for (const auto& r : rdef) {
      const auto pos = r.find('=');
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
    result.bazel_command = boost::filesystem::canonical(result.bazel_command);
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
  for (const auto& r : this->replace) {
    os << "replace = " << r.first << "=" << r.second << "\n";
  }
  for (const auto& c : this->configs) {
    os << "config = " << c << "\n";
  }
  for (const auto& t : this->targets) {
    os << "targets = " << t << "\n";
  }
  return os;
}

} // namespace bcc
