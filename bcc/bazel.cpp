

#include "bcc/bazel.hpp"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>

#include <boost/filesystem.hpp>
#include <boost/process.hpp>

namespace bcc {
namespace {

/// Run `bazel info $location` and return it as path
boost::filesystem::path
bazel_info(boost::filesystem::path const& bazel_commmand, std::string_view location)
{
  const auto info = std::string_view("info");
  boost::process::ipstream outs;
  boost::process::ipstream errs;
  boost::process::child bazel_proc(
    bazel_commmand, info.data(), location.data(), boost::process::std_out > outs, boost::process::std_err > errs);

  auto line = std::string{};
  std::getline(outs, line);
  bazel_proc.wait();
  const auto rc = bazel_proc.exit_code();
  if (rc != 0) {
    std::ostringstream oss;
    oss << errs.rdbuf();
    throw bazel_error(oss.str());
  }
  return boost::filesystem::path(line);
}
} // namespace

bazel_error::bazel_error(std::string const& what)
  : std::runtime_error(what)
{
}
workspace_error::workspace_error()
  : std::logic_error("workspace is invalid")
{
}
json_error::json_error()
  : std::runtime_error("JSON document is invalid")
{
}

bazel
bazel::create(boost::filesystem::path const& bazel_path, std::vector<std::string> bazel_startup_options)
{
  auto workspace = bazel_info(bazel_path, "workspace");
  auto execution_root = bazel_info(bazel_path, "execution_root");

  if (workspace.empty()) {
    throw workspace_error();
  }

  return bazel(bazel_path, std::move(bazel_startup_options), workspace, execution_root);
}

boost::json::value
bazel::aquery(std::string const& query,
              std::vector<std::string> const& bazel_flags,
              std::vector<std::string> const& configs) const
{
  std::vector<std::string> args(std::begin(bazel_startup_options_), std::end(bazel_startup_options_));
  args.push_back("aquery");
  args.push_back("--output=jsonproto");
  args.push_back("--ui_event_filters=-info");
  args.push_back("--noshow_progress");
  std::copy(std::begin(bazel_flags), std::end(bazel_flags), std::back_inserter(args));
  std::transform(std::begin(configs), std::end(configs), std::back_inserter(args), [&](auto c) {
    return std::string("--config=") + c;
  });
  args.push_back(query);

  boost::process::ipstream outs;
  boost::process::ipstream errs;
  boost::process::child bazel_proc(bazel_command_, boost::process::args(args), boost::process::std_out > outs);

  auto json_parser = boost::json::stream_parser{};
  json_parser.reset();
  auto line = std::string{};

  while (std::getline(outs, line)) {
    auto ec = boost::json::error_code{};
    json_parser.write(line, ec);
    if (ec) {
      throw json_error();
    }
  }
  bazel_proc.wait();
  const auto rc = bazel_proc.exit_code();
  if (rc != 0 || !json_parser.done()) {
    throw json_error();
  }

  return json_parser.release();
}

bazel::bazel(boost::filesystem::path bazel_commands,
             std::vector<std::string> bazel_startup_options,
             boost::filesystem::path workspace_path,
             boost::filesystem::path execution_root)
  : bazel_command_(std::move(bazel_commands))
  , bazel_startup_options_(std::move(bazel_startup_options))
  , workspace_path_(std::move(workspace_path))
  , execution_root_(std::move(execution_root))
{
}
} // namespace bcc
