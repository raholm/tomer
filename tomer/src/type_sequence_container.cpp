#include "type_sequence_container.h"

TypeSequenceContainer::TypeSequenceContainer()
  : alphabet_{new Alphabet()}
{

}

TypeSequenceContainer::TypeSequenceContainer(const Corpus& tokens)
  : types_(tokens.size()), alphabet_{new Alphabet()}
{
  add(tokens);
}

void TypeSequenceContainer::add(const Corpus& tokens) {
  for (auto const& doc : tokens) add(doc);
}

void TypeSequenceContainer::add(const Document& tokens) {
  types_.push_back(TypeSequence(tokens, alphabet_));
}

TypeSequence TypeSequenceContainer::at(std::size_t position) const {
  return types_.at(position);
}

std::size_t TypeSequenceContainer::size() const {
  return types_.size();
}
