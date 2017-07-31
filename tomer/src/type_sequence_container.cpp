#include "type_sequence_container.h"

TypeSequenceContainer::TypeSequenceContainer()
  : alphabet_{new Alphabet()}
{

}

TypeSequenceContainer::TypeSequenceContainer(const Corpus& tokens)
  : type_{tokens.size()}, alphabet_{new Alphabet()}
{
  add(tokens);
}

void TypeSequenceContainer::add(const Corpus& tokens) {
  for (auto const& doc : tokens) add(doc);
}

void TypeSequenceContainer::add(const Document& tokens) {
  types_.push_back(TypeSequence(tokens, alphabet_));
}

TypeVector TypeSequenceContainer::at(uint position) const {
  return types_.at(position);
}

uint TypeSequenceContainer::size() const {
  return types_.size();
}
