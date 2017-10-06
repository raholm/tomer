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

  void calculate_word_counts_and_window_count(const Corpus& documents,
                                              size_t window_size,
                                              WordTopicEvaluatorData* data) {
    auto ndocs = documents.size();
    size_t nwindows, window_count = 0;
    WordWindow words_in_window(window_size);
    WordCounter& word_counts = data->word_counts;
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

  void calculate_word_index_counts_and_window_count(const Matrix<WordIndex>& documents,
                                                    size_t window_size,
                                                    WordIndexTopicEvaluatorData* data) {
    auto ndocs = documents.size();
    size_t nwindows, window_count = 0;
    WordIndexWindow words_in_window(window_size);
    WordIndexCounter& word_index_counts = data->word_index_counts;
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
          word_index_counts.update(left_word);

          for (unsigned right_idx = left_idx + 1; right_idx < nwords; ++right_idx) {
            auto right_word = words_in_window.at(right_idx);
            word_index_counts.update(left_word, right_word);
          }
        }
      }
    }

    data->window_count = window_count;
  }

  void calculate_word_counts_and_window_count(const String& filename,
                                              size_t window_size,
                                              const FixedWordIndexTokenizer& tokenizer,
                                              SparseWordIndexTopicEvaluatorData* data) {
    std::ifstream file{filename};
    String document;
    size_t document_size;
    WordIndex current_word_index, other_word_index;
    Vector<WordIndex> current_word_indexes;
    WordIndexWindow current_word_window(window_size);

    auto& word_index_counts = data->word_index_counts;
    auto& window_count = data->window_count;

    if (file.fail()) {
      throw new std::runtime_error("Reference file could not be opened.");
    }

    while (std::getline(file, document)) {
      current_word_indexes = tokenizer.transform(document);
      document_size = current_word_indexes.size();

      if (window_size == INF_WORD_WINDOW) {
        remove_duplicates_inplace(&current_word_indexes, &document_size);

        for (unsigned current_idx = 0; current_idx < document_size; ++current_idx) {
          current_word_index = current_word_indexes.at(current_idx);

          if (current_word_index != WordToIndexTransformer::unobserved_word_index) {
            for (unsigned other_idx = 0; other_idx < document_size; ++other_idx) {
              other_word_index = current_word_indexes.at(other_idx);

              if (other_word_index != WordToIndexTransformer::unobserved_word_index)
                word_index_counts.update(current_word_index, other_word_index);
            }
          }
        }

        ++window_count;
      } else {

      }
    }

    file.close();
  }

} // namespace tomer
