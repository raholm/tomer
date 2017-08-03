// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// evaluate_left_to_right_cpp
Rcpp::NumericVector evaluate_left_to_right_cpp(const Rcpp::DataFrame& corpus, std::size_t n_docs, const Rcpp::DataFrame& alphabet, std::size_t n_topics, const Rcpp::DataFrame& topic_counts, const Rcpp::DataFrame& type_topic_counts, const Rcpp::NumericVector& alpha, double beta, std::size_t n_particles, bool resampling);
RcppExport SEXP _tomer_evaluate_left_to_right_cpp(SEXP corpusSEXP, SEXP n_docsSEXP, SEXP alphabetSEXP, SEXP n_topicsSEXP, SEXP topic_countsSEXP, SEXP type_topic_countsSEXP, SEXP alphaSEXP, SEXP betaSEXP, SEXP n_particlesSEXP, SEXP resamplingSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::DataFrame& >::type corpus(corpusSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type n_docs(n_docsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::DataFrame& >::type alphabet(alphabetSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type n_topics(n_topicsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::DataFrame& >::type topic_counts(topic_countsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::DataFrame& >::type type_topic_counts(type_topic_countsSEXP);
    Rcpp::traits::input_parameter< const Rcpp::NumericVector& >::type alpha(alphaSEXP);
    Rcpp::traits::input_parameter< double >::type beta(betaSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type n_particles(n_particlesSEXP);
    Rcpp::traits::input_parameter< bool >::type resampling(resamplingSEXP);
    rcpp_result_gen = Rcpp::wrap(evaluate_left_to_right_cpp(corpus, n_docs, alphabet, n_topics, topic_counts, type_topic_counts, alpha, beta, n_particles, resampling));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_tomer_evaluate_left_to_right_cpp", (DL_FUNC) &_tomer_evaluate_left_to_right_cpp, 10},
    {NULL, NULL, 0}
};

RcppExport void R_init_tomer(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
