#include "theory/gor/theory_gor.h"
#include "theory_gor.h"

#include "expr/node_traversal.h"
#include "theory/theory_model.h"
#include "theory/trust_substitutions.h"
#include "util/result.h"
#include "util/statistics_registry.h"
#include "util/utility.h"

namespace cvc5::internal {
namespace theory {
namespace gor {

TheoryGenericOrderRelation::TheoryGenericOrderRelation(Env& env,
    OutputChannel& out,
    Valuation valuation)
: Theory(THEORY_GOR, env, out, valuation),
d_rewriter(nodeManager()),
d_state(env, valuation),
d_im(env, *this, d_state, getStatsPrefix(THEORY_GOR)),
d_eqNotify(d_im),
d_numVars(0)
{
  d_theoryState = &d_state;
  d_inferManager = &d_im;
}

TheoryGenericOrderRelation::~TheoryGenericOrderRelation() {
  // std::cout << "destroyyyyy\n";
}

TheoryRewriter* TheoryGenericOrderRelation::getTheoryRewriter()
{
  // std::cout << "getRewriter\n";
  return &d_rewriter;
}

ProofRuleChecker* TheoryGenericOrderRelation::getProofChecker()
{
  // std::cout << "proofChecker\n";
  return nullptr;
}

bool TheoryGenericOrderRelation::needsEqualityEngine(EeSetupInfo& esi)
{
  // std::cout << "equalityyyy\n";
  esi.d_notify = &d_eqNotify;
  esi.d_name = "theory::gor::ee";
  return true;
}

void TheoryGenericOrderRelation::finishInit()
{
  // std::cout << "finishInit\n";
  Assert(d_equalityEngine != nullptr);

  d_equalityEngine->addFunctionKind(Kind::GENERIC_SMALLER_THAN);
}

void TheoryGenericOrderRelation::postCheck(Effort level) {
  for (const auto& [var, index] : d_varMap){
      std::cout << "  " << var << " => " << index << "\n";
  }
  
  // Print the matrix
  std::cout << "Matrix contents (" << d_matrix.size() << "x" << (d_matrix.empty() ? 0 : d_matrix[0].size()) << "):\n";
  for (size_t i = 0; i < d_matrix.size(); ++i) {
    for (size_t j = 0; j < d_matrix[i].size(); ++j) {
      std::cout << (d_matrix[i][j] ? "1" : "0") << " ";
    }
    std::cout << "\n";
  }

  if (isHasCircle()) {
    const Node conflict = nodeManager()->mkConst(true);
    d_im.conflict(conflict, InferenceId::FF_LEMMA);
  }
}

bool TheoryGenericOrderRelation::isHasCircle() {
  return true;
}

void TheoryGenericOrderRelation::notifyFact(TNode atom,
                                            bool pol,
                                            TNode fact,
                                            bool isInternal)
{
  // std::cout << "notifyyyy\n";
  if (atom.getKind() == Kind::GENERIC_SMALLER_THAN && !d_matrix.empty()) {
    // std::cout << "notifyyyy  smaller than\n";
    TNode var0 = atom[0];
    TNode var1 = atom[1];
    size_t src = d_varMap[var0];
    size_t dst = d_varMap[var1];
    d_matrix[src][dst] = true;
  }
  // std::cout << "notifyyyy  finish\n";
}

bool TheoryGenericOrderRelation::collectModelValues(
    TheoryModel* m, const std::set<Node>& termSet)
{
  // std::cout << "collectModelValues\n";
  return true;
}

void TheoryGenericOrderRelation::computeCareGraph() {
  std::cout << "computeGraph\n";
}

TrustNode TheoryGenericOrderRelation::explain(TNode) {
  std::cout << "explain\n";
  return TrustNode();
}

// Node TheoryGenericOrderRelation::getModelValue(TNode) { 
//     return Node();
// }

void TheoryGenericOrderRelation::preRegisterTerm(TNode node) {
  if (node.isVar()) {
    if (d_varMap.find(node) == d_varMap.end()) {
      // std::cout << node << " was added\n";
      d_varMap[node] = d_numVars;
      // std::cout << d_varMap[node] << " new inx\n";
      d_numVars++;
    }
    // d_im.assertInternalFact(node, true, InferenceId::UNKNOWN, node);
  }
  // std::cout << node << "\n";
}

TrustNode TheoryGenericOrderRelation::ppRewrite(TNode n,
                                                std::vector<SkolemLemma>& lems)
{
  // std::cout << "ppRewrite\n";
  return TrustNode();
}

// PPAssertStatus TheoryGenericOrderRelation::ppAssert(
//     TrustNode tin, TrustSubstitutionMap& outSubstitutions)
// {
//   return PPAssertStatus();
// }

void TheoryGenericOrderRelation::presolve() {
  std::cout << "preeeeeeeeeeeeeeeeee\n";
  // Initialize adjacency matrix.
  for (size_t i = 0; i < d_numVars; ++i)
  {
    d_matrix.emplace_back(d_numVars, false);
  }
}

bool TheoryGenericOrderRelation::isEntailed(Node n, bool pol) {
  std::cout << "isEntailed\n";
  return false; 
}


bool TheoryGenericOrderRelation::needsCheckLastEffort() {
    // std::cout << "=== needsCheckLastEffort called" << std::endl;

  // Initialize the adjacency matrix.
  for (size_t i = 0; i < d_numVars; ++i)
  {
    d_matrix.emplace_back(d_numVars, false);
  }

  // Force the theory to participate in solving
  return true;
}

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal