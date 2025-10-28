#ifndef DIFFINK_SYNTAXTREE_VIRTUALTREE_H
#define DIFFINK_SYNTAXTREE_VIRTUALTREE_H

#include "DiffInk/SyntaxTree/VirtualNode.h"

namespace diffink {

class VirtualTree {
private:
  std::list<VirtualNode> NodeStore;
  VirtualNode *Root{nullptr};

public:
  VirtualTree() noexcept = default;

  VirtualTree(const VirtualTree &Rhs) = delete;

  void build(const HashNode &Root);

  void buildSubtree(VirtualNode *Root);

  VirtualNode *pushBack(const HashNode &Original, VirtualNode *Parent);

  VirtualNode *insert(const HashNode &Original, VirtualNode *Parent,
                      std::size_t Index);

  void clear() noexcept;

  void setRoot(VirtualNode *Root) noexcept { this->Root = Root; }

  VirtualNode *getRoot() const noexcept { return Root; }

  static void makeMetadata(VirtualNode *Root) noexcept;

  static void erase(VirtualNode *Parent, std::size_t Index) {
    Parent->Children.erase(Parent->Children.cbegin() + Index);
  }

  static void move(VirtualNode *Src, VirtualNode *Parent, std::size_t Index);
};

} // namespace diffink

#endif // DIFFINK_SYNTAXTREE_VIRTUALTREE_H