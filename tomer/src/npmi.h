#ifndef TOMER_NPMI_H_
#define TOMER_NPMI_H_

#include <cmath>
#include <algorithm>
#include <exception>

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

  class TopicWordIndexRelation {
  public:
    explicit TopicWordIndexRelation(WordIndex word)
      : word_{word},
        related_words_{} {}
    explicit TopicWordIndexRelation(WordIndex word, const WordIndex& related_word)
      : word_{word},
        related_words_{} {
          update(related_word);
        }
    explicit TopicWordIndexRelation(WordIndex word, const Vector<WordIndex>& related_words)
      : word_{word},
        related_words_{} {
          update(related_words);
        }

    TopicWordIndexRelation(const TopicWordIndexRelation& other) = default;
    TopicWordIndexRelation(TopicWordIndexRelation&& other) = default;

    inline void update(WordIndex word) {
      if (word == word_ || word == UNOBSERVED_WORDINDEX) return;
      related_words_.insert(word);
    }

    inline void update(const Vector<WordIndex>& words) {
      std::copy_if(words.cbegin(), words.cend(), std::inserter(related_words_, related_words_.end()),
                   [&](WordIndex const& word) {
                     return word != word_ && word != UNOBSERVED_WORDINDEX;
                   });
    }

    inline bool is_related_to(const WordIndex& word) const {
      return related_words_.find(word) != related_words_.end();
    }

  private:
    WordIndex word_;
    Set<WordIndex> related_words_;

  };

  class TopicWordIndexRelationMap {
  public:
    explicit TopicWordIndexRelationMap() = default;
    explicit TopicWordIndexRelationMap(const Vector<WordIndex>& words,
                                       const Vector<TopicWordIndexRelation> relations) {
      for (unsigned i = 0; i < words.size(); ++i)
        if (words.at(i) != UNOBSERVED_WORDINDEX)
          relations_.insert(std::make_pair(words.at(i), relations.at(i)));
    }

    TopicWordIndexRelationMap(const TopicWordIndexRelationMap& other) = default;
    TopicWordIndexRelationMap(TopicWordIndexRelationMap&& other) = default;

    ~TopicWordIndexRelationMap() = default;

    void update(const WordIndex& word, const WordIndex related_word) {
      if (word == UNOBSERVED_WORDINDEX) return;
      auto it = relations_.find(word);
      if (it == relations_.end())
        relations_.insert(std::make_pair(word, TopicWordIndexRelation(word, related_word)));
      else
        it->second.update(related_word);
    }

    void update(const WordIndex& word, const Vector<WordIndex>& related_words) {
      if (word == UNOBSERVED_WORDINDEX) return;
      auto it = relations_.find(word);
      if (it == relations_.end())
        relations_.insert(std::make_pair(word, TopicWordIndexRelation(word, related_words)));
      else
        it->second.update(related_words);
    }

    inline bool contains(const WordIndex& word) const {
      return relations_.find(word) != relations_.end();
    }

    inline bool is_related_to(const WordIndex& word, const WordIndex& related) const {
      auto it = relations_.find(word);
      if (it == relations_.end()) return false;
      return it->second.is_related_to(related);
    }

    inline const TopicWordIndexRelation& get_relation(const WordIndex& word) const {
      auto it = relations_.find(word);
      if (it == relations_.end())
        throw std::out_of_range("Word does not exist.");
      return it->second;
    }

  private:
    Map<WordIndex, TopicWordIndexRelation> relations_;

  };

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

  class NormalisedPointwiseMutualInformationEvaluator {
  public:
    explicit NormalisedPointwiseMutualInformationEvaluator(const WordCount& word_counts,
                                                           size_t window_count)
      : word_counts_{word_counts},
        window_count_{window_count} {}

    ~NormalisedPointwiseMutualInformationEvaluator() = default;

    inline double evaluate(const Vector<Word>& topic_words) const {
      double val = 0.0;
      Word left_word, right_word;
      auto n = topic_words.size();

      for (unsigned i = 0; i < (n - 1); ++i) {
        left_word = topic_words.at(i);

        for (unsigned j = i + 1; j < n; ++j) {
          right_word = topic_words.at(j);

          if (left_word != right_word)
            val += compute_association(left_word, right_word);
        }
      }

      return val;
    }

  private:
    WordCount word_counts_;
    size_t window_count_;

    inline double compute_association(const Word& left, const Word& right) const {
      auto left_count = word_counts_.get_count(left);
      auto right_count = word_counts_.get_count(right);
      auto combined_count = word_counts_.get_count(left, right);

      if (missing_count(left_count) ||
          missing_count(right_count) ||
          missing_count(combined_count)) return 0.0;

      double numerator = log((double) (combined_count * window_count_) / (left_count * right_count));
      double denominator = -log((double) combined_count / window_count_);
      if (denominator == 0) return 0.0;
      return numerator / denominator;
    }

    inline bool missing_count(size_t count) const {
      return count == 0;
    }

  };

  using NpmiEvaluator = NormalisedPointwiseMutualInformationEvaluator;
  using NPMIEvaluator = NormalisedPointwiseMutualInformationEvaluator;

} // namespace tomer

#endif // TOMER_NPMI_H_
