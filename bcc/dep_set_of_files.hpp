#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <boost/core/span.hpp>

#include "external/io_bazel/src/main/protobuf/analysis_v2.pb.h"

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

private:
  std::unordered_map<std::uint32_t, entry> table_;

  mutable std::unordered_map<std::uint32_t, std::vector<std::string>> cache_;

  artifacts artifacts_;
};
}
