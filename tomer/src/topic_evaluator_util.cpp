#include "topic_evaluator_util.h"

namespace tomer {

  WordCount create_word_counts(const Corpus& topics) {
    auto ntopics = topics.size();

    WordToIndexTransformer transformer;
    TopicWordIndexRelationMap relations;

    for (unsigned i = 0; i < ntopics; ++i) {
      auto topic_words = topics.at(i);
      transformer.update(topic_words);
      auto topic_word_indexes = transformer.transform(topic_words);

      for (unsigned j = 0; j < topic_words.size(); ++j) {
        relations.update(topic_word_indexes.at(j), topic_word_indexes);
      }
    }

    return WordCount(transformer, relations);
  }

  void calculate_word_counts_and_window_count(const Corpus& documents,
                                              size_t window_size,
                                              TopicEvaluatorData* data) {
    auto ndocs = documents.size();
    size_t nwindows, window_count = 0;
    WordWindow words_in_window(window_size);
    WordCount& word_counts = data->word_counts;
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
          words_in_window = doc_words;
          remove_duplicates(&words_in_window);
          nwords = words_in_window.size();
        } else {
          head_id = (j > window_size) ? j - window_size : 0;
          tail_id = std::min((size_t) j, (size_t) doc_words.size());
          nwords = tail_id - head_id;

          for (unsigned k = 0; k < nwords; ++k) {
            words_in_window.at(k) = doc_words.at(head_id + k);
          }

          remove_duplicates_inplace(&words_in_window, &nwords);
        }

        for (unsigned left_idx = 0; left_idx < nwords; ++left_idx) {
          auto left_word = words_in_window.at(left_idx);
          word_counts.update(left_word);

          for (unsigned right_idx = left_idx + 1; right_idx < nwords; ++right_idx) {
            auto right_word = words_in_window.at(right_idx);
            word_counts.update(left_word, right_word);
          }
        }
      }
    }

    data->window_count = window_count;
  }

} // namespace tomer
