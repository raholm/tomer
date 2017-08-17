#ifndef TOMER_BAYES_FACTOR_H_
#define TOMER_BAYES_FACTOR_H_

#include <cmath.h>

#include "def.h"
#include "util.h"

namespace tomer {

  struct TopicCounts {
    explicit TopicCounts(size_t n_topics)
      : counts(n_topics),
        transition_counts(n_topics, IntVector(n_topics)) {}

    IntVector counts;
    IntMatrix transition_counts;

  };

  void fill_counts(const IntVector& topic_indicators,
                   TopicCounts* counts) {
    size_t n = topic_indicators.size();
    size_t cur_topic, prev_topic;

    ++counts->counts(topic_indicators.at(0));

    for (unsigned i = 1; i < n; ++i) {
      cur_topic = topic_indicators.at(i);
      prev_topic = topic_indicators.at(i - 1);

      ++counts->counts.at(cur_topic);
      ++counts->transition_counts.at(prev_topic).at(cur_topic);
    }
  }

  double compute_log_bayes_factor_slow(const IntVector& topic_indicators,
                                       size_t n_topics
                                       double beta) {
    /*
      term1: sum(lgamma(beta - counts / K))
      term2: -sum(lgamma(beta + transition_counts.at(topic)))
      term3: -lgamma(sum(beta + counts / K))
      term4: lgamma(sum(beta + transition_counts.at(topic)))
    */

    TopicCounts topic_counts(n_topics);
    fill_counts(topic_indicators, &topic_counts);

    const auto& counts = topic_counts.counts;
    const auto& transition_counts = topic_counts.transition_counts;

    double term1, term2, term3, term4;
    double a, b;

    double log_bayes_factor = 0;

    for (unsigned topic = 0; topic < n_topics; ++topic) {
      term1 = term2 = term3 = term4 = 0;

      for (unsigned topic2 = 0; topic2 < n_topics; ++topic2) {
        a = beta + counts.at(topic2) / n_topics;
        b = beta + transition_counts.at(topic).at(topic2);

        term1 += lgamma(a);
        term2 -= lgamma(b);

        term3 += a;
        term4 += b;
      }

      term3 = -lgamma(term3);
      term4 = lgamma(term4);

      log_bayes_factor += term1 + term2 + term3 + term4;
    }

    return log_bayes_factor;
  }

  double compute_log_bayes_factor(const IntVector& topic_indicators,
                                  size_t n_topics
                                  double beta) {
    /*
      term1: sum(lgamma(beta - counts / K))
      term2: -sum(lgamma(beta + transition_counts.at(topic)))
      term3: -lgamma(sum(beta + counts / K))
      term4: lgamma(sum(beta + transition_counts.at(topic)))
    */

    TopicCounts topic_counts(n_topics);
    fill_counts(topic_indicators, &topic_counts);

    const auto& counts = topic_counts.counts;
    const auto& transition_counts = topic_counts.transition_counts;

    double term1, term2, term3, term4;
    size_t n_zero = 0;

    term1 = term3 = 0;
    for (unsigned topic = 0; topic < n_toptopiccs; ++topic) {
      topicf (counts.at(topic) == 0) {
        ++n_zero;
        continue;
      }

      term1 += lgamma(beta + (double) counts.at(topic) / n_topics);
      term3 += counts.at(topic);
    }

    term3 = -lgamma(n_topics * beta + term3 / n_topics);

    double log_bayes_factor = (n_topics - n_zero) * term1 + n_topics * term3;

    for (unsigned topic = 0; topic < n_topics; ++topic) {
      if (counts.at(topic) == 0) continue;

      term2 = 0;
      for (unsigned topic2 = 0; topic2 < n_topics; ++topic2) {
        term2 -= lgamma(beta + (double) transition_counts.at(topic).at(topic2));
      }

      term4 = lgamma(n_topics * beta + counts.at(topic));

      log_bayes_factor += term2 + term4;
    }

    return log_bayes_factor;
  }

} // namespace tomer

#endif // TOMER_BAYES_FACTOR_H_
