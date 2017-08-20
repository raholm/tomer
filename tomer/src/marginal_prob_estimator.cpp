#include "marginal_prob_estimator.h"

namespace tomer {

  double LDATokenMarginalProbEstimator::get_prob(const LeftToRightState& state) const {
    Topic current_topic = state.topic;

    // TODO: Is this supposed to be the local or global counts?
    double prob_word_given_topic_numerator =
      state.current_type_topic_counts.at(current_topic) + state.beta;
    double prob_word_given_topic_denominator =
      state.topic_counts.at(current_topic) + state.beta_sum;
    double prob_word_given_topic =
      prob_word_given_topic_numerator / prob_word_given_topic_denominator;

    double prob_topic_given_topics_numerator =
      state.local_topic_counts.at(current_topic) + state.alpha.at(current_topic);
    double prob_topic_given_topics_denominator =
      state.n_tokens_seen + state.alpha_sum;
    double prob_topic_given_topics =
      prob_topic_given_topics_numerator / prob_topic_given_topics_denominator;

    return prob_word_given_topic * prob_topic_given_topics;
  }

  SparseLDATokenMarginalProbEstimator::SparseLDATokenMarginalProbEstimator()
    : has_begun_{false},
      smoothing_only_mass_{0},
      cached_coefficients_{},
      topic_beta_mass_{0},
      topic_term_mass_{0},
      topic_index_{},
      dense_index_{0},
      non_zero_topics_{0} {}

  void SparseLDATokenMarginalProbEstimator::init(const LeftToRightState& state) {
    cached_coefficients_ = DoubleVector(state.n_topics);
    smoothing_only_mass_ = 0;

    for (unsigned topic = 0; topic < state.n_topics; ++topic) {
      double denom = (state.topic_counts.at(topic) + state.beta_sum);
      smoothing_only_mass_ += state.alpha.at(topic) * state.beta / denom;
      cached_coefficients_.at(topic) = state.alpha.at(topic) / denom;
    }
  }

  void SparseLDATokenMarginalProbEstimator::begin(const LeftToRightState& state) {
    if (has_begun_) return;

    dense_index_ = 0;
    non_zero_topics_ = 0;

    topic_beta_mass_ = 0;
    topic_term_mass_ = 0;

    topic_index_ = IntVector(state.n_topics);

    has_begun_ = true;
  }

  void SparseLDATokenMarginalProbEstimator::end(const LeftToRightState& state) {
    Topic topic;

    for (unsigned i = 0; i < non_zero_topics_; ++i) {
      topic = topic_index_.at(i);
      cached_coefficients_.at(topic) = state.alpha.at(topic) /
        (state.topic_counts.at(topic) + state.beta_sum);
    }

    dense_index_ = 0;
    non_zero_topics_ = 0;

    topic_beta_mass_ = 0;
    topic_term_mass_ = 0;

    std::fill(topic_index_.begin(), topic_index_.end(), 0);
  }

  void SparseLDATokenMarginalProbEstimator::update_addition(const LeftToRightState& state) {
    update(state, true);
  }

  void SparseLDATokenMarginalProbEstimator::update_elimination(const LeftToRightState& state) {
    update(state, false);
  }

  void SparseLDATokenMarginalProbEstimator::update(const LeftToRightState& state,
                                                   bool incr) {
    Topic topic = state.topic;
    double denom = (state.topic_counts.at(topic) + state.beta_sum);

    // Remove their mass before the count changed
    if (incr)
      topic_beta_mass_ -= state.beta * (state.local_topic_counts.at(topic) - 1) / denom;
    else
      topic_beta_mass_ -= state.beta * (state.local_topic_counts.at(topic) + 1) / denom;


    // Add their mass after the count changed
    topic_beta_mass_ += state.beta * state.local_topic_counts.at(topic) / denom;

    cached_coefficients_.at(topic) =
      (state.alpha.at(topic) + state.local_topic_counts.at(topic)) / denom;

    if (incr) maintain_dense_index_addition(state);
    else maintain_dense_index_elimination(state);
  }


  void SparseLDATokenMarginalProbEstimator::maintain_dense_index_addition(const LeftToRightState& state) {
    Topic topic = state.topic;

    if (state.local_topic_counts.at(topic) == 1) {
      dense_index_ = non_zero_topics_;

      while (dense_index_ > 0 && topic_index_.at(dense_index_ - 1) > topic) {
        topic_index_.at(dense_index_) = topic_index_.at(dense_index_ - 1);
        dense_index_--;
      }

      topic_index_.at(dense_index_) = topic;
      non_zero_topics_++;
    }
  }

  void SparseLDATokenMarginalProbEstimator::maintain_dense_index_elimination(const LeftToRightState& state) {
    Topic topic = state.topic;

    if (state.local_topic_counts.at(topic) == 0) {
      dense_index_ = 0;

      while (topic_index_.at(dense_index_) != topic)
        dense_index_++;

      while (dense_index_ < non_zero_topics_) {
        if (dense_index_ < topic_index_.size() - 1) {
          topic_index_.at(dense_index_) = topic_index_.at(dense_index_ + 1);
        }

        dense_index_++;
      }

      non_zero_topics_--;
    }
  }

  double SparseLDATokenMarginalProbEstimator::get_prob(const LeftToRightState& state) const {
    update_topic_scores(state);
    return  (smoothing_only_mass_ + topic_beta_mass_ + topic_term_mass_) /
      (state.alpha_sum + state.n_tokens_seen);
  }

  void SparseLDATokenMarginalProbEstimator::update_topic_scores(const LeftToRightState& state) const {
    size_t index = 0;
    int current_topic, current_value;
    double score;

    topic_term_mass_ = 0.0;

    while (index < state.current_type_topic_counts.size() &&
           state.current_type_topic_counts.at(index) > 0) {
      current_topic = index;
      current_value = state.current_type_topic_counts.at(current_topic);

      score = cached_coefficients_.at(current_topic) * current_value;

      topic_term_mass_ += score;

      index++;
    }
  }

} // namespace tomer
