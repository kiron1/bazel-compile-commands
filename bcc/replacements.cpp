#include "bcc/replacements.hpp"

namespace bcc {
void
replacements::add(replacements::value_type value)
{
  definitions_.emplace_back(std::move(value));
}

void
replacements::add_all(std::vector<value_type> value)
{
  std::move(value.begin(), value.end(), std::back_inserter(definitions_));
}

std::string
replacements::apply(std::string input) const
{
  for (auto const& def : definitions_) {
    auto pos = std::size_t{ 0 };
    auto fpos = std::size_t{ 0 };
    while ((fpos = input.find(def.first, pos)) != std::string::npos) {
      input.replace(fpos, def.first.size(), def.second);
      // advance search position to find the next occourence
      pos += def.second.size();
    }
  }
  return input;
}
}
