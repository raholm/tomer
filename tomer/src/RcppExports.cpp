// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// evaluate_left_to_right_cpp
double evaluate_left_to_right_cpp(const Rcpp::DataFrame& corpus, size_t n_docs, const Rcpp::DataFrame& alphabet, size_t n_topics, const Rcpp::DataFrame& topic_counts, const Rcpp::DataFrame& type_topic_counts, const Rcpp::NumericVector& alpha, double beta, size_t n_particles, bool resampling, unsigned seed);
RcppExport SEXP _tomer_evaluate_left_to_right_cpp(SEXP corpusSEXP, SEXP n_docsSEXP, SEXP alphabetSEXP, SEXP n_topicsSEXP, SEXP topic_countsSEXP, SEXP type_topic_countsSEXP, SEXP alphaSEXP, SEXP betaSEXP, SEXP n_particlesSEXP, SEXP resamplingSEXP, SEXP seedSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::DataFrame& >::type corpus(corpusSEXP);
    Rcpp::traits::input_parameter< size_t >::type n_docs(n_docsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::DataFrame& >::type alphabet(alphabetSEXP);
    Rcpp::traits::input_parameter< size_t >::type n_topics(n_topicsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::DataFrame& >::type topic_counts(topic_countsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::DataFrame& >::type type_topic_counts(type_topic_countsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::NumericVector& >::type alpha(alphaSEXP);
    Rcpp::traits::input_parameter< double >::type beta(betaSEXP);
    Rcpp::traits::input_parameter< size_t >::type n_particles(n_particlesSEXP);
    Rcpp::traits::input_parameter< bool >::type resampling(resamplingSEXP);
    Rcpp::traits::input_parameter< unsigned >::type seed(seedSEXP);
    rcpp_result_gen = Rcpp::wrap(evaluate_left_to_right_cpp(corpus, n_docs, alphabet, n_topics, topic_counts, type_topic_counts, alpha, beta, n_particles, resampling, seed));
    return rcpp_result_gen;
END_RCPP
}
// evaluate_npmi_cpp
Rcpp::NumericVector evaluate_npmi_cpp(const Rcpp::StringVector& topics, const Rcpp::StringVector& documents, size_t window_size);
RcppExport SEXP _tomer_evaluate_npmi_cpp(SEXP topicsSEXP, SEXP documentsSEXP, SEXP window_sizeSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::StringVector& >::type topics(topicsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::StringVector& >::type documents(documentsSEXP);
    Rcpp::traits::input_parameter< size_t >::type window_size(window_sizeSEXP);
    rcpp_result_gen = Rcpp::wrap(evaluate_npmi_cpp(topics, documents, window_size));
    return rcpp_result_gen;
END_RCPP
}
// evaluate_npmi_with_cache_cpp
Rcpp::NumericVector evaluate_npmi_with_cache_cpp(const Rcpp::StringVector& topics, const Rcpp::StringVector& documents, size_t window_size, const Rcpp::CharacterVector& filename);
RcppExport SEXP _tomer_evaluate_npmi_with_cache_cpp(SEXP topicsSEXP, SEXP documentsSEXP, SEXP window_sizeSEXP, SEXP filenameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::StringVector& >::type topics(topicsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::StringVector& >::type documents(documentsSEXP);
    Rcpp::traits::input_parameter< size_t >::type window_size(window_sizeSEXP);
    Rcpp::traits::input_parameter< const Rcpp::CharacterVector& >::type filename(filenameSEXP);
    rcpp_result_gen = Rcpp::wrap(evaluate_npmi_with_cache_cpp(topics, documents, window_size, filename));
    return rcpp_result_gen;
END_RCPP
}
// evaluate_npmi_from_file_cpp
Rcpp::NumericVector evaluate_npmi_from_file_cpp(const Rcpp::StringVector& topics, size_t window_size, const Rcpp::CharacterVector& filename);
RcppExport SEXP _tomer_evaluate_npmi_from_file_cpp(SEXP topicsSEXP, SEXP window_sizeSEXP, SEXP filenameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::StringVector& >::type topics(topicsSEXP);
    Rcpp::traits::input_parameter< size_t >::type window_size(window_sizeSEXP);
    Rcpp::traits::input_parameter< const Rcpp::CharacterVector& >::type filename(filenameSEXP);
    rcpp_result_gen = Rcpp::wrap(evaluate_npmi_from_file_cpp(topics, window_size, filename));
    return rcpp_result_gen;
END_RCPP
}
// create_word_count_cache_cpp
void create_word_count_cache_cpp(const Rcpp::StringVector& documents, size_t window_size, const Rcpp::CharacterVector& filename);
RcppExport SEXP _tomer_create_word_count_cache_cpp(SEXP documentsSEXP, SEXP window_sizeSEXP, SEXP filenameSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::StringVector& >::type documents(documentsSEXP);
    Rcpp::traits::input_parameter< size_t >::type window_size(window_sizeSEXP);
    Rcpp::traits::input_parameter< const Rcpp::CharacterVector& >::type filename(filenameSEXP);
    create_word_count_cache_cpp(documents, window_size, filename);
    return R_NilValue;
END_RCPP
}
// compute_markovian_bf_test_cpp
double compute_markovian_bf_test_cpp(const Rcpp::IntegerVector& topic_indicators, size_t n_topics, double beta);
RcppExport SEXP _tomer_compute_markovian_bf_test_cpp(SEXP topic_indicatorsSEXP, SEXP n_topicsSEXP, SEXP betaSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::IntegerVector& >::type topic_indicators(topic_indicatorsSEXP);
    Rcpp::traits::input_parameter< size_t >::type n_topics(n_topicsSEXP);
    Rcpp::traits::input_parameter< double >::type beta(betaSEXP);
    rcpp_result_gen = Rcpp::wrap(compute_markovian_bf_test_cpp(topic_indicators, n_topics, beta));
    return rcpp_result_gen;
END_RCPP
}
// compute_markovian_lr_test_cpp
double compute_markovian_lr_test_cpp(const Rcpp::IntegerVector& topic_indicators, size_t n_topics);
RcppExport SEXP _tomer_compute_markovian_lr_test_cpp(SEXP topic_indicatorsSEXP, SEXP n_topicsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::IntegerVector& >::type topic_indicators(topic_indicatorsSEXP);
    Rcpp::traits::input_parameter< size_t >::type n_topics(n_topicsSEXP);
    rcpp_result_gen = Rcpp::wrap(compute_markovian_lr_test_cpp(topic_indicators, n_topics));
    return rcpp_result_gen;
END_RCPP
}
// compute_chunking_bf_test_cpp
double compute_chunking_bf_test_cpp(const Rcpp::IntegerMatrix& topic_indicators, size_t n_topics, double beta);
RcppExport SEXP _tomer_compute_chunking_bf_test_cpp(SEXP topic_indicatorsSEXP, SEXP n_topicsSEXP, SEXP betaSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::IntegerMatrix& >::type topic_indicators(topic_indicatorsSEXP);
    Rcpp::traits::input_parameter< size_t >::type n_topics(n_topicsSEXP);
    Rcpp::traits::input_parameter< double >::type beta(betaSEXP);
    rcpp_result_gen = Rcpp::wrap(compute_chunking_bf_test_cpp(topic_indicators, n_topics, beta));
    return rcpp_result_gen;
END_RCPP
}
// evaluate_topic_coherence_cpp
Rcpp::NumericVector evaluate_topic_coherence_cpp(const Rcpp::StringVector& topics, const Rcpp::StringVector& documents);
RcppExport SEXP _tomer_evaluate_topic_coherence_cpp(SEXP topicsSEXP, SEXP documentsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::StringVector& >::type topics(topicsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::StringVector& >::type documents(documentsSEXP);
    rcpp_result_gen = Rcpp::wrap(evaluate_topic_coherence_cpp(topics, documents));
    return rcpp_result_gen;
END_RCPP
}
// evaluate_topic_coherence_with_cache_cpp
Rcpp::NumericVector evaluate_topic_coherence_with_cache_cpp(const Rcpp::StringVector& topics, const Rcpp::StringVector& documents, const Rcpp::CharacterVector& filename);
RcppExport SEXP _tomer_evaluate_topic_coherence_with_cache_cpp(SEXP topicsSEXP, SEXP documentsSEXP, SEXP filenameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::StringVector& >::type topics(topicsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::StringVector& >::type documents(documentsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::CharacterVector& >::type filename(filenameSEXP);
    rcpp_result_gen = Rcpp::wrap(evaluate_topic_coherence_with_cache_cpp(topics, documents, filename));
    return rcpp_result_gen;
END_RCPP
}
// evaluate_topic_coherence_from_file_cpp
Rcpp::NumericVector evaluate_topic_coherence_from_file_cpp(const Rcpp::StringVector& topics, const Rcpp::CharacterVector& filename);
RcppExport SEXP _tomer_evaluate_topic_coherence_from_file_cpp(SEXP topicsSEXP, SEXP filenameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::StringVector& >::type topics(topicsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::CharacterVector& >::type filename(filenameSEXP);
    rcpp_result_gen = Rcpp::wrap(evaluate_topic_coherence_from_file_cpp(topics, filename));
    return rcpp_result_gen;
END_RCPP
}
// run_testthat_tests
bool run_testthat_tests();
RcppExport SEXP _tomer_run_testthat_tests() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(run_testthat_tests());
    return rcpp_result_gen;
END_RCPP
}

RcppExport SEXP _rcpp_module_boot_mod_bayes_factor();

static const R_CallMethodDef CallEntries[] = {
    {"_tomer_evaluate_left_to_right_cpp", (DL_FUNC) &_tomer_evaluate_left_to_right_cpp, 11},
    {"_tomer_evaluate_npmi_cpp", (DL_FUNC) &_tomer_evaluate_npmi_cpp, 3},
    {"_tomer_evaluate_npmi_with_cache_cpp", (DL_FUNC) &_tomer_evaluate_npmi_with_cache_cpp, 4},
    {"_tomer_evaluate_npmi_from_file_cpp", (DL_FUNC) &_tomer_evaluate_npmi_from_file_cpp, 3},
    {"_tomer_create_word_count_cache_cpp", (DL_FUNC) &_tomer_create_word_count_cache_cpp, 3},
    {"_tomer_compute_markovian_bf_test_cpp", (DL_FUNC) &_tomer_compute_markovian_bf_test_cpp, 3},
    {"_tomer_compute_markovian_lr_test_cpp", (DL_FUNC) &_tomer_compute_markovian_lr_test_cpp, 2},
    {"_tomer_compute_chunking_bf_test_cpp", (DL_FUNC) &_tomer_compute_chunking_bf_test_cpp, 3},
    {"_tomer_evaluate_topic_coherence_cpp", (DL_FUNC) &_tomer_evaluate_topic_coherence_cpp, 2},
    {"_tomer_evaluate_topic_coherence_with_cache_cpp", (DL_FUNC) &_tomer_evaluate_topic_coherence_with_cache_cpp, 3},
    {"_tomer_evaluate_topic_coherence_from_file_cpp", (DL_FUNC) &_tomer_evaluate_topic_coherence_from_file_cpp, 2},
    {"_tomer_run_testthat_tests", (DL_FUNC) &_tomer_run_testthat_tests, 0},
    {"_rcpp_module_boot_mod_bayes_factor", (DL_FUNC) &_rcpp_module_boot_mod_bayes_factor, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_tomer(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
