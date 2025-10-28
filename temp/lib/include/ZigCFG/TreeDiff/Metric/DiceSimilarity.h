#ifndef DIFFINK_TREEDIFF_METRIC_DICESIMILARITY_H
#define DIFFINK_TREEDIFF_METRIC_DICESIMILARITY_H

#include "DiffInk/TreeDiff/TreeDiff.h"

namespace diffink::metric {

constexpr double computeDiceSimilarity(std::size_t Commons, std::size_t Size1,
                                       std::size_t Size2) {
  return static_cast<double>(Commons + Commons) / (Size1 + Size2);
}

double computeDiceSimilarity(TreeDiff &Mapping,
                             const std::unordered_set<VirtualNode *> &Old,
                             VirtualNode *New);

} // namespace diffink::metric

#endif // DIFFINK_TREEDIFF_METRIC_DICESIMILARITY_H