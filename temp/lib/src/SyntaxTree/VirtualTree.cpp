#include "DiffInk/SyntaxTree/VirtualTree.h"

namespace diffink {

void VirtualTree::build(const HashNode &Root) {
  clear();
  setRoot(pushBack(Root, nullptr));
  buildSubtree(getRoot());
  makeMetadata(getRoot());
}

void VirtualTree::buildSubtree(VirtualNode *Root) {
  Root->VirtualHeight = 0;
  for (const auto &Child : Root->Original.getChildren()) {
    auto Temp = pushBack(Child, Root);
    buildSubtree(Temp);
  }
}

void VirtualTree::clear() noexcept {
  NodeStore.clear();
  Root = nullptr;
}

VirtualNode *VirtualTree::pushBack(const HashNode &Original,
                                   VirtualNode *Parent) {
  auto NewNode = &NodeStore.emplace_back(VirtualNode{Original, Parent});
  if (Parent)
    Parent->Children.push_back(NewNode);
  return NewNode;
}

VirtualNode *VirtualTree::insert(const HashNode &Original, VirtualNode *Parent,
                                 std::size_t Index) {
  auto NewNode = &NodeStore.emplace_back(VirtualNode{Original, Parent});
  if (Parent)
    Parent->Children.insert(Parent->Children.cbegin() + Index, NewNode);
  return NewNode;
}

void VirtualTree::makeMetadata(VirtualNode *Root) noexcept {
  if (!Root)
    return;
  Root->VirtualHeight = 0;
  Root->VirtualSize = 1;
  for (auto Child : Root->Children) {
    makeMetadata(Child);
    Root->VirtualHeight =
        std::max(Root->VirtualHeight, Child->VirtualHeight + 1);
    Root->VirtualSize += Child->VirtualSize;
  }
}

void VirtualTree::move(VirtualNode *Src, VirtualNode *Parent,
                       std::size_t Index) {
  std::size_t SrcIndex = 0;
  for (; SrcIndex != Src->Parent->Children.size(); ++SrcIndex)
    if (Src->Parent->Children[SrcIndex] == Src)
      break;

  Src->Parent->Children.erase(Src->Parent->Children.cbegin() + SrcIndex);
  Parent->Children.insert(
      Parent->Children.cbegin() +
          (Src->Parent == Parent && SrcIndex < Index ? Index - 1 : Index),
      Src);
  Src->Parent = Parent;
}

} // namespace diffink