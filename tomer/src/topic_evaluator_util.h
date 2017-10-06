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

  template<typename Type, typename Data>
  void calculate_word_counts_and_window_count(const Matrix<Type>& documents,
                                              size_t window_size,
                                              Data* data) {
    auto ndocs = documents.size();
    size_t nwindows, window_count = 0;
    Vector<Type> current_window(window_size);
    auto& word_counts = data->word_counts;
    size_t head_id, tail_id, nwords;

    for (unsigned i = 0; i < ndocs; ++i) {
      auto doc_words = documents.at(i);
      auto doc_length = doc_words.size();

      if (window_size == INF_WORD_WINDOW)
        nwindows = 1;
      else
        nwindows = doc_length + window_size - 1;

      window_count += nwindows;

      for (unsigned j = 1; j < (nwindows + 1); ++j) {
        if (window_size == INF_WORD_WINDOW) {
          current_window = doc_words;
          remove_duplicates(&current_window);
          nwords = current_window.size();
        } else {
          head_id = (j > window_size) ? j - window_size : 0;
          tail_id = std::min((size_t) j, (size_t) doc_words.size());
          nwords = tail_id - head_id;

          for (unsigned k = 0; k < nwords; ++k) {
            current_window.at(k) = doc_words.at(head_id + k);
          }

          remove_duplicates_inplace(&current_window, &nwords);
        }

        for (unsigned left_idx = 0; left_idx < nwords; ++left_idx) {
          auto left_word = current_window.at(left_idx);
          word_counts.update(left_word);

          for (unsigned right_idx = left_idx + 1; right_idx < nwords; ++right_idx) {
            auto right_word = current_window.at(right_idx);
            word_counts.update(left_word, right_word);
          }
        }
      }
    }

    data->window_count = window_count;
  }

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

  struct WordIndexTopicEvaluatorData {
    explicit WordIndexTopicEvaluatorData(const WordIndexCounter& word_index_counts)
      : word_counts{word_index_counts},
        window_count{0} {}

    explicit WordIndexTopicEvaluatorData(WordIndexCounter&& word_index_counts)
      : word_counts{std::move(word_index_counts)},
        window_count{0} {}

    WordIndexCounter word_counts;
    size_t window_count;

  };

  WordIndexCounter create_word_index_counts(const Matrix<WordIndex>& topics);

  struct SparseWordIndexTopicEvaluatorData {
    explicit SparseWordIndexTopicEvaluatorData(const SparseWordIndexCounter& word_index_counts)
      : word_index_counts{word_index_counts},
        window_count{0} {}

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
