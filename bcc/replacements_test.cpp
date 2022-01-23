
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bcc/replacements.hpp"

using testing::Eq;

namespace {

TEST(replacement, apply_empty)
{

  auto repl = bcc::replacements();
  auto r = repl.apply("TEST 123 abc");

  EXPECT_THAT(r, Eq("TEST 123 abc"));
}

TEST(replacement, apply_simple)
{

  auto repl = bcc::replacements();
  repl.add({ "TEST", "not a test" });
  auto r = repl.apply("TEST 123 abc");

  EXPECT_THAT(r, Eq("not a test 123 abc"));
}

TEST(replacement, apply_complex)
{

  auto repl = bcc::replacements();
  repl.add({ "TEST", "xxx" });
  repl.add({ "123", "one" });
  auto r = repl.apply("TEST TESTTEST test TEST 123 abc");

  EXPECT_THAT(r, Eq("xxx xxxxxx test xxx one abc"));
}

} // namespace
