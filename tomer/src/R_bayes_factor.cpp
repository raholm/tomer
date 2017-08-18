#include <Rcpp.h>

#include "def.h"
#include "topic_indicator_mode.h"
#include "bayes_factor.h"

using namespace tomer;

void mode_update(TypeTopicIndicatorMode* const mode,
                 const Rcpp::StringVector& tokens,
                 const Rcpp::IntegerVector& topic_indicators) {
  mode->update(Rcpp::as<StringVector>(tokens),
               Rcpp::as<IntVector>(topic_indicators));
}

Rcpp::List mode_get_data(TypeTopicIndicatorMode* const mode) {
  auto data = mode->get_data();

  Rcpp::StringVector types(data.size());
  Rcpp::IntegerVector topic_indicators(data.size());

  for (unsigned i = 0; i < data.size(); ++i) {
    auto pair = data.at(i);
    types(i) = pair.first;
    topic_indicators(i) = pair.second;
  }

  return Rcpp::List::create(Rcpp::Named("types") = types,
                            Rcpp::Named("topic_indocators") = topic_indicators);
}

Rcpp::IntegerVector mode_tokens_to_topic_indicators(TypeTopicIndicatorMode* const mode,
                                                    const Rcpp::StringVector& tokens) {
  IntVector topic_indicators;
  topic_indicators.reserve(tokens.size());

  for (auto const& token : tokens) {
    auto type = Rcpp::as<TypeTopicIndicatorMode::type>(token);
    if (mode->contains(type))
      topic_indicators.push_back(mode->get_mode(type));
  }

  return Rcpp::wrap(topic_indicators);
}

double compute_log_bayes_factor_cpp(const Rcpp::IntegerVector& topic_indicators,
                                    size_t n_topics,
                                    double beta) {
  return compute_log_bayes_factor(Rcpp::as<IntVector>(topic_indicators), n_topics, beta);
}

RCPP_MODULE(mod_bayes_factor) {
  Rcpp::class_<TypeTopicIndicatorMode>("TypeTopicIndicatorMode")
    .constructor()

    .method("update", &mode_update)
    .method("tokens_to_topic_indicators", &mode_tokens_to_topic_indicators)
    .method("get_data", &mode_get_data)
    ;

  function("compute_log_bayes_factor", &compute_log_bayes_factor_cpp);
}
