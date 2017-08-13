#include "type_sequence_builder.h"

#include "type_sequence.h"

namespace tomer {

  TypeSequenceBuilder::typeSequenceBuilder()
    : container_{},
      alphabet_{std::make_shared<Alphabet>(Alphabet())},
      fixed_{false} {}

  TypeSequenceBuilder::typeSequenceBuilder(const Alphabet& alphabet, bool fixed)
    : container_{},
      alphabet_{std::make_shared<Alphabet>(Alphabet{alphabet})},
      fixed_{fixed} {}

  TypeSequenceBuilder::typeSequenceBuilder(Alphabet&& alphabet, bool fixed)
    : container_{},
      alphabet_{std::make_shared<Alphabet>(Alphabet{std::move(alphabet)})},
      fixed_{fixed} {}

  void TypeSequenceBuilder::add(const Corpus& corpus) {
    for (auto const& document : corpus) add(document);
  }

  void TypeSequenceBuilder::add(const Document& document) {
    TypeSequenceBuilder::type_vector types;

    if (fixed_)
      types = create_type_vector(document);
    else
      types = create_type_vector_and_update_alphabet(document);

    TypeSequence ts{std::move(types), alphabet_};
    container_.add(ts);
  }

  const TypeSequenceContainer& TypeSequenceBuilder::get_data() const {
    return container_;
  }

  TypeSequenceBuilder::type_vector
  TypeSequenceBuilder::create_type_vector(const Document& document) {
    TypeSequenceBuilder::type_vector types(document.size());
    TypeSequenceBuilder::type type;

    for (auto const& token : document) {
      if (alphabet_->has(token)) {
        type = alphabet_->at(token);
        types.push_back(type);
      }
    }

    return types;
  }

  TypeSequenceBuilder::type_vector
  TypeSequenceBuilder::create_type_vector_and_update_alphabet(const Document& document) {
    TypeSequenceBuilder::type_vector types(document.size());
    TypeSequenceBuilder::type type;

    for (auto const& token : document) {
      type = alphabet_->add(token);
      types.push_back(type);
    }

    return types;
  }

} // namespace tomer
