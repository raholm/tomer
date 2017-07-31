#ifndef TYPE_SEQUENCE_H
#define TYPE_SEQUENCE_H

#include "def.h"

class TypeSequenceContainer;

class TypeSequence {
public:
  TypeSequence() = default;
  TypeSequence(const Document& tokens);

  ~TypeSequence() = default;

  TypeSequence(const TypeSequence& other) = default;
  TypeSequence(TypeSequence&& other) = default;

  TypeSequence& operator=(const TypeSequence& rhs) = default;
  TypeSequence& operator=(TypeSequence&& rhs) = default;

  Type at(std::size_t position) const;
  Token token_at(std::size_t position) const;

  uint size() const;
  uint length() const;

private:
  TypeVector types_;
  AlphabetPtr alphabet_;

  TypeSequence(const Document& tokens, AlphabetPtr alphabet);

  void create_types_and_update_alphabet(const Document& tokens);
  Type get_next_type() const;

  friend class TypeSequenceContainer;

};


#endif // TYPE_SEQUENCE_H
