#include "DiffInk/TreeDiff/ByteLFramework.h"

namespace diffink {

void ByteLFramework::clear() noexcept {
  CommonOldAncestors.clear();
  UncommonOldNodes.clear();
  UncommonNewNodes.clear();
}

void ByteLFramework::makeMetadata(TreeDiff &Diff) {
  clear();
  makeOldMetadata(Diff, Diff.getOldRoot());
  makeNewMetadata(Diff, Diff.getNewRoot());
}

void ByteLFramework::makeOldMetadata(TreeDiff &Diff, VirtualNode *Root) {
  Root->VirtualHeight = 0;
  Root->VirtualSize = 1;
  if (!Diff.findOldToNewMapping(Root))
    UncommonOldNodes.insert(Root);
  else if (!Root->isLeaf())
    for (auto Child : Root->Children) {
      makeOldMetadata(Diff, Child);
      Root->VirtualHeight =
          std::max(Root->VirtualHeight, Child->VirtualHeight + 1);
      Root->VirtualSize += Child->VirtualSize;
    }
}

void ByteLFramework::makeNewMetadata(TreeDiff &Diff, VirtualNode *Root) {
  Root->VirtualHeight = 0;
  Root->VirtualSize = 1;
  if (!Diff.findNewToOldMapping(Root))
    UncommonNewNodes.insert(Root);
  else if (!Root->isLeaf())
    for (auto Child : Root->Children) {
      makeNewMetadata(Diff, Child);
      Root->VirtualHeight =
          std::max(Root->VirtualHeight, Child->VirtualHeight + 1);
      Root->VirtualSize += Child->VirtualSize;
    }
}

std::pair<std::size_t, std::size_t>
ByteLFramework::countCommons(VirtualNode *Left, VirtualNode *Right) const {
  std::size_t Exact{0}, Struct{0};
  VirtualNode::traversePostOrder(
      Left, Right, [&Exact, &Struct](VirtualNode *Left, VirtualNode *Right) {
        if (HashNode::equalExactly(Left->Original, Right->Original))
          ++Exact;
        if (HashNode::equalStructurally(Left->Original, Right->Original))
          ++Struct;
      });
  return {Exact, Struct};
}

void ByteLFramework::match(TreeDiff &Diff) {
  if (Diff.isIncparsed()) {
    makeMetadata(Diff);
    if (!existsEmptyUncommons()) {
      gumtree::GreedyTopDown().match(Diff, UncommonOldNodes, UncommonNewNodes);
      BottomUp->match(Diff);
      std::unordered_set<VirtualNode *> OldUncommonParents;
      for (auto Node : UncommonOldNodes)
        OldUncommonParents.insert(Node->Parent);
      for (auto Node : OldUncommonParents)
        Rec->match(Diff, Node, Diff.findOldToNewMapping(Node));
    }
  } else {
    clear();
    gumtree::GreedyTopDown().match(Diff, {Diff.getOldRoot()},
                                   {Diff.getNewRoot()});
    BottomUp->match(Diff);
  }
  clear();
}

std::unique_ptr<ByteLFramework> makeByteLSimple() noexcept {
  return std::make_unique<ByteLFramework>(
      std::make_unique<gumtree::SimpleBottomUp>(),
      std::make_unique<recovery::SimpleRecovery>());
}

} // namespace diffink
