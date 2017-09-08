#ifndef TOMER_DOCUMENT_SAMPLER_H_
#define TOMER_DOCUMENT_SAMPLER_H_

#include "def.h"

namespace tomer {

  struct SampledCorpus {
    explicit SampledCorpus(size_t size)
      : doc_ids(size),
        types(size),
        topics(size) {}

    IntVector doc_ids;
    TypeVector types;
    TopicVector topics;

  };

  struct SampledDocument {
    explicit SampledDocument(size_t size)
      : id(0),
        types(size),
        topics(size) {}
    explicit SampledDocument(size_t size, size_t id)
      : id(id),
        types(size),
        topics(size) {}


    size_t id;
    TypeVector types;
    TopicVector topics;

  };

  SampledCorpus sample_corpus(const Vector<size_t>& length,
                              const DoubleMatrix& doc_topic_probs,
                              const DoubleMatrix& topic_type_probs);

  SampledDocument sample_document(size_t length,
                                  const DoubleVector& topic_probs,
                                  const DoubleMatrix& topic_type_probs);

  // Pair<TypeMatrix, TopicMatrix> sample_corpus(const Vector<size_t>& length,
  //                                             const DoubleMatrix& doc_topic_probs,
  //                                             const DoubleMatrix& topic_type_probs);

  // Pair<TypeMatrix, TopicMatrix> sample_corpus(const Vector<size_t>& length,
  //                                             const DoubleVector& topic_probs,
  //                                             const DoubleMatrix& topic_type_probs);

  // Pair<TypeVector, TopicVector> sample_document(size_t length,
  //                                               const DoubleVector& topic_probs,
  //                                               const DoubleMatrix& topic_type_probs);

} // namespace tomer

#endif // TOMER_DOCUMENT_SAMPLER_H_
