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
  // std::cout << "constructor\n";
  d_theoryState = &d_state;
  d_inferManager = &d_im;
}

TheoryGenericOrderRelation::~TheoryGenericOrderRelation() {}

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
  // std::cout << "needEquality\n";
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

  if (isHasCycle()) {
    const Node conflict = nodeManager()->mkConst(true);
    d_im.conflict(conflict, InferenceId::GOR_LEMMA);
  }
}

bool TheoryGenericOrderRelation::isHasCycle() {
  // Initiate reachable mat such (after finishing build it) reachable[i][j] == true iff there's a path from i to j.
  // In order to build it, the method uses Floyd-Warshall algorithm.
  std::vector<std::vector<bool>> reachable = d_matrix;  

  // Floyd-Warshall: closure over paths
  for (size_t k = 0; k < d_numVars; ++k)
  {
    for (size_t i = 0; i < d_numVars; ++i)
    {
      for (size_t j = 0; j < d_numVars; ++j)
      {
        if (!reachable[i][j])
        {
          reachable[i][j] = reachable[i][k] && reachable[k][j];
        }
      }
    }
  }

  // Check if there's a cycle in the graph by checking if there's an node that has a path to itself
  for (size_t i = 0; i < d_numVars; ++i) {
    if (reachable[i][i]) {
      return true;
    }
  }

  // Return false in case there's no node with path to itself.
  return false; 
}

void TheoryGenericOrderRelation::notifyFact(TNode atom,
                                            bool pol,
                                            TNode fact,
                                            bool isInternal)
{
  // std::cout << "notifyFact: " << atom << "\n";
  if (atom.getKind() == Kind::GENERIC_SMALLER_THAN && !d_matrix.empty()) {
    TNode var0 = atom[0];
    TNode var1 = atom[1];
    d_matrix[d_varMap[var0]][d_varMap[var1]] = true;
  }
}

bool TheoryGenericOrderRelation::collectModelValues(
    TheoryModel* m, const std::set<Node>& termSet)
{
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
  // std::cout << "preRegister: " << node << " : " << node.getKind() << "\n";
  if (node.isVar()) {
    if (d_varMap.find(node) == d_varMap.end()) {
      d_varMap[node] = d_numVars;
      d_numVars++;
    }
    // return;
  }

  // if (node.getKind() != Kind::GENERIC_SMALLER_THAN)
  // {
  //   std::stringstream ss;
  //   ss << "Unsupported assertion in QF_GOR logic: " << node;
  //   throw LogicException(ss.str());
  // }
}

TrustNode TheoryGenericOrderRelation::ppRewrite(TNode n,
                                                std::vector<SkolemLemma>& lems)
{
  // std::cout << "ppRewrite " << n << std::endl;
  return TrustNode();
}

// PPAssertStatus TheoryGenericOrderRelation::ppAssert(
//     TrustNode tin, TrustSubstitutionMap& outSubstitutions)
// {
//   return PPAssertStatus();
// }

void TheoryGenericOrderRelation::presolve() {
  std::cout << "preSolve\n";
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
  std::cout << "lastEffort\n";
  if (d_matrix.size() == d_numVars) {
    return false;
  }
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