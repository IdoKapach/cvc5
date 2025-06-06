#include "cvc5_private.h"

#ifndef CVC5__THEORY__GOR__THEORY_GOR_H
#define CVC5__THEORY__GOR__THEORY_GOR_H

#include <memory>
#include <map>

#include "smt/logic_exception.h"
#include "theory/care_pair_argument_callback.h"
#include "theory/gor/theory_gor_rewriter.h"
#include "theory/theory.h"
#include "theory/theory_eq_notify.h"
#include "theory/theory_inference_manager.h"
#include "theory/theory_state.h"

namespace cvc5::internal {
namespace theory {
namespace gor {

class TheoryGenericOrderRelation : public Theory
{
 public:
  /** Constructs a new instance of TheoryGenericOrderRelation */
  TheoryGenericOrderRelation(Env& env, OutputChannel& out, Valuation valuation);
  ~TheoryGenericOrderRelation() override;

  //--------------------------------- initialization
  /** get the official theory rewriter of this theory */
  TheoryRewriter* getTheoryRewriter() override;
  /** get the proof checker of this theory */
  ProofRuleChecker* getProofChecker() override;
  /**
   * Returns true if we need an equality engine. If so, we initialize the
   * information regarding how it should be setup. For details, see the
   * documentation in Theory::needsEqualityEngine.
   */
  bool needsEqualityEngine(EeSetupInfo& esi) override;
  /** finish initialization */
  void finishInit() override;
  //--------------------------------- end initialization

  //--------------------------------- standard check
  /** Post-check, called after the fact queue of the theory is processed. */
  void postCheck(Effort level) override;
  /** Notify fact */
  void notifyFact(TNode atom, bool pol, TNode fact, bool isInternal) override;
  //--------------------------------- end standard check
  /** Collect model values in m based on the relevant terms given by termSet */
  bool collectModelValues(TheoryModel* m,
                          const std::set<Node>& termSet) override;
  void computeCareGraph() override;
  TrustNode explain(TNode) override;
  // Node getModelValue(TNode) override;
  std::string identify() const override { return "THEORY_GOR"; }
  void preRegisterTerm(TNode node) override;
  TrustNode ppRewrite(TNode n, std::vector<SkolemLemma>& lems) override;
  // PPAssertStatus ppAssert(TrustNode tin,
  //                         TrustSubstitutionMap& outSubstitutions) override;
  void presolve() override;
  bool isEntailed(Node n, bool pol);


  bool needsCheckLastEffort() override;

 private:
 std::vector<Node> d_relations;


  TheoryGenericOrderRelationRewriter d_rewriter;

  /** The state of the gor solver at full effort */
  TheoryState d_state;

  /** The inference manager */
  TheoryInferenceManager d_im;

  /** Manages notifications from our equality engine */
  TheoryEqNotifyClass d_eqNotify;

  /** Map from variables to the first element of their list */
  std::map<TNode, size_t> d_varMap;

  /** i,jth entry stores edges from i to j. */
  std::vector<std::vector<bool>> d_matrix;

  /** Number of variables in the graph */
  size_t d_numVars;

  bool isHasCycle();
}; /* class TheoryGenericOrderRelation */

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal

#endif /* CVC5__THEORY__GOR__THEORY_GOR_H */