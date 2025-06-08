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
  // std::cout << "postRewrite: " << t << "\n";
  Trace("gor::rw::post") << "gor::postRewrite: " << t << std::endl;
  return RewriteResponse(REWRITE_DONE, t);
}

RewriteResponse TheoryGenericOrderRelationRewriter::preRewrite(TNode t)
{
  // std::cout << "preRewrite: " << t << "\n";
  Trace("gor::rw::pre") << "gor::preRewrite: " << t << std::endl;
  // if (t.getKind() == Kind::NOT && t[0].getKind() == Kind::NOT)
  // {
  //   std::cout << "notnot: " << t << "\n";
  //   // (not (not x)) → x
  //   return RewriteResponse(REWRITE_AGAIN, t[0][0]);
  // }
  // else if (t.getKind() == Kind::NOT) {
  //   std::cout << "not: " << t << "\n";
  //   TNode nt = d_nm->mkNode(Kind::GENERIC_SMALLER_THAN, t[0][1], t[0][0]);
  //   std::cout << "switch: " << nt << "\n";
  //   return RewriteResponse(REWRITE_DONE, nt);
  // }

  return RewriteResponse(REWRITE_DONE, t);
}

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal
