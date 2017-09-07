#include <testthat.h>

#include <numeric>
#include <cmath>
#include <random>

#include "test_helper.h"
#include "statistic.h"

namespace tomer {
  namespace test {

    template<typename T>
    static bool equal(T a, T b) {
      // return (std::fabs(a - b) <= std::numeric_limits<T>::epsilon() * std::fmax(std::fabs(a), std::fabs(b)));
      return std::fabs(a - b) <= 0.1;
    }

    static IntVector generate_topic_indicators(size_t n_topics, size_t size) {
      static std::default_random_engine generator;
      static std::uniform_int_distribution<size_t> dist(0, n_topics - 1);
      static auto gen = std::bind(dist, generator);

      IntVector data(size);
      std::generate(data.begin(), data.end(), gen);
      return data;
    }

    static size_t generate_n_topics(size_t min, size_t max) {
      static std::default_random_engine generator;
      static std::uniform_int_distribution<size_t> dist(min, max);
      return dist(generator);
    }

    static size_t generate_size(size_t min, size_t max) {
      static std::default_random_engine generator;
      static std::uniform_int_distribution<size_t> dist(min, max);
      return dist(generator);
    }

    static double generate_beta() {
      static std::default_random_engine generator;
      static std::uniform_real_distribution<double> dist(0, 1);
      return dist(generator);
    }

    context("Sequence Bayes Factor") {
      test_that("fill_sequence_bf_test_data fills in the correct counts") {
        size_t n_topics = 2;
        SequenceBFTestData topic_counts{n_topics};
        IntVector topic_indicators{0, 1, 0, 0, 1, 1, 1, 0, 0};

        fill_sequence_bf_test_data(topic_indicators, &topic_counts);

        const IntVector& counts = topic_counts.counts;
        const IntMatrix& transition_counts = topic_counts.transition_counts;

        expect_true(counts.at(0) == 4);
        expect_true(counts.at(1) == 4);

        expect_true(transition_counts.at(0).at(0) == 2);
        expect_true(transition_counts.at(0).at(1) == 2);
        expect_true(transition_counts.at(1).at(0) == 2);
        expect_true(transition_counts.at(1).at(1) == 2);
      }

      test_that("compute_sequence_bf_test is correct") {
        size_t n_topics = 2;
        double beta = 0;
        IntVector topic_indicators{0, 1, 0, 0, 1, 1, 1, 0, 0};

        double expected = log(1) + log(1) + log(1) + log(1) -
          log(3 * 2 * 1) - log(3 * 2 * 1) -
          log(1) - log(1) - log(1) - log(1) +
          log(3 * 2 * 1) + log(3 * 2 * 1);
        double actual = compute_sequence_bf_test(topic_indicators, n_topics, beta);

        expect_true(is_equal(actual, expected));

        topic_indicators = {1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1};
        actual = compute_sequence_bf_test(topic_indicators, n_topics, 1);
        expected = log((double) 8 / 7);
        expect_true(is_equal(actual, expected));
      }

      test_that("compute_sequence_bf_test is equal to compute_sequence_bf_test_slow") {
        size_t n_topics, min_n_topics=2, max_n_topics=1000;
        size_t size, min_size = 10, max_size = 100000;
        IntVector topic_indicators;
        double beta;

        for (unsigned i = 0; i < 100; ++i) {
          n_topics = generate_n_topics(min_n_topics, max_n_topics);
          size = generate_size(min_size, max_size);
          topic_indicators = generate_topic_indicators(n_topics, size);
          beta = generate_beta();

          expect_true(equal(compute_sequence_bf_test(topic_indicators, n_topics, beta),
                            compute_sequence_bf_test_slow(topic_indicators, n_topics, beta)));
        }
      }
    }

    context("Chunking Bayes Factor") {

    }

    context("Chunking Likelihood Ratio") {
      test_that("fill_chunking_lr_test_data fills in the correct counts") {
        size_t n_topics = 2;
        ChunkingLRTestData topic_counts{n_topics};
        IntVector topic_indicators{0, 1, 0, 0, 1, 1, 1, 0, 0};

        fill_chunking_lr_test_data(topic_indicators, &topic_counts);

        const IntVector& counts = topic_counts.counts;
        const IntMatrix& transition_counts = topic_counts.transition_counts;

        expect_true(counts.at(0) == 4);
        expect_true(counts.at(1) == 4);

        expect_true(transition_counts.at(0).at(0) == 2);
        expect_true(transition_counts.at(0).at(1) == 2);
        expect_true(transition_counts.at(1).at(0) == 2);
        expect_true(transition_counts.at(1).at(1) == 2);
      }

      test_that("compute_chunking_lr_test is correct") {
        size_t n_topics = 2;
        double beta = 1;
        IntVector topic_indicators{0, 1, 0, 0, 1, 1, 1, 0, 0};
        int nd = topic_indicators.size();
        int nd1 = nd - 1;
        double denom = nd + n_topics * beta;
        denom *= denom;

        double expected = 2 *
          (2 * log(2 / (nd1 * (4 + beta) * (4 + beta) / denom)) + // (0 -> 0)
           2 * log(2 / (nd1 * (4 + beta) * (4 + beta) / denom)) + // (0 -> 1)
           2 * log(2 / (nd1 * (4 + beta) * (4 + beta) / denom)) + // (1 -> 0)
           2 * log(2 / (nd1 * (4 + beta) * (4 + beta) / denom)) // (1 -> 1)
           );
        double actual = compute_chunking_lr_test(topic_indicators, n_topics, beta);
        expect_true(is_equal(actual, expected));

        topic_indicators = {1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1};
        nd = topic_indicators.size();
        nd1 = nd - 1;
        denom = nd + n_topics * beta;
        denom *= denom;

        expected = 2 *
          (2 * log(2 / (nd1 * (4 + beta) * (4 + beta) / denom)) + // (0 -> 0)
           2 * log(2 / (nd1 * (4 + beta) * (6 + beta) / denom)) + // (0 -> 1)
           2 * log(2 / (nd1 * (6 + beta) * (4 + beta) / denom)) + // (1 -> 0)
           4 * log(4 / (nd1 * (6 + beta) * (6 + beta) / denom)) // (1 -> 1)
           );
        actual = compute_chunking_lr_test(topic_indicators, n_topics, beta);
        expect_true(is_equal(actual, expected));
      }

    }

  } // namespace test
} // namespace tomer
