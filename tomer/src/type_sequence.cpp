#include "type_sequence_container.h"

TypeSequence::TypeSequence(const TypeSequence::TypeVector& types, TypeSequence::AlphabetPtr alphabet)
  : types_{types}, alphabet_{alphabet}
{

}

TypeSequence::TypeSequence(TypeSequence::TypeVector&& types, TypeSequence::AlphabetPtr alphabet)
  : types_{std::move(types)}, alphabet_{alphabet}
{

}

const TypeSequence::Type& TypeSequence::at(TypeSequence::size_type position) const {
  return types_.at(position);
}

const TypeSequence::Token& TypeSequence::token_at(TypeSequence::size_type position) const {
  auto type = at(position);
  return alphabet_->at(type);
}

TypeSequence::size_type TypeSequence::size() const {
  return types_.size();
}

TypeSequence::size_type TypeSequence::length() const {
  return size();
}
