#include <Rcpp.h>

#include "def.h"
#include "util.h"
#include "tokenizer.h"
#include "topic_evaluator.h"
#include "topic_evaluator_util.h"
#include "topic_evaluator_cache.h"

using namespace tomer;

// [[Rcpp::export]]
Rcpp::NumericVector evaluate_topic_coherence_cpp(const Rcpp::StringVector& topics,
                                                 const Rcpp::StringVector& documents) {
  WordIndexTokenizer tokenizer;
  Matrix<WordIndexTokenizer::Token> tops = tokenizer.transform(topics);
  Matrix<WordIndexTokenizer::Token> docs = tokenizer.transform(documents);

  WordIndexTopicEvaluatorData data(std::move(create_word_index_counts(tops)));
  calculate_word_index_counts_and_window_count(docs, INF_WORD_WINDOW, &data);

  CompressedTopicCoherenceEvaluator evaluator(std::move(data.word_index_counts));
  auto ntopics = tops.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : tops)
    vals.push_back(evaluator.evaluate(topic));

  return Rcpp::wrap(vals);
}

// [[Rcpp::export]]
Rcpp::NumericVector evaluate_topic_coherence_with_cache_cpp(const Rcpp::StringVector& topics,
                                                            const Rcpp::StringVector& documents,
                                                            const Rcpp::CharacterVector& filename) {
  String tmp_filename = Rcpp::as<String>(filename);

  WordIndexTopicEvaluatorDataCache cache;
  Matrix<WordIndexTokenizer::Token> tops;
  Matrix<WordIndexTokenizer::Token> docs;

  // Cache related stuff
  if (file_exists(tmp_filename)) {
    // Cache exists
    cache = WordIndexTopicEvaluatorDataCacheReader().read(tmp_filename);

    WordIndexTokenizerCache tokenizer{std::move(cache.transformer)};
    tops = tokenizer.transform(topics);
    docs = tokenizer.transform(documents);
  } else {
    // Create cache
    WordIndexTokenizer tokenizer;
    tops = tokenizer.transform(topics);
    docs = tokenizer.transform(documents);

    WordToIndexTransformerCache transformer_cache{std::move(tokenizer.get_transformer())};

    WordIndexTopicEvaluatorData data(std::move(create_word_index_counts(tops)));
    calculate_word_index_counts_and_window_count(docs, INF_WORD_WINDOW, &data);

    WordIndexCounterCache counter_cache{data.word_index_counts};

    cache.transformer = std::move(transformer_cache);
    cache.word_index_counts = std::move(counter_cache);
    cache.window_count = data.window_count;

    bool status_ok = WordIndexTopicEvaluatorDataCacheWriter().write(cache, tmp_filename);
    if (!status_ok)
      throw std::runtime_error("Could not write out cache to disk.");
  }
  // ----

  CompressedAndCachedTopicCoherenceEvaluator evaluator(std::move(cache.word_index_counts));
  auto ntopics = tops.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : tops)
    vals.push_back(evaluator.evaluate(topic));

  return Rcpp::wrap(vals);
}

// [[Rcpp::export]]
Rcpp::NumericVector evaluate_topic_coherence_from_file_cpp(const Rcpp::StringVector& topics,
                                                           const Rcpp::CharacterVector& filename) {

  FixedWordIndexTokenizer tokenizer;
  Matrix<WordIndex> topic_word_indexes = tokenizer.transform(Rcpp::as<StringVector>(topics));
  tokenizer.set_full();
  size_t n_tokens = tokenizer.get_transformer().get_indexes().size();

  SparseWordIndexTopicEvaluatorData data{std::move(SparseWordIndexCounter(n_tokens))};
  calculate_word_counts_and_window_count(Rcpp::as<String>(filename),
                                         INF_WORD_WINDOW,
                                         tokenizer,
                                         &data);

  SparseCompressedTopicCoherenceEvaluator evaluator(std::move(data.word_index_counts));

  auto ntopics = topic_word_indexes.size();
  Vector<double> vals;
  vals.reserve(ntopics);

  for (auto const& topic : topic_word_indexes)
    vals.push_back(evaluator.evaluate(topic));

  return Rcpp::wrap(vals);
}
