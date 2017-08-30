#ifndef TOMER_WORD_TRANSFORMER_H_
#define TOMER_WORD_TRANSFORMER_H_

#include "def.h"

namespace tomer {

  static const WordIndex UNOBSERVED_WORDINDEX = -1;
  static const Word UNKNOWN_WORD = "<!!unknown!!>";

  class WordToIndexTransformer {
  public:
    WordToIndexTransformer()
      : next_index_{0},
        words_{},
        indexes_{} {}

    WordToIndexTransformer(const WordToIndexTransformer& other) = default;
    WordToIndexTransformer(WordToIndexTransformer&& other) = default;

    ~WordToIndexTransformer() = default;

    inline void update(const Word& word) {
      add_if_missing(word);
    }

    inline void update(const Vector<Word>& words) {
      for (auto const& word : words)
        add_if_missing(word);
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

    inline Word revert(const WordIndex& index) {
      return get_word_or_invalid_word(index);
    }

    inline Vector<Word> revert(const Vector<WordIndex>& indexes) {
      Vector<Word> words;
      words.reserve(indexes.size());

      for (auto const& index : indexes)
        words.push_back(revert(index));

      return words;
    }

  private:
    WordIndex next_index_;
    Vector<Word> words_;
    Map<Word, WordIndex> indexes_;

    inline void add_if_missing(const Word& word) {
      auto p = indexes_.insert(std::make_pair(word, next_index_));
      if (p.second) {
        words_.push_back(word);
        ++next_index_;
      }
    }

    inline Word get_word_or_invalid_word(const WordIndex& index) const {
      if (index < 0 || index >= next_index_) return UNKNOWN_WORD;
      return words_.at(index);
    }

    inline WordIndex get_index_or_invalid_index(const Word& word) const {
      auto it = indexes_.find(word);
      if (it == indexes_.end()) return UNOBSERVED_WORDINDEX;
      return it->second;
    }

  };

} // namespace tomer

#endif // TOMER_WORD_TRANSFORMER_H_
