#ifndef TOMER_STATISTIC_H_
#define TOMER_STATISTIC_H_

#include "def.h"

namespace tomer {

  // Markovian Test (Bayes Factor)
  struct MarkovianBFTestData {
    explicit MarkovianBFTestData(size_t n_topics)
      : counts(n_topics),
        transition_counts(n_topics, IntVector(n_topics)) {}

    IntVector counts;
    IntMatrix transition_counts;

  };

  void fill_markovian_bf_test_data(const IntVector& topic_indicators,
                                   MarkovianBFTestData* counts);
  double compute_markovian_bf_test_slow(const IntVector& topic_indicators,
                                        size_t n_topics,
                                        double beta);
  double compute_markovian_bf_test(const IntVector& topic_indicators,
                                   size_t n_topics,
                                   double beta);

  // Markovian Test (Likelihood Ratio)
  struct MarkovianLRTestData {
    explicit MarkovianLRTestData(size_t n_topics)
      : counts(n_topics),
        transition_counts(n_topics, IntVector(n_topics)) {}

    IntVector counts;
    IntMatrix transition_counts;

  };

  void fill_markovian_lr_test_data(const IntVector& topic_indicators,
                                   MarkovianLRTestData* data);
  double compute_markovian_lr_test(const IntVector& topic_indicators,
                                   size_t n_topics);

  // Chunking Test (Bayes Factor)
  struct ChunkingBFTestData {
    explicit ChunkingBFTestData(size_t n_topics, size_t n_chunks)
      : global_counts(n_topics),
        local_counts(n_chunks, IntVector(n_topics)) {}

    IntVector global_counts;
    IntMatrix local_counts;

  };

  void fill_chunking_bf_test_data(const IntMatrix& topic_indicators,
                                  ChunkingBFTestData* data);
  double compute_chunking_bf_test(const IntMatrix& topic_indicators,
                                  size_t n_topics,
                                  double beta);

  // Chunking Test (Likelihood Ratio)

} // namespace tomer

#endif // TOMER_STATISTIC_H_
