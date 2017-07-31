#include "type_sequence_builder.h"

#include "type_sequence.h"

TypeSequenceBuilder::TypeSequenceBuilder()
  : next_type_{0}, container_{}, alphabet_{new Alphabet()}
{

}

void TypeSequenceBuilder::add(const Corpus& corpus) {
  for (auto const& document : corpus) add(document);
}

void TypeSequenceBuilder::add(const Document& document) {
  auto types = create_type_vector_and_update_alphabet(document);
  TypeSequence ts{std::move(types), alphabet_};
  container_.add(ts);
}

const TypeSequenceContainer& TypeSequenceBuilder::get_data() const {
  return container_;
}

TypeVector TypeSequenceBuilder::create_type_vector_and_update_alphabet(const Document& document) {
  TypeVector types(document.size());

  for (auto const& token : document) {
    auto p = alphabet_->insert(std::pair<Token, Type>(token, next_type_));
    types.push_back(p.first->second);

    if (p.second) ++next_type_;
  }

  return types;
}
