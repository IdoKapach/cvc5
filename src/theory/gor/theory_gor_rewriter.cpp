#include "theory/gor/theory_gor_rewriter.h"

namespace cvc5::internal {
namespace theory {
namespace gor {

TheoryGenericOrderRelationRewriter::TheoryGenericOrderRelationRewriter(NodeManager* nm)
    : TheoryRewriter(nm)
{
}

RewriteResponse TheoryGenericOrderRelationRewriter::postRewrite(TNode node)
{
  return RewriteResponse(REWRITE_DONE, original_term);
}

RewriteResponse TheoryGenericOrderRelationRewriter::preRewrite(TNode node)
{
  return RewriteResponse(REWRITE_DONE, original_term);
}

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal
