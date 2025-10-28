#include "DiffInk/TreeDiff/GumTree/GreedyTopDown.h"

namespace diffink::gumtree {

void GreedyTopDown::match(TreeDiff &Diff,
                          const std::unordered_set<VirtualNode *> &Old,
                          const std::unordered_set<VirtualNode *> &New) {
  std::vector<std::pair<VirtualNode *, VirtualNode *>> Candidates;
  std::map<uint32_t, std::vector<VirtualNode *>> OldQueue, NewQueue;

  for (auto Subtree : Old)
    if (Subtree->Original.getHeight() >= MinHeight)
      OldQueue[Subtree->Original.getHeight()].push_back(Subtree);
  for (auto Subtree : New)
    if (Subtree->Original.getHeight() >= MinHeight)
      NewQueue[Subtree->Original.getHeight()].push_back(Subtree);

  auto insertMapping = [&Diff](VirtualNode *OldNode, VirtualNode *NewNode) {
    VirtualNode::traversePostOrder(
        OldNode, NewNode, [&Diff](VirtualNode *Left, VirtualNode *Right) {
          Diff.insertMapping(Left, Right);
        });
  };

  while (!OldQueue.empty() && !NewQueue.empty()) {
    const auto &[OldHeight, OldSubtrees] = *OldQueue.rbegin();
    const auto &[NewHeight, NewSubtrees] = *NewQueue.rbegin();

    if (OldHeight > NewHeight) {
      for (auto Subtree : OldSubtrees)
        for (auto Child : Subtree->Children)
          if (Child->Original.getHeight() >= MinHeight)
            OldQueue[Child->Original.getHeight()].push_back(Child);
      OldQueue.erase(std::prev(OldQueue.end()));
      continue;
    }

    if (OldHeight != NewHeight) {
      for (auto Subtree : NewSubtrees)
        for (auto Child : Subtree->Children)
          if (Child->Original.getHeight() >= MinHeight)
            NewQueue[Child->Original.getHeight()].push_back(Child);
      NewQueue.erase(std::prev(NewQueue.end()));
      continue;
    }

    std::unordered_map<VirtualNode *, std::vector<VirtualNode *>>
        OldToNewCandidates, NewToOldCandidates;
    std::unordered_set<VirtualNode *> MappedCandidates;

    for (auto OldSubtree : OldSubtrees)
      for (auto NewSubtree : NewSubtrees)
        if (HashNode::equalExactly(OldSubtree->Original,
                                   NewSubtree->Original)) {
          OldToNewCandidates[OldSubtree].push_back(NewSubtree);
          NewToOldCandidates[NewSubtree].push_back(OldSubtree);
          MappedCandidates.insert(OldSubtree);
          MappedCandidates.insert(NewSubtree);
        }

    for (auto Item : OldToNewCandidates) {
      auto OldSubtree = Item.first;
      auto MappedSubtrees = Item.second;

      if (MappedSubtrees.size() == 1 &&
          NewToOldCandidates[MappedSubtrees[0]].size() == 1)
        insertMapping(OldSubtree, MappedSubtrees[0]);
      else
        for (auto Subtree : MappedSubtrees)
          Candidates.emplace_back(OldSubtree, Subtree);
    }

    for (auto Subtree : OldSubtrees)
      if (!MappedCandidates.contains(Subtree))
        for (auto Child : Subtree->Children)
          if (Child->Original.getHeight() >= MinHeight)
            OldQueue[Child->Original.getHeight()].push_back(Child);

    for (auto Subtree : NewSubtrees)
      if (!MappedCandidates.contains(Subtree))
        for (auto Child : Subtree->Children)
          if (Child->Original.getHeight() >= MinHeight)
            NewQueue[Child->Original.getHeight()].push_back(Child);

    OldQueue.erase(std::prev(OldQueue.end()));
    NewQueue.erase(std::prev(NewQueue.end()));
  }

  std::sort(Candidates.begin(), Candidates.end(), Comparator(Diff));
  std::unordered_set<VirtualNode *> MappedSubtrees;
  MappedSubtrees.reserve(Candidates.size());

  for (auto Item : Candidates)
    if (!MappedSubtrees.contains(Item.first) &&
        !MappedSubtrees.contains(Item.second)) {
      insertMapping(Item.first, Item.second);
      MappedSubtrees.insert(Item.first);
      MappedSubtrees.insert(Item.second);
    }
}

} // namespace diffink::gumtree