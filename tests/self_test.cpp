#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
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
boost::json::value
run(boost::filesystem::path const& commmand, std::vector<std::string_view> args)
{
  auto outs = boost::process::ipstream{};
  auto errs = boost::process::ipstream{};
  auto proc = boost::process::child(commmand,
                                    boost::process::args(args),
                                    boost::process::std_out > outs //,
  );                                                               // boost::process::std_err > errs);

  auto json_parser = boost::json::stream_parser{};
  json_parser.reset();
  auto line = std::string{};

  while (std::getline(outs, line)) {
    auto ec = boost::json::error_code{};
    json_parser.write(line, ec);
    if (ec) {
      throw std::runtime_error("invalid JSON");
    }
  }
  proc.wait();
  auto const rc = proc.exit_code();
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
  ASSERT_THAT(boost::filesystem::exists(bcc_path), IsTrue()) << bcc_path;

  auto const bazel_path = runfiles->Rlocation("bazel-compile-commands/tests/bazel-mock");
  ASSERT_THAT(bazel_path, Not(IsEmpty())) << bazel_path;
  ASSERT_THAT(boost::filesystem::exists(bazel_path), IsTrue()) << bazel_path;

  auto const result = run(bcc_path, { "-B", bazel_path.c_str(), "-a", "-o-" });

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
    EXPECT_THAT(cu_obj.find("command"), Not(Eq(end)));
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
