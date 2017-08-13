#ifndef TOMER_LEFT_TO_RIGHT_EVALUATOR_H_
#define TOMER_LEFT_TO_RIGHT_EVALUATOR_H_

#include "def.h"
#include "left_to_right_state.h"
#include "marginal_prob_estimator.h"
#include "topic_sampler.h"
#include "type_sequence.h"
#include "type_sequence_container.h"

#include <memory>

namespace tomer {

  class LeftToRightEvaluator {
  public:
    LeftToRightEvaluator(size_t n_topics,
                         const DoubleVector& alpha,
                         double beta,
                         const IntVector& topic_counts,
                         const IntMatrix& type_topic_counts,
                         std::unique_ptr<MarginalProbEsimator>&& prob_estimator,
                         std::unique_ptr<TopicSampler>&& topic_sampler);

    ~LeftToRightEvaluator() = default;

    double evaluate(const TypeSequenceContainer& types,
                    size_t n_particles, bool resampling);

    double evaluate(const TypeSequence& types,
                    size_t n_particles, bool resampling);

  private:
    LeftToRightState state_;
    std::unique_ptr<MarginalProbEsimator> prob_estimator_;
    std::unique_ptr<TopicSampler> topic_sampler_;

    DoubleVector get_word_probabilities(const TypeSequence& types, bool resampling);

    bool is_valid(const Type& type);

    void init();
    void begin();
    void end();

    void update_addition(const Type& type, const Topic& topic, size_t position);
    void update_elimination(const Type& type, const Topic& topic, size_t position);

  };

} // namespace tomer

#endif // TOMER_LEFT_TO_RIGHT_EVALUATOR_H_
