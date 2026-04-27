#include "bcc/platform.hpp"

#include <optional>
#include <sstream>
#include <string_view>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/process.hpp>

namespace bcc {

namespace {
/// Return the stdout content of a command.
std::optional<std::string>
output_of(std::string_view cmd, std::vector<std::string_view> args)
{
  try {
    auto cmd_path = boost::process::environment::find_executable(cmd.data());
    if (cmd_path.empty()) {
      return std::nullopt;
    }

    boost::asio::io_context ctx;
    boost::asio::readable_pipe out_pipe(ctx);
    boost::asio::readable_pipe err_pipe(ctx);
    std::vector<std::string> str_args(args.begin(), args.end());
    boost::process::process proc(ctx, cmd_path, str_args, boost::process::process_stdio{ {}, out_pipe, err_pipe });

    std::string out_buf;
    boost::system::error_code read_ec;
    auto n = boost::asio::read(out_pipe, boost::asio::dynamic_buffer(out_buf), read_ec);
    (void)n;

    std::string err_buf;
    boost::asio::read(err_pipe, boost::asio::dynamic_buffer(err_buf), read_ec);

    auto const rc = proc.wait();
    if (rc != 0) {
      throw platform_error(err_buf);
    }

    // Return just the first line
    auto pos = out_buf.find('\n');
    if (pos != std::string::npos) {
      out_buf.resize(pos);
    }
    return out_buf;
  } catch (boost::system::system_error const& ex) {
    return std::nullopt;
  }
}
}

platform_error::platform_error(std::string const& what)
  : std::runtime_error(what)
{
}

replacements
platform_replacements(std::string execution_root)
{
  replacements result;

#if defined(__APPLE__)
  auto const devdir = output_of("xcode-select", { "--print-path" });
  auto const sdkroot = output_of("xcrun", { "--show-sdk-path" });

  // See
  // https://github.com/bazelbuild/bazel/blob/47edc57806056f3c8764241ed41b8acc72bd2ebf/tools/osx/crosstool/wrapped_clang.cc
  result.add({ "DEBUG_PREFIX_MAP_PWD=.", "-fdebug-prefix-map=" + execution_root + "=." });
  if (devdir.has_value()) {
    result.add({ "__BAZEL_XCODE_DEVELOPER_DIR__", devdir.value() });
  }
  if (devdir.has_value()) {
    result.add({ "__BAZEL_XCODE_SDKROOT__", sdkroot.value() });
  }
#else
  (void)execution_root;
#endif

  return result;
}

}
