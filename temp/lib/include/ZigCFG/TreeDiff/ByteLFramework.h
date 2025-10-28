#ifndef DIFFINK_TREEDIFF_BYTELFRAMEWORK_H
#define DIFFINK_TREEDIFF_BYTELFRAMEWORK_H

#include "DiffInk/TreeDiff/GumTree/Framework.h"

namespace diffink {

class ByteLFramework : public TreeDiff::Matcher {
private:
  static constexpr XXH64_hash_t UncommonTypeHash{0};
  std::unique_ptr<Matcher> BottomUp;
  std::unique_ptr<TreeDiff::Recovery> Rec;

  std::unordered_set<VirtualNode *> UncommonOldNodes;
  std::unordered_set<VirtualNode *> UncommonNewNodes;
  std::vector<VirtualNode *> CommonOldAncestors;

private:
  void clear() noexcept;

  bool existsEmptyUncommons() const noexcept {
    return UncommonOldNodes.empty() || UncommonNewNodes.empty();
  }

  void makeMetadata(TreeDiff &Diff);

  void makeOldMetadata(TreeDiff &Diff, VirtualNode *Root);

  void makeNewMetadata(TreeDiff &Diff, VirtualNode *Root);

  std::pair<std::size_t, std::size_t> countCommons(VirtualNode *Left,
                                                   VirtualNode *Right) const;

public:
  ByteLFramework(std::unique_ptr<Matcher> BottomUp,
                 std::unique_ptr<TreeDiff::Recovery> Rec) noexcept
      : BottomUp(std::move(BottomUp)), Rec(std::move(Rec)) {}

  ~ByteLFramework() final = default;

  void match(TreeDiff &Diff) final;
};

std::unique_ptr<ByteLFramework> makeByteLSimple() noexcept;

} // namespace diffink

#endif // DIFFINK_TREEDIFF_BYTELFRAMEWORK_H