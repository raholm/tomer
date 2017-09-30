#ifndef TOMER_COUNTER_H_
#define TOMER_COUNTER_H_

#include "def.h"
#include "word_transformer.h"
#include "word_relation.h"

namespace tomer {

  template<typename T>
  class Counter {
  public:
    using Type = T;

    virtual ~Counter() = default;

    virtual void update(const Type&) = 0;
    virtual void update(const Type&, const Type&) = 0;
    virtual void update(const Vector<Type>&) = 0;
    virtual void update(const Vector<Type>&, const Vector<Type>&) = 0;

    virtual Count get_count(const Type&) const = 0;
    virtual Count get_count(const Type&, const Type&) const = 0;

  };

  class WordCounter : public Counter<Word> {
  public:
    using BaseClass = Counter<Word>;
    using Type = BaseClass::Type;

    explicit WordCounter(const WordToIndexTransformer& transformer,
                         const TopicWordIndexRelationMap& word_relations)
      : transformer_{transformer},
        word_relations_{word_relations},
        counts_{} {}

    WordCounter(const WordCounter& other) = default;
    WordCounter(WordCounter&& other) = default;

    void update(const Vector<Type>& words) override {
      for (auto const& word : words) update(word);
    }

    void update(const Type& word) override {
      add_or_incr(word);
    }

    void update(const Vector<Type>& words1, const Vector<Type>& words2) override {
      auto n = std::min(words1.size(), words2.size());
      for (unsigned i = 0; i < n; ++i) update(words1.at(i), words2.at(i));
    }

    void update(const Type& word1, const Type& word2) override {
      if (word1 == word2) return;
      auto word1_index = transformer_.transform(word1);
      auto word2_index = transformer_.transform(word2);
      if (word1_index == WordToIndexTransformer::unobserved_word_index ||
          word2_index == WordToIndexTransformer::unobserved_word_index) return;

      if ((word_relations_.contains(word1_index) &&
           word_relations_.is_related_to(word1_index, word2_index)) ||
          (word_relations_.contains(word2_index) &&
           word_relations_.is_related_to(word2_index, word1_index))) {
        Type combined = get_combined_word(word1, word2);
        transformer_.update(combined);
        add_or_incr(combined);
      }
    }

    Count get_count(const Type& word) const override {
      auto it = counts_.find(transformer_.transform(word));
      if (it == counts_.end()) return 0;
      return it->second;
    }

    Count get_count(const Type& word1, const Type& word2) const override {
      return get_count(get_combined_word(word1, word2));
    }

  private:
    WordToIndexTransformer transformer_;
    TopicWordIndexRelationMap word_relations_;
    Map<WordIndex, Count> counts_;

    void add_or_incr(const Type& word) {
      auto index = transformer_.transform(word);
      if (index == WordToIndexTransformer::unobserved_word_index) return;
      auto p = counts_.insert(std::make_pair(index, 1));
      if (!p.second) counts_[index] += 1;
    }

    Word get_combined_word(const Type& word1, const Type& word2) const {
      return (word1 < word2) ? word1 + "|" + word2 : word2 + "|" + word1;
    }

  };

  class WordIndexCounter : public Counter<WordIndex> {
  public:
    using BaseClass = Counter<WordIndex>;
    using Type = BaseClass::Type;

    explicit WordIndexCounter(const TopicWordIndexRelationMap& word_relations)
      : word_relations_{word_relations},
        counts_{} {}

    WordIndexCounter(const WordIndexCounter& other) = default;
    WordIndexCounter(WordIndexCounter&& other) = default;

    void update(const Vector<Type>& indexes) override {
      for (auto const& index : indexes) update(index);
    }

    void update(const Type& index) override {
      add_or_incr(index);
    }

    void update(const Vector<Type>& indexes1, const Vector<Type>& indexes2) override {
      auto n = std::min(indexes1.size(), indexes2.size());
      for (unsigned i = 0; i < n; ++i) update(indexes1.at(i), indexes2.at(i));
    }

    void update(const Type& index1, const Type& index2) override {
      if (index1 == index2) return;
      if (index1 == WordToIndexTransformer::unobserved_word_index ||
          index2 == WordToIndexTransformer::unobserved_word_index) return;
      if ((word_relations_.contains(index1) &&
           word_relations_.is_related_to(index1, index2)) ||
          (word_relations_.contains(index2) &&
           word_relations_.is_related_to(index2, index1))) {
        add_or_incr(index1, index2);
      }
    }

    Count get_count(const Type& index) const override {
      auto it = counts_.find(index);
      if (it == counts_.end()) return 0;
      return it->second;
    }

    Count get_count(const WordIndex& index1, const WordIndex& index2) const override {
      if (index1 == WordToIndexTransformer::unobserved_word_index ||
          index2 == WordToIndexTransformer::unobserved_word_index ||
          index1 == index2) return 0;
      auto it = pair_counts_.find(combine(index1, index2));
      if (it == pair_counts_.end()) return 0;
      return it->second;
    }

    const Map<WordIndex, Count>& get_counts() const {
      return counts_;
    }

    const Map<Pair<WordIndex, WordIndex>, Count>& get_pair_counts() const {
      return pair_counts_;
    }

  private:
    TopicWordIndexRelationMap word_relations_;
    Map<Type, Count> counts_;
    Map<Pair<Type, Type>, Count> pair_counts_;

    void add_or_incr(const Type& index) {
      if (index == WordToIndexTransformer::unobserved_word_index) return;
      auto p = counts_.insert(std::make_pair(index, 1));
      if (!p.second) ++counts_[index];
    }

    void add_or_incr(const Type& index1, const Type& index2) {
      if (index1 == WordToIndexTransformer::unobserved_word_index ||
          index2 == WordToIndexTransformer::unobserved_word_index) return;
      auto index = combine(index1, index2);
      auto p = pair_counts_.insert(std::make_pair(index, 1));
      if (!p.second) ++pair_counts_[index];
    }

    Pair<WordIndex, WordIndex> combine(const WordIndex& index1, const WordIndex& index2) const {
      return (index1 < index2) ? std::make_pair(index1, index2) : std::make_pair(index2, index1);
    }

  };

  class WordIndexCounterCache : public Counter<WordIndex> {
  public:
    explicit WordIndexCounterCache() = default;
    explicit WordIndexCounterCache(const WordIndexCounter& counter)
      : counts_{counter.get_counts()},
        pair_counts_{counter.get_pair_counts()} {}
    explicit WordIndexCounterCache(WordIndexCounter&& counter)
      : counts_{std::move(counter.get_counts())},
        pair_counts_{std::move(counter.get_pair_counts())} {}

    void update(const Vector<Type>& indexes) override {
      for (auto const& index : indexes) update(index);
    }

    void update(const WordIndex& index) override {
      add_or_incr(index);
    }

    void update(const WordIndex& index1, const WordIndex& index2) override {
      add_or_incr(index1, index2);
    }

    void update(const Vector<Type>& indexes1, const Vector<Type>& indexes2) override {
      auto n = std::min(indexes1.size(), indexes2.size());
      for (unsigned i = 0; i < n; ++i) update(indexes1.at(i), indexes2.at(i));
    }

    Count get_count(const WordIndex& index) const {
      auto it = counts_.find(index);
      if (it == counts_.end()) return 0;
      return it->second;
    }

    Count get_count(const WordIndex& index1, const WordIndex& index2) const override {
      if (index1 == WordToIndexTransformer::unobserved_word_index ||
          index2 == WordToIndexTransformer::unobserved_word_index ||
          index1 == index2) return 0;
      auto it = pair_counts_.find(combine(index1, index2));
      if (it == pair_counts_.end()) return 0;
      return it->second;
    }

    const Map<WordIndex, Count>& get_counts() const {
      return counts_;
    }

    const Map<Pair<WordIndex, WordIndex>, Count>& get_pair_counts() const {
      return pair_counts_;
    }

    void set(const WordIndex& index, const Count& count) {
      auto it = counts_.insert(std::make_pair(index, count));
      if (!it.second) counts_[index] = count;
    }

    void set(const WordIndex& index1, const WordIndex& index2, const Count& count) {
      auto index = combine(index1, index2);
      auto it = pair_counts_.insert(std::make_pair(index, count));
      if (!it.second) pair_counts_[index] = count;
    }

  private:
    Map<WordIndex, Count> counts_;
    Map<Pair<WordIndex, WordIndex>, Count> pair_counts_;

    void add_or_incr(const WordIndex& index) {
      if (index == WordToIndexTransformer::unobserved_word_index) return;
      auto p = counts_.insert(std::make_pair(index, 1));
      if (!p.second) ++counts_[index];
    }

    void add_or_incr(const Type& index1, const Type& index2) {
      if (index1 == WordToIndexTransformer::unobserved_word_index ||
          index2 == WordToIndexTransformer::unobserved_word_index) return;
      auto index = combine(index1, index2);
      auto p = pair_counts_.insert(std::make_pair(index, 1));
      if (!p.second) ++pair_counts_[index];
    }

    Pair<WordIndex, WordIndex> combine(const WordIndex& index1, const WordIndex& index2) const {
      return (index1 < index2) ? std::make_pair(index1, index2) : std::make_pair(index2, index1);
    }

  };

} // namespace tomer

#endif // TOMER_COUNTER_H_
