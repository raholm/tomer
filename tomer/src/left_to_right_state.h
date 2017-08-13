#ifndef TOMER_LEFT_TO_RIGHT_STATE_H_
#define TOMER_LEFT_TO_RIGHT_STATE_H_

#include "def.h"

#include <numeric>

namespace tomer {

  struct LeftToRightState {
    LeftToRightState(std::size_t n_topics,
                     const DoubleVector& alpha,
                     double beta,
                     const IntVector& topic_counts,
                     const IntMatrix& type_topic_counts) :
      n_topics{n_topics},
      n_types{type_topic_counts.size()},
      alpha{alpha},
      alpha_sum{std::accumulate(alpha.cbegin(), alpha.cend(), 0.0)},
      beta{beta},
      beta_sum{type_topic_counts.size() * beta},
      topic_counts{topic_counts},
      type_topic_counts{type_topic_counts},
      local_topic_counts(n_topics),
      current_type_topic_counts{} {}

    ~LeftToRightState() = default;

    const std::size_t n_topics;
    const std::size_t n_types;

    const DoubleVector alpha;
    const double alpha_sum;

    const double beta;
    const double beta_sum;

    const IntVector topic_counts;
    const IntMatrix type_topic_counts;

    // Variables that changes over time
    std::size_t n_tokens_seen;
    std::size_t doc_position;
    IntVector doc_topics;

    Type type;
    Topic topic;

    IntVector local_topic_counts;
    // TODO: Perhaps have this as a pointer to const to prevent copying
    IntVector current_type_topic_counts;
  };

} // namespace tomer

#endif // TOMER_LEFT_TO_RIGHT_STATE_H_
