#include "type_sequence_container.h"

TypeSequenceContainer::TypeSequenceContainer()
  : alphabet_{new Alphabet()}
{

}

TypeSequenceContainer::TypeSequenceContainer(const Corpus& corpus)
  : types_(corpus.size()), alphabet_{new Alphabet()}
{
  add(corpus);
}

void TypeSequenceContainer::add(const Corpus& corpus) {
  for (auto const& document : corpus) add(document);
}

void TypeSequenceContainer::add(const Document& document) {
  add(TypeSequence{document, alphabet_});
}

void TypeSequenceContainer::add(const TypeSequence& type_sequence) {
  types_.push_back(type_sequence);
}

const TypeSequence& TypeSequenceContainer::at(std::size_t position) const {
  return types_.at(position);
}

std::size_t TypeSequenceContainer::size() const {
  return types_.size();
}
