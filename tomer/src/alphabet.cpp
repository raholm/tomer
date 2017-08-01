#include "alphabet.h"

Alphabet::Alphabet()
  : next_type_{0}, alphabet_{}, inv_alphabet_{}
{

}

Alphabet::Alphabet(const std::map<Alphabet::Token, Alphabet::Type>& alphabet)
  : next_type_{0}
{
  for (auto const& a : alphabet) {
    auto p = alphabet_.insert(a);
    if (p.second) {
      inv_alphabet_.insert(std::make_pair(a.second, a.first));
      next_type_ = std::max(next_type_, a.second) + 1;
    }
  }
}

Alphabet::Type Alphabet::add(const Alphabet::Token& token) {
  auto p = alphabet_.insert(std::make_pair(token, next_type_));
  if (p.second) {
    inv_alphabet_.insert(std::make_pair(next_type_, token));
    ++next_type_;
  }
  return p.first->second;
}

bool Alphabet::has(const Alphabet::Type& type) const {
  return inv_alphabet_.find(type) != inv_alphabet_.cend();
}

bool Alphabet::has(const Alphabet::Token& token) const {
  return alphabet_.find(token) != alphabet_.cend();
}

const Alphabet::Token& Alphabet::at(const Alphabet::Type& position) {
  return inv_alphabet_.at(position);
}
const Alphabet::Type& Alphabet::at(const Alphabet::Token& position) {
  return alphabet_.at(position);
}

Alphabet::size_type Alphabet::size() const {
  return alphabet_.size();
}
