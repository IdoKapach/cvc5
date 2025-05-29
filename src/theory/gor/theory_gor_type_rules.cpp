#include "theory/gor/theory_gor_type_rules.h"
#include "base/check.h"

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
  Assert(n.getNumChildren() == 2);

  TypeNode t1 = n[0].getType(check);
  TypeNode t2 = n[1].getType(check);

  if (check && t1 != t2)
  {
    if (errOut)
    {
      *errOut << "GENERIC_SMALLER_THAN requires both arguments to have the same type. Got: "
              << t1 << " and " << t2;
    }
    throw TypeCheckingExceptionPrivate(n, "Mismatched types in GENERIC_SMALLER_THAN");
  }

  // Result is always Boolean
  return nodeManager->booleanType();
}

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal