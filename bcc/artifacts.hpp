#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

#include "external/io_bazel/src/main/protobuf/analysis_v2.pb.h"

#include "bcc/path_fragments.hpp"

namespace bcc {
///  Artefacts for lookup.
class artifacts
{
public:
  artifacts(google::protobuf::RepeatedPtrField<analysis::Artifact> const& artifacts, path_fragments const& fragments);

  std::string_view path_of_artifact(std::uint32_t artifact_id) const;

private:
  std::unordered_map<std::uint32_t, std::uint32_t> artifacts_;
  path_fragments const& fragments_;
};
}
