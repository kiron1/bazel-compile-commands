#include "bcc/dep_set_of_files.hpp"

#include <cstdint>

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

}
