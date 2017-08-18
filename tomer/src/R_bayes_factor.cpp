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

double mode_compute_log_bayes_factor(TypeTopicIndicatorMode* const mode,
                                     const Rcpp::StringVector& tokens,
                                     size_t n_topics,
                                     double beta) {
  IntVector topic_indicators;
  topic_indicators.reserve(tokens.size());

  for (const auto& token : tokens) {
    auto type = Rcpp::as<TypeTopicIndicatorMode::type>(token);
    if (mode->contains(type))
      topic_indicators.push_back(mode->get_mode(type));
  }

  return compute_log_bayes_factor(topic_indicators, n_topics, beta);
}

RCPP_MODULE(mod_bayes_factor) {
  Rcpp::class_<TypeTopicIndicatorMode>("TypeTopicIndicatorMode")
    .constructor()

    .method("update", &mode_update)
    .method("compute_log_bayes_factor", &mode_compute_log_bayes_factor)
    ;
}
