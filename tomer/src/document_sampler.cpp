#include "document_sampler.h"

#include <random>
#include <numeric>

namespace tomer {

  static Vector<std::discrete_distribution<Topic>> create_topic_dists(size_t ndocs,
                                                                      const DoubleMatrix& doc_topic_probs) {
    Vector<std::discrete_distribution<Topic>> topic_dists(ndocs);
    for (unsigned i = 0; i < ndocs; ++i)
      topic_dists.at(i) = std::discrete_distribution<Type>{doc_topic_probs.at(i).cbegin(),
                                                           doc_topic_probs.at(i).cend()};
    return topic_dists;
  }

  static std::discrete_distribution<Topic> create_topic_dist(const DoubleVector& topic_probs) {
    return std::discrete_distribution<Topic>{topic_probs.cbegin(), topic_probs.cend()};
  }

  static Vector<std::discrete_distribution<Type>> create_type_dists(size_t ntopics,
                                                                    const DoubleMatrix& topic_type_probs) {
    Vector<std::discrete_distribution<Type>> type_dists(ntopics);
    for (unsigned i = 0; i < ntopics; ++i)
      type_dists.at(i) = std::discrete_distribution<Type>{topic_type_probs.at(i).cbegin(),
                                                          topic_type_probs.at(i).cend()};
    return type_dists;
  }

  static std::mt19937 create_generator() {
    std::mt19937 generator;
    generator.seed(std::random_device()());
    return generator;
  }

  SampledCorpus sample_corpus(const Vector<size_t>& length,
                              const DoubleMatrix& doc_topic_probs,
                              const DoubleMatrix& topic_type_probs) {
    auto size = std::accumulate(length.cbegin(), length.cend(), 0);
    SampledCorpus corpus(size);

    auto& doc_ids = corpus.doc_ids;
    auto& types = corpus.types;
    auto& topics = corpus.topics;

    auto ndocs = length.size();
    auto ntopics = topic_type_probs.size();

    auto generator = create_generator();
    auto topic_dists = create_topic_dists(ndocs, doc_topic_probs);
    auto type_dists = create_type_dists(ntopics, topic_type_probs);

    size_t current_doc = 0;
    size_t cumsum_of_processed_doc_lengths = 0;

    for (unsigned i = 0; i < size; ++i) {
      auto current_topic = topic_dists.at(current_doc)(generator);
      auto current_type = type_dists.at(current_topic)(generator);

      doc_ids.at(i) = current_doc;
      types.at(i) = current_type;
      topics.at(i) = current_topic;

      if ((i - cumsum_of_processed_doc_lengths) == length.at(current_doc))
        cumsum_of_processed_doc_lengths += length.at(current_doc++);
    }

    return corpus;
  }

  SampledDocument sample_document(size_t length,
                                  const DoubleVector& topic_probs,
                                  const DoubleMatrix& topic_type_probs) {
    SampledDocument document(length);

    auto& types = document.types;
    auto& topics = document.topics;

    auto ntopics = topic_type_probs.size();

    auto generator = create_generator();
    auto topic_dist = create_topic_dist(topic_probs);
    auto type_dists = create_type_dists(ntopics, topic_type_probs);

    for (unsigned i = 0; i < length; ++i) {
      auto current_topic = topic_dist(generator);
      auto current_type = type_dists.at(current_topic)(generator);

      types.at(i) = current_type;
      topics.at(i) = current_topic;
    }

    return document;
  }

  // Pair<TypeMatrix, TopicMatrix> sample_corpus(const Vector<size_t>& length,
  //                                             const DoubleMatrix& doc_topic_probs,
  //                                             const DoubleMatrix& topic_type_probs) {
  //   auto ndocs = length.size();
  //   auto ntopics = topic_type_probs.size();

  //   TypeMatrix types(ndocs);
  //   TopicMatrix topics(ndocs);

  //   for (unsigned i = 0; i < ndocs; ++i) {
  //     auto l = length.at(i);
  //     types.at(i) = TypeVector(l);
  //     topics.at(i) = TopicVector(l);
  //   }

  //   std::default_random_engine generator;
  //   auto topic_dists = create_topic_dists(ndocs, doc_topic_probs);
  //   auto type_dists = create_type_dists(ntopics, topic_type_probs);

  //   for (unsigned doc_idx = 0; doc_idx < ndocs; ++ndocs) {
  //     auto topic_dist = topic_dists.at(doc_idx);

  //     for (unsigned type_idx = 0; type_idx < length.at(doc_idx); ++type_idx) {
  //       auto current_topic = topic_dist(generator);
  //       auto current_type = type_dists.at(current_topic)(generator);

  //       types.at(doc_idx).at(type_idx) = current_type;
  //       topics.at(doc_idx).at(type_idx) = current_topic;
  //     }
  //   }

  //   return std::make_pair(types, topics);
  // }

  // Pair<TypeMatrix, TopicMatrix> sample_corpus(const Vector<size_t>& length,
  //                                             const DoubleVector& topic_probs,
  //                                             const DoubleMatrix& topic_type_probs) {
  //   auto ndocs = length.size();
  //   auto ntopics = topic_type_probs.size();

  //   TypeMatrix types(ndocs);
  //   TopicMatrix topics(ndocs);

  //   for (unsigned i = 0; i < ndocs; ++i) {
  //     auto l = length.at(i);
  //     types.at(i) = TypeVector(l);
  //     topics.at(i) = TopicVector(l);
  //   }

  //   std::default_random_engine generator;
  //   auto topic_dist = create_topic_dist(topic_probs);
  //   auto type_dists = create_type_dists(ntopics, topic_type_probs);

  //   for (unsigned doc_idx = 0; doc_idx < ndocs; ++ndocs) {
  //     for (unsigned type_idx = 0; type_idx < length.at(doc_idx); ++type_idx) {
  //       auto current_topic = topic_dist(generator);
  //       auto current_type = type_dists.at(current_topic)(generator);

  //       types.at(doc_idx).at(type_idx) = current_type;
  //       topics.at(doc_idx).at(type_idx) = current_topic;
  //     }
  //   }

  //   return std::make_pair(types, topics);
  // }


  // Pair<TypeVector, TopicVector> sample_document(size_t length,
  //                                               const DoubleVector& topic_probs,
  //                                               const DoubleMatrix& topic_type_probs) {
  //   TypeVector types(length);
  //   TopicVector topics(length);

  //   auto ntopics = topic_type_probs.size();

  //   std::default_random_engine generator;
  //   auto topic_dist = create_topic_dist(topic_probs);
  //   auto type_dists = create_type_dists(ntopics, topic_type_probs);

  //   for (unsigned i = 0; i < length; ++i) {
  //     auto current_topic = topic_dist(generator);
  //     auto current_type = type_dists.at(current_topic)(generator);

  //     topics.at(i) = current_topic;
  //     types.at(i) = current_type;
  //   }

  //   return std::make_pair(types, topics);
  // }

} // namespace tomer
