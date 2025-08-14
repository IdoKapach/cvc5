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