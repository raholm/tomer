#ifndef TOMER_STATISTIC_H_
#define TOMER_STATISTIC_H_

#include "def.h"

namespace tomer {

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

  struct GTestData {
    explicit GTestData(size_t n_topics)
      : counts(n_topics),
        transition_counts(n_topics, IntVector(n_topics)) {}

    IntVector counts;
    IntMatrix transition_counts;

  };

  void fill_g_test_data(const IntVector& topic_indicators,
                        GTestData* data);
  double compute_gscore(const IntVector& topic_indicators,
                        size_t n_topics,
                        double beta);

} // namespace tomer

#endif // TOMER_STATISTIC_H_
