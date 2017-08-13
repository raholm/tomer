#ifndef TOMER_ALPHABET_H_
#define TOMER_ALPHABET_H_

#include <memory>

#include "def.h"

namespace tomer {

  class Alphabet {
  public:
    using Token = String;
    using Type = size_t;
    using SharedPtr = std::shared_ptr<Alphabet>;

    Alphabet();
    Alphabet(const Map<Token, Type>& alphabet);
    Alphabet(const Alphabet& other) = default;;
    Alphabet(Alphabet&& other) = default;;

    ~Alphabet() = default;

    Alphabet& operator=(const Alphabet& rhs) = default;
    Alphabet& operator=(Alphabet&& rhs) = default;

    Type add(const Token& token);

    inline bool has(const Type& type) const {
      return inv_alphabet_.find(type) != inv_alphabet_.cend();
    }

    inline bool has(const Token& token) const {
      return alphabet_.find(token) != alphabet_.cend();
    }

    inline const Token& at(const Type& position) {
      return inv_alphabet_.at(position);
    }

    inline const Type& at(const Token& position) {
      return alphabet_.at(position);
    }

    inline size_t size() const {
      return alphabet_.size();
    }

  private:
    Type next_type_;
    Map<Token, Type> alphabet_;
    Map<Type, Token> inv_alphabet_;

  };

} // namespace tomer

#endif // TOMER_ALPHABET_H_
