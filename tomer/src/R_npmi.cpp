#include <Rcpp.h>

#include "def.h"
#include "util.h"
#include "topic_evaluator.h"

using namespace tomer;

struct NpmiData {
  explicit NpmiData(const WordCount& word_counts)
    : word_counts{word_counts},
      window_count{0} {}

  explicit NpmiData(WordCount&& word_counts)
    : word_counts{std::move(word_counts)},
      window_count{0} {}

  WordCount word_counts;
  size_t window_count;

};

WordCount create_word_counts(const Rcpp::List& topics) {
  auto ntopics = topics.size();

  WordToIndexTransformer transformer;
  TopicWordIndexRelationMap relations;

  for (unsigned i = 0; i < ntopics; ++i) {
    Vector<Word> topic_words = Rcpp::as<Vector<Word>>(topics(i));
    transformer.update(topic_words);
    auto topic_word_indexes = transformer.transform(topic_words);

    for (unsigned j = 0; j < topic_words.size(); ++j) {
      relations.update(topic_word_indexes.at(j), topic_word_indexes);
    }
  }

  return WordCount(transformer, relations);
}

void calculate_word_counts_and_window_count(const Rcpp::List& documents,
                                            size_t window_size,
                                            NpmiData& data) {
  auto ndocs = documents.size();
  size_t nwindows, window_count = 0;
  Vector<Word> words_in_window(window_size);
  WordCount& word_counts = data.word_counts;
  size_t head_id, tail_id, nwords;

  for (unsigned i = 0; i < ndocs; ++i) {
    Vector<Word> doc_words = Rcpp::as<Vector<Word>>(documents(i));
    auto doc_length = doc_words.size();

    if (window_size == 0)
      nwindows = 1;
    else
      nwindows = doc_length + window_size - 1;

    window_count += nwindows;

    for (unsigned j = 1; j < (nwindows + 1); ++j) {
      if (window_size == 0) {
        words_in_window = doc_words;
        remove_duplicates(&words_in_window);
        nwords = words_in_window.size();
      } else {
        head_id = (j > window_size) ? j - window_size : 0;
        tail_id = std::min((size_t) j, (size_t) doc_words.size());
        nwords = tail_id - head_id;

        for (unsigned k = 0; k < nwords; ++k) {
          words_in_window.at(k) = doc_words.at(head_id + k);
        }

        remove_duplicates_inplace(&words_in_window, &nwords);
      }

      for (unsigned left_idx = 0; left_idx < nwords; ++left_idx) {
        auto left_word = words_in_window.at(left_idx);
        word_counts.update(left_word);

        for (unsigned right_idx = left_idx + 1; right_idx < nwords; ++right_idx) {
          auto right_word = words_in_window.at(right_idx);
          word_counts.update(left_word, right_word);
        }
      }
    }
  }

  data.window_count = window_count;
}

// [[Rcpp::export]]
Rcpp::NumericVector evaluate_npmi_cpp(const Rcpp::List& topics,
                                      const Rcpp::List& documents,
                                      size_t window_size) {
  NpmiData data(std::move(create_word_counts(topics)));
  calculate_word_counts_and_window_count(documents, window_size, data);

  NpmiEvaluator evaluator(data.word_counts, data.window_count);
  auto ntopics = topics.size();
  Vector<double> vals(ntopics);

  for (unsigned i = 0; i < ntopics; ++i) {
    Vector<Word> topic_words = Rcpp::as<Vector<Word>>(topics(i));
    vals.at(i) = evaluator.evaluate(topic_words);
  }

  return Rcpp::wrap(vals);
}
