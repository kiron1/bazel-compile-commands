#include "bcc/dep_set_of_files.hpp"

#include <cassert>
#include <queue>
#include <string_view>

namespace bcc {

dep_set_of_files::dep_set_of_files(google::protobuf::RepeatedPtrField<analysis::DepSetOfFiles> const& dep_set,
                                   artifacts const& a)
  : artifacts_(a)
{
  for (auto const& k : dep_set) {
    const auto e = entry{ k.transitive_dep_set_ids(), k.direct_artifact_ids() };
    table_.insert({ k.id(), e });
  }
}

dep_set
dep_set_of_files::get(std::uint32_t id) const
{
  if (const auto it = cache_.find(id); it != cache_.end()) {
    return dep_set(it->second);
  }
  std::vector<std::string_view> result;

  std::queue<entry> q;

  const auto it = table_.find(id);

  for (const auto& k : it->second.direct) {
    result.push_back(artifacts_.path_of_artifact(k));
  }
  for (const auto& k : it->second.transitive) {
    const auto j = table_.find(k);
    assert(j != table_.end());
    q.push(j->second);
  }

  while (!q.empty()) {
    const auto& front = q.front();
    for (const auto& k : front.direct) {
      result.push_back(artifacts_.path_of_artifact(k));
    }
    for (const auto& k : front.transitive) {
      const auto j = table_.find(k);
      assert(j != table_.end());
      q.push(j->second);
    }

    q.pop();
  }

  const auto r = cache_.emplace(id, std::move(result));
  assert(r.second == true); ///< insertion must happen when reaching here

  return dep_set(r.first->second);
}

}
