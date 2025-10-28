#ifndef DIFFINK_TREEDIFF_GUMTREE_GREEDYBOTTOMUP_H
#define DIFFINK_TREEDIFF_GUMTREE_GREEDYBOTTOMUP_H

#include "DiffInk/TreeDiff/GumTree/HyperParameters.h"
#include "DiffInk/TreeDiff/Metric/DiceSimilarity.h"
#include "DiffInk/TreeDiff/Recovery/OptimalRecovery.h"

namespace diffink::gumtree {

class GreedyBottomUp : public TreeDiff::Matcher {
private:
  const double MinDice;
  recovery::OptimalRecovery Recovery;

private:
  void match(TreeDiff &Diff, VirtualNode *Node);

public:
  GreedyBottomUp(uint32_t MaxSize = DefaultMaxSize,
                 double MinDice = DefaultMinDice) noexcept
      : MinDice{MinDice}, Recovery(MaxSize) {}

  ~GreedyBottomUp() final = default;

  void match(TreeDiff &Diff) final;
};

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_GUMTREE_GREEDYBOTTOMUP_H