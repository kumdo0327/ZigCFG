#include "DiffInk/TreeDiff/GumTree/SimpleBottomUp.h"

namespace diffink::gumtree {

void SimpleBottomUp::match(TreeDiff &Diff, VirtualNode *Node) {
  if (Node->Original.isLeaf() || Diff.findOldToNewMapping(Node))
    return;

  std::vector<VirtualNode *> Seeds;
  VirtualNode::traversePostOrder(
      Node, [this, &Diff, &Seeds](VirtualNode *Node) {
        auto Partner = Diff.findOldToNewMapping(Node);
        if (Partner)
          Seeds.push_back(Partner);
      });

  std::vector<VirtualNode *> Candidates;
  {
    std::unordered_set<VirtualNode *> Visited;
    for (auto Seed : Seeds)
      for (Seed = Seed->Parent;
           Seed->Parent != nullptr && !Visited.contains(Seed);
           Seed = Seed->Parent) {
        Visited.insert(Seed);
        if (Seed->Original.getType() == Node->Original.getType() &&
            !Diff.findNewToOldMapping(Seed))
          Candidates.push_back(Seed);
      }
  }

  if (Candidates.empty())
    return;
  auto Desendants = Node->makeDescendants();
  VirtualNode *Best{nullptr};
  double Max{-1.0};

  for (auto Candidate : Candidates) {
    auto AutoThreshold =
        Threshold ? *Threshold
                  : metric::computeAutoChawatheThreshold(Node, Candidate);
    auto Similarity =
        metric::computeChawatheSimilarity(Diff, Desendants, Candidate);
    if (Similarity > Max && Similarity >= 0) {
      Max = Similarity;
      Best = Candidate;
    }
  }

  if (Best) {
    Diff.insertMapping(Node, Best);
    Recovery.match(Diff, Node, Best);
  }
}

void SimpleBottomUp::match(TreeDiff &Diff) {
  Diff.getOldRoot()->traversePostOrder(
      Diff.getOldRoot(), [this, &Diff](VirtualNode *Node) {
        if (Node != Diff.getOldRoot())
          match(Diff, Node);
        else {
          if (!Diff.findOldToNewMapping(Node))
            Diff.insertMapping(Node, Diff.getNewRoot());
          Recovery.match(Diff, Node, Diff.getNewRoot());
        }
      });
}

} // namespace diffink::gumtree