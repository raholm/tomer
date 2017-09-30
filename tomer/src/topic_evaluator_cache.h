#ifndef TOPIC_EVALUATOR_CACHE_H_
#define TOPIC_EVALUATOR_CACHE_H_

#include <fstream>
#include <sstream>

#include "def.h"
#include "topic_evaluator.h"
#include "word_transformer.h"

namespace tomer {

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

      size_t window_count, transformer_rows, single_counter_rows, pair_counter_rows;
      String line;
      WordToIndexTransformerCache transformer;
      WordIndexCounterCache counter;

      getline(infile, line);

      std::istringstream iss{line};

      iss >> window_count >> transformer_rows >> single_counter_rows >> pair_counter_rows;

      size_t row_counter = 0;

      Word word;
      WordIndex word_index, word_index1, word_index2;
      Count count;

      while(getline(infile, line)) {
        iss.clear();
        iss.str(line);

        if (row_counter < transformer_rows) {
          iss >> word >> word_index;
          transformer.set(word, word_index);
        } else if (row_counter < single_counter_rows) {
          iss >> word_index >> count;
          counter.set(word_index, count);
        } else {
          iss >> word_index1 >> word_index2 >> count;
          counter.set(word_index1, word_index2, count);
        }

        ++row_counter;
      }

      infile.close();

      return WordIndexTopicEvaluatorDataCache(std::move(transformer),
                                              std::move(counter),
                                              window_count);
    }

  };

  class WordIndexTopicEvaluatorDataCacheWriter {
  public:
    bool write(const WordIndexTopicEvaluatorDataCache& cache,
               const String& filename) const {
      std::ofstream outfile{filename};

      if (outfile.fail()) return false;

      auto window_count = cache.window_count;
      auto indexes = cache.transformer.get_indexes();
      auto counts = cache.word_index_counts.get_counts();
      auto pair_counts = cache.word_index_counts.get_pair_counts();

      outfile << window_count << " "
              << indexes.size() << " "
              << counts.size() <<  " "
              << pair_counts.size() << std::endl;

      for (auto const& word_wordindex : indexes) {
        outfile << word_wordindex.first << " "
                << word_wordindex.second << std::endl;
      }

      for (auto const& wordindex_count : counts) {
        outfile << wordindex_count.first << " "
                << wordindex_count.second << std::endl;
      }

      for (auto const& wordindex_wordindex_count : pair_counts) {
        outfile << wordindex_wordindex_count.first.first << " "
                << wordindex_wordindex_count.first.second << " "
                << wordindex_wordindex_count.second << std::endl;
      }

      outfile.flush();
      outfile.close();

      return true;
    }

  };

} // namespace tomer

#endif // TOPIC_EVALUATOR_CACHE_H_
