#ifndef TYPE_SEQUENCE_H
#define TYPE_SEQUENCE_H

#include <vector>

#include "alphabet.h"

class TypeSequenceBuilder;

class TypeSequence {
public:
  using Type = Alphabet::Type;
  using TypeVector = std::vector<Type>;
  using Token = Alphabet::Token;
  using AlphabetPtr = Alphabet::SPtr;
  using size_type = TypeVector::size_type;

  TypeSequence(const TypeSequence& other) = default;
  TypeSequence(TypeSequence&& other) = default;

  ~TypeSequence() = default;

  TypeSequence& operator=(const TypeSequence& rhs) = default;
  TypeSequence& operator=(TypeSequence&& rhs) = default;

  const Type& at(size_type position) const;
  const Token& token_at(size_type position) const;

  size_type size() const;
  size_type length() const;

private:
  TypeVector types_;
  AlphabetPtr alphabet_;

  TypeSequence(const TypeVector& types, AlphabetPtr alphabet);
  TypeSequence(TypeVector&& types, AlphabetPtr alphabet);

  friend class TypeSequenceBuilder;

};


#endif // TYPE_SEQUENCE_H
