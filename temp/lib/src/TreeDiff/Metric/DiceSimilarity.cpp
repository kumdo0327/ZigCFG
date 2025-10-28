#include "DiffInk/TreeDiff/Metric/DiceSimilarity.h"

namespace diffink::metric {

double computeDiceSimilarity(TreeDiff &Mapping,
                             const std::unordered_set<VirtualNode *> &Old,
                             VirtualNode *New) {
  std::size_t Count{0};
  VirtualNode::traversePostOrder(New, [&, New](VirtualNode *Node) {
    if (Node != New && Old.contains(Mapping.findNewToOldMapping(Node)))
      ++Count;
  });
  return computeDiceSimilarity(Count, Old.size(), New->VirtualSize - 1);
}

} // namespace diffink::metric