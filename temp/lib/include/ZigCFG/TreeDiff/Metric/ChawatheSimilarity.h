#ifndef DIFFINK_TREEDIFF_METRIC_CHAWATHESIMILARITY_H
#define DIFFINK_TREEDIFF_METRIC_CHAWATHESIMILARITY_H

#include "DiffInk/TreeDiff/TreeDiff.h"
#include <cmath>

namespace diffink::metric {

constexpr double computeChawatheSimilarity(std::size_t Commons,
                                           std::size_t Size1,
                                           std::size_t Size2) {
  return static_cast<double>(Commons) / std::max(Size1, Size2);
}

double computeChawatheSimilarity(TreeDiff &Mapping,
                                 const std::unordered_set<VirtualNode *> &Old,
                                 VirtualNode *New);

double computeAutoChawatheThreshold(VirtualNode *Old,
                                    VirtualNode *New) noexcept;

} // namespace diffink::metric

#endif // DIFFINK_TREEDIFF_METRIC_CHAWATHESIMILARITY_H