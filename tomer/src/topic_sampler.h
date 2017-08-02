#ifndef TOPIC_SAMPLER_H
#define TOPIC_SAMPLER_H

#include <random>

#include "def.h"
#include "left_to_right_state.h"

class TopicSampler {
 public:
  TopicSampler() = default;

  virtual ~TopicSampler() = default;

  virtual void begin(const LeftToRightState& state) = 0;
  virtual void end(const LeftToRightState& state) = 0;

  virtual void update_addition(const LeftToRightState& state) = 0;
  virtual void update_elimination(const LeftToRightState& state) = 0;

  virtual double get_word_prob(const LeftToRightState& state) const = 0;
  virtual Topic sample_topic(const LeftToRightState& state) const = 0;

 private:

};

class LDATopicSampler : public TopicSampler {
public:
  LDATopicSampler();

  ~LDATopicSampler() = default;

  void begin(const LeftToRightState& state) override;
  void end(const LeftToRightState& state) override;

  void update_addition(const LeftToRightState& state) override;
  void update_elimination(const LeftToRightState& state) override;

  double get_word_prob(const LeftToRightState& state) const override;
  Topic sample_topic(const LeftToRightState& state) const override;

private:
  std::random_device rand_dev_;
  mutable std::default_random_engine gen_;
  mutable std::discrete_distribution<Topic> dist_;

  DoubleVector get_topic_probabilities(const LeftToRightState& state) const;

};

class SparseLDATopicSampler : public TopicSampler {
public:
  SparseLDATopicSampler();

  ~SparseLDATopicSampler() = default;

  void begin(const LeftToRightState& state) override;
  void end(const LeftToRightState& state) override;

  void update_addition(const LeftToRightState& state) override;
  void update_elimination(const LeftToRightState& state) override;

  double get_word_prob(const LeftToRightState& state) const override;
  Topic sample_topic(const LeftToRightState& state) const override;

private:
  std::random_device rand_dev_;
  mutable std::default_random_engine gen_;
  mutable std::uniform_real_distribution<double> dist_;

  bool has_init_;

  double smoothing_only_mass_;
  DoubleVector cached_coefficients_;

  mutable DoubleVector topic_term_scores_;
  IntVector topic_term_indices_;
  IntVector topic_term_values_;

  double topic_beta_mass_;
  mutable double topic_term_mass_;

  IntVector topic_index_;
  mutable std::size_t dense_index_;
  std::size_t non_zero_topics_;

  void update(const LeftToRightState& state, bool incr);
  void maintain_dense_index__addition(const LeftToRightState& state);
  void maintain_dense_index__elimination(const LeftToRightState& state);
  void update_topic_scores(const LeftToRightState& state) const;

};

#endif // TOPIC_SAMPLER_H
