#ifndef TOPIC_EVALUATOR_CACHE_H_
#define TOPIC_EVALUATOR_CACHE_H_

#include "def.h"

namespace tomer {

  class TopicEvaluatorCacheReader {
  public:
    bool read(const String& filename) {

    }

    WordToIndexTransformerCache& get_transformer() const {
      return transformer_;
    }

    WordIndexCounterCache& get_counter() const {
      return counter_;
    }

  private:
    WordToIndexTransformerCache transformer_
      WordIndexCounterCache counter_;

  };

  class TopicEvaluatorCacheWriter {
  public:
    bool write(const WordToIndexTransformerCache& transformer,
               const WordIndexCounterCache& counter,
               const String& filename) const;

  };

  class WordIndexCounterCache {
  public:
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

  };

  class WordToIndexTransformerCache {
  public:
    inline void update(const Word& word, const WordIndex& index) {
      indexes_.insert(std::make_pair(word, index));
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

  private:
    Map<Word, WordIndex> indexes_;

    inline WordIndex get_index_or_invalid_index(const Word& word) const {
      auto it = indexes_.find(word);
      if (it == indexes_.end()) return unobserved_word_index;
      return it->second;
    }

  };

} // namespace tomer

#endif // TOPIC_EVALUATOR_CACHE_H_
