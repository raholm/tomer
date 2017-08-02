#ifndef LEFT_TO_RIGHT_EVALUATOR_MOD_H
#define LEFT_TO_RIGHT_EVALUATOR_MOD_H

#include "def.h"
#include "left_to_right_state.h"
#include "topic_sampler.h"
#include "type_sequence.h"
#include "type_sequence_container.h"

#include <memory>

using DocumentTypeSequence = TypeSequence;
using CorpusTypeSequence = TypeSequenceContainer;

class LeftToRightEvaluatorMod {
public:
  LeftToRightEvaluatorMod(std::size_t n_topics,
                          const DoubleVector& alpha,
                          double beta,
                          const IntVector& topic_counts,
                          const IntMatrix& type_topic_counts,
                          std::unique_ptr<TopicSampler>&& sampler);


  double evaluate(const CorpusTypeSequence& types,
                  std::size_t n_particles, bool resampling);

  double evaluate(const DocumentTypeSequence& types,
                  std::size_t n_particles, bool resampling);

private:
  LeftToRightState state_;
  std::unique_ptr<TopicSampler> sampler_;

  DoubleVector get_word_probabilities(const DocumentTypeSequence& types, bool resampling);

  bool is_valid(const Type& type);

  void update_addition(const Type& type, const Topic& topic, std::size_t position);
  void update_elimination(const Type& type, const Topic& topic, std::size_t position);

};

#endif // LEFT_TO_RIGHT_EVALUATOR_MOD_H
