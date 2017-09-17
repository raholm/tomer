#ifndef TOMER_TOPIC_EVALUATOR_H_
#define TOMER_TOPIC_EVALUATOR_H_

#include "def.h"
#include "counter.h"

namespace tomer {

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

  class TopicCoherenceEvaluator : public TopicEvaluator {
  public:
    using BaseClass = TopicEvaluator;

    TopicCoherenceEvaluator(const WordCounter& word_counts)
      : word_counts_{word_counts} {}

  protected:
    inline double compute_association(const Word& left, const Word& right) const override {
      auto left_count = word_counts_.get_count(left);
      auto combined_count = word_counts_.get_count(left, right);
      if (missing_count(left_count)) return 0.0;
      return log((double) (combined_count + 1) / left_count);
    }

  private:
    WordCounter word_counts_;

    inline bool missing_count(size_t count) const {
      return count == 0;
    }

  };

  class NormalisedPointwiseMutualInformationEvaluator : public TopicEvaluator {
  public:
    using BaseClass = TopicEvaluator;

    explicit NormalisedPointwiseMutualInformationEvaluator(const WordCounter& word_counts,
                                                           size_t window_count)
      : word_counts_{word_counts},
        window_count_{window_count} {}

    explicit NormalisedPointwiseMutualInformationEvaluator(WordCounter&& word_counts,
                                                           size_t window_count)
      : word_counts_{std::move(word_counts)},
        window_count_{window_count} {}

  protected:
    double compute_association(const Word& left, const Word& right) const override {
      auto left_count = word_counts_.get_count(left);
      auto right_count = word_counts_.get_count(right);
      auto combined_count = word_counts_.get_count(left, right);

      if (missing_count(left_count) ||
          missing_count(right_count) ||
          missing_count(combined_count)) return 0.0;

      double denominator = -log((double) combined_count / window_count_);
      if (denominator == 0) return 0.0;
      double numerator = log((double) (combined_count * window_count_) / (left_count * right_count));
      return numerator / denominator;
    }

  private:
    WordCounter word_counts_;
    size_t window_count_;

    inline bool missing_count(size_t count) const {
      return count == 0;
    }

  };

  using NpmiEvaluator = NormalisedPointwiseMutualInformationEvaluator;

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

  class CompressedTopicCoherenceEvaluator : public CompressedTopicEvaluator {
  public:
    using BaseClass = CompressedTopicEvaluator;

    CompressedTopicCoherenceEvaluator(const WordIndexCounter& word_index_counts)
      : word_index_counts_{word_index_counts} {}

  protected:
    inline double compute_association(const WordIndex& left, const WordIndex& right) const override {
      auto left_count = word_index_counts_.get_count(left);
      auto combined_count = word_index_counts_.get_count(left, right);
      if (missing_count(left_count)) return 0.0;
      return log((double) (combined_count + 1) / left_count);
    }

  private:
    WordIndexCounter word_index_counts_;

    inline bool missing_count(size_t count) const {
      return count == 0;
    }

  };

  class CompressedNormalisedPointwiseMutualInformationEvaluator : public CompressedTopicEvaluator {
  public:
    using BaseClass = CompressedTopicEvaluator;

    explicit CompressedNormalisedPointwiseMutualInformationEvaluator(const WordIndexCounter& word_index_counts,
                                                                     size_t window_count)
      : word_index_counts_{word_index_counts},
        window_count_{window_count} {}

    explicit CompressedNormalisedPointwiseMutualInformationEvaluator(WordIndexCounter&& word_index_counts,
                                                                     size_t window_count)
      : word_index_counts_{std::move(word_index_counts)},
        window_count_{window_count} {}

  protected:
    double compute_association(const WordIndex& left, const WordIndex& right) const override {
      auto left_count = word_index_counts_.get_count(left);
      auto right_count = word_index_counts_.get_count(right);
      auto combined_count = word_index_counts_.get_count(left, right);

      if (missing_count(left_count) ||
          missing_count(right_count) ||
          missing_count(combined_count)) return 0.0;

      double denominator = -log((double) combined_count / window_count_);
      if (denominator == 0) return 0.0;
      double numerator = log((double) (combined_count * window_count_) / (left_count * right_count));
      return numerator / denominator;
    }

  private:
    WordIndexCounter word_index_counts_;
    size_t window_count_;

    inline bool missing_count(size_t count) const {
      return count == 0;
    }

  };

  using CompressedNpmiEvaluator = CompressedNormalisedPointwiseMutualInformationEvaluator;

} // namespace tomer

#endif // TOMER_TOPIC_EVALUATOR_H_
