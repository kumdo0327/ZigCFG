#include "DiffInk/SyntaxTree/VirtualNode.h"

namespace diffink {

void VirtualNode::_makePostOrder(std::vector<VirtualNode *> &PostOrder,
                                 VirtualNode *Iter) {
  if (Iter->Children.empty())
    PostOrder.push_back(Iter);
  else {
    for (auto &Child : Iter->Children)
      _makePostOrder(PostOrder, Child);
    PostOrder.push_back(Iter);
  }
}

void VirtualNode::_makeDescendants(
    std::unordered_set<VirtualNode *> &Descendants, VirtualNode *Iter) {
  Descendants.insert(Iter);
  for (auto &Child : Iter->Children)
    _makeDescendants(Descendants, Child);
}

std::vector<VirtualNode *> VirtualNode::makePostOrder() {
  std::vector<VirtualNode *> PostOrder;
  _makePostOrder(PostOrder, this);
  return PostOrder;
}

std::unordered_set<VirtualNode *> VirtualNode::makeDescendants() {
  std::unordered_set<VirtualNode *> Descendants;
  _makeDescendants(Descendants, this);
  Descendants.erase(this);
  return Descendants;
}

std::size_t VirtualNode::findChild(VirtualNode *Child) {
  for (std::size_t i{0}; i != Children.size(); ++i)
    if (Children[i] == Child)
      return i;
  return Children.size();
}

} // namespace diffink