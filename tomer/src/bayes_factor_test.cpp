#include <testthat.h>

#include <numeric>
#include <cmath>
#include <random>

#include "bayes_factor.h"

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

    context("Bayes Factor") {
      test_that("fill_counts fills in the correct counts") {
        size_t n_topics = 2;
        TopicCounts topic_counts{n_topics};
        IntVector topic_indicators{0, 1, 0, 0, 1, 1, 1, 0, 0};

        fill_counts(topic_indicators, &topic_counts);

        const IntVector& counts = topic_counts.counts;
        const IntMatrix& transition_counts = topic_counts.transition_counts;

        expect_true(counts.at(0) == 4);
        expect_true(counts.at(1) == 4);

        expect_true(transition_counts.at(0).at(0) == 2);
        expect_true(transition_counts.at(0).at(1) == 2);
        expect_true(transition_counts.at(1).at(0) == 2);
        expect_true(transition_counts.at(1).at(1) == 2);
      }

      test_that("compute_log_bayes_factor is correct") {
        size_t n_topics = 2;
        double beta = 0;
        IntVector topic_indicators{0, 1, 0, 0, 1, 1, 1, 0, 0};

        double expected = log(1) + log(1) + log(1) + log(1) -
          log(3 * 2 * 1) - log(3 * 2 * 1) -
          log(1) - log(1) - log(1) - log(1) +
          log(3 * 2 * 1) + log(3 * 2 * 1);
        double actual = compute_log_bayes_factor(topic_indicators, n_topics, beta);

        expect_true(equal(actual, expected));
      }

      test_that("compute_log_bayes_factor is equal to compute_log_bayes_factor_slow") {
        size_t n_topics, min_n_topics=2, max_n_topics=1000;
        size_t size, min_size = 10, max_size = 100000;
        IntVector topic_indicators;
        double beta;

        for (unsigned i = 0; i < 100; ++i) {
          n_topics = generate_n_topics(min_n_topics, max_n_topics);
          size = generate_size(min_size, max_size);
          topic_indicators = generate_topic_indicators(n_topics, size);
          beta = generate_beta();

          expect_true(equal(compute_log_bayes_factor(topic_indicators, n_topics, beta),
                            compute_log_bayes_factor_slow(topic_indicators, n_topics, beta)));
        }
      }
    }

  } // namespace test
} // namespace tomer
