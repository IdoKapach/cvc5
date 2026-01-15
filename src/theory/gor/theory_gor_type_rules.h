#include "cvc5_private.h"

#ifndef CVC5__THEORY__GOR__THEORY_GOR_TYPE_RULES_H
#define CVC5__THEORY__GOR__THEORY_GOR_TYPE_RULES_H

#include "expr/node.h"
#include "expr/type_node.h"

namespace cvc5::internal {
namespace theory {
namespace gor {
class GenericSmallerThanTypeRule
{
 public:
  static TypeNode preComputeType(NodeManager* nm, TNode n);
  static TypeNode computeType(NodeManager* nodeManager,
                              TNode n,
                              bool check,
                              std::ostream* errOut);
};
}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal

#endif /* CVC5__THEORY__GOR__THEORY_GOR_TYPE_RULES_H */