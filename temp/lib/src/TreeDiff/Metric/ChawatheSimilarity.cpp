#include "DiffInk/TreeDiff/Metric/ChawatheSimilarity.h"

namespace diffink::metric {

double computeChawatheSimilarity(TreeDiff &Mapping,
                                 const std::unordered_set<VirtualNode *> &Old,
                                 VirtualNode *New) {
  std::size_t Count{0};
  VirtualNode::traversePostOrder(New, [&, New](VirtualNode *Node) {
    if (Node != New && Old.contains(Mapping.findNewToOldMapping(Node)))
      ++Count;
  });
  return computeChawatheSimilarity(Count, Old.size(), New->VirtualSize - 1);
}

double computeAutoChawatheThreshold(VirtualNode *Old,
                                    VirtualNode *New) noexcept {
  return 1.0 / (1.0 + std::log(Old->VirtualSize + New->VirtualSize - 2));
}

} // namespace diffink::metric