#include "topic_sampler.h"

#include <iostream>
#include <algorithm>

LDATopicSampler::LDATopicSampler()
  : rand_dev_{}, gen_{rand_dev_()}, dist_{}
{

}

void LDATopicSampler::init(const LeftToRightState& state) {

}

void LDATopicSampler::begin(const LeftToRightState& state) {

}

void LDATopicSampler::end(const LeftToRightState& state) {

}

void LDATopicSampler::update_addition(const LeftToRightState& state) {

}

void LDATopicSampler::update_elimination(const LeftToRightState& state) {

}

double LDATopicSampler::get_word_prob(const LeftToRightState& state) {
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

Topic LDATopicSampler::sample_topic(const LeftToRightState& state) {
  auto probs = get_topic_probabilities(state);
  dist_ = std::discrete_distribution<Topic>{probs.begin(), probs.end()};
  Topic topic = dist_(gen_);
  return topic;
}

DoubleVector LDATopicSampler::get_topic_probabilities(const LeftToRightState& state) {
  DoubleVector probs(state.n_topics);
  double left_numerator, left_denominator;
  double left_factor, right_factor;

  for (unsigned topic = 0; topic < state.n_topics; ++topic) {
    left_numerator = state.current_type_topic_counts.at(topic) + state.beta;
    left_denominator = state.topic_counts.at(topic) + state.beta_sum;
    left_factor = left_numerator / left_denominator;

    right_factor = state.local_topic_counts.at(topic) + state.alpha.at(topic);

    // right_factor = std::count_if(state_.doc_topics.cbegin(),
    //                              state_.doc_topics.cbegin() + state_.position,
    //                              [topic](const auto& t) {
    //                                return t == topic;
    //                              }) + alpha_.at(topic);

    probs.at(topic) = left_factor * right_factor;
  }

  return probs;
}

SparseLDATopicSampler::SparseLDATopicSampler() :
  rand_dev_{},
  gen_{rand_dev_()},
  dist_{0.0, 1.0},
  has_begun_{false},
  smoothing_only_mass_{0},
  cached_coefficients_{},
  topic_term_scores_{},
  topic_beta_mass_{0},
  topic_term_mass_{0},
  topic_index_{},
  dense_index_{0},
  non_zero_topics_{0}
{}

void SparseLDATopicSampler::init(const LeftToRightState& state) {
  cached_coefficients_ = DoubleVector(state.n_topics);
  smoothing_only_mass_ = 0;

  for (unsigned topic = 0; topic < state.n_topics; ++topic) {
    double denom = (state.topic_counts.at(topic) + state.beta_sum);
    smoothing_only_mass_ += state.alpha.at(topic) * state.beta / denom;
    cached_coefficients_.at(topic) = state.alpha.at(topic) / denom;
  }
}

void SparseLDATopicSampler::begin(const LeftToRightState& state) {
  if (has_begun_) return;

  dense_index_ = 0;
  non_zero_topics_ = 0;

  topic_beta_mass_ = 0;
  topic_term_mass_ = 0;

  topic_index_ = IntVector(state.n_topics);
  topic_term_scores_ = DoubleVector(state.n_topics);


  has_begun_ = true;
}

void SparseLDATopicSampler::end(const LeftToRightState& state) {
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

  for (unsigned i = 0; i < state.n_topics; ++i) {
    topic_term_scores_.at(i) = 0;
    topic_index_.at(i) = 0;
  }
}

void SparseLDATopicSampler::update_addition(const LeftToRightState& state) {
  update(state, true);
}

void SparseLDATopicSampler::update_elimination(const LeftToRightState& state) {
  update(state, false);
}

void SparseLDATopicSampler::update(const LeftToRightState& state,
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


void SparseLDATopicSampler::maintain_dense_index_addition(const LeftToRightState& state) {
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

void SparseLDATopicSampler::maintain_dense_index_elimination(const LeftToRightState& state) {
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

double SparseLDATopicSampler::get_word_prob(const LeftToRightState& state) {
  update_topic_scores(state);
  return  (smoothing_only_mass_ + topic_beta_mass_ + topic_term_mass_) /
    (state.alpha_sum + state.n_tokens_seen);
}

void SparseLDATopicSampler::update_topic_scores(const LeftToRightState& state) {
  int index = 0;
  int current_topic, current_value;
  double score;

  topic_term_mass_ = 0.0;

  while (index < state.current_type_topic_counts.size() &&
         state.current_type_topic_counts.at(index) > 0) {
    current_topic = index;
    current_value = state.current_type_topic_counts.at(current_topic);

    score = cached_coefficients_.at(current_topic) * current_value;

    topic_term_mass_ += score;
    topic_term_scores_.at(current_topic) = score;

    index++;
  }
}

Topic SparseLDATopicSampler::sample_topic(const LeftToRightState& state) {
  update_topic_scores(state);
  double sample = dist_(gen_) * (smoothing_only_mass_ +
                                 topic_beta_mass_ +
                                 topic_term_mass_);
  double orig_sample = sample;

  int topic, new_topic = -1;

  if (sample < topic_term_mass_) {
    topic = -1;

    while (sample > 0) {
      ++topic;
      sample -= topic_term_scores_.at(topic);
    }

    new_topic = topic;
  } else {
    sample -= topic_term_mass_;

    if (sample < topic_beta_mass_) {
      sample /= state.beta;

      for (dense_index_ = 0; dense_index_ < non_zero_topics_; ++dense_index_) {
        topic = topic_index_.at(dense_index_);

        sample -= state.local_topic_counts.at(topic) / (state.topic_counts.at(topic) + state.beta_sum);

        if (sample <= 0.0) {
          new_topic = topic;
          break;
        }
      }
    } else {
      sample -= topic_beta_mass_;
      sample /= state.beta;
      new_topic = 0;
      sample -= state.alpha.at(new_topic) / (state.topic_counts.at(new_topic) + state.beta_sum);

      while (sample > 0.0) {
        ++new_topic;
        sample -= state.alpha.at(new_topic) / (state.topic_counts.at(new_topic) + state.beta_sum);
      }
    }
  }

  if (new_topic == -1) {
    // std::cerr << "Sampling error: " << orig_sample << " " << sample << " "
    //           << smoothing_only_mass_ << " " << topic_beta_mass << " "
    //           << topic_term_mass << std::endl;

    new_topic = state.n_topics - 1; // TODO is this appropriate
  }

  return new_topic;
}
