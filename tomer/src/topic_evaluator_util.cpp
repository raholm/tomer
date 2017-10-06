#include "topic_evaluator_util.h"

#include <fstream>

namespace tomer {

  WordCounter create_word_counts(const Corpus& topics) {
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

    return WordCounter(transformer, relations);
  }

  WordIndexCounter create_word_index_counts(const Matrix<WordIndex>& topics) {
    auto ntopics = topics.size();

    TopicWordIndexRelationMap relations;

    for (unsigned i = 0; i < ntopics; ++i) {
      auto topic_words = topics.at(i);

      for (unsigned j = 0; j < topic_words.size(); ++j) {
        relations.update(topic_words.at(j), topic_words);
      }
    }

    return WordIndexCounter(relations);
  }

  static bool is_valid_index(const WordIndex& index) {
    return index != WordToIndexTransformer::unobserved_word_index;
  }

  void calculate_word_counts_and_window_count(const String& filename,
                                              size_t window_size,
                                              const FixedWordIndexTokenizer& tokenizer,
                                              SparseWordIndexTopicEvaluatorData* data) {
    String document;
    size_t document_size;
    Vector<WordIndex> current_word_indexes;

    WordIndexWindow current_word_window(window_size);
    size_t nwindows, head_id, tail_id, nwords;

    auto& word_index_counts = data->word_index_counts;
    auto& window_count = data->window_count;

    std::ifstream file{filename};

    if (file.fail()) {
      throw std::runtime_error("Reference file could not be opened.");
    }

    while (std::getline(file, document)) {
      current_word_indexes = tokenizer.transform(document);
      document_size = current_word_indexes.size();

      if (window_size == INF_WORD_WINDOW)
        nwindows = 1;
      else
        nwindows = document_size + window_size - 1;

      for (unsigned j = 1; j < (nwindows + 1); ++j) {
        if (window_size == INF_WORD_WINDOW) {
          current_word_window = current_word_indexes;
          remove_duplicates(&current_word_window);
          nwords = current_word_window.size();
        } else {
          head_id = (j > window_size) ? j - window_size : 0;
          tail_id = std::min((size_t) j, document_size);
          nwords = tail_id - head_id;

          for (unsigned i = 0; i < nwords; ++i)
            current_word_window.at(i) = current_word_indexes.at(head_id + i);

          remove_duplicates_inplace(&current_word_window, &nwords);
        }

        for (unsigned left_idx = 0; left_idx < nwords; ++left_idx) {
          auto left_word_index = current_word_window.at(left_idx);

          if (is_valid_index(left_word_index)) {
            word_index_counts.update(left_word_index);

            for (unsigned right_idx = left_idx + 1; right_idx < nwords; ++right_idx) {
              auto right_word_index = current_word_window.at(right_idx);

              if (is_valid_index(right_word_index))
                word_index_counts.update(left_word_index, right_word_index);
            }
          }
        }

        window_count += nwindows;
      }
    }

    file.close();
  }

} // namespace tomer
