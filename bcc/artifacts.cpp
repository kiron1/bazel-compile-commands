#include "bcc/artifacts.hpp"

namespace bcc {

artifacts::artifacts(google::protobuf::RepeatedPtrField<analysis::Artifact> const& artifacts,
                     path_fragments const& fragments)
  : fragments_(fragments)
{
  for (auto const& k : artifacts) {
    artifacts_.insert({ k.id(), k.path_fragment_id() });
  }
}

std::string_view
artifacts::path_of_artifact(std::uint32_t artifact_id) const
{
  std::string_view result;
  if (const auto ait = artifacts_.find(artifact_id); ait != artifacts_.end()) {
    result = fragments_.build(ait->second);
  }
  return result;
}

}
