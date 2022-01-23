#include "bcc/platform.hpp"

#include <boost/process.hpp>
#include <sstream>
#include <string_view>

namespace bcc {

namespace {
/// Return the stdout content of a command.
std::string
output_of(std::string_view cmd, std::vector<std::string_view> args)
{
  boost::process::ipstream outs;
  boost::process::ipstream errs;

  auto cmd_path = boost::process::search_path(cmd.data());
  boost::process::child proc(cmd_path,
                             boost::process::args(args),
                             boost::process::std_out > outs,
                             boost::process::std_err > errs);

  auto line = std::string{};
  std::getline(outs, line);
  proc.wait();
  const auto rc = proc.exit_code();
  if (rc != 0) {
    std::ostringstream oss;
    oss << errs.rdbuf();
    throw platform_error(oss.str());
  }
  return line;
}
}

platform_error::platform_error(std::string const& what)
  : std::runtime_error(what)
{}

replacements
platform_replacements()
{
  const auto devdir = output_of("xcode-select", { "--print-path" });
  const auto sdkroot = output_of("xcrun", { "--show-sdk-path" });

  replacements result;
  result.add({ "DEBUG_PREFIX_MAP_PWD", "-fdebug-prefix-map" });
  result.add({ "__BAZEL_XCODE_DEVELOPER_DIR__", devdir });
  result.add({ "__BAZEL_XCODE_SDKROOT__", sdkroot });

  return result;
}

}
