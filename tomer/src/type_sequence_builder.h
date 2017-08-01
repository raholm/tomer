#ifndef TYPE_SEQUENCE_BUILDER_H
#define TYPE_SEQUENCE_BUILDER_H

#include "def.h"
#include "alphabet.h"
#include "type_sequence_container.h"

class TypeSequenceBuilder {
public:
  using Type = Alphabet::Type;
  using TypeVector = std::vector<Type>;
  using AlphabetPtr = Alphabet::SPtr;

  TypeSequenceBuilder();
  TypeSequenceBuilder(const Alphabet& alphabet, bool fixed);
  TypeSequenceBuilder(Alphabet&& alphabet, bool fixed);

  ~TypeSequenceBuilder() = default;

  void add(const Corpus& corpus);
  void add(const Document& document);

  const TypeSequenceContainer& get_data() const;

private:
  TypeSequenceContainer container_;
  AlphabetPtr alphabet_;
  bool fixed_;

  TypeVector create_type_vector(const Document& document);
  TypeVector create_type_vector_and_update_alphabet(const Document& document);

  TypeSequenceBuilder(const TypeSequenceBuilder& other) = delete;
  TypeSequenceBuilder(TypeSequenceBuilder&& other) = delete;

  TypeSequenceBuilder& operator=(const TypeSequenceBuilder& rhs) = delete;
  TypeSequenceBuilder& operator=(TypeSequenceBuilder&& rhs) = delete;

};

#endif // TYPE_SEQUENCE_BUILDER_H
