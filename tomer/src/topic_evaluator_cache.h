#ifndef TOPIC_EVALUATOR_CACHE_H_
#define TOPIC_EVALUATOR_CACHE_H_

#include <fstream>
#include <sstream>

#include "def.h"
#include "topic_evaluator.h"
#include "word_transformer.h"

namespace tomer {

  class WordIndexTopicEvaluatorDataCacheWriter;

  class WordIndexCounterCache {
  public:
    void update(const WordIndex& index, const Count& count) {
      auto it = counts_.insert(std::make_pair(index, count));
      if (!it.second) counts_[index] = count;
    }

    void update(const WordIndex& index1, const WordIndex& index2, const Count& count) {
      update(get_combined_word_index(index1, index2), count);
    }

    Count get_count(const WordIndex& index) const {
      auto it = counts_.find(index);
      if (it == counts_.end()) return 0;
      return it->second;
    }

    Count get_count(const WordIndex& index1, const WordIndex& index2) const {
      return get_count(get_combined_word_index(index1, index2));
    }

  private:
    Map<WordIndex, Count> counts_;

    WordIndex get_combined_word_index(const Type& index1, const Type& index2) const {
      /*
        To combine two indexes we require that it result in a unique index.
        By assuming there a no more than 100000 unique tokens (or word indexes),
        we can use this simple solution.
      */
      const size_t max_unique_indexes = 100000;
      return (index1 > index2) ?
        index1 * max_unique_indexes + index2 :
        index2 * max_unique_indexes + index1;
    }

    friend WordIndexTopicEvaluatorDataCacheWriter;

  };

  class WordToIndexTransformerCache {
  public:
    inline void update(const Word& word, const WordIndex& index) {
      indexes_.insert(std::make_pair(word, index));
    }

    inline void update(const Vector<Word>& words, const Vector<WordIndex>& indexes) {
      for (unsigned i = 0; i < words.size(); ++i)
        update(words.at(i), indexes.at(i));
    }

    inline WordIndex transform(const Word& word) const {
      return get_index_or_invalid_index(word);
    }

    inline Vector<WordIndex> transform(const Vector<Word>& words) const {
      Vector<WordIndex> indexes;
      indexes.reserve(words.size());

      for (auto const& word : words)
        indexes.push_back(transform(word));

      return indexes;
    }

    inline Matrix<WordIndex> transform(const Matrix<Word>& words) const {
      Matrix<WordIndex> indexes;
      indexes.reserve(words.size());

      for (auto const& doc : words)
        indexes.push_back(transform(doc));

      return indexes;
    }

  private:
    Map<Word, WordIndex> indexes_;

    inline WordIndex get_index_or_invalid_index(const Word& word) const {
      auto it = indexes_.find(word);
      if (it == indexes_.end()) return WordToIndexTransformer::unobserved_word_index;
      return it->second;
    }

    friend WordIndexTopicEvaluatorDataCacheWriter;

  };

  struct WordIndexTopicEvaluatorDataCache {
    explicit WordIndexTopicEvaluatorDataCache(WordToIndexTransformerCache&& transformer,
                                              WordIndexCounterCache&& counts,
                                              size_t window_count)
      : transformer{std::move(transformer)},
        word_index_counts{std::move(counts)},
        window_count{window_count} {}

    explicit WordIndexTopicEvaluatorDataCache() = default;

    WordToIndexTransformerCache transformer;
    WordIndexCounterCache word_index_counts;
    size_t window_count;

  };

  class WordIndexTopicEvaluatorDataCacheReader {
  public:
    WordIndexTopicEvaluatorDataCache read(const String& filename) {
      std::ifstream infile{filename};

      if (infile.fail()) return WordIndexTopicEvaluatorDataCache();

      size_t window_count, transformer_rows, counter_rows;
      String line;
      WordToIndexTransformerCache transformer;
      WordIndexCounterCache counter;

      getline(infile, line);

      std::istringstream iss{line};

      iss >> window_count >> transformer_rows >> counter_rows;

      size_t row_counter = 0;

      Word word;
      WordIndex word_index;
      Count count;

      while(getline(infile, line)) {
        iss.clear();
        iss.str(line);

        if (row_counter < transformer_rows) {
          iss >> word >> word_index;
          transformer.update(word, word_index);
        } else {
          iss >> word_index >> count;
          counter.update(word_index, count);
        }

        ++row_counter;
      }

      infile.close();

      return WordIndexTopicEvaluatorDataCache(std::move(transformer), std::move(counter), window_count);
    }

  };

  class WordIndexTopicEvaluatorDataCacheWriter {
  public:
    bool write(const WordIndexTopicEvaluatorDataCache& cache,
               const String& filename) const {
      std::ofstream outfile;

      if (outfile.fail()) return false;

      outfile << cache.window_count << " "
              << cache.transformer.indexes_.size() << " "
              << cache.word_index_counts.counts_.size() << std::endl;

      for (const auto& word_wordindex : cache.transformer.indexes_) {
        outfile << word_wordindex.first << " "
                << word_wordindex.second << std::endl;
      }

      for (const auto& wordindex_count : cache.word_index_counts.counts_) {
        outfile << wordindex_count.first << " "
                << wordindex_count.second << std::endl;
      }

      return true;
    }

  };

  void calculate_word_index_counts_and_window_count_with_cache(const Matrix<WordIndex>& documents,
                                                               size_t window_size,
                                                               WordIndexTopicEvaluatorData* data,
                                                               WordIndexTopicEvaluatorDataCache* cache) {
    auto ndocs = documents.size();
    size_t nwindows, window_count =  0;
    WordIndexWindow words_in_window(window_size);
    WordIndexCounter& word_index_counts = data->word_index_counts;
    size_t head_id, tail_id, nwords;

    auto& cache_counter = cache->word_index_counts;

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
          cache_counter.update(left_word, word_index_counts.get_count(left_word));

          for (unsigned right_idx = left_idx + 1; right_idx < nwords; ++right_idx) {
            auto right_word = words_in_window.at(right_idx);
            word_index_counts.update(left_word, right_word);
            cache_counter.update(left_word, right_word, word_index_counts.get_count(left_word, right_word));
          }
        }
      }
    }

    data->window_count = window_count;
    cache->window_count = window_count;
  }

  class CompressedAndCachedNormalisedPointwiseMutualInformationEvaluator : public CompressedTopicEvaluator {
  public:
    using BaseClass = CompressedTopicEvaluator;

    explicit CompressedAndCachedNormalisedPointwiseMutualInformationEvaluator(const WordIndexCounterCache& word_index_counts,
                                                                              size_t window_count)
      : word_index_counts_{word_index_counts},
        window_count_{window_count} {}

    explicit CompressedAndCachedNormalisedPointwiseMutualInformationEvaluator(WordIndexCounterCache&& word_index_counts,
                                                                              size_t window_count)
      : word_index_counts_{std::move(word_index_counts)},
        window_count_{window_count} {}

  protected:
    double compute_association(const WordIndex& left, const WordIndex& right) const override {
      auto left_count = word_index_counts_.get_count(left);
      auto right_count = word_index_counts_.get_count(right);
      auto combined_count = word_index_counts_.get_count(left, right);

      if (missing_count(left_count) ||
          missing_count(right_count) ||
          missing_count(combined_count)) return 0.0;

      double denominator = -log((double) combined_count / window_count_);
      if (denominator == 0) return 0.0;
      double numerator = log((double) (combined_count * window_count_) / (left_count * right_count));
      return numerator / denominator;
    }

  private:
    WordIndexCounterCache word_index_counts_;
    size_t window_count_;

    inline bool missing_count(size_t count) const {
      return count == 0;
    }

  };

  using CompressedAndCachedNpmiEvaluator = CompressedAndCachedNormalisedPointwiseMutualInformationEvaluator;


} // namespace tomer

#endif // TOPIC_EVALUATOR_CACHE_H_
