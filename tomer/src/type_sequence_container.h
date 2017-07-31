#ifndef TYPE_SEQUENCE_CONTAINER_H
#define TYPE_SEQUENCE_CONTAINER_H

#include "type_sequence.h"

class TypeSequenceContainer {
public:
  TypeSequenceContainer();
  TypeSequenceContainer(const Corpus& tokens);

  TypeSequenceContainer(const TypeSequenceContainer& other) = default;

  ~TypeSequenceContainer() = default;

  TypeSequenceContainer& operator=(const TypeSequenceContainer& rhs) = default;

  void add(const Corpus& tokens);
  void add(const Document& tokens);
  void add(const TypeSequence& ts);

  const TypeSequence& at(std::size_t position) const;

  std::size_t size() const;

private:
  std::vector<TypeSequence> types_;
  AlphabetPtr alphabet_;

  void create_types_and_update_alphabet(const Document& tokens);
  Type get_next_type() const;

  TypeSequenceContainer(TypeSequenceContainer&& other) = delete;

  TypeSequenceContainer& operator=(TypeSequenceContainer&& rhs) = delete;

};

#endif // TYPE_SEQUENCE_CONTAINER_H
