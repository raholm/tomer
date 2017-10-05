#ifndef TOMER_TOPIC_EVALUATOR_UTIL_H_
#define TOMER_TOPIC_EVALUATOR_UTIL_H_

#include "def.h"
#include "util.h"
#include "counter.h"
#include "word_transformer.h"
#include "word_relation.h"
#include "tokenizer.h"

namespace tomer {

  const size_t INF_WORD_WINDOW = 0;

  struct WordTopicEvaluatorData {
    explicit WordTopicEvaluatorData(const WordCounter& word_counts)
      : word_counts{word_counts},
        window_count{0} {}

    explicit WordTopicEvaluatorData(WordCounter&& word_counts)
      : word_counts{std::move(word_counts)},
        window_count{0} {}

    WordCounter word_counts;
    size_t window_count;

  };

  WordCounter create_word_counts(const Corpus& topics);
  void calculate_word_counts_and_window_count(const Corpus& documents,
                                              size_t window_size,
                                              WordTopicEvaluatorData* data);

  struct WordIndexTopicEvaluatorData {
    explicit WordIndexTopicEvaluatorData(const WordIndexCounter& word_index_counts)
      : word_index_counts{word_index_counts},
        window_count{0} {}

    explicit WordIndexTopicEvaluatorData(WordIndexCounter&& word_index_counts)
      : word_index_counts{std::move(word_index_counts)},
        window_count{0} {}

    WordIndexCounter word_index_counts;
    size_t window_count;

  };

  WordIndexCounter create_word_index_counts(const Matrix<WordIndex>& topics);
  void calculate_word_index_counts_and_window_count(const Matrix<WordIndex>& documents,
                                                    size_t window_size,
                                                    WordIndexTopicEvaluatorData* data);

  struct SparseWordIndexTopicEvaluatorData {
    explicit SparseWordIndexTopicEvaluatorData(SparseWordIndexCounter&& word_index_counts)
      : word_index_counts{std::move(word_index_counts)},
        window_count{0} {}

    SparseWordIndexCounter word_index_counts;
    size_t window_count;

  };

  void calculate_word_counts_and_window_count(const String& filename,
                                              size_t window_size,
                                              const FixedWordIndexTokenizer& tokenizer,
                                              SparseWordIndexTopicEvaluatorData* data);

} // namespace tomer

#endif // TOMER_TOPIC_EVALUATOR_UTIL_H_
