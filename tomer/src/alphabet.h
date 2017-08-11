#ifndef TOMER_ALPHABET_H_
#define TOMER_ALPHABET_H_

#include <memory>

#include "def.h"

namespace tomer {

  class Alphabet {
  public:
    using token = String;
    using type = size_t;
    using shared_ptr = std::shared_ptr<Alphabet>;

    Alphabet();
    Alphabet(const Map<token, type>& alphabet);
    Alphabet(const Alphabet& other) = default;;
    Alphabet(Alphabet&& other) = default;;

    ~Alphabet() = default;

    Alphabet& operator=(const Alphabet& rhs) = default;
    Alphabet& operator=(Alphabet&& rhs) = default;

    type add(const token& token);

    inline bool has(const type& type) const {
      return inv_alphabet_.find(type) != inv_alphabet_.cend();
    }

    inline bool has(const token& token) const {
      return alphabet_.find(token) != alphabet_.cend();
    }

    inline const token& at(const type& position) {
      return inv_alphabet_.at(position);
    }

    inline const type& at(const token& position) {
      return alphabet_.at(position);
    }

    inline size_t size() const {
      return alphabet_.size();
    }

  private:
    type next_type_;
    Map<token, type> alphabet_;
    Map<type, token> inv_alphabet_;

  };

} // namespace tomer

#endif // TOMER_ALPHABET_H_
