#ifndef TOMER_TOPIC_SAMPLER_H_
#define TOMER_TOPIC_SAMPLER_H_

#include <random>

#include "def.h"
#include "left_to_right_state.h"

namespace tomer {

  class TopicSampler {
  public:
    TopicSampler() = default;

    virtual ~TopicSampler() = default;

    virtual void init(const LeftToRightState& state) {}

    virtual void begin(const LeftToRightState& state) {}
    virtual void end(const LeftToRightState& state) {}

    virtual void update_addition(const LeftToRightState& state) {}
    virtual void update_elimination(const LeftToRightState& state) {};

    virtual Topic next_topic(const LeftToRightState& state) const = 0;

  };

  class LDATopicSampler : public TopicSampler {
  public:
    LDATopicSampler();

    ~LDATopicSampler() = default;

    Topic next_topic(const LeftToRightState& state) const override;

  private:
    std::random_device rand_dev_;
    mutable std::mt19937 gen_;
    mutable std::discrete_distribution<Topic> dist_;

    DoubleVector get_topic_probabilities(const LeftToRightState& state) const;

  };

  class SparseLDATopicSampler : public TopicSampler {
  public:
    SparseLDATopicSampler();

    ~SparseLDATopicSampler() = default;

    void init(const LeftToRightState& state) override;

    void begin(const LeftToRightState& state) override;
    void end(const LeftToRightState& state) override;

    void update_addition(const LeftToRightState& state) override;
    void update_elimination(const LeftToRightState& state) override;

    Topic next_topic(const LeftToRightState& state) const override;

  private:
    std::random_device rand_dev_;
    mutable std::mt19937 gen_;
    mutable std::uniform_real_distribution<double> dist_;

    bool has_begun_;

    double smoothing_only_mass_;
    DoubleVector cached_coefficients_;

    mutable DoubleVector topic_term_scores_;

    double topic_beta_mass_;
    mutable double topic_term_mass_;

    IntVector topic_index_;
    mutable size_t dense_index_;
    size_t non_zero_topics_;

    void update(const LeftToRightState& state, bool incr);
    void maintain_dense_index_addition(const LeftToRightState& state);
    void maintain_dense_index_elimination(const LeftToRightState& state);
    void update_topic_scores(const LeftToRightState& state) const;

  };

} // namespace tomer

#endif // TOMER_TOPIC_SAMPLER_H_
