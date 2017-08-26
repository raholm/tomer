#ifndef TOMER_NPMI_H_
#define TOMER_NPMI_H_

#include "def.h"

namespace tomer {

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

    inline void update(const Vector<Word>& doc) {
      for (auto const& word : doc)
        add_if_missing(word);
    }

    inline int transform(const Word& word) const {
      return has(word) ? at(word) : -1;
    }

    inline Vector<int> transform(const Vector<Word>& doc) const {
      Vector<int> indexes;
      indexes.reserve(doc.size());

      for (auto const& word : doc) {
        if (has(word))
          indexes.push_back(at(word));
        else
          indexes.push_back(-1);
      }

      return indexes;
    }

    inline bool has(const Word& word) const {
      return indexes_.find(word) != indexes_.end();
    }

    const WordIndex& at(const Word& word) const {
      auto it = indexes_.find(word);
      if (indexes_ == indexes_.end())
        throw std::out_of_range("Word does not exist.");
      return it->second;
    }

    const Word& at(const WordIndex& index) const {
      if (index < 0 || index >= next_index_)
        throw std::out_of_range("Index does not exist.");
      return words_.at(index);
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

  };

  class TopicWordIndexRelation {
  public:
    explicit TopicWordIndexRelation(WordIndex word)
      : word_{word}
        related_words_{} {}
    explicit TopicWordIndexRelation(WordIndex word, const WordIndex& related_word)
      : word_{word}
        related_words_{related_word} {}
    explicit TopicWordIndexRelation(WordIndex word, const Vector<WordIndex>& related_words)
      : word_{word}
        related_words_{related_words.cbegin(), related_words.cend()} {}

    TopicWordIndexRelation(const TopicWordIndexRelation& other) = default;
    TopicWordIndexRelation(TopicWordIndexRelation&& other) = default;

    inline void update(WordIndex word) {
      if (word == word_) return;
      related_words_.insert(word);
    }

    inline void update(const Vector<WordIndex>& words) {
      std::copy_if(words.cbegin(), words.cend(), std::inserter(related_words_, related_words_.end())
                   [&](auto const& word) return word != word_);
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
                                       const Vector<TopicWorldRelation> relations) {
      for (unsigned i = 0; i < words; ++i)
        relations_.insert(std::make_pair(words.at(i), relations.at(i)));
    }

    TopicWordIndexRelationMap(const TopicWordIndexRelationMap& other) = default;
    TopicWordIndexRelationMap(TopicWordIndexRelationMap&& other) = default;

    ~TopicWordIndexRelationMap() = default;

    void update(const WordIndex& word, const WordIndex related_word) {
      auto it = relations_.find(word);
      if (it == relations_.end())
        relations_insert(std::make_pair(word, TopicWordIndexRelation(word, related_word)));
      else
        it->update(related_word);
    }

    void update(const WordIndex& word, const Vector<WordIndex>& related_words) {
      auto it = relations_.find(word);
      if (it == relations_.end())
        relations_insert(std::make_pair(word, TopicWordIndexRelation(word, related_words)));
      else
        it->update(related_word);
    }

    inline bool contains(const WordIndex& word) const {
      return relations_.find(word) != relations_.end();
    }

    inline const TopicWordIndexRelation& get_relation(const WordIndex& word) const {
      auto it = relations_.find(word);
      if (it == relations.end())
        throw std::out_of_range("Word does not exist.");
      return it.first->second;
    }

  private:
    Map<WordIndex, TopicWordIndexRelation> relations_;

  };

  class WordCount {
  public:
    explicit WordCount(const WordToIndexTransformer& transformer,
                       const TopicWordRelationMap& word_relations)
      : transformer_{transformer},
        word_relations_{word_relations},
        counts_{} {}

    inline update(const Word& word) {
      if (transformer_.has(word)) add_or_incr(word);
    }

    inline update(const Word& word1, const Word& word2) {
      if (word1 == word2) return;

      if ((word_relations_.contains(word1) &&
           word_relations_.get_relation(word1).is_related_to(word2)) ||
          (word_relations_.contains(word2) &&
           word_relations_.get_relation(word2).is_related_to(word1))) {
        Word combined = get_combined_word(word1, word2);
        transformer_.update(combined);
        add_or_incr(combined);
      }
    }

    inline Count get_count(const Word& word) {
      if (!transformer_.has(word)) return 0;
      auto it = counts_.find(transformer_.transform(word));
      if (it == counts_.end())
        return 0;
      return it->second;
    }

    inline Count get_count(const Word& word1, const Word& word2) {
      return get_count(get_combined_word(word1, word2));
    }

  private:
    WordToIndexTransformer transformer_;
    TopicWordRelationMap word_relations_;
    Map<WordIndex, Count> counts_;

    inline add_or_incr(const Word& word) {
      auto index = transformer_.transform(word);
      auto p = counts_.insert(std::make_pair(index, 1));
      if (!p.second)
        counts_[index] += 1;
    }

    inline Word get_combined_word(const Word& word1, const Word& word2) const {
      return (word1 < word2) ? word1 + "|" + word2 : word2 + "|" word1;
    }

  };

  class NormalisedPointwiseMutualInformationEvaluator {
  public:
    explicit NormalisedPointwiseMutualInformationEvaluator(const WordCount& word_counts,
                                                           size_t window_count)
      : word_counts_{word_counts},
        window_count_{window_count} {}

    ~NormalisedPointwiseMutualInformationEvaluator() = default;

    double evaluate(const Vector<Word>& topic_words) const {
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

    double compute_association(const Word& left, const Word& right) const {
      auto left_count = word_counts_.get_count(left);
      auto right_count = word_counts_.get_count(left);
      auto combined_count = word_counts_.get_count(left, right);

      if (left_count == 0 || right_count == 0 || combined_count == 0) return 0.0;

      double numerator = log((double) (combined_count * window_count_) / (left_count * right_count));
      double denominator = -log((double) combined_count / window_count_);
      return numerator / denominator;
    }

  };

  typename NpmiEvaluator = NormalisedPointwiseMutualInformationEvaluator;
  typename NPMIEvaluator = NormalisedPointwiseMutualInformationEvaluator;



} // namespace tomer

#endif // TOMER_NPMI_H_
