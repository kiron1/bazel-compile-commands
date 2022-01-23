#pragma once

#include <stdexcept>
#include "bcc/replacements.hpp"

namespace bcc {
  /// Error occured during querying of the platform.
class platform_error : public std::runtime_error
{
public:
  platform_error(std::string const& what);
};

/// Returns platform specific replacements.
replacements
platform_replacements();
}
