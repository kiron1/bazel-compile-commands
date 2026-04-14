#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <queue>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <boost/core/span.hpp>

#include "bcc/detail/analysis_v2.hpp"

#include "bcc/artifacts.hpp"

namespace bcc {
/// One dependency set
class dep_set
{
  friend class dep_set_of_files;

public:
  template<typename Predicate>
  std::optional<std::string> find_if(Predicate pred) const
  {
    auto result = std::optional<std::string>{};

    auto const it = std::find_if(std::begin(set_), std::end(set_), pred);
    if (it != std::end(set_)) {
      result = *it;
    }
    return result;
  }

  std::vector<std::string> const& all() const { return set_; }

private:
  explicit dep_set(std::vector<std::string> set)
    : set_(std::move(set))
  {
  }
  std::vector<std::string> set_;
};

/// Dependency set of files for lookup.
class dep_set_of_files
{
  struct entry
  {
    std::vector<std::uint32_t> transitive;
    std::vector<std::uint32_t> direct;
  };

public:
  explicit dep_set_of_files(google::protobuf::RepeatedPtrField<analysis::DepSetOfFiles> const& dep_set, artifacts a);

  dep_set get(std::uint32_t id) const;

  /// Search for the first artifact matching a predicate via BFS traversal,
  /// returning immediately when found. This avoids materializing all artifact
  /// paths when only one match is needed (e.g., finding the source file).
  template<typename Predicate>
  std::optional<std::string> find_first_matching(std::uint32_t id, Predicate pred) const
  {
    // If the dep set was already fully materialized (cached), search the cache.
    if (auto const it = cache_.find(id); it != cache_.end()) {
      auto const found = std::find_if(it->second.begin(), it->second.end(), pred);
      if (found != it->second.end()) {
        return *found;
      }
      return std::nullopt;
    }

    // BFS traversal with early exit — resolve artifact paths on the fly
    // and return immediately when a match is found.
    auto const root_it = table_.find(id);
    if (root_it == table_.end()) {
      return std::nullopt;
    }

    // Check direct artifacts of the root entry
    for (auto const& k : root_it->second.direct) {
      auto const path = artifacts_.path_of_artifact(k);
      if (pred(path)) {
        return path;
      }
    }

    // BFS through transitive dep sets
    std::set<std::uint32_t> visited;
    std::queue<entry const*> q;
    for (auto const& k : root_it->second.transitive) {
      if (visited.insert(k).second) {
        auto const j = table_.find(k);
        if (j != table_.end()) {
          q.push(&j->second);
        }
      }
    }

    while (!q.empty()) {
      auto const* front = q.front();
      q.pop();
      for (auto const& k : front->direct) {
        auto const path = artifacts_.path_of_artifact(k);
        if (pred(path)) {
          return path;
        }
      }
      for (auto const& k : front->transitive) {
        if (visited.insert(k).second) {
          auto const j = table_.find(k);
          if (j != table_.end()) {
            q.push(&j->second);
          }
        }
      }
    }

    return std::nullopt;
  }

private:
  std::unordered_map<std::uint32_t, entry> table_;

  mutable std::unordered_map<std::uint32_t, std::vector<std::string>> cache_;

  artifacts artifacts_;
};
}
