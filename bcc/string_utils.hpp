#pragma once

#include <string_view>

namespace bcc {

/// starts_with for pre C++20
inline bool
starts_with(std::string_view const& sv, std::string_view const& prefix)
{
  return std::string_view(sv.data(), std::min(sv.size(), prefix.size())) == prefix;
}

/// ends_with for pre C++20
inline bool
ends_with(std::string_view const& sv, std::string_view const& suffix)
{
  return sv.size() >= suffix.size() && sv.compare(sv.size() - suffix.size(), std::string_view::npos, suffix) == 0;
}

/// Check if a path has a C/C++/ObjC source file extension.
inline bool
is_cc_suffix(std::string_view const& v)
{
  return ends_with(v, ".C") || ends_with(v, ".c") || ends_with(v, ".cc") || ends_with(v, ".cxx") ||
         ends_with(v, ".c++") || ends_with(v, ".cpp") || ends_with(v, ".m") || ends_with(v, ".mm");
}

} // namespace bcc
