#include "DiffInk/TreeDiff/GumTree/Framework.h"

namespace diffink::gumtree {

void Framework::match(TreeDiff &Diff) {
  GreedyTopDown().match(Diff, {Diff.getOldRoot()}, {Diff.getNewRoot()});
  BottomUp->match(Diff);
}

std::unique_ptr<Framework> Framework::makeSimple() noexcept {
  return std::make_unique<Framework>(std::make_unique<SimpleBottomUp>());
}

std::unique_ptr<Framework> Framework::makeOptimal() noexcept {
  return std::make_unique<Framework>(std::make_unique<GreedyBottomUp>());
}

std::unique_ptr<Framework> Framework::makeOptimalInf() noexcept {
  return std::make_unique<Framework>(
      std::make_unique<GreedyBottomUp>(std::numeric_limits<uint32_t>::max()));
}

} // namespace diffink::gumtree