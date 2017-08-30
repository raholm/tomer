#ifndef TOMER_TOPIC_INDICATOR_MODE_H_
#define TOMER_TOPIC_INDICATOR_MODE_H_

#include "def.h"

namespace tomer {

  template<typename T>
  class TopicIndicatorMode {
  public:
    using Type = T;
    using TopicIndicator = size_t;
    using TypeTopicIndicatorPair = Pair<Type, TopicIndicator>;

    using TypeVector = Vector<Type>;
    using TopicIndicatorVector = Vector<TopicIndicator>;
    using TypeTopicIndicatorPairVector = Vector<TypeTopicIndicatorPair>;

  private:
    using Count = size_t;
    using TypeTopicIndicatorCountPair = Pair<TypeTopicIndicatorPair, Count>;

  public:
    TopicIndicatorMode() = default;
    TopicIndicatorMode(const TypeVector& types,
                       const TopicIndicatorVector& topic_indicators) {
      update(types, topic_indicators);
    }

    TopicIndicatorMode(const TypeTopicIndicatorPairVector& type_topic_indicators) {
      update(type_topic_indicators);
    }

    TopicIndicatorMode(const TopicIndicatorMode& other) = default;
    TopicIndicatorMode(TopicIndicatorMode&& other) = default;

    ~TopicIndicatorMode() = default;

    inline void update(const TypeVector& types,
                       const TopicIndicatorVector& topic_indicators) {
      if (types.size() != topic_indicators.size())
        throw std::invalid_argument("Types and Topic Indicators have different sizes.");

      for (unsigned i = 0; i < types.size(); ++i)
        insert_or_add_element(std::make_pair(std::make_pair(types.at(i), topic_indicators.at(i)), 1));
    }

    inline void update(const TypeTopicIndicatorPairVector& type_topic_indicators) {
      for (auto const& pair : type_topic_indicators)
        insert_or_add_element(std::make_pair(pair, 1));
    }

    inline bool contains(const Type& type) const {
      return modes_.find(type) != modes_.end();
    }

    inline Vector<TypeTopicIndicatorPair> get_data() const {
      return Vector<TypeTopicIndicatorPair>{modes_.cbegin(), modes_.cend()};
    }

    inline TopicIndicator get_mode(const Type& type) const {
      auto it = modes_.find(type);
      if (it == modes_.end())
        throw std::out_of_range("Type has no mode.");
      return it->second;
    }

  private:
    Map<Type, TopicIndicator> modes_;
    Map<Pair<Type, TopicIndicator>, Count> counts_;

    void insert_or_add_element(const TypeTopicIndicatorCountPair& element) {
      if (!counts_.insert(element).second)
        counts_[element.first] += element.second;

      auto type = element.first.first;
      auto topic_indicator = element.first.second;

      if (contains(type)) {
        if (counts_[element.first] > counts_[std::make_pair(type, get_mode(type))])
          modes_[type] = topic_indicator;
      } else {
        modes_[type] = topic_indicator;
      }
    }

  };

  struct LDAType {
    LDAType() = default;

    bool operator<(const LDAType& rhs) const {
      return (doc < rhs.doc) ||
        (doc == rhs.doc && pos < rhs.pos) ||
        (doc == rhs.doc && pos == rhs.pos && type < rhs.type);
    }

    size_t doc;
    size_t pos;
    size_t type;

  };

  using TypeTopicIndicatorMode = TopicIndicatorMode<LDAType>;

} // namespace tomer

#endif // TOMER_TOPIC_INDICATOR_MODE_H_
