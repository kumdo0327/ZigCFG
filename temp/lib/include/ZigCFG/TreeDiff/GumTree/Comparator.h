#ifndef DIFFINK_TREEDIFF_GUMTREE_COMPARATOR_H
#define DIFFINK_TREEDIFF_GUMTREE_COMPARATOR_H

#include "DiffInk/TreeDiff/Metric/DiceSimilarity.h"

namespace diffink::gumtree {

class Comparator {
private:
  using NodePair = std::pair<VirtualNode *, VirtualNode *>;

  struct PairHash {
    std::size_t operator()(NodePair Pair) const {
      return std::hash<VirtualNode *>{}(Pair.first) ^
             std::hash<VirtualNode *>{}(Pair.second);
    }
  };

private:
  TreeDiff &Mapping;

  std::unordered_map<VirtualNode *, std::unordered_set<VirtualNode *>>
      SiblingsCache;
  std::unordered_map<NodePair, double, PairHash> SiblingSimilarityCache;
  std::unordered_map<NodePair, double, PairHash> AncestorSimilarityCache;

private:
  const std::unordered_set<VirtualNode *> &makeSiblings(VirtualNode *Node);

  std::vector<VirtualNode *> makeAncestors(VirtualNode *Node);

  double computeSiblingSimilarity(NodePair Pair);

  double computeAncestorSimilarity(NodePair Pair);

  int64_t computePositionSimilarity(NodePair Pair);

  int64_t computeTextualSimilarity(NodePair Pair);

  int64_t computeOffsetSimilarity(NodePair Pair);

public:
  Comparator(TreeDiff &Mapping) : Mapping{Mapping} {}

  bool operator()(NodePair Left, NodePair Right);
};

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_GUMTREE_COMPARATOR_H