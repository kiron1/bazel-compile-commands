#include "bcc/path_fragments.hpp"

#include <cassert>

namespace bcc {

path_fragments::path_fragments(google::protobuf::RepeatedPtrField<analysis::PathFragment> const& fragments)
{
  for (auto const& k : fragments) {
    const auto e = entry{ k.label(), k.parent_id() };
    fragments_.insert({ k.id(), e });
  }
}

std::string_view
path_fragments::build(std::uint32_t id) const
{
  if (const auto it = cache_.find(id); it != cache_.end()) {
    return it->second.native();
  }
  std::filesystem::path result;
  int path_fragment_id;

  // there must be at least on path fragment (i.e. no need to check for end)
  auto pfit = fragments_.find(id);
  result = pfit->second.label;
  path_fragment_id = pfit->second.parent;

  pfit = fragments_.find(path_fragment_id);
  for (; path_fragment_id != 0 && pfit != fragments_.end(); pfit = fragments_.find(path_fragment_id)) {
    result = pfit->second.label / result;
    path_fragment_id = pfit->second.parent;
  }

  const auto r = cache_.insert({ id, result });
  assert(r.second == true); ///< insertion must happen when reaching here

  return r.first->second.native();
}

}
