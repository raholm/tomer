#include "statistic.h"

#include <numeric>
#include <cmath>

namespace tomer {

  // Markovian Test (Bayes Factor)
  void fill_markovian_bf_test_data(const IntVector& topic_indicators,
                                   MarkovianBFTestData* counts) {
    size_t n = topic_indicators.size();
    size_t cur_topic, prev_topic;

    for (unsigned i = 1; i < n; ++i) {
      cur_topic = topic_indicators.at(i);
      prev_topic = topic_indicators.at(i - 1);

      ++counts->counts.at(cur_topic);
      ++counts->transition_counts.at(prev_topic).at(cur_topic);
    }
  }

  double compute_markovian_bf_test_slow(const IntVector& topic_indicators,
                                        size_t n_topics,
                                        double beta) {
    /*
      term1: sum(lgamma(beta - counts / K))
      term2: -sum(lgamma(beta + transition_counts.at(topic)))
      term3: -lgamma(sum(beta + counts / K))
      term4: lgamma(sum(beta + transition_counts.at(topic)))
    */

    MarkovianBFTestData topic_counts(n_topics);
    fill_markovian_bf_test_data(topic_indicators, &topic_counts);

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

  double compute_markovian_bf_test(const IntVector& topic_indicators,
                                   size_t n_topics,
                                   double beta) {
    /*
      term1: sum(lgamma(beta - counts / K) - lgamma(beta + transition_counts.at(topic)))
      term2: -lgamma(sum(beta + counts / K))
      term3: lgamma(sum(beta + transition_counts.at(topic)))
    */

    MarkovianBFTestData topic_counts(n_topics);
    fill_markovian_bf_test_data(topic_indicators, &topic_counts);

    const auto& counts = topic_counts.counts;
    const auto& transition_counts = topic_counts.transition_counts;

    double term1, term2, term3;
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

  // Markovian Test (Likelihood Ratio)
  void fill_markovian_lr_test_data(const IntVector& topic_indicators,
                                  MarkovianLRTestData* data) {
    MarkovianBFTestData tmp_data(data->counts.size());
    fill_markovian_bf_test_data(topic_indicators, &tmp_data);
    data->counts = std::move(tmp_data.counts);
    data->transition_counts = std::move(tmp_data.transition_counts);
  }

  double compute_markovian_lr_test(const IntVector& topic_indicators,
                                   size_t n_topics,
                                   double beta) {
    MarkovianLRTestData data(n_topics);
    fill_markovian_lr_test_data(topic_indicators, &data);

    const auto& counts = data.counts;
    const auto& transition_counts = data.transition_counts;

    double score = 0;
    double beta_mul = beta * n_topics;
    size_t n = topic_indicators.size();
    size_t n1 = n - 1;
    double denom = n1 + beta_mul;

    for (unsigned i = 0; i < n_topics; ++i) {
      if (counts.at(i) == 0) continue;
      double ni = counts.at(i);
      double a = (ni + beta) / denom;

      for (unsigned j = 0; j < n_topics; ++j) {
        if (transition_counts.at(i).at(j) == 0) continue;
        double nij = transition_counts.at(i).at(j);
        double nj = counts.at(j);
        double b = (nj + beta) / denom;

        score += nij * log(nij / (n1 * a * b));
      }
    }

    return 2 * score;
  }

  // Chunking Test (Bayes Factor)
  void fill_chunking_bf_test_data(const IntMatrix& topic_indicators,
                                  ChunkingBFTestData* data) {
    auto n_chunks = topic_indicators.size();

    auto& global_counts = data->global_counts;
    auto& local_counts = data->local_counts;

    for (unsigned chunk_idx = 0; chunk_idx < n_chunks; ++chunk_idx) {
      const auto& current_topic_indicators = topic_indicators.at(chunk_idx);
      auto chunk_size = current_topic_indicators.size();

      for (unsigned topic_idx = 0; topic_idx < chunk_size; ++topic_idx) {
        auto current_topic = current_topic_indicators.at(topic_idx);
        ++global_counts.at(current_topic);
        ++local_counts.at(chunk_idx).at(current_topic);
      }
    }
  }

  double compute_chunking_bf_test(const IntMatrix& topic_indicators,
                                  size_t n_topics,
                                  double beta) {
    auto n_chunks = topic_indicators.size();
    IntVector chunk_sizes(n_chunks);
    size_t n = 0;

    for (unsigned i = 0; i < n_chunks; ++i) {
      chunk_sizes.at(i) = topic_indicators.at(i).size();
      n += chunk_sizes.at(i);
    }

    ChunkingBFTestData data(n_topics, n_chunks);
    fill_chunking_bf_test_data(topic_indicators, &data);

    const auto& global_counts = data.global_counts;
    const auto& local_counts = data.local_counts;

    double term1 = 0, term2 = 0, term3 = 0, term4 = 0, term5;
    double beta_mul = beta * n_topics;

    // Compute  term1
    for (unsigned k = 0; k < n_topics; ++k) {
      if (global_counts.at(k) == 0) continue;
      term1 += lgamma(beta + global_counts.at(k));
    }

    // Compute term2
    term2 = -lgamma(beta_mul + n);

    // Compute term3
    if (beta > 0 && beta_mul > 0)
      term3 = (n_chunks - 1) * (n_topics * lgamma(beta) - lgamma(beta_mul));

    // Compute term5
    term5 = std::accumulate(chunk_sizes.cbegin(),
                            chunk_sizes.cend(),
                            0.0,
                            [beta_mul](const double& acc, const size_t& chunk_size) {
                              return acc + lgamma(beta_mul + chunk_size);
                            });

    // Compute term4
    size_t n_zero_global = 0, n_zero_local = 0;

    for (unsigned k = 0; k < n_topics; ++k) {
      if (global_counts.at(k) == 0) {
        ++n_zero_global;
        continue;
      }

      for (unsigned c = 0; c < n_chunks; ++c) {
        const auto& current_counts = local_counts.at(c);
        if (current_counts.at(k) == 0) {
          ++n_zero_local;
          continue;
        }

        term4 -= lgamma(beta + (double) current_counts.at(k));
      }
    }

    if (beta > 0 && (n_zero_global > 0 || n_zero_local > 0)) {
      term4 -= (n_zero_global * n_chunks + n_zero_local) * lgamma(beta);
    }

    double score = term1 + term2 + term3 + term4 + term5;
    return score;
  }

} // namespace tomer
