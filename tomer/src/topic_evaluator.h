#ifndef TOMER_TOPIC_EVALUATOR_H_
#define TOMER_TOPIC_EVALUATOR_H_

#include "def.h"
#include "word_count.h"

namespace tomer {

  class TopicEvaluator {
  public:
    TopicEvaluator() = default;

    virtual ~TopicEvaluator() = default;

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

    protected:
      virtual double compute_association(const Word& left, const Word& right) const = 0;

  };

  class TopicCoherenceEvaluator : public TopicEvaluator {
  public:
    using BaseClass = TopicEvaluator;

    TopicCoherenceEvaluator(const WordCount& word_counts)
      : word_counts_{word_counts} {}

    ~TopicCoherenceEvaluator() = default;

  protected:
    inline double compute_association(const Word& left, const Word& right) const override {
      auto right_count = word_counts_.get_count(right);
      auto combined_count = word_counts_.get_count(left, right);
      return log((combined_count + 1) / right_count);
    }

  private:
    WordCount word_counts_;

  };

  class NormalisedPointwiseMutualInformationEvaluator : public TopicEvaluator {
  public:
    using BaseClass = TopicEvaluator;

    explicit NormalisedPointwiseMutualInformationEvaluator(const WordCount& word_counts,
                                                           size_t window_count)
      : word_counts_{word_counts},
        window_count_{window_count} {}

    ~NormalisedPointwiseMutualInformationEvaluator() = default;

  protected:
    double compute_association(const Word& left, const Word& right) const override {
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

  private:
    WordCount word_counts_;
    size_t window_count_;

    inline bool missing_count(size_t count) const {
      return count == 0;
    }

  };

  using NpmiEvaluator = NormalisedPointwiseMutualInformationEvaluator;
  using NPMIEvaluator = NormalisedPointwiseMutualInformationEvaluator;

} // namespace tomer

#endif // TOMER_TOPIC_EVALUATOR_H_
