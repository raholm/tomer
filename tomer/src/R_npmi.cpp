#include <Rcpp.h>
#include <exception>

#include <iostream>
#include <chrono>

#include "def.h"
#include "util.h"
#include "topic_evaluator.h"
#include "topic_evaluator_util.h"
#include "topic_evaluator_cache.h"
#include "tokenizer.h"

using namespace tomer;

// [[Rcpp::export]]
Rcpp::NumericVector evaluate_npmi_cpp(const Rcpp::StringVector& topics,
                                      const Rcpp::StringVector& documents,
                                      size_t window_size) {
  StringVector tmp_tops = Rcpp::as<StringVector>(topics);
  StringVector tmp_docs = Rcpp::as<StringVector>(documents);

  WordIndexTokenizer tokenizer;
  Matrix<WordIndexTokenizer::Token> tops = tokenizer.transform(tmp_tops);
  Matrix<WordIndexTokenizer::Token> docs = tokenizer.transform(tmp_docs);

  WordIndexTopicEvaluatorData data(std::move(create_word_index_counts(tops)));
  calculate_word_index_counts_and_window_count(docs, window_size, &data);

  CompressedNpmiEvaluator evaluator(std::move(data.word_index_counts), data.window_count);
  auto ntopics = tops.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : tops)
    vals.push_back(evaluator.evaluate(topic));

  return Rcpp::wrap(vals);
}

// [[Rcpp::export]]
Rcpp::NumericVector evaluate_npmi_with_cache_cpp(const Rcpp::StringVector& topics,
                                                 const Rcpp::StringVector& documents,
                                                 size_t window_size,
                                                 const Rcpp::CharacterVector& filename) {
  StringVector tmp_tops = Rcpp::as<StringVector>(topics);
  StringVector tmp_docs = Rcpp::as<StringVector>(documents);
  String tmp_filename = Rcpp::as<String>(filename);

  WordIndexTopicEvaluatorDataCache cache;
  Matrix<WordIndexTokenizer::Token> tops;
  Matrix<WordIndexTokenizer::Token> docs;

  // Cache related stuff
  if (file_exists(tmp_filename)) {
    // Cache exists
    cache = WordIndexTopicEvaluatorDataCacheReader().read(tmp_filename);

    WordIndexTokenizerCache tokenizer{cache.transformer};
    tops = tokenizer.transform(tmp_tops);
    docs = tokenizer.transform(tmp_docs);
  } else {
    // Create cache
    WordIndexTokenizer tokenizer;
    tops = tokenizer.transform(tmp_tops);
    docs = tokenizer.transform(tmp_docs);

    WordToIndexTransformerCache  transformer_cache{tokenizer.get_transformer()};

    WordIndexTopicEvaluatorData data(std::move(create_word_index_counts(tops)));
    calculate_word_index_counts_and_window_count(docs, window_size, &data);

    WordIndexCounterCache counter_cache{data.word_index_counts};

    cache.transformer = std::move(transformer_cache);
    cache.word_index_counts = std::move(counter_cache);
    cache.window_count = data.window_count;

    bool status_ok = WordIndexTopicEvaluatorDataCacheWriter().write(cache, tmp_filename);
    if (!status_ok)
      throw std::runtime_error("Could not write out cache to disk.");
  }
  // ----

  CompressedAndCachedNpmiEvaluator evaluator(std::move(cache.word_index_counts), cache.window_count);
  auto ntopics = tops.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : tops)
    vals.push_back(evaluator.evaluate(topic));

  return Rcpp::wrap(vals);
}

// [[Rcpp::export]]
void create_word_count_cache_cpp(const Rcpp::StringVector& documents,
                                 size_t window_size,
                                 const Rcpp::CharacterVector& filename) {
  if (file_exists(Rcpp::as<String>(filename)))
    throw std::runtime_error("File already exists.");

  WordIndexTokenizer tokenizer;
  Matrix<WordIndexTokenizer::Token> docs = tokenizer.transform(Rcpp::as<StringVector>(documents));
  WordToIndexTransformerCache  transformer_cache{tokenizer.get_transformer()};

  auto ndocs = documents.size();
  size_t nwindows, window_count = 0;
  WordIndexWindow words_in_window(window_size);
  WordIndexCounterCache word_index_counts;

  size_t head_id, tail_id, nwords;

  for (unsigned i = 0; i < ndocs; ++i) {
    auto doc_words = docs.at(i);
    auto doc_length = doc_words.size();

    if (window_size == INF_WORD_WINDOW)
      nwindows = 1;
    else
      nwindows = doc_length + window_size - 1;

    window_count += nwindows;

    for (unsigned j = 1; j < (nwindows + 1); ++j) {
      if (window_size == INF_WORD_WINDOW) {
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
        word_index_counts.update(left_word);

        for (unsigned right_idx = left_idx + 1; right_idx < nwords; ++right_idx) {
          auto right_word = words_in_window.at(right_idx);
          word_index_counts.update(left_word, right_word);
        }
      }
    }
  }

  WordIndexTopicEvaluatorDataCache cache{std::move(transformer_cache), std::move(word_index_counts), window_size};
  bool status_ok = WordIndexTopicEvaluatorDataCacheWriter().write(cache, Rcpp::as<String>(filename));
  if (!status_ok)
    throw std::runtime_error("Could not write out cache to disk.");
}
