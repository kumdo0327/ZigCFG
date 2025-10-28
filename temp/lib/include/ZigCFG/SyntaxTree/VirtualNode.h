#ifndef DIFFINK_SYNTAXTREE_VIRTUALNODE_H
#define DIFFINK_SYNTAXTREE_VIRTUALNODE_H

#include "DiffInk/SyntaxTree/HashNode.h"

namespace diffink {

struct VirtualNode {
  const HashNode &Original;
  VirtualNode *Parent{nullptr};
  std::vector<VirtualNode *> Children{};
  bool Marker{false};
  uint32_t VirtualHeight{0};
  uint32_t VirtualSize{1};

  void _makePostOrder(std::vector<VirtualNode *> &PostOrder, VirtualNode *Iter);

  void _makeDescendants(std::unordered_set<VirtualNode *> &Descendants,
                        VirtualNode *Iter);

  bool isLeaf() const noexcept { return Children.empty(); }

  std::vector<VirtualNode *> makePostOrder();

  std::unordered_set<VirtualNode *> makeDescendants();

  std::size_t findChild(VirtualNode *Child);

  template <class Function>
  static void traversePostOrder(VirtualNode *Node, Function &&Func);

  template <class Function>
  static void traversePostOrder(VirtualNode *Left, VirtualNode *Right,
                                Function &&Func);
};

} // namespace diffink

#include "DiffInk/SyntaxTree/VirtualNode.hpp"

#endif // DIFFINK_SYNTAXTREE_VIRTUALNODE_H