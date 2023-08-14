#pragma once

#include <cstdint>
#include <filesystem>
#include <string_view>
#include <unordered_map>

#include "external/io_bazel/src/main/protobuf/analysis_v2.pb.h"

namespace bcc {
///  Path fragment table to build full path from workspace.
class path_fragments
{
public:
  struct entry
  {
    std::string label;    ///< one fragment of the path
    std::uint32_t parent; ///< 0 is root
  };

  explicit path_fragments(google::protobuf::RepeatedPtrField<analysis::PathFragment> const& fragments);

  std::string_view build(std::uint32_t id) const;

private:
  std::unordered_map<std::uint32_t, entry> fragments_;
  mutable std::unordered_map<std::uint32_t, std::filesystem::path> cache_;
};
}
