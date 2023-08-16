#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <boost/json.hpp>

#include "external/io_bazel/src/main/protobuf/analysis_v2.pb.h"

namespace bcc {

class bazel_error : public std::runtime_error
{
public:
  bazel_error(std::filesystem::path const& path, std::vector<std::string> args, int rc, std::string error);
};

class workspace_error : public std::logic_error
{
public:
  workspace_error();
};

class proto_error : public std::runtime_error
{
public:
  proto_error();
  proto_error(std::string const& what);
};

class bazel
{
public:
  static bazel create(std::filesystem::path const& bazel_path, std::vector<std::string> bazel_startup_options);

  // Path of the `bazel` executable.
  std::filesystem::path command_path() const { return bazel_command_; };

  // Return the path to the current workspace
  std::filesystem::path workspace_path() const { return workspace_path_; };

  // Return the path to the execution root of bazel
  std::filesystem::path execution_root() const { return execution_root_; };

  // Execute an `aquery` on the current workspace.
  analysis::ActionGraphContainer aquery(std::string const& query,
                                        std::vector<std::string> const& bazel_flags,
                                        std::vector<std::string> const& configs) const;

private:
  bazel() = delete;
  bazel(std::filesystem::path bazel_command,
        std::vector<std::string> bazel_startup_options,
        std::filesystem::path workspace,
        std::filesystem::path execution_root);

private:
  std::filesystem::path bazel_command_;
  std::vector<std::string> bazel_startup_options_;
  std::filesystem::path workspace_path_;
  std::filesystem::path execution_root_;
};
} // namespace bcc
