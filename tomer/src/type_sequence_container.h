#ifndef TYPE_SEQUENCE_CONTAINER_H
#define TYPE_SEQUENCE_CONTAINER_H

#include "type_sequence.h"

class TypeSequenceBuilder;

class TypeSequenceContainer {
public:
  using TypeSequences = std::vector<TypeSequence>;
  using size_type = TypeSequences::size_type;

  TypeSequenceContainer(const TypeSequenceContainer& other) = default;
  TypeSequenceContainer(TypeSequenceContainer&& other) = default;

  ~TypeSequenceContainer() = default;

  TypeSequenceContainer& operator=(const TypeSequenceContainer& rhs) = default;
  TypeSequenceContainer& operator=(TypeSequenceContainer&& rhs) = default;

  const TypeSequence& at(size_type position) const;

  size_type size() const;

private:
  TypeSequences types_;

  TypeSequenceContainer() = default;

  void add(const TypeSequence& ts);
  void add(TypeSequence&& ts);

  friend class TypeSequenceBuilder;

};

#endif // TYPE_SEQUENCE_CONTAINER_H
