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

class GorMat {
  public:
  /** Vector of the vars that participate in the gor operations */
  std::set<TNode> d_vars;

  /** Map from gor arguments to the first element of their list */
  std::map<TNode, size_t> d_gorExpMap;

  /** Adjacency matrix. The i,jth entry stores edges from i to j. */
  std::vector<std::vector<bool>> d_matrix;

  /** Number of distinct gor arguments in the graph (in the adjacency matrix) */
  size_t d_numExps;

  /** Vector of TNode pairs that mustn't have a path from the first arg to the second one */
  std::vector<std::pair<TNode, TNode>> d_forbiddenPaths;

  /** Matrix that each exit i,j of it equals true iff there's a path from i to j of length of at most d_numVars */
  std::vector<std::vector<bool>> d_reachableMatrix; 

  // vector that contains all the pairs that the gor relation was applied on
  // Node d_gorPairs = Node::null();
  std::vector<Node> d_gorPairs;

  GorMat() {d_numExps = 0;}

  // Check if the graph contain a cycle, and if yes, return also one cycle path
  std::pair<bool, std::vector<TNode>> isHasCycle();

  // Check if the graph contain forbidden path
  std::optional<std::pair<TNode, TNode>> containForbiddenPath();

  void computeReachableMatrix();

  private:
  std::vector<std::vector<bool>> selfBoolMatProduct(std::vector<std::vector<bool>>& mat);
};

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

  // Map from each typeNode to its GorMat
  std::map<TypeNode, GorMat> d_matMap;

  bool d_lastEffortCalled = false;

  // enforce for each (i,j) with d_reachableMatrix[i][j] == true, exp_i != exp_j as a lemma during solving
  void enforceDisequalities(GorMat& gorMat, TypeNode type);

  std::vector<TNode> computeForbiddenPath(GorMat& gorMat, std::pair<TNode, TNode>* pair);

}; /* class TheoryGenericOrderRelation */

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal

#endif /* CVC5__THEORY__GOR__THEORY_GOR_H */