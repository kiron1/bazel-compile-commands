
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bcc/compile_commands.hpp"

using testing::IsTrue;

namespace {

TEST(compile_commands, simple)
{
  EXPECT_THAT(true, IsTrue());
}

} // namespace
