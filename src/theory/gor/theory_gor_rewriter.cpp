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
  Trace("gor::rw::post") << "gor::postRewrite: " << t << std::endl;
  return RewriteResponse(REWRITE_DONE, t);
}

RewriteResponse TheoryGenericOrderRelationRewriter::preRewrite(TNode t)
{
  Trace("gor::rw::pre") << "gor::preRewrite: " << t << std::endl;
  return RewriteResponse(REWRITE_DONE, t);
}

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal
