#include "statistic.h"

#include <numeric>
#include <cmath>

namespace tomer {

  void fill_chunking_test_data(const IntMatrix& topic_indicators,
                               ChunkingTestData* data) {
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

  double compute_chunking_test(const IntMatrix& topic_indicators,
                               size_t n_topics,
                               double beta) {
    auto n_chunks = topic_indicators.size();
    IntVector chunk_sizes(n_chunks);
    size_t n = 0;

    for (unsigned i = 0; i < n_chunks; ++i) {
      chunk_sizes.at(i) = topic_indicators.at(i).size();
      n += chunk_sizes.at(i);
    }

    ChunkingTestData data(n_topics, n_chunks);
    fill_chunking_test_data(topic_indicators, &data);

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
    term5 = lgamma(beta_mul + (double) std::accumulate(chunk_sizes.cbegin(),
                                                       chunk_sizes.cend(),
                                                       0));

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

    // for (unsigned c = 0; c < n_chunks; ++c) {
    //   const auto& current_counts = local_counts.at(c);
    //   term5 += chunk_sizes.at(c);

    //   for (unsigned k = 0; k < n_topics; ++k) {
    //     if (current_counts.at(k) == 0) continue;
    //     auto a = beta + (double) current_counts.at(k);
    //     term4 -= lgamma(a);
    //   }
    // }

    double score = term1 + term2 + term3 + term4 + term5;
    return score;
  }

  void fill_g_test_data(const IntVector& topic_indicators,
                        GScoreTestData* data) {
    size_t n = topic_indicators.size();
    size_t cur_topic, prev_topic;

    auto& counts = data->counts;
    auto& transition_counts = data->transition_counts;

    for (unsigned i = 1; i < n; ++i) {
      cur_topic = topic_indicators.at(i);
      prev_topic = topic_indicators.at(i - 1);

      ++counts.at(cur_topic);
      ++transition_counts.at(prev_topic).at(cur_topic);
    }
  }

  double compute_gscore(const IntVector& topic_indicators,
                        size_t n_topics,
                        double beta) {
    GScoreTestData data(n_topics);
    fill_g_test_data(topic_indicators, &data);

    const auto& counts = data.counts;
    const auto& transition_counts = data.transition_counts;

    double score = 0;
    double beta_mul = beta * n_topics;
    size_t n = topic_indicators.size();
    size_t n1 = n - 1;
    double denom = n + beta_mul;

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

    return score;
  }

} // namespace tomer