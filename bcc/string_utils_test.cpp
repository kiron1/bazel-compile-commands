
#include <string_view>
#include <tuple>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bcc/string_utils.hpp"

using testing::Eq;

namespace {

struct string_match_case
{
  std::string_view input;
  std::string_view pattern;
  bool expected;
};

class starts_with_test : public testing::TestWithParam<string_match_case>
{
};

TEST_P(starts_with_test, matches)
{

  auto const& [input, pattern, expected] = GetParam();
  EXPECT_THAT(bcc::starts_with(input, pattern), Eq(expected));
}

INSTANTIATE_TEST_SUITE_P(string_utils,
                         starts_with_test,
                         testing::Values(string_match_case{ "hello world", "hello", true },
                                         string_match_case{ "hello", "hello", true },
                                         string_match_case{ "hello", "", true },
                                         string_match_case{ "", "", true },
                                         string_match_case{ "hello", "world", false },
                                         string_match_case{ "hi", "hello", false },
                                         string_match_case{ "", "a", false }));

class ends_with_test : public testing::TestWithParam<string_match_case>
{
};

TEST_P(ends_with_test, matches)
{

  auto const& [input, pattern, expected] = GetParam();
  EXPECT_THAT(bcc::ends_with(input, pattern), Eq(expected));
}

INSTANTIATE_TEST_SUITE_P(string_utils,
                         ends_with_test,
                         testing::Values(string_match_case{ "hello.cpp", ".cpp", true },
                                         string_match_case{ ".cpp", ".cpp", true },
                                         string_match_case{ "hello", "", true },
                                         string_match_case{ "", "", true },
                                         string_match_case{ "hello.cpp", ".hpp", false },
                                         string_match_case{ ".c", ".cpp", false },
                                         string_match_case{ "", ".c", false }));

struct suffix_case
{
  std::string_view path;
  bool expected;
};

class is_cc_suffix_test : public testing::TestWithParam<suffix_case>
{
};

TEST_P(is_cc_suffix_test, matches)
{

  auto const& [path, expected] = GetParam();
  EXPECT_THAT(bcc::is_cc_suffix(path), Eq(expected));
}

INSTANTIATE_TEST_SUITE_P(string_utils,
                         is_cc_suffix_test,
                         testing::Values(suffix_case{ "src/main.cpp", true },
                                         suffix_case{ "lib/helper.cc", true },
                                         suffix_case{ "lib/helper.c", true },
                                         suffix_case{ "lib/helper.C", true },
                                         suffix_case{ "lib/helper.cxx", true },
                                         suffix_case{ "lib/helper.c++", true },
                                         suffix_case{ "lib/helper.m", true },
                                         suffix_case{ "lib/helper.mm", true },
                                         suffix_case{ "include/header.h", false },
                                         suffix_case{ "include/header.hpp", false },
                                         suffix_case{ "script.py", false },
                                         suffix_case{ "Makefile", false }));

} // namespace
