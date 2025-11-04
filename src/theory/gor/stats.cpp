#include "theory/gor/stats.h"

#include <iostream>

#include "base/output.h"
#include "util/statistics_registry.h"

namespace cvc5::internal {
namespace theory {
namespace gor {

GorStatistics::GorStatistics(StatisticsRegistry& registry,
                           const std::string& prefix)
    : d_numReductions(registry.registerInt(prefix + "num_reductions")),
      d_reductionTime(registry.registerTimer(prefix + "reduction_time")),
      d_modelConstructionTime(
          registry.registerTimer(prefix + "model_construction_time")),
      d_numConstructionErrors(
          registry.registerInt(prefix + "num_construction_errors"))
{
  Trace("gor::stats") << "gor registered 4 stats" << std::endl;
}

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal