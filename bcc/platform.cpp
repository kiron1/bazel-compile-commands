#include "bcc/platform.hpp"

#include <optional>
#include <sstream>
#include <string_view>
#include <boost/process.hpp>

namespace bcc {

namespace {
/// Return the stdout content of a command.
std::optional<std::string>
output_of(std::string_view cmd, std::vector<std::string_view> args)
{
  try {
    boost::process::ipstream outs;
    boost::process::ipstream errs;

    auto cmd_path = boost::process::search_path(cmd.data());
    boost::process::child proc(
      cmd_path, boost::process::args(args), boost::process::std_out > outs, boost::process::std_err > errs);

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
  } catch (boost::process::process_error const& ex) {
    return std::nullopt;
  }
}
}

platform_error::platform_error(std::string const& what)
  : std::runtime_error(what)
{}

replacements
platform_replacements(std::string workspace, std::string execution_root)
{
  replacements result;

#if defined(__APPLE__)
  const auto devdir = output_of("xcode-select", { "--print-path" });
  const auto sdkroot = output_of("xcrun", { "--show-sdk-path" });

  // See
  // https://github.com/bazelbuild/bazel/blob/47edc57806056f3c8764241ed41b8acc72bd2ebf/tools/osx/crosstool/wrapped_clang.cc
  result.add({ "DEBUG_PREFIX_MAP_PWD=.", "-fdebug-prefix-map=" + execution_root + "=." });
  if (devdir.has_value()) {
    result.add({ "__BAZEL_XCODE_DEVELOPER_DIR__", devdir.value() });
  }
  if (devdir.has_value()) {
    result.add({ "__BAZEL_XCODE_SDKROOT__", sdkroot.value() });
  }
#endif

  return result;
}

}
