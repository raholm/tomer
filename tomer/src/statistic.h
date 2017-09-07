#ifndef TOMER_STATISTIC_H_
#define TOMER_STATISTIC_H_

#include "def.h"

namespace tomer {

  // Sequence Test (Bayes Factor)
  struct SequenceTestData {
    explicit SequenceTestData(size_t n_topics)
      : counts(n_topics),
      transition_counts(n_topics, IntVector(n_topics)) {}

    IntVector counts;
    IntMatrix transition_counts;

  };

  void fill_sequence_test_data(const IntVector& topic_indicators,
                               SequenceTestData* counts);
  double compute_sequence_test_slow(const IntVector& topic_indicators,
                                    size_t n_topics,
                                    double beta);
  double compute_sequence_test(const IntVector& topic_indicators,
                               size_t n_topics,
                                  double beta);

  // Chunking Test (Bayes Factor)
  struct ChunkingTestData {
    explicit ChunkingTestData(size_t n_topics, size_t n_chunks)
      : global_counts(n_topics),
        local_counts(n_chunks, IntVector(n_topics)) {}

    IntVector global_counts;
    IntMatrix local_counts;
  };

  void fill_chunking_test_data(const IntMatrix& topic_indicators,
                               ChunkingTestData* data);
  double compute_chunking_test(const IntMatrix& topic_indicators,
                               size_t n_topics,
                               double beta);

  // Chunking Test (Likelihood Ratio)
  struct GScoreTestData {
    explicit GScoreTestData(size_t n_topics)
      : counts(n_topics),
        transition_counts(n_topics, IntVector(n_topics)) {}

    IntVector counts;
    IntMatrix transition_counts;

  };

  void fill_gscore_test_data(const IntVector& topic_indicators,
                             GScoreTestData* data);
  double compute_gscore_test(const IntVector& topic_indicators,
                             size_t n_topics,
                             double beta);

} // namespace tomer

#endif // TOMER_STATISTIC_H_
