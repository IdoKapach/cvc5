#include "cvc5_private.h"

#ifndef CVC5__THEORY__GOR__THEORY_GOR_REWRITER_H
#define CVC5__THEORY__GOR__THEORY_GOR_REWRITER_H

#include <optional>
#include <utility>

#include "expr/node.h"
#include "theory/rewriter.h"

namespace cvc5::internal {
namespace theory {
namespace gor {

class TheoryGenericOrderRelationRewriter : public TheoryRewriter
{
 public:
  TheoryGenericOrderRelationRewriter(NodeManager* nm);

  RewriteResponse postRewrite(TNode node) override;

  RewriteResponse preRewrite(TNode node) override;

}; /* class TheoryGenericOrderRelationRewriter */

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal

#endif /* CVC5__THEORY__GOR__THEORY_GOR_REWRITER_H */