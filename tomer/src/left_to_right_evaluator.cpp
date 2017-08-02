#include "left_to_right_evaluator.h"

#include <iostream>
#include <cmath>
#include <algorithm>

LeftToRightEvaluator::LeftToRightEvaluator(std::size_t n_topics,
                                           const DoubleVector& alpha,
                                           double beta,
                                           const IntVector& topic_counts,
                                           const IntMatrix& type_topic_counts) :
  n_topics_{n_topics},
  alpha_{alpha},
  beta_{beta},
  topic_counts_{topic_counts},
  type_topic_counts_{type_topic_counts},
  cached_coefficients_(n_topics),
  smoothing_only_mass_{0},
  sampler_{}
{
  alpha_sum_ = std::accumulate(alpha.cbegin(), alpha.cend(), 0.0);
  beta_sum_ = type_topic_counts.size() * beta;

  for (unsigned topic = 0; topic < n_topics_; ++topic) {
    double denom = (topic_counts_.at(topic) + beta_sum_);
    smoothing_only_mass_ += alpha_.at(topic) * beta_ / denom;
    cached_coefficients_.at(topic) = alpha_.at(topic) / denom;
  }
}

double LeftToRightEvaluator::evaluate(const CorpusTypeSequence& types,
                                      std::size_t n_particles,
                                      bool resampling) {
  DoubleMatrix particle_probabilities(n_particles);

  double total_log_likelihood = 0;
  double log_n_particles = log(n_particles);
  double doc_log_likelihood, log_prob, sum;
  std::size_t max_len = 0;

  for (unsigned i = 0; i < types.size(); ++i) {
    doc_log_likelihood = 0;

    for (unsigned particle = 0; particle < n_particles; ++particle) {
      particle_probabilities.at(particle) = get_word_probabilities(types.at(i), resampling);
      max_len = std::max(max_len,  particle_probabilities.at(particle).size());
    }

    for (unsigned position = 0; position < max_len; ++position) {
      sum = 0;

      for (unsigned particle = 0; particle < n_particles; ++particle) {
        if (position >= particle_probabilities.at(particle).size()) break;

        sum += particle_probabilities.at(particle).at(position);
      }

      if (sum > 0) {
        log_prob = log(sum) - log_n_particles;
        doc_log_likelihood += log_prob;
      }
    }

    total_log_likelihood += doc_log_likelihood;
  }

  return total_log_likelihood;
}

DoubleVector LeftToRightEvaluator::get_word_probabilities(const DocumentTypeSequence& types,
                                                          bool resampling) {

  uint doc_length = types.length();
  int type, old_topic, new_topic, topic;
  uint tokens_so_far = 0;

  DoubleVector word_probabilities(doc_length);

  LocalState state;

  state.doc_topics = IntVector(doc_length);
  state.topic_counts = IntVector(n_topics_);
  state.topic_index = IntVector(n_topics_);

  state.dense_index = 0;
  state.non_zero_topics = 0;

  state.topic_beta_mass = 0.0;
  state.topic_term_mass = 0.0;

  state.topic_term_scores = DoubleVector(n_topics_);

  for (unsigned limit = 0; limit < doc_length; ++limit) {
    if (resampling) {
      for (unsigned position = 0; position < limit; ++position) {
        type = types.at(position);

        if (type < 0 || type >= type_topic_counts_.size()) continue;

        state.type = type;
        state.type_topic_counts = type_topic_counts_.at(type);

        old_topic = state.doc_topics.at(position);

        remove_topic_and_update_state_and_coefficients(state, old_topic);

        update_topic_scores(state);

        new_topic = sample_new_topic(state);

        if (new_topic == -1)
          new_topic = old_topic;

        add_topic_and_update_state_and_coefficients(state, new_topic, position);
      }
    }

    type = types.at(limit);

    if (type < 0 || type >= type_topic_counts_.size()) continue;

    state.type = type;
    state.type_topic_counts = type_topic_counts_.at(type);

    update_topic_scores(state);

    word_probabilities.at(limit) += (smoothing_only_mass_ +
                                     state.topic_beta_mass +
                                     state.topic_term_mass) / (alpha_sum_ + tokens_so_far);

    new_topic = sample_new_topic(state);

    if (new_topic == -1)
      new_topic = n_topics_ - 1;

    add_topic_and_update_state_and_coefficients(state, new_topic, limit);

    ++tokens_so_far;
  }

  for (unsigned i = 0; i < state.non_zero_topics; ++i) {
    topic = state.topic_index.at(i);
    cached_coefficients_.at(topic) = alpha_.at(topic) / (topic_counts_.at(topic) + beta_sum_);
  }

  return word_probabilities;
}

void LeftToRightEvaluator::add_topic_and_update_state_and_coefficients(LocalState& state,
                                                                       uint topic,
                                                                       uint position) {
  state.doc_topics.at(position) = topic;
  add_or_remove_topic_and_update_state_and_coefficients(state, topic, true);
}

void LeftToRightEvaluator::remove_topic_and_update_state_and_coefficients(LocalState& state,
                                                                          uint topic) {
  add_or_remove_topic_and_update_state_and_coefficients(state, topic, false);
}

void LeftToRightEvaluator::add_or_remove_topic_and_update_state_and_coefficients(LocalState& state,
                                                                                 uint topic,
                                                                                 bool incr) {
  double denom = (topic_counts_.at(topic) + beta_sum_);

  state.topic_beta_mass -= beta_ * state.topic_counts.at(topic) / denom;

  if (incr) ++state.topic_counts.at(topic);
  else --state.topic_counts.at(topic);

  state.topic_beta_mass += beta_ * state.topic_counts.at(topic) / denom;

  cached_coefficients_.at(topic) = (alpha_.at(topic) + state.topic_counts.at(topic)) / denom;

  if (incr) maintain_dense_index_addition(state, topic);
  else maintain_dense_index_elimination(state, topic);
}


void LeftToRightEvaluator::maintain_dense_index_addition(LocalState& state, uint topic) const {
  if (state.topic_counts.at(topic) == 1) {
    state.dense_index = state.non_zero_topics;

    while (state.dense_index > 0 && state.topic_index.at(state.dense_index - 1) > topic) {
      state.topic_index.at(state.dense_index) = state.topic_index.at(state.dense_index - 1);
      --state.dense_index;
    }

    state.topic_index.at(state.dense_index) = topic;
    ++state.non_zero_topics;
  }
}

void LeftToRightEvaluator::maintain_dense_index_elimination(LocalState& state, uint topic) const {
  if (state.topic_counts.at(topic) == 0) {
    state.dense_index = 0;

    while (state.topic_index.at(state.dense_index) != topic)
      state.dense_index++;

    while (state.dense_index < state.non_zero_topics) {
      if (state.dense_index < state.topic_index.size() - 1) {
        state.topic_index.at(state.dense_index) = state.topic_index.at(state.dense_index + 1);
      }

      ++state.dense_index;
    }

    --state.non_zero_topics;
  }
}

void LeftToRightEvaluator::update_topic_scores(LocalState& state) const {
  int index = 0;
  int current_topic, current_value;
  double score;

  state.topic_term_mass = 0.0;

  while (index < state.type_topic_counts.size() &&
         state.type_topic_counts.at(index) > 0) {
    current_topic = index;
    current_value = state.type_topic_counts.at(index);

    score = cached_coefficients_.at(current_topic) * current_value;

    state.topic_term_mass += score;
    state.topic_term_scores.at(index) = score;

    ++index;
  }
}

int LeftToRightEvaluator::sample_new_topic(LocalState& state) {
  double sample = sampler_.next() * (smoothing_only_mass_ +
                                     state.topic_beta_mass +
                                     state.topic_term_mass);
  double orig_sample = sample;

  int topic, new_topic = -1;

  if (sample < state.topic_term_mass) {
    topic = -1;

    while (sample > 0) {
      ++topic;
      sample -= state.topic_term_scores.at(topic);
    }

    new_topic = topic;
  } else {
    sample -= state.topic_term_mass;

    if (sample < state.topic_beta_mass) {
      sample /= beta_;

      for (state.dense_index = 0; state.dense_index < state.non_zero_topics; ++state.dense_index) {
        topic = state.topic_index.at(state.dense_index);

        sample -= state.topic_counts.at(topic) / (topic_counts_.at(topic) + beta_sum_);

        if (sample <= 0.0) {
          new_topic = topic;
          break;
        }
      }
    } else {
      sample -= state.topic_beta_mass;
      sample /= beta_;
      new_topic = 0;
      sample -= alpha_.at(new_topic) / (topic_counts_.at(new_topic) + beta_sum_);

      while (sample > 0.0) {
        ++new_topic;
        sample -= alpha_.at(new_topic) / (topic_counts_.at(new_topic) + beta_sum_);
      }
    }
  }

  // if (new_topic == -1) {
  //   std::cerr << "Sampling error: " << orig_sample << " " << sample << " "
  //             << smoothing_only_mass_ << " " << state.topic_beta_mass << " "
  //             << state.topic_term_mass << std::endl;

  //   new_topic = n_topics_ - 1; // TODO is this appropriate
  // }

  return new_topic;
}
