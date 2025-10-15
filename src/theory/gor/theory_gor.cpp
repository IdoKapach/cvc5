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

bool GorMat::isHasCycle() {
  // Initiate reachable mat such (after finishing build it) reachable[i][j] == true iff there's a path from i to j.
  // In order to build it, the method uses Floyd-Warshall algorithm.
  std::vector<std::vector<bool>> reachable = d_matrix;  
  size_t numVars = d_numExps;

  // Floyd-Warshall: closure over paths
  for (size_t k = 0; k < numVars; ++k)
  {
    for (size_t i = 0; i < numVars; ++i)
    {
      for (size_t j = 0; j < numVars; ++j)
      {
        if (!reachable[i][j])
        {
          reachable[i][j] = reachable[i][k] && reachable[k][j];
        }
      }
    }
  }

  // Check if there's a cycle in the graph by checking if there's an node that has a path to itself
  for (size_t i = 0; i < numVars; ++i) {
    if (reachable[i][i]) {
      return true;
    }
  }

  // Return false in case there's no node with path to itself.
  return false; 
}

void GorMat::computeReachableMatrix() {
  // Intialize d_reachableMatrix as a copy of the d_matrix with true values on the slant
  //  (which represent self loops in the represented graph)
  std::cout << "!!!!!!the reachable computed!!!!!\n";
  d_reachableMatrix = d_matrix;
  for (size_t i = 0; i < d_numExps; ++i) {
      d_reachableMatrix[i][i] = true;
  }

  // compute d_reachableMatrix ^ d_numVars when the exit i,j = true iff there's a path from i to j of length of at most d_numVars
  //  in the graph of c_matrix.
  size_t power = 1;
  while (power < d_numExps) {
    d_reachableMatrix = selfBoolMatProduct(d_reachableMatrix);
    power *= 2;
  }
}

std::optional<std::pair<TNode, TNode>> GorMat::containForbiddenPath() {
  if (d_reachableMatrix.empty()) {
      computeReachableMatrix();
  }

  // check for each pair in d_forbiddenPaths if there's a path from the first arg to the second one. If so, return that pair.
  for (std::pair<TNode, TNode>& pair : d_forbiddenPaths) {
    size_t first = d_gorExpMap[pair.first], second = d_gorExpMap[pair.second];
    if (d_reachableMatrix[first][second]) {
      return pair;
    }
  }

  return std::nullopt;
}

std::vector<std::vector<bool>> GorMat::selfBoolMatProduct(std::vector<std::vector<bool>>& mat) {
  // Initialize nMat which will represent mat * mat.
  std::vector<std::vector<bool>> nMat;
  for (size_t i = 0; i < d_numExps; ++i)
  {
    nMat.emplace_back(d_numExps, false);
  }

  for (size_t i = 0; i < d_numExps; ++i) { 
    for (size_t j = 0; j < d_numExps; ++j) {
      bool bOr = false;
      for (size_t k = 0; k < d_numExps; ++k) {
        bOr = bOr || (mat[i][k] && mat[k][j]);
      }
      nMat[i][j] = bOr;
    }
  }

  return nMat;
}







TheoryGenericOrderRelation::TheoryGenericOrderRelation(Env& env,
    OutputChannel& out,
    Valuation valuation)
: Theory(THEORY_GOR, env, out, valuation),
d_rewriter(nodeManager()),
d_state(env, valuation),
d_im(env, *this, d_state, getStatsPrefix(THEORY_GOR)),
d_eqNotify(d_im)
{
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
  for (auto& [type, gorMat] : d_matMap) {

    // --- PRINTS ---
    std::cout << "Type:" << type << std::endl;
    for (const auto& [var, index] : gorMat.d_gorExpMap){
        std::cout << "  " << var << " => " << index << "\n";
    }
    for (const auto& p : gorMat.d_forbiddenPaths) {
      const auto& first = p.first;
      const auto& second = p.second;
      std::cout << "  " << first << " !!! " << second << "\n";
    }

    std::cout << "VARS: " << gorMat.d_vars << std::endl;
    
    // Print the matrix
    std::cout << "Matrix contents (" << gorMat.d_matrix.size() << "x" << (gorMat.d_matrix.empty() ? 0 : gorMat.d_matrix[0].size()) << "):\n";
    for (size_t i = 0; i < gorMat.d_matrix.size(); ++i) {
      for (size_t j = 0; j < gorMat.d_matrix[i].size(); ++j) {
        std::cout << (gorMat.d_matrix[i][j] ? "1" : "0") << " ";
      }
      std::cout << "\n";
    }
    // --- PRINTS ---

    // check if there's a cycle in the graph which causes a conflict
    if (gorMat.isHasCycle()) {
      const Node conflict = nodeManager()->mkConst(true);
      d_im.conflict(conflict, InferenceId::GOR_LEMMA);
    }
    // check if the graph contains a forbidden path
    std::optional<std::pair<TNode, TNode>> pair = gorMat.containForbiddenPath();
    if (pair) {
      std::cout << "CONFLICT: " << pair ->first << " -> " << pair ->second << "\n";
      const Node conflict = nodeManager()->mkConst(true);
      d_im.conflict(conflict, InferenceId::GOR_LEMMA);
    } 

    enforceDisequalities(gorMat);
  }

}

void TheoryGenericOrderRelation::enforceDisequalities(GorMat& gorMat) {
  // enforce for each (i,j) with d_reachableMatrix[i][j] == true, exp_i != exp_j as a lemma during solving
  for (const auto& [exp_i, i] : gorMat.d_gorExpMap) {
    for (const auto& [exp_j, j] : gorMat.d_gorExpMap) {
      if (gorMat.d_reachableMatrix[i][j] && i != j)
      {
        // Enforce x != y as a lemma during solving
        Node diseq = nodeManager()->mkNode(Kind::NOT, 
                          nodeManager()->mkNode(Kind::EQUAL, exp_i, exp_j));
        d_im.lemma(diseq, InferenceId::GOR_LEMMA);
      }
    }
  }
}


void TheoryGenericOrderRelation::notifyFact(TNode atom,
                                            bool pol,
                                            TNode fact,
                                            bool isInternal)
{
  std::cout << "notifyFact: " << atom << "\n";

  if (atom.getKind() == Kind::GENERIC_SMALLER_THAN && pol) {
    TNode var0 = atom[0];
    TNode var1 = atom[1];
    TypeNode k = var0.getType();
    d_matMap[k].d_matrix[d_matMap[k].d_gorExpMap[var0]][d_matMap[k].d_gorExpMap[var1]] = true;
  }
  
  else if (atom.getKind() == Kind::GENERIC_SMALLER_THAN && !pol) {
    TypeNode t = atom[0].getType();
    TNode arg0 = atom[0];
    TNode arg1 = atom[1];
    // There's no need to add a pair in case the 2 args equal since the graph will contain a cycle in that case.
    if (arg0 == arg1) {
      return;
    }
    // If both args are nodes in the graph, append them to d_forbiddenPaths
    if (d_matMap[t].d_gorExpMap.find(arg0) != d_matMap[t].d_gorExpMap.end() &&
     d_matMap[t].d_gorExpMap.find(arg1) != d_matMap[t].d_gorExpMap.end()) {
      d_matMap[t].d_forbiddenPaths.emplace_back(std::make_pair(arg0, arg1));
    }
  }

}

bool TheoryGenericOrderRelation::collectModelValues(
    TheoryModel* m, const std::set<Node>& termSet)
{
  std::cout << "collectModelValues\n";

  // // For each (i,j) with d_reachableMatrix[i][j] == true, enforce exp_i != exp_j
  // for (const auto& [type, gorMat] : d_matMap)
  // {
  //   if (gorMat.d_reachableMatrix.empty()) {
  //     tM = m;
  //     return true;
  //   }
  //   // Print the matrix
  //   std::cout << "d_reachableMatrix contents (" << gorMat.d_matrix.size() << "x" << (gorMat.d_matrix.empty() ? 0 : gorMat.d_matrix[0].size()) << "):\n";
  //   for (size_t i = 0; i < gorMat.d_matrix.size(); ++i) {
  //     for (size_t j = 0; j < gorMat.d_matrix[i].size(); ++j) {
  //       std::cout << (gorMat.d_reachableMatrix[i][j] ? "1" : "0") << " ";
  //     }
  //     std::cout << "\n";
  //   }
    
  //   for (const auto& [exp_i, i] : gorMat.d_gorExpMap) {
  //     for (const auto& [exp_j, j] : gorMat.d_gorExpMap) {
  //       if (gorMat.d_reachableMatrix[i][j] && i != j) {
  //         std::cout << "  Enforce: " << exp_i << " != " << exp_j << "\n";
  //         if (!m->assertEquality(exp_i, exp_j, true))
  //         {
  //           std::cout << " CAN'T Enforce: " << exp_i << " != " << exp_j << "\n";
  //           return false; // model conflict
  //         }
  //       }
  //     }
  //   }
    
  // }
  
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
  std::cout << "preRegister: " << node << " : " << node.getKind() << "\n";
  // Insert both arguments of the gor operator to d_matMap field of the correspond type.
  if (node.getKind() == Kind::GENERIC_SMALLER_THAN) {
    for (size_t i=0; i<2; i++) {
      TypeNode nK = node[i].getType();
      if (d_matMap.find(nK) == d_matMap.end()) {
        d_matMap[nK] = GorMat();
      }
      if (d_matMap[nK].d_gorExpMap.find(node[i]) == d_matMap[nK].d_gorExpMap.end()) {
        std::cout << "preRegister found: " << node[i] << " : " << node[i].getKind() << "\n";
        d_matMap[nK].d_gorExpMap[node[i]] = d_matMap[nK].d_numExps;
        d_matMap[nK].d_numExps++;
      }
    }
  }
  // Insert to d_vars of the correspond type only the consts and vars that don't have a value.
  else {
    if (!(node.isConst() && node.getMetaKind() == kind::metakind::CONSTANT)) {  
      TypeNode nK = node.getType();
      if (d_matMap.find(nK) == d_matMap.end()) {
        d_matMap[nK] = GorMat();
      }
      d_matMap[nK].d_vars.insert(node);
    }
  }

}

TrustNode TheoryGenericOrderRelation::ppRewrite(TNode n,
                                                std::vector<SkolemLemma>& lems)
{
  // std::cout << "ppRewrite " << n << std::endl;
  return TrustNode::null();
}

// PPAssertStatus TheoryGenericOrderRelation::ppAssert(
//     TrustNode tin, TrustSubstitutionMap& outSubstitutions)
// {
//   return PPAssertStatus();
// }

void TheoryGenericOrderRelation::presolve() {
  std::cout << "preSolve\n";

  for (auto& [type, gorMat] : d_matMap){
    // Initialize adjacency matrix for each kind.
    for (size_t i = 0; i < gorMat.d_numExps; ++i)
    {
      gorMat.d_matrix.emplace_back(gorMat.d_numExps, false);
    }
  }
}

bool TheoryGenericOrderRelation::isEntailed(Node n, bool pol) {
  std::cout << "isEntailed\n";
  return false; 
}


bool TheoryGenericOrderRelation::needsCheckLastEffort() {
  if (d_lastEffortCalled) {
    return false;
  }
  d_lastEffortCalled = true;
  presolve();
  return true;
}

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal