#ifndef TYPE_SEQUENCE_H
#define TYPE_SEQUENCE_H

#include "def.h"

class TypeSequence {
public:
  TypeSequence(const Document& tokens);

  ~TypeSequence() = default;

  Type at(uint position) const;
  Token token_at(uint position) const;

  uint size() const;
  uint length() const;

private:
  TypeVector types_;
  AlphabetPtr alphabet_;

  TypeSequence(const Document& tokens, AlphabetPtr alphabet);

  void create_types_and_update_alphabet(const Document& tokens);
  Type get_next_type() const;

  TypeSequence() = delete;
  TypeSequence(const TypeSequence& other) = delete;
  TypeSequence(TypeSequence&& other) = delete;

  TypeSequence& operator=(const TypeSequence& rhs) = delete;
  TypeSequence& operator=(TypeSequence&& rhs) = delete;

};


#endif // TYPE_SEQUENCE_H
