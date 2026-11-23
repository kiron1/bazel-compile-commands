#pragma once

#include <cstdint>
#include <optional>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "bcc/detail/analysis_v2.hpp"

#include "bcc/artifacts.hpp"

namespace bcc {

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

  /// Search for the first artifact matching a predicate via BFS traversal,
  /// returning immediately when found. This avoids materializing all artifact
  /// paths when only one match is needed (e.g., finding the source file).
  template<typename Predicate>
  std::optional<std::string> find_first_matching(std::uint32_t id, Predicate pred) const
  {
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

  artifacts artifacts_;
};
}
