#ifndef TOMER_BAYES_FACTOR_H_
#define TOMER_BAYES_FACTOR_H_

#include <cmath>

#include "def.h"

namespace tomer {

  struct TopicCounts {
    explicit TopicCounts(size_t n_topics)
      : counts(n_topics),
        transition_counts(n_topics, IntVector(n_topics)) {}

    IntVector counts;
    IntMatrix transition_counts;

  };

  void fill_counts(const IntVector& topic_indicators,
                   TopicCounts* counts);
  double compute_log_bayes_factor_slow(const IntVector& topic_indicators,
                                       size_t n_topics,
                                       double beta);
  double compute_log_bayes_factor(const IntVector& topic_indicators,
                                  size_t n_topics,
                                  double beta);

} // namespace tomer

#endif // TOMER_BAYES_FACTOR_H_
