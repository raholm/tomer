#ifndef TOMER_TYPE_SEQUENCE_H_
#define TOMER_TYPE_SEQUENCE_H_

#include <vector>

#include "alphabet.h"
#include "def.h"

namespace tomer {

  class TypeSequenceBuilder;

  class TypeSequence {
  public:
    using type = Alphabet::type;
    using type_vector = Vector<type>;
    using token = Alphabet::token;
    using alphabet_ptr = Alphabet::shared_ptr;

    TypeSequence(const TypeSequence& other) = default;
    TypeSequence(TypeSequence&& other) = default;

    ~TypeSequence() = default;

    TypeSequence& operator=(const TypeSequence& rhs) = default;
    TypeSequence& operator=(TypeSequence&& rhs) = default;

    inline const Type& at(size_type position) const {
      return types_.at(position);
    }

    inline const Token& token_at(size_type position) const {
      auto type = at(position);
      return alphabet_->at(type);
    }

    inline size_t size() const {
      return types_.size();
    }

    inline size_t length() const {
      return size();
    }

  private:
    TypeVector types_;
    AlphabetPtr alphabet_;

    TypeSequence(const TypeVector& types, AlphabetPtr alphabet)
      : types_{types},
        alphabet_{alphabet} {}

    TypeSequence(TypeVector&& types, AlphabetPtr alphabet)
      : types_{std::move(types)},
        alphabet_{alphabet} {}


    friend class TypeSequenceBuilder;

  };

} // namespace tomer

#endif // TYPE_SEQUENCE_H
