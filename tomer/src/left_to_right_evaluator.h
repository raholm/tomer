#ifndef LEFT_TO_RIGHT_EVALUATOR_H
#define LEFT_TO_RIGHT_EVALUATOR_H

#include "def.h"
#include "type_sequence.h"
#include "type_sequence_container.h"

using CorpusTypeSequence = TypeSequenceContainer;
using DocumentTypeSequence = TypeSequence;

class LeftToRightEvaluator {
public:
  LeftToRightEvaluator(uint n_topics, double beta, double beta_sum,
                       const DoubleVector& alpha, double alpha_sum,
                       const UIntVector& tokens_per_topic,
                       const UIntMatrix& type_topic_counts);

  ~LeftToRightEvaluator() = default;

  double evaluate(const CorpusTypeSequence& types,
                  uint n_particles,
                  bool resampling) const;

private:
  uint n_topics_;
  double beta_;
  double beta_sum_;
  DoubleVector alpha_;
  double alpha_sum_;
  double smoothing_only_mass_;

  uint topic_mask_;
  uint topic_bits_;

  UIntVector tokens_per_topic_;
  DoubleVector cached_coefficients_;

  UIntMatrix type_topic_counts_;

  struct LocalState {
    int dense_index;
    int non_zero_topics;

    double topic_beta_mass;
    double topic_term_mass;

    UIntVector doc_topics;

    UIntVector topic_counts;
    UIntVector topic_index;

    UIntVector type_topic_counts;

    DoubleVector topic_term_scores;
    UIntVector topic_term_indices;
    UIntVector topic_term_values;

  };

  DoubleVector get_word_probabilities(const DocumentTypeSequence& types,
                                      bool resampling) const;

  void add_topic_and_update_state_and_coefficients(LocalState& state,
                                                   uint topic,
                                                   uint position) const;
  void remove_topic_and_update_state_coefficients(LocalState& state,
                                                  uint topic) const;
  void add_or_remove_topic_and_update_state_coefficients(LocalState& state,
                                                         uint topic,
                                                         bool incr) const;
  void maintain_dense_index_addition(LocalState& state, uint topic) const;
  void maintain_dense_index_elimination(LocalState& state, uint topic) const;

  void update_topic_scores(LocalState& state) const;

  int sample_new_topic(LocalState& state) const;
  double sample_from_uniform() const;
};

#endif // LEFT_TO_RIGHT_EVALUATOR_H
