#ifndef DIFFINK_TREEDIFF_DIFFINKMATCHER_H
#define DIFFINK_TREEDIFF_DIFFINKMATCHER_H

#include "DiffInk/TreeDiff/GumTree/Framework.h"

namespace diffink {

class DiffInkMatcher : public TreeDiff::Matcher {
private:
  static constexpr XXH64_hash_t UncommonTypeHash{0};
  std::unique_ptr<Matcher> BottomUp;
  std::unique_ptr<TreeDiff::Recovery> Rec;

  std::unordered_set<VirtualNode *> UncommonOldNodes;
  std::unordered_set<VirtualNode *> UncommonNewNodes;
  std::vector<VirtualNode *> CommonOldAncestors;
  std::unordered_map<VirtualNode *, XXH64_hash_t> VirtualHash;

private:
  void clear() noexcept;

  bool existsEmptyUncommons() const noexcept {
    return UncommonOldNodes.empty() || UncommonNewNodes.empty();
  }

  void makeMetadata(TreeDiff &Diff);

  void makeOldMetadata(TreeDiff &Diff, VirtualNode *Root);

  void makeNewMetadata(TreeDiff &Diff, VirtualNode *Root);

  void makeHash(TreeDiff &Diff);

  void makeOldHash(TreeDiff &Diff, VirtualNode *Root);

  void makeNewHash(TreeDiff &Diff, VirtualNode *Root);

  bool resolveFragmentation(TreeDiff &Diff);

  bool swapCrossMapping(TreeDiff &Diff);

  bool equal(VirtualNode *Left, VirtualNode *Right) const {
    return VirtualHash.at(Left) == VirtualHash.at(Right);
  }

  std::pair<std::size_t, std::size_t> countCommons(VirtualNode *Left,
                                                   VirtualNode *Right) const;

public:
  DiffInkMatcher(std::unique_ptr<Matcher> BottomUp,
                 std::unique_ptr<TreeDiff::Recovery> Rec) noexcept
      : BottomUp(std::move(BottomUp)), Rec(std::move(Rec)) {}

  ~DiffInkMatcher() final = default;

  void match(TreeDiff &Diff) final;
};

std::unique_ptr<DiffInkMatcher> makeDiffinkSimple() noexcept;

std::unique_ptr<DiffInkMatcher> makeDiffinkOptimal() noexcept;

std::unique_ptr<DiffInkMatcher> makeDiffinkOptimalInf() noexcept;

} // namespace diffink

#endif // DIFFINK_TREEDIFF_DIFFINKMATCHER_H