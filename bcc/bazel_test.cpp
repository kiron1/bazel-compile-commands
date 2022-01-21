#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bcc/bazel.hpp"

using testing::IsEmpty;
using testing::Not;

namespace {

TEST(bazel, create) {

  auto bzl = bcc::bazel::create();

  EXPECT_THAT(bzl.command_path(), Not(IsEmpty()));
}

} // namespace
