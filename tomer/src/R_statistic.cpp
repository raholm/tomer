#include <Rcpp.h>

#include "def.h"
#include "util.h"
#include "topic_indicator_mode.h"
#include "statistic.h"

using namespace tomer;

void mode_update(TypeTopicIndicatorMode* const mode,
                 const Rcpp::DataFrame& types_topic_indicators) {
  unsigned n = types_topic_indicators.nrow();

  TypeTopicIndicatorMode::TypeVector types(n);
  TypeTopicIndicatorMode::Type t;

  Rcpp::IntegerVector doc = types_topic_indicators["doc"];
  Rcpp::IntegerVector pos = types_topic_indicators["pos"];
  Rcpp::IntegerVector type = types_topic_indicators["type"];
  Rcpp::IntegerVector topic_indicators = types_topic_indicators["topic"];


  for (unsigned i = 0; i < n; ++i) {
    t.doc = doc(i);
    t.pos = pos(i);
    t.type = type(i);

    types.at(i) = t;
  }

  mode->update(types, convert_from_R(topic_indicators));
}

Rcpp::DataFrame mode_get_data(TypeTopicIndicatorMode* const mode) {
  auto data = mode->get_data();
  unsigned n = data.size();

  Rcpp::IntegerVector doc(n);
  Rcpp::IntegerVector pos(n);
  Rcpp::IntegerVector type(n);
  Rcpp::IntegerVector topic_indicator(n);

  for (unsigned i = 0; i < n; ++i) {
    auto pair = data.at(i);
    doc(i) = pair.first.doc;
    pos(i) = pair.first.pos;
    type(i) = pair.first.type;
    topic_indicator(i) = pair.second;
  }

  return Rcpp::DataFrame::create(Rcpp::Named("doc") = doc,
                                 Rcpp::Named("pos") = pos,
                                 Rcpp::Named("type") = type,
                                 Rcpp::Named("topic") = topic_indicator);
}

Rcpp::IntegerVector mode_types_to_topic_indicators(TypeTopicIndicatorMode* const mode,
                                                   const Rcpp::DataFrame& types) {
  unsigned n = types.nrow();

  IntVector topic_indicators;
  topic_indicators.reserve(n);

  TypeTopicIndicatorMode::Type t;

  Rcpp::IntegerVector doc = types["doc"];
  Rcpp::IntegerVector pos = types["pos"];
  Rcpp::IntegerVector type = types["type"];

  for (unsigned i = 0; i < n; ++i) {
    t.doc = doc(i);
    t.pos = pos(i);
    t.type = type(i);

    if (mode->contains(t))
      topic_indicators.push_back(mode->get_mode(t));
  }

  return Rcpp::wrap(topic_indicators);
}

RCPP_MODULE(mod_bayes_factor) {
  Rcpp::class_<TypeTopicIndicatorMode>("TypeTopicIndicatorMode")
    .default_constructor()
    .method("update", &mode_update)
    .method("types_to_topic_indicators", &mode_types_to_topic_indicators)
    .method("data", &mode_get_data)
    ;
}

// [[Rcpp::export]]
double compute_markovian_bf_test_cpp(const Rcpp::IntegerVector& topic_indicators,
                                     size_t n_topics,
                                     double beta) {
  return compute_markovian_bf_test(convert_from_R(topic_indicators), n_topics, beta);
}

// [[Rcpp::export]]
double compute_markovian_lr_test_cpp(const Rcpp::IntegerVector& topic_indicators,
                                     size_t n_topics) {
  return compute_markovian_lr_test(convert_from_R(topic_indicators), n_topics);
}

// [[Rcpp::export]]
double compute_chunking_bf_test_cpp(const Rcpp::IntegerMatrix& topic_indicators,
                                    size_t n_topics,
                                    double beta) {
  return compute_chunking_bf_test(convert_from_R(topic_indicators), n_topics, beta);
}
