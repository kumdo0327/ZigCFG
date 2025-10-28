#ifndef DIFFINK_TREEDIFF_GUMTREE_SIMPLEBOTTOMUP_H
#define DIFFINK_TREEDIFF_GUMTREE_SIMPLEBOTTOMUP_H

#include "DiffInk/TreeDiff/GumTree/HyperParameters.h"
#include "DiffInk/TreeDiff/Metric/ChawatheSimilarity.h"
#include "DiffInk/TreeDiff/Recovery/SimpleRecovery.h"

namespace diffink::gumtree {

class SimpleBottomUp : public TreeDiff::Matcher {
private:
  recovery::SimpleRecovery Recovery;
  const std::optional<double> Threshold;

private:
  void match(TreeDiff &Diff, VirtualNode *Node);

public:
  SimpleBottomUp(std::optional<double> Threshold = std::nullopt) noexcept
      : Threshold{Threshold} {}

  ~SimpleBottomUp() final = default;

  void match(TreeDiff &Diff) final;
};

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_GUMTREE_SIMPLEBOTTOMUP_H