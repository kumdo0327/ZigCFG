#include "DiffInk/SyntaxTree/VirtualNode.h"

namespace diffink {

template <class Function>
void VirtualNode::traversePostOrder(VirtualNode *Node, Function &&Func) {
  for (auto Child : Node->Children)
    traversePostOrder(Child, Func);
  Func(Node);
}

template <class Function>
void VirtualNode::traversePostOrder(VirtualNode *Left, VirtualNode *Right,
                                    Function &&Func) {
  for (auto LeftIter = Left->Children.begin(),
            RightIter = Right->Children.begin();
       LeftIter != Left->Children.end() && RightIter != Right->Children.end();
       ++LeftIter, ++RightIter) {
    traversePostOrder(*LeftIter, *RightIter, Func);
  }
  Func(Left, Right);
}

} // namespace diffink