#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/json.hpp>
#include <boost/process.hpp>

#include "tools/cpp/runfiles/runfiles.h"

using bazel::tools::cpp::runfiles::Runfiles;
using testing::Eq;
using testing::Ge;
using testing::IsEmpty;
using testing::IsNull;
using testing::IsTrue;
using testing::Not;
using testing::NotNull;

namespace {
std::string
exe_suffix()
{
#ifdef _WIN32
  return ".exe";
#else
  return "";
#endif
}

boost::json::value
run(std::filesystem::path const& commmand, std::vector<std::string_view> args)
{
  boost::asio::io_context ctx;
  boost::asio::readable_pipe out_pipe(ctx);
  std::vector<std::string> str_args(args.begin(), args.end());
  boost::process::process proc(ctx, commmand, str_args, boost::process::process_stdio{ {}, out_pipe, {} });

  std::string out_buf;
  boost::system::error_code read_ec;
  boost::asio::read(out_pipe, boost::asio::dynamic_buffer(out_buf), read_ec);

  auto json_parser = boost::json::stream_parser{};
  json_parser.reset();

  {
    auto ec = boost::system::error_code{};
    json_parser.write(out_buf, ec);
    if (ec) {
      throw std::runtime_error("invalid JSON");
    }
  }

  auto const rc = proc.wait();
  if (rc != 0 || !json_parser.done()) {
    throw std::runtime_error("JSON error");
  }

  return json_parser.release();
}

TEST(self_test, run)
{
  std::string error;
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest(&error));
  ASSERT_THAT(runfiles, NotNull()) << error;

  auto const bcc_path = runfiles->Rlocation("bazel-compile-commands/bcc/bazel-compile-commands");
  ASSERT_THAT(bcc_path, Not(IsEmpty())) << bcc_path;
  ASSERT_THAT(std::filesystem::exists(bcc_path), IsTrue()) << bcc_path;

  auto const bazel_path = runfiles->Rlocation("bazel-compile-commands/tests/bazel-mock") + exe_suffix();
  ASSERT_THAT(bazel_path, Not(IsEmpty())) << bazel_path;
  ASSERT_THAT(std::filesystem::exists(bazel_path), IsTrue()) << bazel_path;

  auto const result = run(bcc_path, { "-B", bazel_path.c_str(), "-o-" });

  ASSERT_THAT(result.is_array(), IsTrue());

  auto seen_files = std::map<std::string, bool>{};
  seen_files.insert({ std::string("main.cpp"), false });

  EXPECT_THAT(result.as_array().size(), Ge(seen_files.size()));

  for (auto const& cu : result.as_array()) {
    ASSERT_THAT(cu.is_object(), IsTrue());
    auto const cu_obj = cu.as_object();
    auto const end = cu_obj.end();

    ASSERT_THAT(cu_obj.find("file"), Not(Eq(end)));
    EXPECT_THAT(cu_obj.find("directory"), Not(Eq(end)));
    EXPECT_THAT(cu_obj.find("command"), Eq(end));
    EXPECT_THAT(cu_obj.find("arguments"), Not(Eq(end)));
    EXPECT_THAT(cu_obj.find("output"), Not(Eq(end)));

    auto const file = cu_obj.at("file").as_string();
    auto const iter = seen_files.find(file.c_str());
    if (iter != std::end(seen_files)) {
      iter->second = true;
    }
  }
  for (auto const& sf : seen_files) {
    EXPECT_THAT(sf.second, IsTrue()) << sf.first;
  }
}

} // namespace
