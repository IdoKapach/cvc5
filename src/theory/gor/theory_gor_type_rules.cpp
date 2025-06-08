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
  // std::cout << "typeRule: " << n << "\n";
  Assert(n.getNumChildren() == 2);

  TNode arg1 = n[0];
  TNode arg2 = n[1];

  if (check)
  {
    if (arg1.getNumChildren() != 0 || arg2.getNumChildren() != 0)
    {
      if (errOut)
      {
        *errOut << "Arguments to gor.< must be variables (got: "
                << arg1 << ", " << arg2 << ")";
      }
      // std::cout << "arg1 = " << arg1 << " has type " << arg1.getType() << ", kind " << arg1.getKind() << std::endl;
      // std::cout << "arg2 = " << arg2 << " has type " << arg2.getType() << ", kind " << arg2.getKind() << std::endl;
      throw TypeCheckingExceptionPrivate(n, "Arguments to gor.< must be variables or constants.");
    }

    TypeNode t1 = arg1.getType();
    TypeNode t2 = arg2.getType();
    if (t1 != t2)
    {
      if (errOut)
      {
        *errOut << "Arguments to gor.< must have same type.";
      }
      throw TypeCheckingExceptionPrivate(n, "Mismatched types in gor.<");
    }
  }

  return nodeManager->booleanType();
}

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal