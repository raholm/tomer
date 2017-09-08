#include "document_sampler.h"

#include <random>

namespace tomer {

  Pair<TypeMatrix, TopicMatrix> sample_corpus(const Vector<size_t>& length,
                                              const DoubleMatrix& doc_topic_probs,
                                              const DoubleMatrix& topic_type_probs) {
    auto ndocs = length.size();
    auto ntopics = topic_type_probs.size();

    TypeMatrix types(ndocs);
    TopicMatrix topics(ndocs);

    for (unsigned i = 0; i < ndocs; ++i) {
      auto l = length.at(i);
      types.at(i) = TypeVector(l);
      topics.at(i) = TopicVector(l);
    }

    std::default_random_engine generator;

    Vector<std::discrete_distribution<Topic>> topic_dists(ndocs);
    Vector<std::discrete_distribution<Type>> type_dists(ntopics);

    for (unsigned i = 0; i < ndocs; ++i) {
      topic_dists.at(i) = std::discrete_distribution<Type>{doc_topic_probs.at(i).cbegin(),
                                                           doc_topic_probs.at(i).cend()};
    }

    for (unsigned i = 0; i < ntopics; ++i) {
      type_dists.at(i) = std::discrete_distribution<Type>{topic_type_probs.at(i).cbegin(),
                                                          topic_type_probs.at(i).cend()};
    }

    for (unsigned doc_idx = 0; doc_idx < ndocs; ++ndocs) {
      auto topic_dist = topic_dists.at(doc_idx);

      for (unsigned type_idx = 0; type_idx < length.at(doc_idx); ++type_idx) {
        auto current_topic = topic_dist(generator);
        auto current_type = type_dists.at(current_topic)(generator);

        types.at(doc_idx).at(type_idx) = current_type;
        topics.at(doc_idx).at(type_idx) = current_topic;
      }
    }

    return std::make_pair(types, topics);
  }

  Pair<TypeMatrix, TopicMatrix> sample_corpus(const Vector<size_t>& length,
                                              const DoubleVector& topic_probs,
                                              const DoubleMatrix& topic_type_probs) {
    auto ndocs = length.size();
    auto ntopics = topic_type_probs.size();

    TypeMatrix types(ndocs);
    TopicMatrix topics(ndocs);

    for (unsigned i = 0; i < ndocs; ++i) {
      auto l = length.at(i);
      types.at(i) = TypeVector(l);
      topics.at(i) = TopicVector(l);
    }

    std::default_random_engine generator;
    std::discrete_distribution<Topic> topic_dist{topic_probs.cbegin(), topic_probs.cend()};
    Vector<std::discrete_distribution<Type>> type_dists(ntopics);

    for (unsigned i = 0; i < ntopics; ++i) {
      type_dists.at(i) = std::discrete_distribution<Type>{topic_type_probs.at(i).cbegin(),
                                                          topic_type_probs.at(i).cend()};
    }

    for (unsigned doc_idx = 0; doc_idx < ndocs; ++ndocs) {
      for (unsigned type_idx = 0; type_idx < length.at(doc_idx); ++type_idx) {
        auto current_topic = topic_dist(generator);
        auto current_type = type_dists.at(current_topic)(generator);

        types.at(doc_idx).at(type_idx) = current_type;
        topics.at(doc_idx).at(type_idx) = current_topic;
      }
    }

    return std::make_pair(types, topics);
  }


  Pair<TypeVector, TopicVector> sample_document(size_t length,
                                                const DoubleVector& topic_probs,
                                                const DoubleMatrix& topic_type_probs) {
    TypeVector types(length);
    TopicVector topics(length);

    auto ntopics = topic_type_probs.size();

    std::default_random_engine generator;
    std::discrete_distribution<Topic> topic_dist{topic_probs.cbegin(), topic_probs.cend()};
    Vector<std::discrete_distribution<Type>> type_dists(ntopics);

    for (unsigned i = 0; i < ntopics; ++i) {
      type_dists.at(i) = std::discrete_distribution<Type>{topic_type_probs.at(i).cbegin(),
                                                          topic_type_probs.at(i).cend()};
    }

    for (unsigned i = 0; i < length; ++i) {
      auto current_topic = topic_dist(generator);
      auto current_type = type_dists.at(current_topic)(generator);

      topics.at(i) = current_topic;
      types.at(i) = current_type;
    }

    return std::make_pair(types, topics);
  }

} // tomer
