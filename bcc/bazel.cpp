

#include "bcc/bazel.hpp"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>

#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/stubs/common.h>

namespace bcc {

namespace {
/// Run `bazel info $location` and return it as path
boost::filesystem::path
bazel_info(boost::filesystem::path const& bazel_path,
           std::vector<std::string> const& bazel_startup_options,
           std::string const& location)
{
  std::vector<std::string> args(std::begin(bazel_startup_options), std::end(bazel_startup_options));
  args.push_back("info");
  args.push_back(location);
  boost::process::ipstream outs;
  boost::process::ipstream errs;
  boost::process::child bazel_proc(
    bazel_path, boost::process::args(args), boost::process::std_out > outs, boost::process::std_err > errs);

  auto line = std::string{};
  std::getline(outs, line);
  bazel_proc.wait();
  const auto rc = bazel_proc.exit_code();
  if (rc != 0) {
    std::ostringstream oss;
    oss << errs.rdbuf();
    throw bazel_error(bazel_path, args, rc, oss.str());
  }
  return boost::filesystem::path(line);
}

std::string
make_bazel_error_message(boost::filesystem::path const& path, std::vector<std::string> args, int rc, std::string error)
{
  std::stringstream msg;
  msg << "bazel command failed with exit code " << rc << ": " << path.native() << " ";
  std::copy(std::begin(args), std::end(args), std::ostream_iterator<std::string>(msg, " "));
  msg << ": " << error;
  return msg.str();
}
}

bazel_error::bazel_error(boost::filesystem::path const& path, std::vector<std::string> args, int rc, std::string error)
  : std::runtime_error(make_bazel_error_message(path, args, rc, error))
{
}

workspace_error::workspace_error()
  : std::logic_error("workspace is invalid")
{
}
proto_error::proto_error()
  : std::runtime_error("JSON document is invalid")
{
}
proto_error::proto_error(std::string const& what)
  : std::runtime_error(what)
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

analysis::ActionGraphContainer
bazel::aquery(std::string const& query,
              std::vector<std::string> const& bazel_flags,
              std::vector<std::string> const& configs) const
{
  std::vector<std::string> args(std::begin(bazel_startup_options_), std::end(bazel_startup_options_));
  args.push_back("aquery");
  args.push_back("--output=proto");
  args.push_back("--ui_event_filters=-info");
  args.push_back("--noshow_progress");
  std::copy(std::begin(bazel_flags), std::end(bazel_flags), std::back_inserter(args));
  std::transform(std::begin(configs), std::end(configs), std::back_inserter(args), [&](auto c) {
    return std::string("--config=") + c;
  });
  args.push_back(query);

  boost::process::ipstream outs;
  boost::process::child bazel_proc(bazel_command_, boost::process::args(args), boost::process::std_out > outs);

  analysis::ActionGraphContainer agc;
  if (!agc.ParseFromIstream(&outs)) {
    throw proto_error("failed to parse aquery output");
  }
  bazel_proc.wait();
  const auto rc = bazel_proc.exit_code();
  if (rc != 0) {
    throw bazel_error(bazel_command_, args, rc, std::string());
  }
  return agc;
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
