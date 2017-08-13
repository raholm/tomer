#include "left_to_right_evaluator.h"

namespace tomer {

  LeftToRightEvaluator::LeftToRightEvaluator(size_t n_topics,
                                             const DoubleVector& alpha,
                                             double beta,
                                             const IntVector& topic_counts,
                                             const IntMatrix& type_topic_counts,
                                             std::unique_ptr<MarginalProbEsimator>&& prob_estimator,
                                             std::unique_ptr<TopicSampler>&& topic_sampler) :
    state_{n_topics, alpha, beta, topic_counts, type_topic_counts},
    prob_estimator_{std::move(prob_estimator)},
    topic_sampler_{std::move(topic_sampler)} {
      init();
    }


  double LeftToRightEvaluator::evaluate(const TypeSequenceContainer& types,
                                        size_t n_particles,
                                        bool resampling) {
    double log_likelihood = 0;

    for (unsigned i = 0; i < types.size(); ++i)
      log_likelihood += evaluate(types.at(i), n_particles, resampling);

    return log_likelihood;
  }

  double LeftToRightEvaluator::evaluate(const TypeSequence& types,
                                        size_t n_particles,
                                        bool resampling) {
    DoubleMatrix particle_probs(n_particles);
    double log_likelihood = 0;
    double log_n_particles = log(n_particles);
    size_t max_len = 0;
    double sum;

    for (unsigned particle = 0; particle < n_particles; ++particle) {
      particle_probs.at(particle) = get_word_probabilities(types, resampling);
      max_len = std::max(max_len, particle_probs.at(particle).size());
    }

    for (unsigned position = 0; position < max_len; ++position) {
      sum = 0;

      for (unsigned particle = 0; particle < n_particles; ++particle) {
        if (position >= particle_probs.at(particle).size()) break;

        sum += particle_probs.at(particle).at(position);
      }

      if (sum > 0) {
        log_likelihood += log(sum) - log_n_particles;
      }
    }

    return log_likelihood;
  }

  DoubleVector LeftToRightEvaluator::get_word_probabilities(const TypeSequence& types,
                                                            bool resampling) {
    size_t doc_length = types.size();
    DoubleVector word_probs(doc_length);

    Type current_type;
    Topic old_topic, new_topic;

    // Reallocate only when necessary
    if (doc_length > state_.doc_topics.size())
      state_.doc_topics = IntVector(doc_length);
    else
      std::fill(state_.doc_topics.begin(), state_.doc_topics.end(), 0);

    std::fill(state_.local_topic_counts.begin(), state_.local_topic_counts.end(), 0);

    state_.n_tokens_seen = 0;
    state_.doc_position = 0;

    begin();

    for (unsigned limit = 0; limit < doc_length; ++limit) {
      if (resampling) {
        for (unsigned position = 0; position < limit; ++position) {
          current_type = types.at(position);

          if (!is_valid(current_type))
            continue;

          old_topic = state_.doc_topics.at(position);

          update_elimination(current_type, old_topic, position);

          new_topic = topic_sampler_->next_topic(state_);

          update_addition(current_type, new_topic, position);
        }
      }

      current_type = types.at(limit);

      if (!is_valid(current_type))
        continue;

      state_.doc_position = limit;
      state_.type = current_type;
      state_.current_type_topic_counts = state_.type_topic_counts.at(current_type);
      state_.topic = state_.doc_topics.at(limit);

      word_probs.at(limit) = prob_estimator_->get_prob(state_);

      new_topic = topic_sampler_->next_topic(state_);

      update_addition(current_type, new_topic, limit);

      state_.n_tokens_seen++;
    }

    end();

    return word_probs;
  }

  bool LeftToRightEvaluator::is_valid(const Type& type) {
    return type >= 0 && type < state_.n_types;
  }

  void LeftToRightEvaluator::init() {
    prob_estimator_->init(state_);
    topic_sampler_->init(state_);
  }

  void LeftToRightEvaluator::begin() {
    prob_estimator_->begin(state_);
    topic_sampler_->begin(state_);
  }

  void LeftToRightEvaluator::end() {
    prob_estimator_->end(state_);
    topic_sampler_->end(state_);
  }

  void LeftToRightEvaluator::update_elimination(const Type& type,
                                                const Topic& topic,
                                                size_t position) {
    state_.doc_position = position;
    state_.type = type;
    state_.current_type_topic_counts = state_.type_topic_counts.at(type);
    state_.topic = topic;

    if (state_.local_topic_counts.at(topic) > 0)
      state_.local_topic_counts.at(topic)--;

    prob_estimator_->update_elimination(state_);
    topic_sampler_->update_elimination(state_);
  }

  void LeftToRightEvaluator::update_addition(const Type& type,
                                             const Topic& topic,
                                             size_t position) {
    state_.doc_position = position;
    state_.type = type;
    state_.topic = topic;
    state_.doc_topics.at(position) = topic;
    state_.local_topic_counts.at(topic)++;

    prob_estimator_->update_addition(state_);
    topic_sampler_->update_addition(state_);
  }

} // namespace tomer
