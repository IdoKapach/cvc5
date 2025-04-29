#include "theory/gor/theory_gor_type_rules.h"

namespace cvc5::internal {
namespace theory {
namespace gor {

TypeNode GenericSmallerThanTypeRule::preComputeType(NodeManager* nm, TNode n)
{
  return TypeNode::null();
}

TypeNode GenericSmallerThanTypeRule::computeType(NodeManager* nodeManager,
                                                 TNode n,
                                                 bool check,
                                                 std::ostream* errOut)
{
  return TypeNode::null();
}

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal