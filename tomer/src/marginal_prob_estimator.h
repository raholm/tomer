#ifndef TOMER_MARGINAL_PROB_ESTIMATOR_H_
#define TOMER_MARGINAL_PROB_ESTIMATOR_H_

#include "def.h"
#include "left_to_right_state.h"

namespace tomer {

  class MarginalProbEsimator {
  public:
    MarginalProbEsimator() = default;

    virtual ~MarginalProbEsimator() = default;

    virtual void init(const LeftToRightState& state) {}

    virtual void begin(const LeftToRightState& state) {}
    virtual void end(const LeftToRightState& state) {}

    virtual void update_addition(const LeftToRightState& state) {}
    virtual void update_elimination(const LeftToRightState& state) {};

    virtual double get_prob(const LeftToRightState& state) const = 0;

  };

  class LDATokenMarginalProbEstimator : public MarginalProbEsimator {
  public:
    LDATokenMarginalProbEstimator() = default;

    ~LDATokenMarginalProbEstimator() = default;

    double get_prob(const LeftToRightState& state) const override;

  };

  class SparseLDATokenMarginalProbEstimator : public MarginalProbEsimator {
  public:
    SparseLDATokenMarginalProbEstimator();

    ~SparseLDATokenMarginalProbEstimator() = default;

    void init(const LeftToRightState& state) override;

    void begin(const LeftToRightState& state) override;
    void end(const LeftToRightState& state) override;

    void update_addition(const LeftToRightState& state) override;
    void update_elimination(const LeftToRightState& state) override;

    double get_prob(const LeftToRightState& state) const override;

  private:
    bool has_begun_;

    double smoothing_only_mass_;
    DoubleVector cached_coefficients_;

    double topic_beta_mass_;
    mutable double topic_term_mass_;

    IntVector topic_index_;
    size_t dense_index_;
    size_t non_zero_topics_;

    void update(const LeftToRightState& state, bool incr);
    void maintain_dense_index_addition(const LeftToRightState& state);
    void maintain_dense_index_elimination(const LeftToRightState& state);
    void update_topic_scores(const LeftToRightState& state) const;

  };

} // namespace tomer

#endif // TOMER_MARGINAL_PROB_ESTIMATOR_H_
