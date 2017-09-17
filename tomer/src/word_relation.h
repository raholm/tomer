#ifndef TOMER_WORD_RELATION_H_
#define TOMER_WORD_RELATION_H_

#include <algorithm>

#include "def.h"
#include "word_transformer.h"

namespace tomer {

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
      if (word == word_ || word == WordToIndexTransformer::unobserved_word_index) return;
      related_words_.insert(word);
    }

    inline void update(const Vector<WordIndex>& words) {
      std::copy_if(words.cbegin(), words.cend(), std::inserter(related_words_, related_words_.end()),
                   [&](WordIndex const& word) {
                     return word != word_ && word != WordToIndexTransformer::unobserved_word_index;
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
        if (words.at(i) != WordToIndexTransformer::unobserved_word_index)
          relations_.insert(std::make_pair(words.at(i), relations.at(i)));
    }

    TopicWordIndexRelationMap(const TopicWordIndexRelationMap& other) = default;
    TopicWordIndexRelationMap(TopicWordIndexRelationMap&& other) = default;

    ~TopicWordIndexRelationMap() = default;

    void update(const WordIndex& word, const WordIndex related_word) {
      if (word == WordToIndexTransformer::unobserved_word_index) return;
      auto it = relations_.find(word);
      if (it == relations_.end())
        relations_.insert(std::make_pair(word, TopicWordIndexRelation(word, related_word)));
      else
        it->second.update(related_word);
    }

    void update(const WordIndex& word, const Vector<WordIndex>& related_words) {
      if (word == WordToIndexTransformer::unobserved_word_index) return;
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

  private:
    Map<WordIndex, TopicWordIndexRelation> relations_;

  };

} // namespace tomer

#endif // TOMER_WORD_RELATION_H_
