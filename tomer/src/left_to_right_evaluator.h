#ifndef LEFT_TO_RIGHT_EVALUATOR_H
#define LEFT_TO_RIGHT_EVALUATOR_H

#include <vector>

#include "def.h"
#include "type_sequence.h"
#include "type_sequence_container.h"

using DocumentTypeSequence = TypeSequence;
using CorpusTypeSequence = TypeSequenceContainer;

class LeftToRightEvaluator {
public:
  LeftToRightEvaluator(std::size_t n_topics,
                       const DoubleVector& alpha,
                       double beta,
                       const IntVector& tokens_per_topic,
                       const IntMatrix& type_topic_counts);

  ~LeftToRightEvaluator() = default;

  double evaluate(const CorpusTypeSequence& types,
                  std::size_t n_particles,
                  bool resampling);

private:
  std::size_t n_topics_;
  double beta_;
  double beta_sum_;
  DoubleVector alpha_;
  double alpha_sum_;

  double smoothing_only_mass_;

  IntVector tokens_per_topic_;
  IntMatrix type_topic_counts_;
  DoubleVector cached_coefficients_;

  struct LocalState {
    std::size_t dense_index;
    std::size_t non_zero_topics;

    double topic_beta_mass;
    double topic_term_mass;

    IntVector doc_topics;
    IntVector topic_counts;
    IntVector topic_index;

    std::size_t type;
    IntVector type_topic_counts;

    DoubleVector topic_term_scores;
    IntVector topic_term_indices;
    IntVector topic_term_values;

  };

  DoubleVector get_word_probabilities(const DocumentTypeSequence& types,
                                      bool resampling);

  void add_topic_and_update_state_and_coefficients(LocalState& state,
                                                   uint topic,
                                                   uint position);
  void remove_topic_and_update_state_and_coefficients(LocalState& state,
                                                      uint topic);
  void add_or_remove_topic_and_update_state_and_coefficients(LocalState& state,
                                                             uint topic,
                                                             bool incr);
  void maintain_dense_index_addition(LocalState& state, uint topic) const;
  void maintain_dense_index_elimination(LocalState& state, uint topic) const;

  void update_topic_scores(LocalState& state) const;

  int sample_new_topic(LocalState& state) const;
  double sample_from_uniform() const;

};

#endif // LEFT_TO_RIGHT_EVALUATOR_H
