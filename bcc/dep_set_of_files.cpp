#include "bcc/dep_set_of_files.hpp"

#include <cassert>
#include <cstdint>
#include <queue>
#include <string_view>

namespace bcc {

dep_set_of_files::dep_set_of_files(google::protobuf::RepeatedPtrField<analysis::DepSetOfFiles> const& dep_set,
                                   artifacts a)
  : artifacts_(std::move(a))
{
  for (auto const& k : dep_set) {
    auto const e =
      entry{ std::vector<std::uint32_t>(k.transitive_dep_set_ids().begin(), k.transitive_dep_set_ids().end()),
             std::vector<std::uint32_t>(k.direct_artifact_ids().begin(), k.direct_artifact_ids().end()) };
    table_.insert({ k.id(), e });
  }
}

dep_set
dep_set_of_files::get(std::uint32_t id) const
{
  if (auto const it = cache_.find(id); it != cache_.end()) {
    return dep_set(it->second);
  }
  std::vector<std::string> result;
  std::set<std::uint32_t> visited_ids;

  std::queue<entry> q;

  auto const it = table_.find(id);

  for (auto const& k : it->second.direct) {
    result.push_back(artifacts_.path_of_artifact(k));
  }
  for (auto const& k : it->second.transitive) {
    if (!visited_ids.insert(k).second) {
      continue;
    }
    auto const j = table_.find(k);
    assert(j != table_.end());
    q.push(j->second);
  }

  while (!q.empty()) {
    auto const& front = q.front();
    for (auto const& k : front.direct) {
      result.push_back(artifacts_.path_of_artifact(k));
    }
    for (auto const& k : front.transitive) {
      if (!visited_ids.insert(k).second) {
        continue;
      }
      auto const j = table_.find(k);
      assert(j != table_.end());
      q.push(j->second);
    }

    q.pop();
  }

  auto const r = cache_.emplace(id, std::move(result));
  assert(r.second == true); ///< insertion must happen when reaching here

  return dep_set(r.first->second);
}

}
