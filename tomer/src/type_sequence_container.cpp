#include "type_sequence_container.h"

void TypeSequenceContainer::add(const TypeSequence& type_sequence) {
  types_.push_back(type_sequence);
}

void TypeSequenceContainer::add(TypeSequence&& type_sequence) {
  types_.push_back(std::move(type_sequence));
}

const TypeSequence& TypeSequenceContainer::at(TypeSequenceContainer::size_type position) const {
  return types_.at(position);
}

TypeSequenceContainer::size_type TypeSequenceContainer::size() const {
  return types_.size();
}
