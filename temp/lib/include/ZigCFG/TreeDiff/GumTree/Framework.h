#ifndef DIFFINK_TREEDIFF_GUMTREE_FRAMEWORK_H
#define DIFFINK_TREEDIFF_GUMTREE_FRAMEWORK_H

#include "DiffInk/TreeDiff/GumTree/GreedyBottomUp.h"
#include "DiffInk/TreeDiff/GumTree/GreedyTopDown.h"
#include "DiffInk/TreeDiff/GumTree/SimpleBottomUp.h"

namespace diffink::gumtree {

class Framework : public TreeDiff::Matcher {
private:
  std::unique_ptr<Matcher> BottomUp;

public:
  Framework(std::unique_ptr<Matcher> BottomUp) noexcept
      : BottomUp(std::move(BottomUp)) {}

  ~Framework() final = default;

  void match(TreeDiff &Diff) final;

public:
  static std::unique_ptr<Framework> makeSimple() noexcept;

  static std::unique_ptr<Framework> makeOptimal() noexcept;

  static std::unique_ptr<Framework> makeOptimalInf() noexcept;
};

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_GUMTREE_FRAMEWORK_H