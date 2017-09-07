#ifndef TOMER_STATISTIC_H_
#define TOMER_STATISTIC_H_

#include "def.h"

namespace tomer {

  // Sequence Test (Bayes Factor)
  struct SequenceBFTestData {
    explicit SequenceBFTestData(size_t n_topics)
      : counts(n_topics),
        transition_counts(n_topics, IntVector(n_topics)) {}

    IntVector counts;
    IntMatrix transition_counts;

  };

  void fill_sequence_bf_test_data(const IntVector& topic_indicators,
                                  SequenceBFTestData* counts);
  double compute_sequence_bf_test_slow(const IntVector& topic_indicators,
                                       size_t n_topics,
                                       double beta);
  double compute_sequence_bf_test(const IntVector& topic_indicators,
                                  size_t n_topics,
                                  double beta);

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
  struct ChunkingLRTestData {
    explicit ChunkingLRTestData(size_t n_topics)
      : counts(n_topics),
        transition_counts(n_topics, IntVector(n_topics)) {}

    IntVector counts;
    IntMatrix transition_counts;

  };

  void fill_chunking_lr_test_data(const IntVector& topic_indicators,
                                  ChunkingLRTestData* data);
  double compute_chunking_lr_test(const IntVector& topic_indicators,
                                  size_t n_topics,
                                  double beta);

} // namespace tomer

#endif // TOMER_STATISTIC_H_
