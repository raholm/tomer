#ifndef TOMER_TOPIC_INDICATOR_MODE_H_
#define TOMER_TOPIC_INDICATOR_MODE_H_

#include "def.h"

namespace tomer {

  template<typename T>
  class TopicIndicatorMode {
  public:
    using type = T;
    using topic_indicator = size_t;
    using type_topic_indicator_pair = Pair<type, topic_indicator>;

    using type_vector = Vector<type>;
    using topic_indicator_vector = Vector<topic_indicator>;
    using type_topic_indicator_pair_vector = Vector<type_topic_indicator_pair>;

  private:
    using count = size_t;
    using type_topic_indicator_count_pair = Pair<type_topic_indicator_pair, count>;

  public:
    TopicIndicatorMode() = default;
    TopicIndicatorMode(const type_vector& types,
                       const topic_indicator_vector& topic_indicators) {
      update(types, topic_indicators);
    }

    TopicIndicatorMode(const type_topic_indicator_pair_vector& type_topic_indicators) {
      update(type_topic_indicators);
    }

    TopicIndicatorMode(const TopicIndicatorMode& other) = default;
    TopicIndicatorMode(TopicIndicatorMode&& other) = default;

    ~TopicIndicatorMode() = default;

    void update(const type_vector& types,
                const topic_indicator_vector& topic_indicators) {
      if (types.size() != topic_indicators.size())
        throw std::invalid_argument("Types and Topic Indicators have different sizes.");

      for (unsigned i = 0; i < types.size(); ++i)
        insert_or_add_element(std::make_pair(std::make_pair(types.at(i), topic_indicators.at(i)), 1));
    }

    void update(const type_topic_indicator_pair_vector& type_topic_indicators) {
      for (auto const& pair : type_topic_indicators)
        insert_or_add_element(std::make_pair(pair, 1));
    }

    inline bool contains(const type& type) const {
      return modes_.find(type) != modes_.end();
    }

    inline topic_indicator get_mode(const type& type) const {
      auto it = modes_.find(type);
      if (it == modes_.end())
        throw std::out_of_range("Type has no mode.");
      return it->second;
    }

  private:
    Map<type, topic_indicator> modes_;
    Map<Pair<type, topic_indicator>, count> counts_;

    void insert_or_add_element(const type_topic_indicator_count_pair& pair) {
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

} // namespace tomer

#endif // TOMER_TOPIC_INDICATOR_MODE_H_
