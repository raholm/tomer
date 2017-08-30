#ifndef TOMER_WORD_COUNT_H_
#define TOMER_WORD_COUNT_H_

#include "word_transformer.h"
#include "word_relation.h"

namespace tomer {

  class WordCount {
  public:
    explicit WordCount(const WordToIndexTransformer& transformer,
                       const TopicWordIndexRelationMap& word_relations)
      : transformer_{transformer},
        word_relations_{word_relations},
        counts_{} {}

    WordCount(const WordCount& other) = default;
    WordCount(WordCount&& other) = default;

    inline void update(const Vector<Word>& words) {
      for (auto const& word : words) update(word);
    }

    inline void update(const Word& word) {
      add_or_incr(word);
    }

    inline void update(const Vector<Word>& words1, const Vector<Word>& words2) {
      auto n = std::min(words1.size(), words2.size());
      for (unsigned i = 0; i < n; ++i) update(words1.at(i), words2.at(i));
    }

    inline void update(const Word& word1, const Word& word2) {
      if (word1 == word2) return;
      auto word1_index = transformer_.transform(word1);
      auto word2_index = transformer_.transform(word2);
      if (word1_index == UNOBSERVED_WORDINDEX ||
          word2_index == UNOBSERVED_WORDINDEX) return;

      if ((word_relations_.contains(word1_index) &&
           word_relations_.is_related_to(word1_index, word2_index)) ||
          (word_relations_.contains(word2_index) &&
           word_relations_.is_related_to(word2_index, word1_index))) {
        Word combined = get_combined_word(word1, word2);
        transformer_.update(combined);
        add_or_incr(combined);
      }
    }

    inline Count get_count(const Word& word) const {
      auto it = counts_.find(transformer_.transform(word));
      if (it == counts_.end()) return 0;
      return it->second;
    }

    inline Count get_count(const Word& word1, const Word& word2) const {
      return get_count(get_combined_word(word1, word2));
    }

  private:
    WordToIndexTransformer transformer_;
    TopicWordIndexRelationMap word_relations_;
    Map<WordIndex, Count> counts_;

    inline void add_or_incr(const Word& word) {
      auto index = transformer_.transform(word);
      if (index == UNOBSERVED_WORDINDEX) return;
      auto p = counts_.insert(std::make_pair(index, 1));
      if (!p.second) counts_[index] += 1;
    }

    inline Word get_combined_word(const Word& word1, const Word& word2) const {
      return (word1 < word2) ? word1 + "|" + word2 : word2 + "|" + word1;
    }

  };

} // namespace tomer

#endif // TOMER_WORD_COUNT_H_
