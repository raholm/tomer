#include "type_sequence_container.h"

#include <algorithm>
#include <iterator>

TypeSequence::TypeSequence(const Document& tokens)
  : types_(tokens.size()), alphabet_{new Alphabet()}
{
  create_types_and_update_alphabet(tokens);
}

TypeSequence::TypeSequence(TypeVector&& tokens, AlphabetPtr alphabet)
  : types_{std::move(tokens)}, alphabet_{alphabet}
{

}

TypeSequence::TypeSequence(const Document& tokens, AlphabetPtr alphabet)
  : types_(tokens.size()), alphabet_{alphabet}
{
  create_types_and_update_alphabet(tokens);
}

const Type& TypeSequence::at(std::size_t position) const {
  return types_.at(position);
}

const Token& TypeSequence::token_at(std::size_t position) const {
  Type type = types_.at(position);
  auto it = alphabet_->begin();
  std::advance(it, type);
  return it->first;
}

void TypeSequence::create_types_and_update_alphabet(const Document& tokens) {
  Type type = get_next_type();
  Token token;

  for (unsigned i = 0; i < tokens.size(); ++i) {
    token = tokens.at(i);

    if (!(alphabet_->find(token) == alphabet_->end())) {
      alphabet_->at(token) = type++;
    }

    types_.at(i) = alphabet_->find(token)->second;
  }
}

Type TypeSequence::get_next_type() const {
  if (alphabet_->empty()) return 0;
  else return std::max_element(alphabet_->cbegin(), alphabet_->cend(),
                               [](const std::pair<Token, Type>& p1,
                                  const std::pair<Token, Type>& p2) {
                                 return p1.second < p2.second;
                               })->second + 1;
}

uint TypeSequence::size() const {
  return types_.size();
}

uint TypeSequence::length() const {
  return size();
}
