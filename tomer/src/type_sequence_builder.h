#ifndef TOMER_TYPE_SEQUENCE_BUILDER_H_
#define TOMER_TYPE_SEQUENCE_BUILDER_H_

#include "def.h"
#include "alphabet.h"
#include "type_sequence_container.h"

namespace tomer {

  class TypeSequenceBuilder {
  public:
    using type = Alphabet::type;
    using type_vector = std::vector<type>;
    using AlphabetPtr = Alphabet::shared_ptr;

    TypeSequenceBuilder();
    TypeSequenceBuilder(const Alphabet& alphabet, bool fixed);
    TypeSequenceBuilder(Alphabet&& alphabet, bool fixed);

    ~TypeSequenceBuilder() = default;

    void add(const Corpus& corpus);
    void add(const Document& document);

    const TypeSequenceContainer& get_data() const;

  private:
    typeSequenceContainer container_;
    AlphabetPtr alphabet_;
    bool fixed_;

    type_vector create_type_vector(const Document& document);
    type_vector create_type_vector_and_update_alphabet(const Document& document);

    TypeSequenceBuilder(const TypeSequenceBuilder& other) = delete;
    TypeSequenceBuilder(TypeSequenceBuilder&& other) = delete;

    TypeSequenceBuilder& operator=(const TypeSequenceBuilder& rhs) = delete;
    TypeSequenceBuilder& operator=(TypeSequenceBuilder&& rhs) = delete;

  };

} // namespace tomer

#endif // TOMER_TYPE_SEQUENCE_BUILDER_H_
