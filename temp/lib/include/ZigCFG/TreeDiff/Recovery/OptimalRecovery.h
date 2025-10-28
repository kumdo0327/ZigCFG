#ifndef DIFFINK_TREEDIFF_RECOVERY_OPTIMALRECOVERY_H
#define DIFFINK_TREEDIFF_RECOVERY_OPTIMALRECOVERY_H

#include "DiffInk/TreeDiff/TreeDiff.h"
#include <limits>
#include <stack>

namespace diffink::recovery {

class OptimalRecovery : public TreeDiff::Recovery {
private:
  const uint32_t MaxSize;

private:
  int computeUpdateCost(VirtualNode *Left, VirtualNode *Right) const;

  void tryMapping(TreeDiff &Mapping, VirtualNode *Old, VirtualNode *New) const;

public:
  OptimalRecovery(uint32_t MaxSize) noexcept : MaxSize{MaxSize} {}

  ~OptimalRecovery() = default;

  void match(TreeDiff &Mapping, VirtualNode *Old, VirtualNode *New) final;
};

} // namespace diffink::recovery

#endif // DIFFINK_TREEDIFF_RECOVERY_OPTIMALRECOVERY_H