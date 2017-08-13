#ifndef TOMER_TYPE_SEQUENCE_CONTAINER_H_
#define TOMER_TYPE_SEQUENCE_CONTAINER_H_

#include "type_sequence.h"
#include "def.h"

namespace tomer {

  class TypeSequenceBuilder;

  class TypeSequenceContainer {
  public:
    TypeSequenceContainer(const TypeSequenceContainer& other) = default;
    TypeSequenceContainer(TypeSequenceContainer&& other) = default;

    ~TypeSequenceContainer() = default;

    TypeSequenceContainer& operator=(const TypeSequenceContainer& rhs) = default;
    TypeSequenceContainer& operator=(TypeSequenceContainer&& rhs) = default;

    inline const TypeSequence& at(size_t position) const {
      return types_.at(position);
    }

    inline size_t size() const {
      return types_.size();
    }

  private:
    Vector<TypeSequence> types_;

    TypeSequenceContainer() = default;

    inline void add(const TypeSequence& type_sequence) {
      types_.push_back(type_sequence);
    }

    inline void add(TypeSequence&& type_sequence) {
      types_.push_back(std::move(type_sequence));
    }

    friend class TypeSequenceBuilder;

  };

} // namespace tomer

#endif // TOMER_TYPE_SEQUENCE_CONTAINER_H_
