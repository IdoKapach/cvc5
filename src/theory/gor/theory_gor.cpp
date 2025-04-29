#include "theory/gor/theory_gor.h"
#include "theory_gor.h"

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
d_stats(
std::make_unique<GorStatistics>(statisticsRegistry(), "theory::gor::"))
{
d_theoryState = &d_state;
d_inferManager = &d_im;
}

TheoryGenericOrderRelation::~TheoryGenericOrderRelation() {}

TheoryRewriter* TheoryGenericOrderRelation::getTheoryRewriter()
{
  if (!options().gor.gor)
  {
    return nullptr;
  }
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

void TheoryGenericOrderRelation::postCheck(Effort level) {}

void TheoryGenericOrderRelation::notifyFact(TNode atom,
                                            bool pol,
                                            TNode fact,
                                            bool isInternal)
{}

bool TheoryGenericOrderRelation::collectModelValues(
    TheoryModel* m, const std::set<Node>& termSet)
{
  return false;
}

void TheoryGenericOrderRelation::computeCareGraph() {}

TrustNode TheoryGenericOrderRelation::explain(TNode) {
    return TrustNode();
}

Node TheoryGenericOrderRelation::getModelValue(TNode) { 
    return Node();
}

void TheoryGenericOrderRelation::preRegisterTerm(TNode node) {}

TrustNode TheoryGenericOrderRelation::ppRewrite(TNode n,
                                                std::vector<SkolemLemma>& lems)
{
  return TrustNode();
}

PPAssertStatus TheoryGenericOrderRelation::ppAssert(
    TrustNode tin, TrustSubstitutionMap& outSubstitutions)
{
  return PPAssertStatus();
}

void TheoryGenericOrderRelation::presolve() {}

bool TheoryGenericOrderRelation::isEntailed(Node n, bool pol) {
    return false; 
}

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal