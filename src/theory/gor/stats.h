#include "cvc5_private.h"

#ifndef CVC5__THEORY__GOR__STATS_H
#define CVC5__THEORY__GOR__STATS_H

#include <string>

#include "util/statistics_stats.h"

namespace cvc5::internal {
namespace theory {
namespace gor {

struct GorStatistics
{
  // Number of groebner-basis reductions
  IntStat d_numReductions;
  // Time spent in groebner-basis reductions
  TimerStat d_reductionTime;
  // Time spent in model construction
  TimerStat d_modelConstructionTime;
  // Number of times that model construction gave an error
  IntStat d_numConstructionErrors;

  GorStatistics(StatisticsRegistry& reg, const std::string& prefix);
};

}  // namespace gor
}  // namespace theory
}  // namespace cvc5::internal

#endif /* CVC5__THEORY__GOR__STATS_H */