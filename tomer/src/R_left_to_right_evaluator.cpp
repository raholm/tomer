#include <Rcpp.h>
#include <string>
#include <vector>
#include <memory>

#include "def.h"
#include "alphabet.h"
#include "type_sequence_builder.h"
#include "left_to_right_evaluator.h"

using namespace tomer;

Corpus create_corpus_from_R(const Rcpp::DataFrame& corpus,
                            size_t n_docs) {
  Corpus c;
  c.reserve(n_docs);

  Document current_doc;

  IntVector doc_id = Rcpp::as<IntVector>(corpus["id"]);
  StringVector doc_token = Rcpp::as<StringVector>(corpus["token"]);

  size_t current_id = doc_id.at(0);
  size_t previous_id = current_id;
  current_doc.push_back(doc_token.at(0));

  for (unsigned i = 1; i < doc_id.size(); ++i) {
    current_id = doc_id.at(i);

    if (current_id != previous_id) {
      c.push_back(current_doc);
      current_doc = Document{doc_token.at(i)};
    } else {
      current_doc.push_back(doc_token.at(i));
    }

    previous_id = current_id;
  }

  c.push_back(current_doc);

  return c;
}

Alphabet create_alphabet_from_R(const Rcpp::DataFrame& alphabet) {
  Map<Alphabet::Token, Alphabet::Type> a{};

  IntVector types = Rcpp::as<IntVector>(alphabet["type"]);
  StringVector tokens = Rcpp::as<StringVector>(alphabet["token"]);

  size_t type;
  std::string token;

  for (unsigned i = 0; i < types.size(); ++i) {
    type = types.at(i);
    token = tokens.at(i);
    a.insert(std::make_pair(token, type));
  }

  return Alphabet{a};
}

IntVector create_topic_counts_from_R(const Rcpp::DataFrame& topic_counts,
                                     size_t n_topics) {
  IntVector topics = Rcpp::as<IntVector>(topic_counts["topic"]);
  IntVector counts = Rcpp::as<IntVector>(topic_counts["count"]);
  IntVector tc(n_topics);

  size_t topic, count;

  for (unsigned i = 0; i < topics.size(); ++i) {
    topic = topics.at(i);
    count = counts.at(i);
    tc.at(topic) = count;
  }

  return tc;
}

IntMatrix create_type_topic_counts_from_R(const Rcpp::DataFrame& type_topic_counts,
                                          size_t n_types,
                                          size_t n_topics) {
  IntMatrix ttc(n_types);

  for (unsigned i = 0; i < n_types; ++i) {
    ttc.at(i) = IntVector(n_topics, 0);
  }

  IntVector types = Rcpp::as<IntVector>(type_topic_counts["type"]);
  IntVector topics = Rcpp::as<IntVector>(type_topic_counts["topic"]);
  IntVector counts = Rcpp::as<IntVector>(type_topic_counts["count"]);

  size_t type, topic, count;

  for (unsigned i = 0; i < types.size(); ++i) {
    type = types.at(i);
    topic = topics.at(i);
    count = counts.at(i);
    ttc.at(type).at(topic) = count;
  }

  return ttc;
}

// [[Rcpp::export]]
double evaluate_left_to_right_cpp(const Rcpp::DataFrame& corpus,
                                  size_t n_docs,
                                  const Rcpp::DataFrame& alphabet,
                                  size_t n_topics,
                                  const Rcpp::DataFrame& topic_counts,
                                  const Rcpp::DataFrame& type_topic_counts,
                                  const Rcpp::NumericVector& alpha,
                                  double beta,
                                  size_t n_particles,
                                  bool resampling,
                                  unsigned seed) {
  Corpus _corpus = create_corpus_from_R(corpus, n_docs);
  Alphabet _alphabet = create_alphabet_from_R(alphabet);
  size_t n_types = _alphabet.size();

  TypeSequenceBuilder builder{std::move(_alphabet), true};
  builder.add(_corpus);

  TypeSequenceContainer type_sequences = builder.get_data();

  IntVector _topic_counts = create_topic_counts_from_R(topic_counts, n_topics);
  IntMatrix _type_topic_counts = create_type_topic_counts_from_R(type_topic_counts,
                                                                 n_types,
                                                                 n_topics);
  DoubleVector _alpha = Rcpp::as<DoubleVector>(alpha);

  std::unique_ptr<MarginalProbEsimator> estimator = std::unique_ptr<MarginalProbEsimator>(new SparseLDATokenMarginalProbEstimator());
  std::unique_ptr<TopicSampler> sampler = std::unique_ptr<TopicSampler>(new SparseLDATopicSampler());

  if (seed > 0)
    sampler->set_seed(seed);

  LeftToRightEvaluator evaluator(n_topics,
                                 _alpha,
                                 beta,
                                 _topic_counts,
                                 _type_topic_counts,
                                 std::move(estimator),
                                 std::move(sampler));
  return evaluator.evaluate(type_sequences, n_particles, resampling);
}
