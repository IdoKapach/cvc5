#include "theory/gor/theory_gor_rewriter.h"

namespace cvc5::internal {
namespace theory {
namespace gor {

TheoryGenericOrderRelationRewriter::TheoryGenericOrderRelationRewriter(NodeManager* nm)
    : TheoryRewriter(nm)
{
}

RewriteResponse TheoryGenericOrderRelationRewriter::postRewrite(TNode t)
{
  Trace("ff::rw::post") << "ff::postRewrite: " << t << std::endl;
  return RewriteResponse(REWRITE_DONE, t);
}

RewriteResponse TheoryGenericOrderRelationRewriter::preRewrite(TNode t)
{
  Trace("ff::rw::pre") << "ff::preRewrite: " << t << std::endl;
  return RewriteResponse(REWRITE_DONE, t);
}

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal
