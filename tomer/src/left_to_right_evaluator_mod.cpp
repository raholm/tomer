#include "left_to_right_evaluator_mod.h"

#include <iostream>

LeftToRightEvaluatorMod::LeftToRightEvaluatorMod(std::size_t n_topics,
                                                 const DoubleVector& alpha,
                                                 double beta,
                                                 const IntVector& topic_counts,
                                                 const IntMatrix& type_topic_counts,
                                                 std::unique_ptr<TopicSampler>&& sampler) :
  state_{n_topics, alpha, beta, topic_counts, type_topic_counts},
  sampler_{std::move(sampler)}
{

}


double LeftToRightEvaluatorMod::evaluate(const CorpusTypeSequence& types,
                                         std::size_t n_particles,
                                         bool resampling) {
  double log_likelihood = 0;

  for (unsigned i = 0; i < types.size(); ++i)
    log_likelihood += evaluate(types.at(i), n_particles, resampling);

  return log_likelihood;
}

double LeftToRightEvaluatorMod::evaluate(const DocumentTypeSequence& types,
                                         std::size_t n_particles,
                                         bool resampling) {
  DoubleMatrix particle_probs(n_particles);
  double log_likelihood = 0;
  double log_n_particles = log(n_particles);
  std::size_t max_len = 0;
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

DoubleVector LeftToRightEvaluatorMod::get_word_probabilities(const DocumentTypeSequence& types,
                                                             bool resampling) {
  std::size_t doc_length = types.size();
  DoubleVector word_probs(doc_length);

  Type current_type;
  Topic old_topic, new_topic;

  // if (doc_length > state_.doc_topics.size())
  state_.doc_topics = IntVector(doc_length);

  state_.n_tokens_seen = 0;
  state_.doc_position = 0;

  // sampler_->begin(state_);

  for (unsigned limit = 0; limit < doc_length; ++limit) {
    if (resampling) {
      for (unsigned position = 0; position < limit; ++position) {
        current_type = types.at(position);

        if (!is_valid(current_type))
          continue;

        old_topic = state_.doc_topics.at(position);

        update_elimination(current_type, old_topic, position);

        new_topic = sampler_->sample_topic(state_);

        update_addition(current_type, new_topic, position);
      }
    }

    current_type = types.at(limit);

    if (!is_valid(current_type))
      continue;

    old_topic = state_.doc_topics.at(limit);

    state_.doc_position = limit;
    state_.type = current_type;
    state_.current_type_topic_counts = state_.type_topic_counts.at(current_type);
    state_.topic = old_topic;

    word_probs.at(limit) = sampler_->get_word_prob(state_);

    // new_topic = sampler_->sample_topic(state_);

    // update_addition(current_type, new_topic, limit);

    // state_.n_tokens_seen++;
  }

  // sampler_->end(state_);

  return word_probs;
}

bool LeftToRightEvaluatorMod::is_valid(const Type& type) {
  return type > 0 && type < state_.n_types;
}

void LeftToRightEvaluatorMod::update_elimination(const Type& type,
                                                 const Topic& topic,
                                                 std::size_t position) {
  state_.doc_position = position;
  state_.type = type;
  state_.current_type_topic_counts = state_.type_topic_counts.at(type);
  state_.topic = topic;

  if (state_.local_topic_counts.at(topic) > 0)
    state_.local_topic_counts.at(topic)--;

  sampler_->update_elimination(state_);
}

void LeftToRightEvaluatorMod::update_addition(const Type& type,
                                              const Topic& topic,
                                              std::size_t position) {
  state_.doc_position = position;
  state_.type = type;
  state_.topic = topic;
  state_.doc_topics.at(position) = topic;
  state_.local_topic_counts.at(topic)++;

  sampler_->update_addition(state_);
}
