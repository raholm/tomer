#include "bayes_factor.h"

namespace tomer {

  void fill_counts(const IntVector& topic_indicators,
                   TopicCounts* counts) {
    size_t n = topic_indicators.size();
    size_t cur_topic, prev_topic;

    for (unsigned i = 1; i < n; ++i) {
      cur_topic = topic_indicators.at(i);
      prev_topic = topic_indicators.at(i - 1);

      ++counts->counts.at(cur_topic);
      ++counts->transition_counts.at(prev_topic).at(cur_topic);
    }
  }

  double compute_log_bayes_factor_slow(const IntVector& topic_indicators,
                                       size_t n_topics,
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

    for (unsigned r = 0; r < n_topics; ++r) {
      term1 = term2 = term3 = term4 = 0;

      for (unsigned c = 0; c < n_topics; ++c) {
        a = beta + (double) transition_counts.at(r).at(c);
        b = beta + (double) counts.at(c) / n_topics;

        if (a > 0) {
          term1 += lgamma(a);
          term4 += a;
        }

        if (b > 0) {
          term2 -= lgamma(b);
          term3 += b;
        }
      }

      if (term3 > 0) {
        term3 = lgamma(term3);
      }

      if (term4 > 0) {
        term4 = -lgamma(term4);
      }

      log_bayes_factor += term1 + term2 + term3 + term4;
    }

    return log_bayes_factor;
  }

  double compute_log_bayes_factor(const IntVector& topic_indicators,
                                  size_t n_topics,
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

    double beta_mul = n_topics * beta;

    term1 = term2 = term3 = 0;

    for (unsigned i = 0; i < n_topics; ++i) {
      if (counts.at(i) == 0) {
        ++n_zero;
        continue;
      }

      auto term1_right = lgamma(beta + (double) counts.at(i) / n_topics);

      for (unsigned j = 0; j < n_topics; ++j) {
        auto term1_left = beta + (double) transition_counts.at(i).at(j);
        term1 += (term1_left > 0) ? lgamma(term1_left) - term1_right : -term1_right;
      }

      term2 += counts.at(i);

      term3 -= lgamma(beta_mul + counts.at(i));
    }

    term2 = n_topics * lgamma(beta_mul + (double) term2 / n_topics);

    if (beta_mul > 0 && n_zero > 0)
      term3 -= n_zero * lgamma(beta_mul);

    double log_bayes_factor = term1 + term2 + term3;
    return log_bayes_factor;
  }

} // namespace tomer
