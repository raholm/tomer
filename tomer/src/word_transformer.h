#ifndef TOMER_WORD_TRANSFORMER_H_
#define TOMER_WORD_TRANSFORMER_H_

#include "def.h"

namespace tomer {

  class WordToIndexTransformer {
  public:
    static const WordIndex unobserved_word_index;
    static const Word unobserved_word;

  public:
    WordToIndexTransformer()
      : next_index_{0},
        words_{},
        indexes_{} {}

    WordToIndexTransformer(const WordToIndexTransformer& other) = default;
    WordToIndexTransformer(WordToIndexTransformer&& other) = default;

    ~WordToIndexTransformer() = default;

    void update(const Word& word) {
      add_if_missing(word);
    }

    WordIndex update_and_transform(const Word& word) {
      return add_if_missing_and_transform(word);
    }

    void update(const Vector<Word>& words) {
      for (auto const& word : words)
        update(word);
    }

    Vector<WordIndex> update_and_transform(const Vector<Word>& words) {
      Vector<WordIndex> indexes;
      indexes.reserve(words.size());

      for (auto const& word: words)
        indexes.push_back(update_and_transform(word));

      return indexes;
    }

    WordIndex transform(const Word& word) const {
      return get_index_or_invalid_index(word);
    }

    Vector<WordIndex> transform(const Vector<Word>& words) const {
      Vector<WordIndex> indexes;
      indexes.reserve(words.size());

      for (auto const& word : words)
        indexes.push_back(transform(word));

      return indexes;
    }

    Word revert(const WordIndex& index) {
      return get_word_or_invalid_word(index);
    }

    Vector<Word> revert(const Vector<WordIndex>& indexes) {
      Vector<Word> words;
      words.reserve(indexes.size());

      for (auto const& index : indexes)
        words.push_back(revert(index));

      return words;
    }

    const Map<Word, WordIndex>& get_indexes() const {
      return indexes_;
    }

  private:
    WordIndex next_index_;
    Vector<Word> words_;
    Map<Word, WordIndex> indexes_;

    void add_if_missing(const Word& word) {
      auto p = indexes_.insert(std::make_pair(word, next_index_));
      if (p.second) {
        words_.push_back(word);
        ++next_index_;
      }
    }

    WordIndex add_if_missing_and_transform(const Word& word) {
      auto p = indexes_.insert(std::make_pair(word, next_index_));
      if (p.second) {
        words_.push_back(word);
        return next_index_++;
      }
      return p.first->second;
    }

    Word get_word_or_invalid_word(const WordIndex& index) const {
      if (index < 0 || index >= next_index_) return unobserved_word;
      return words_.at(index);
    }

    WordIndex get_index_or_invalid_index(const Word& word) const {
      auto it = indexes_.find(word);
      if (it == indexes_.end()) return unobserved_word_index;
      return it->second;
    }

  };

  class WordToIndexTransformerCache {
  public:
    explicit WordToIndexTransformerCache() = default;
    explicit WordToIndexTransformerCache(const WordToIndexTransformer& transformer)
      : indexes_{transformer.get_indexes()} {}
    explicit WordToIndexTransformerCache(WordToIndexTransformer&& transformer)
      : indexes_{std::move(transformer.get_indexes())} {}

    void set(const Word& word, const WordIndex& index) {
      indexes_.insert(std::make_pair(word, index));
    }

    void set(const Vector<Word>& words, const Vector<WordIndex>& indexes) {
      for (unsigned i = 0; i < words.size(); ++i)
        set(words.at(i), indexes.at(i));
    }

    WordIndex transform(const Word& word) const {
      return get_index_or_invalid_index(word);
    }

    Vector<WordIndex> transform(const Vector<Word>& words) const {
      Vector<WordIndex> indexes;
      indexes.reserve(words.size());

      for (auto const& word : words)
        indexes.push_back(transform(word));

      return indexes;
    }

    Matrix<WordIndex> transform(const Matrix<Word>& words) const {
      Matrix<WordIndex> indexes;
      indexes.reserve(words.size());

      for (auto const& doc : words)
        indexes.push_back(transform(doc));

      return indexes;
    }

    const Map<Word, WordIndex>& get_indexes() const {
      return indexes_;
    }

  private:
    Map<Word, WordIndex> indexes_;

    inline WordIndex get_index_or_invalid_index(const Word& word) const {
      auto it = indexes_.find(word);
      if (it == indexes_.end()) return WordToIndexTransformer::unobserved_word_index;
      return it->second;
    }

  };

} // namespace tomer

#endif // TOMER_WORD_TRANSFORMER_H_
