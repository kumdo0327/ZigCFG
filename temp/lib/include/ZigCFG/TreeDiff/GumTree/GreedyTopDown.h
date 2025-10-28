#ifndef DIFFINK_TREEDIFF_GUMTREE_GREEDYTOPDOWN_H
#define DIFFINK_TREEDIFF_GUMTREE_GREEDYTOPDOWN_H

#include "DiffInk/TreeDiff/GumTree/Comparator.h"
#include "DiffInk/TreeDiff/GumTree/HyperParameters.h"
#include "DiffInk/TreeDiff/TreeDiff.h"
#include <map>

namespace diffink::gumtree {

class GreedyTopDown {
private:
  const uint32_t MinHeight;

public:
  GreedyTopDown(uint32_t MinHeight = gumtree::DefaultMinHeight,
                float MinDice = gumtree::DefaultMinDice) noexcept
      : MinHeight{MinHeight} {}

  void match(TreeDiff &Diff, const std::unordered_set<VirtualNode *> &Old,
             const std::unordered_set<VirtualNode *> &New);
};

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_GUMTREE_GREEDYTOPDOWN_H