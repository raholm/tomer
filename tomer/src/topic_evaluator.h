#ifndef TOMER_TOPIC_EVALUATOR_H_
#define TOMER_TOPIC_EVALUATOR_H_

#include "def.h"
#include "counter.h"

namespace tomer {

  template<typename Type, typename Counter>
  static double compute_topic_coherence_association(const Type& left,
                                                    const Type& right,
                                                    const Counter& counts) {
    auto left_count = counts.get_count(left);
    auto combined_count = counts.get_count(left, right);
    if (left_count == 0) return 0.0;
    return log((double) (combined_count + 1) / left_count);
  }

  template<typename Type, typename Counter>
  static double compute_npmi_association(const Type& left,
                                         const Type& right,
                                         const Counter& counts,
                                         size_t window_count) {
    auto left_count = counts.get_count(left);
    auto right_count = counts.get_count(right);
    auto combined_count = counts.get_count(left, right);

    if (left_count == 0 ||
        right_count == 0 ||
        combined_count == 0) return 0.0;

    double denominator = -log((double) combined_count / window_count);
    if (denominator == 0) return 0.0;
    double numerator = log((double) (combined_count * window_count) / (left_count * right_count));
    return numerator / denominator;
  }

  class TopicEvaluator {
  public:
    TopicEvaluator() = default;

    virtual ~TopicEvaluator() = default;

    double evaluate(const Vector<Word>& topic_words) const {
      double val = 0.0;
      Word left_word, right_word;
      auto n = topic_words.size();

      for (unsigned j = 1; j < n; ++j) {
        right_word = topic_words.at(j);

        for (unsigned i = 0; i < j; ++i) {
          left_word = topic_words.at(i);

          if (left_word != right_word)
            val += compute_association(left_word, right_word);
        }
      }

      return val;
    }

  protected:
    virtual double compute_association(const Word& left, const Word& right) const = 0;

  };

  class CompressedTopicEvaluator {
  public:
    CompressedTopicEvaluator() = default;

    virtual ~CompressedTopicEvaluator() = default;

    double evaluate(const Vector<WordIndex>& topic_words) const {
      double val = 0.0;
      WordIndex left_word, right_word;
      auto n = topic_words.size();

      for (unsigned j = 1; j < n; ++j) {
        right_word = topic_words.at(j);

        for (unsigned i = 0; i < j; ++i) {
          left_word = topic_words.at(i);

          if (left_word != right_word)
            val += compute_association(left_word, right_word);
        }
      }

      return val;
    }

  protected:
    virtual double compute_association(const WordIndex& left, const WordIndex& right) const = 0;

  };

  class TopicCoherenceEvaluator : public TopicEvaluator {
  public:
    using BaseClass = TopicEvaluator;

    TopicCoherenceEvaluator(const WordCounter& word_counts)
      : word_counts_{word_counts} {}

  protected:
    double compute_association(const Word& left, const Word& right) const override {
      return compute_topic_coherence_association(left, right, word_counts_);
    }

  private:
    WordCounter word_counts_;

  };

  class CompressedTopicCoherenceEvaluator : public CompressedTopicEvaluator {
  public:
    using BaseClass = CompressedTopicEvaluator;

    CompressedTopicCoherenceEvaluator(const WordIndexCounter& word_index_counts)
      : word_index_counts_{word_index_counts} {}

  protected:
    double compute_association(const WordIndex& left, const WordIndex& right) const override {
      return compute_topic_coherence_association(left, right, word_index_counts_);
    }

  private:
    WordIndexCounter word_index_counts_;

  };

  class SparseCompressedTopicCoherenceEvaluator : public CompressedTopicEvaluator {
  public:
    using BaseClass = CompressedTopicEvaluator;

    explicit SparseCompressedTopicCoherenceEvaluator(const SparseWordIndexCounter& word_index_counts)
      : word_index_counts_{word_index_counts} {}

    explicit SparseCompressedTopicCoherenceEvaluator(SparseWordIndexCounter&& word_index_counts)
      : word_index_counts_{std::move(word_index_counts)} {}

  protected:
    double compute_association(const WordIndex& left, const WordIndex& right) const override {
      return compute_topic_coherence_association(left, right, word_index_counts_);
    }

  private:
    SparseWordIndexCounter word_index_counts_;

  };

  class CompressedAndCachedTopicCoherenceEvaluator : public CompressedTopicEvaluator {
  public:
    using BaseClass = CompressedTopicEvaluator;

    explicit CompressedAndCachedTopicCoherenceEvaluator(const WordIndexCounterCache& word_index_counts)
      : word_index_counts_{word_index_counts} {}

    explicit CompressedAndCachedTopicCoherenceEvaluator(WordIndexCounterCache&& word_index_counts)
      : word_index_counts_{std::move(word_index_counts)} {}

  protected:
    double compute_association(const WordIndex& left, const WordIndex& right) const override {
      return compute_topic_coherence_association(left, right, word_index_counts_);
    }

  private:
    WordIndexCounterCache word_index_counts_;

  };

  class NpmiEvaluator : public TopicEvaluator {
  public:
    using BaseClass = TopicEvaluator;

    explicit NpmiEvaluator(const WordCounter& word_counts,
                           size_t window_count)
      : word_counts_{word_counts},
        window_count_{window_count} {}

    explicit NpmiEvaluator(WordCounter&& word_counts,
                           size_t window_count)
      : word_counts_{std::move(word_counts)},
        window_count_{window_count} {}

  protected:
    double compute_association(const Word& left, const Word& right) const override {
      return compute_npmi_association(left, right, word_counts_, window_count_);
    }

  private:
    WordCounter word_counts_;
    size_t window_count_;

  };

  class CompressedNpmiEvaluator : public CompressedTopicEvaluator {
  public:
    using BaseClass = CompressedTopicEvaluator;

    explicit CompressedNpmiEvaluator(const WordIndexCounter& word_index_counts,
                                     size_t window_count)
      : word_index_counts_{word_index_counts},
        window_count_{window_count} {}

    explicit CompressedNpmiEvaluator(WordIndexCounter&& word_index_counts,
                                     size_t window_count)
      : word_index_counts_{std::move(word_index_counts)},
        window_count_{window_count} {}

  protected:
    double compute_association(const WordIndex& left, const WordIndex& right) const override {
      return compute_npmi_association(left, right, word_index_counts_, window_count_);
    }

  private:
    WordIndexCounter word_index_counts_;
    size_t window_count_;

  };

  class SparseCompressedNpmiEvaluator : public CompressedTopicEvaluator {
  public:
    using BaseClass = CompressedTopicEvaluator;

    explicit SparseCompressedNpmiEvaluator(const SparseWordIndexCounter& word_index_counts,
                                           size_t window_count)
      : word_index_counts_{word_index_counts},
        window_count_{window_count} {}

    explicit SparseCompressedNpmiEvaluator(SparseWordIndexCounter&& word_index_counts,
                                           size_t window_count)
      : word_index_counts_{std::move(word_index_counts)},
        window_count_{window_count} {}

  protected:
    double compute_association(const WordIndex& left, const WordIndex& right) const override {
      return compute_npmi_association(left, right, word_index_counts_, window_count_);
    }

  private:
    SparseWordIndexCounter word_index_counts_;
    size_t window_count_;

  };

  class CompressedAndCachedNpmiEvaluator : public CompressedTopicEvaluator {
  public:
    using BaseClass = CompressedTopicEvaluator;

    explicit CompressedAndCachedNpmiEvaluator(const WordIndexCounterCache& word_index_counts,
                                              size_t window_count)
      : word_index_counts_{word_index_counts},
        window_count_{window_count} {}

    explicit CompressedAndCachedNpmiEvaluator(WordIndexCounterCache&& word_index_counts,
                                              size_t window_count)
      : word_index_counts_{std::move(word_index_counts)},
        window_count_{window_count} {}

  protected:
    double compute_association(const WordIndex& left, const WordIndex& right) const override {
      return compute_npmi_association(left, right, word_index_counts_, window_count_);
    }

  private:
    WordIndexCounterCache word_index_counts_;
    size_t window_count_;

  };

} // namespace tomer

#endif // TOMER_TOPIC_EVALUATOR_H_
