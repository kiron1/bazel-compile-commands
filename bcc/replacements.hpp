#pragma once

#include <string>
#include <utility>
#include <vector>

namespace bcc {
class replacements
{
public:
  /// `Every occurnce of `first` will be replaced with `second`.
  using value_type = std::pair<std::string, std::string>;
  /// Add a replacement definition.
  void add(value_type value);
  /// Returns a string with all replacements applied.
  std::string apply(std::string input) const;
  /// Return list of replacements.
  std::vector<value_type> const& definitions() const { return definitions_; }

private:
  std::vector<value_type> definitions_ = {};
};

}
