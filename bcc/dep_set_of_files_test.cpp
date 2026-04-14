
#include <cstdint>
#include <string_view>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bcc/dep_set_of_files.hpp"

using testing::Eq;
using testing::IsFalse;
using testing::IsTrue;

namespace {

bool
ends_with(std::string_view const& sv, std::string_view const& suffix)
{
  return sv.size() >= suffix.size() && sv.compare(sv.size() - suffix.size(), std::string_view::npos, suffix) == 0;
}

bool
is_cc_source(std::string_view const& v)
{
  return ends_with(v, ".cpp") || ends_with(v, ".cc") || ends_with(v, ".c");
}

/// Helper to build protobuf objects for a dep set tree.
///
/// Constructs a tree with the following structure:
///
///   dep_set 1 (root)
///     direct: artifact 10 -> "include/header.h"
///     transitive: [dep_set 2]
///
///   dep_set 2
///     direct: artifact 20 -> "src/main.cpp"
///             artifact 21 -> "src/util.h"
///     transitive: [dep_set 3]
///
///   dep_set 3
///     direct: artifact 30 -> "lib/helper.cc"
///             artifact 31 -> "lib/helper.h"
///     transitive: []
///
struct test_fixture
{
  google::protobuf::RepeatedPtrField<analysis::PathFragment> path_fragments;
  google::protobuf::RepeatedPtrField<analysis::Artifact> artifacts;
  google::protobuf::RepeatedPtrField<analysis::DepSetOfFiles> dep_sets;

  test_fixture()
  {
    add_path_fragment(1, "include", 0);
    add_path_fragment(2, "header.h", 1);
    add_path_fragment(3, "src", 0);
    add_path_fragment(4, "main.cpp", 3);
    add_path_fragment(5, "util.h", 3);
    add_path_fragment(6, "lib", 0);
    add_path_fragment(7, "helper.cc", 6);
    add_path_fragment(8, "helper.h", 6);

    add_artifact(10, 2); // artifact 10 -> "include/header.h"
    add_artifact(20, 4); // artifact 20 -> "src/main.cpp"
    add_artifact(21, 5); // artifact 21 -> "src/util.h"
    add_artifact(30, 7); // artifact 30 -> "lib/helper.cc"
    add_artifact(31, 8); // artifact 31 -> "lib/helper.h"

    add_dep_set(3, { 30, 31 }, {});
    add_dep_set(2, { 20, 21 }, { 3 });
    add_dep_set(1, { 10 }, { 2 });
  }

  void
  add_path_fragment(std::uint32_t id, std::string const& label, std::uint32_t parent)
  {
    auto* pf = path_fragments.Add();
    pf->set_id(id);
    pf->set_label(label);
    if (parent != 0) {
      pf->set_parent_id(parent);
    }
  }

  void
  add_artifact(std::uint32_t id, std::uint32_t path_fragment_id)
  {
    auto* a = artifacts.Add();
    a->set_id(id);
    a->set_path_fragment_id(path_fragment_id);
  }

  void
  add_dep_set(std::uint32_t id,
              std::vector<std::uint32_t> const& direct,
              std::vector<std::uint32_t> const& transitive)
  {
    auto* ds = dep_sets.Add();
    ds->set_id(id);
    for (auto const d : direct) {
      ds->add_direct_artifact_ids(d);
    }
    for (auto const t : transitive) {
      ds->add_transitive_dep_set_ids(t);
    }
  }

  bcc::dep_set_of_files
  build() const
  {
    auto frags = bcc::path_fragments(path_fragments);
    auto arts = bcc::artifacts(artifacts, frags);
    return bcc::dep_set_of_files(dep_sets, std::move(arts));
  }
};

TEST(dep_set_of_files, find_first_matching_finds_source_in_transitive)
{

  auto fixture = test_fixture();
  auto dsf = fixture.build();

  // Root dep set 1: direct=[header.h], transitive=[dep_set 2 which has main.cpp]
  auto const result = dsf.find_first_matching(1, is_cc_source);

  ASSERT_THAT(result.has_value(), IsTrue());
  EXPECT_THAT(result.value(), Eq("src/main.cpp"));
}

TEST(dep_set_of_files, find_first_matching_finds_direct_source)
{

  auto fixture = test_fixture();
  auto dsf = fixture.build();

  // Dep set 2: direct=[main.cpp, util.h], main.cpp should be found directly
  auto const result = dsf.find_first_matching(2, is_cc_source);

  ASSERT_THAT(result.has_value(), IsTrue());
  EXPECT_THAT(result.value(), Eq("src/main.cpp"));
}

TEST(dep_set_of_files, find_first_matching_returns_nullopt_when_no_match)
{

  auto fixture = test_fixture();
  auto dsf = fixture.build();

  auto const result = dsf.find_first_matching(1, [](std::string_view const& v) { return ends_with(v, ".py"); });

  EXPECT_THAT(result.has_value(), IsFalse());
}

TEST(dep_set_of_files, find_first_matching_returns_nullopt_for_unknown_id)
{

  auto fixture = test_fixture();
  auto dsf = fixture.build();

  auto const result = dsf.find_first_matching(999, is_cc_source);

  EXPECT_THAT(result.has_value(), IsFalse());
}

TEST(dep_set_of_files, find_first_matching_uses_cache_from_get)
{

  auto fixture = test_fixture();
  auto dsf = fixture.build();

  // Call get() first to populate the cache for dep set 1
  auto const all = dsf.get(1);
  ASSERT_THAT(all.all().empty(), IsFalse());

  // find_first_matching should now use the cached path
  auto const result = dsf.find_first_matching(1, is_cc_source);

  ASSERT_THAT(result.has_value(), IsTrue());
  EXPECT_THAT(result.value(), Eq("src/main.cpp"));

  // Verify it matches what get() + find_if() would return
  auto const expected = all.find_if(is_cc_source);
  ASSERT_THAT(expected.has_value(), IsTrue());
  EXPECT_THAT(result.value(), Eq(expected.value()));
}

TEST(dep_set_of_files, find_first_matching_agrees_with_get_find_if)
{

  auto fixture = test_fixture();

  for (std::uint32_t const id : { 1u, 2u, 3u }) {
    auto dsf = fixture.build();
    auto const via_get = dsf.get(id).find_if(is_cc_source);

    auto dsf2 = fixture.build();
    auto const via_find = dsf2.find_first_matching(id, is_cc_source);

    EXPECT_THAT(via_find.has_value(), Eq(via_get.has_value())) << "dep set id=" << id;
    if (via_find.has_value() && via_get.has_value()) {
      EXPECT_THAT(via_find.value(), Eq(via_get.value())) << "dep set id=" << id;
    }
  }
}

} // namespace
