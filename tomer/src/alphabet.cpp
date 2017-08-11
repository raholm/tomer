#include "alphabet.h"

namespace tomer {

  Alphabet::Alphabet()
    : next_type_{0},
      alphabet_{},
      inv_alphabet_{} {}

  Alphabet::Alphabet(const Map<Alphabet::token, Alphabet::type>& alphabet)
    : next_type_{0} {
    for (auto const& a : alphabet) {
      auto p = alphabet_.insert(a);
      if (p.second) {
        inv_alphabet_.insert(std::make_pair(a.second, a.first));
        next_type_ = std::max(next_type_, a.second) + 1;
      }
    }
  }

  Alphabet::Type Alphabet::add(const Alphabet::token& token) {
    auto p = alphabet_.insert(std::make_pair(token, next_type_));
    if (p.second) {
      inv_alphabet_.insert(std::make_pair(next_type_, token));
      ++next_type_;
    }
    return p.first->second;
  }

} // namespace tomer
