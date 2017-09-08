#ifndef TOMER_DOCUMENT_SAMPLER_H_
#define TOMER_DOCUMENT_SAMPLER_H_

#include "def.h"

namespace tomer {

  Pair<TypeMatrix, TopicMatrix> sample_corpus(const Vector<size_t>& length,
                                              const DoubleMatrix& doc_topic_probs,
                                              const DoubleMatrix& topic_type_probs);

  Pair<TypeMatrix, TopicMatrix> sample_corpus(const Vector<size_t>& length,
                                              const DoubleVector& topic_probs,
                                              const DoubleMatrix& topic_type_probs);

  Pair<TypeVector, TopicVector> sample_document(size_t length,
                                                const DoubleVector& topic_probs,
                                                const DoubleMatrix& topic_type_probs);

} // namespace tomer

#endif // TOMER_DOCUMENT_SAMPLER_H_
