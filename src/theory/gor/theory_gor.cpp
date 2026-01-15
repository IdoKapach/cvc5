#include "theory/gor/theory_gor.h"
#include "theory_gor.h"

#include "expr/node_traversal.h"
#include "theory/theory_model.h"
#include "theory/trust_substitutions.h"
#include "util/result.h"
#include "util/statistics_registry.h"
#include "util/utility.h"

#include "expr/skolem_manager.h"
#include "util/string.h"

#include "proof/trust_node.h"
#include "proof/proof_generator.h"
#include "expr/node.h"
#include "expr/node_manager.h"


namespace cvc5::internal {
namespace theory {
namespace gor {

std::pair<bool, std::vector<TNode>> GorMat::isHasCycle() {
  // Initiate reachable mat such (after finishing build it) reachable[i][j].first == true iff there's a path from i to j.
  // and reachable[i][j].second contains one such path as a vector of TNodes.
  // In order to build it, the method uses Floyd-Warshall algorithm.
  std::vector<std::vector<std::pair<bool, std::vector<TNode>>>> reachable;  
  size_t numVars = d_numExps;

  // Initialize reachable as a copy of d_matrix in the first component of the pair, and empty vector in the second component.
  for (size_t i = 0; i < numVars; ++i)
  {
    reachable.emplace_back();
    for (size_t j = 0; j < numVars; ++j)
    {
      if (i == j) {
        reachable[i].emplace_back(false, std::vector<TNode>{});
        continue;
      }
      if (d_matrix[i][j]) {
        TNode exp_i = std::find_if(d_gorExpMap.begin(), d_gorExpMap.end(), 
        [i](const std::pair<TNode, size_t>& pair) {
            return pair.second == i;
        })->first;
        TNode exp_j = std::find_if(d_gorExpMap.begin(), d_gorExpMap.end(), 
        [j](const std::pair<TNode, size_t>& pair) {
            return pair.second == j;
        })->first;
        reachable[i].emplace_back(d_matrix[i][j], std::vector<TNode>{exp_i, exp_j});
      }
      else {
        reachable[i].emplace_back(d_matrix[i][j], std::vector<TNode>{});
      }
    }
  }

  // Floyd-Warshall: closure over paths
  for (size_t k = 0; k < numVars; ++k)
  {
    for (size_t i = 0; i < numVars; ++i)
    {
      for (size_t j = 0; j < numVars; ++j)
      {
        if (!reachable[i][j].first)
        {
          reachable[i][j].first = reachable[i][k].first && reachable[k][j].first;
          if (reachable[i][j].first) {
            // Update the path info if a path from i to j is found
            reachable[i][j].second = reachable[i][k].second;
            reachable[i][j].second.insert(reachable[i][j].second.end(),
                                          reachable[k][j].second.begin(),
                                          reachable[k][j].second.end());
          }
        }
      }
    }
  }

  // Check if there's a cycle in the graph by checking if there's an node that has a path to itself
  for (size_t i = 0; i < numVars; ++i) {
    if (reachable[i][i].first) {
      return reachable[i][i];
    }
  }

  // Return false in case there's no node with path to itself.
  return std::make_pair(false, std::vector<TNode>{});
}

void GorMat::computeReachableMatrix() {
  // Intialize d_reachableMatrix as a copy of the d_matrix with true values on the slant
  //  (which represent self loops in the represented graph)
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
  return &d_rewriter;
}

ProofRuleChecker* TheoryGenericOrderRelation::getProofChecker()
{
  return nullptr;
}

bool TheoryGenericOrderRelation::needsEqualityEngine(EeSetupInfo& esi)
{
  esi.d_notify = &d_eqNotify;
  esi.d_name = "theory::gor::ee";
  return true;
}

void TheoryGenericOrderRelation::finishInit()
{
  Assert(d_equalityEngine != nullptr);

  d_equalityEngine->addFunctionKind(Kind::GENERIC_SMALLER_THAN);
}

void TheoryGenericOrderRelation::postCheck(Effort level) {
  for (auto& [type, gorMat] : d_matMap) {

    // --- PRINTS ---
    Trace("gor::solver") << "Type: " << type << std::endl;
    for (const auto& [var, index] : gorMat.d_gorExpMap){
        Trace("gor::solver") << "  " << var << " => " << index << "\n";
    }
    for (const auto& p : gorMat.d_forbiddenPaths) {
      const auto& first = p.first;
      const auto& second = p.second;
      Trace("gor::solver") << "  " << first << " !!! " << second << "\n";
    }

    Trace("gor::solver") << "VARS: " << gorMat.d_vars << std::endl;
    
    // Print the matrix
    Trace("gor::solver") << "Matrix contents (" << gorMat.d_matrix.size() << "x" << (gorMat.d_matrix.empty() ? 0 : gorMat.d_matrix[0].size()) << "):\n";
    for (size_t i = 0; i < gorMat.d_matrix.size(); ++i) {
      for (size_t j = 0; j < gorMat.d_matrix[i].size(); ++j) {
        Trace("gor::solver") << (gorMat.d_matrix[i][j] ? "1" : "0") << " ";
      }
      Trace("gor::solver") << "\n";
    }
    // --- PRINTS ---

    // check if there's a cycle in the graph which causes a conflict
    std::pair<bool, std::vector<TNode>> cycleResult = gorMat.isHasCycle();
    if (cycleResult.first) {
      Trace("gor::solver") << "CONFLICT: cycle detected\n";
      // send the cycle path as a conflict (and between the cycle edges)
      Node conflict;
      if (cycleResult.second.size() == 1) {        
        conflict = cycleResult.second[0];      
      } 
      else {
        NodeManager* nm = nodeManager();
        Node newLit = nm->mkNode(Kind::GENERIC_SMALLER_THAN, cycleResult.second[0], cycleResult.second[1]);
        for (size_t i = 2; i < cycleResult.second.size(); ++i) {
          Node s = cycleResult.second[i++];
          Node b = cycleResult.second[i];
          newLit = nm->mkNode(Kind::AND, newLit, nm->mkNode(Kind::GENERIC_SMALLER_THAN, s, b));
        }
        conflict = newLit;
        Trace("gor::solver") << "Conflict cycle literals: " << conflict << std::endl;
      }
      d_im.conflict(conflict, InferenceId::GOR_LEMMA);
      return;
    }
    // check if the graph contains a forbidden path
    std::optional<std::pair<TNode, TNode>> pair = gorMat.containForbiddenPath();
    if (pair) {
      Trace("gor::solver") << "CONFLICT: " << pair ->first << " -> " << pair ->second << "\n";
      std::vector<TNode> pathLiterals = computeForbiddenPath(gorMat, &(*pair));
      Node conflict = nodeManager()->mkNode(Kind::AND, pathLiterals);
      d_im.conflict(conflict, InferenceId::GOR_LEMMA);
      return;
    }

    enforceDisequalities(gorMat, type);
  }

}

// Single Source Shortest Path from source to all other nodes in gorMat, returns the father nodes vector
std::vector<TNode> SSSP(GorMat& gorMat, TNode source) {
  std::vector<TNode> fatherNodes(gorMat.d_numExps, TNode::null());
  std::queue<TNode> toVisit;
  toVisit.push(source);

  while (!toVisit.empty()) {
    TNode current = toVisit.front();
    size_t current_idx = gorMat.d_gorExpMap[current];
    toVisit.pop();
    for (const auto& [node, idx] : gorMat.d_gorExpMap) {
      if (gorMat.d_matrix[current_idx][idx] && fatherNodes[idx] == TNode::null()) {
        fatherNodes[idx] = current;
        toVisit.push(node);
      }
    }
  }
  return fatherNodes;
}

std::vector<TNode> TheoryGenericOrderRelation::computeForbiddenPath(GorMat& gorMat, std::pair<TNode, TNode>* pair) {      
      // Build conflict: the forbidden edge + the path that exists
      TNode forbiddenEdge = nodeManager()->mkNode(Kind::GENERIC_SMALLER_THAN, pair->first, pair->second);
      TNode notForbiddenEdge = nodeManager()->mkNode(Kind::NOT, forbiddenEdge);
      
      // Collect edges forming the path from pair->first to pair->second
      std::vector<TNode> pathLiterals;
      pathLiterals.push_back(notForbiddenEdge);
      
      // Add edges that form the forbidden path
      TNode source = pair->first;
      TNode destination = pair->second;
      std::vector<TNode> fatherNodes = SSSP(gorMat, source);

      // Reconstruct path
      TNode pathNode = destination;
      std::vector<TNode> reversePath;
      while (pathNode != source) {
        reversePath.push_back(pathNode);
        pathNode = fatherNodes[gorMat.d_gorExpMap[pathNode]];
      }
      reversePath.push_back(source);
      std::reverse(reversePath.begin(), reversePath.end());
      // Build path literals
      for (size_t i = 0; i < reversePath.size() - 1; ++i) {
        Node lit = nodeManager()->mkNode(Kind::GENERIC_SMALLER_THAN, 
                                          reversePath[i], reversePath[i+1]);
        pathLiterals.push_back(lit);
      }
      return pathLiterals;
}

void TheoryGenericOrderRelation::enforceDisequalities(GorMat& gorMat, TypeNode type) {
  std::vector<Node> pairs;

  // enforce for each (i,j) with d_reachableMatrix[i][j] == true, exp_i != exp_j as a lemma during solving
  for (const auto& [exp_i, i] : gorMat.d_gorExpMap) {
    std::vector<TNode> fatherNodes = SSSP(gorMat, exp_i);
    for (const auto& [exp_j, j] : gorMat.d_gorExpMap) {
      if (gorMat.d_reachableMatrix[i][j] && i != j)
      {
        // Enforce x != y as a lemma during solving
        Trace("gor::solver") << "ENFORCE DISEQUALITY: " << exp_i << " != " << exp_j << "\n";
        Node diseq = nodeManager()->mkNode(Kind::NOT, 
                          nodeManager()->mkNode(Kind::EQUAL, exp_i, exp_j));

        
        std::vector<TNode> pathLiterals;
        if (!gorMat.d_matrix[i][j]) {
          // construct the path from exp_i to exp_j which will be used as the reason for the lemma
          // Reconstruct path
          TNode pathNode = exp_j;
          std::vector<TNode> reversePath;
          while (pathNode != exp_i) {
            reversePath.push_back(pathNode);
            pathNode = fatherNodes[gorMat.d_gorExpMap[pathNode]];
          }
          reversePath.push_back(exp_i);
          std::reverse(reversePath.begin(), reversePath.end());
          // Build path literals
          for (size_t k = 0; k < reversePath.size() - 1; ++k) {
            Node lit = nodeManager()->mkNode(Kind::GENERIC_SMALLER_THAN, 
                                              reversePath[k], reversePath[k+1]);
            pathLiterals.push_back(lit);
          }
        }

        // submit the lemma
        Node reason;
        if (gorMat.d_matrix[i][j]) {
          // direct edge exists, define reason as this edge
          reason = nodeManager()->mkNode(Kind::GENERIC_SMALLER_THAN, exp_i, exp_j);
        }
        else {
          // direct edge doesn't exist, define reason as the path from exp_i to exp_j
          reason = nodeManager()->mkNode(Kind::AND, pathLiterals);
        }
        Node lemma = nodeManager()->mkNode(Kind::IMPLIES, reason, diseq);
        d_im.lemma(lemma, InferenceId::GOR_LEMMA);

        // Store the pair (exp_i, exp_j) in d_gorPairs field
        Node pairNode = nodeManager()->mkNode(Kind::SEXPR,
                                            exp_i, exp_j);
                                            
        pairs.push_back(pairNode);
      }
    }
  }

  gorMat.d_gorPairs = pairs;
}


void TheoryGenericOrderRelation::notifyFact(TNode atom,
                                            bool pol,
                                            TNode fact,
                                            bool isInternal)
{
  Trace("gor::solver") << "notifyFact: " << atom << "\n";

  if (atom.getKind() == Kind::GENERIC_SMALLER_THAN && pol) {
    TNode var0 = atom[0];
    TNode var1 = atom[1];
    // If the arguments are syntactically equal, the atom must be false (irreflexivity)
    if (var0 == var1) {
      d_im.conflict(atom, InferenceId::GOR_LEMMA);
      return;
    }
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
  // Log the gor pairs for modeling
  for (const auto& [type, gorMat] : d_matMap) {
    if (!gorMat.d_gorPairs.empty()) {
      Trace("gor::model") << "gor pairs for type '" << type << "': " << gorMat.d_gorPairs << "\n";
      Trace("gor::solver") << "gor pairs for type '" << type << "': " << gorMat.d_gorPairs << "\n";
    }
  }
  
  return true;
}

void TheoryGenericOrderRelation::computeCareGraph() {
  Trace("gor::solver") << "computeGraph\n";
}

TrustNode TheoryGenericOrderRelation::explain(TNode lit) {
  Trace("gor::solver") << "explain: " << lit << std::endl;  
  // Default: shouldn't reach here
  Trace("gor::solver") << "Warning: unexpected literal in explain: " << lit << std::endl;
  return TrustNode::mkTrustPropExp(lit, lit, nullptr);
}


void TheoryGenericOrderRelation::preRegisterTerm(TNode node) {
  Trace("gor::solver") << "preRegister: " << node << " : " << node.getKind() << "\n";
  // Insert both arguments of the gor operator to d_matMap field of the correspond type.
  if (node.getKind() == Kind::GENERIC_SMALLER_THAN) {
    for (size_t i=0; i<2; i++) {
      TypeNode nK = node[i].getType();
      if (d_matMap.find(nK) == d_matMap.end()) {
        d_matMap[nK] = GorMat();
      }
      if (d_matMap[nK].d_gorExpMap.find(node[i]) == d_matMap[nK].d_gorExpMap.end()) {
        Trace("gor::solver") << "preRegister found: " << node[i] << " : " << node[i].getKind() << "\n";
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
  return TrustNode::null();
}

void TheoryGenericOrderRelation::presolve() {
  Trace("gor::solver") << "preSolve\n";

  for (auto& [type, gorMat] : d_matMap){
    // Initialize adjacency matrix for each kind.
    for (size_t i = 0; i < gorMat.d_numExps; ++i)
    {
      gorMat.d_matrix.emplace_back(gorMat.d_numExps, false);
    }
  }
}

bool TheoryGenericOrderRelation::isEntailed(Node n, bool pol) {
  Trace("gor::solver") << "isEntailed\n";
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