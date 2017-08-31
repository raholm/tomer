#ifndef TOMER_TOPIC_EVALUATOR_UTIL_H_
#define TOMER_TOPIC_EVALUATOR_UTIL_H_

#include "def.h"
#include "util.h"
#include "word_count.h"
#include "word_transformer.h"
#include "word_relation.h"

namespace tomer {

  const size_t INF_WORD_WINDOW = 0;

  struct TopicEvaluatorData {
    explicit TopicEvaluatorData(const WordCount& word_counts)
      : word_counts{word_counts},
        window_count{0} {}

    explicit TopicEvaluatorData(WordCount&& word_counts)
      : word_counts{std::move(word_counts)},
        window_count{0} {}

    WordCount word_counts;
    size_t window_count;

  };

  WordCount create_word_counts(const Corpus& topics);
  void calculate_word_counts_and_window_count(const Corpus& documents,
                                              size_t window_size,
                                              TopicEvaluatorData* data);
} // namespace tomer

#endif // TOMER_TOPIC_EVALUATOR_UTIL_H_
