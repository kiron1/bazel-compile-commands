#pragma once

#include <cstdint>
#include <filesystem>
#include <unordered_map>

#include "bcc/detail/analysis_v2.hpp"

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

  std::string build(std::uint32_t id) const;

private:
  std::unordered_map<std::uint32_t, entry> fragments_;
  mutable std::unordered_map<std::uint32_t, std::filesystem::path> cache_;
};
}
