#include "DiffInk/TreeDiff/DiffInkMatcher.h"

namespace diffink {

void DiffInkMatcher::clear() noexcept {
  CommonOldAncestors.clear();
  UncommonOldNodes.clear();
  UncommonNewNodes.clear();
  VirtualHash.clear();
}

void DiffInkMatcher::makeMetadata(TreeDiff &Diff) {
  clear();
  makeOldMetadata(Diff, Diff.getOldRoot());
  makeNewMetadata(Diff, Diff.getNewRoot());
}

void DiffInkMatcher::makeOldMetadata(TreeDiff &Diff, VirtualNode *Root) {
  Root->VirtualHeight = 0;
  Root->VirtualSize = 1;
  if (!Diff.findOldToNewMapping(Root))
    UncommonOldNodes.insert(Root);
  else if (!Root->isLeaf())
    for (auto Child : Root->Children) {
      makeOldMetadata(Diff, Child);
      Root->VirtualHeight =
          std::max(Root->VirtualHeight, Child->VirtualHeight + 1);
      Root->VirtualSize += Child->VirtualSize;
    }
}

void DiffInkMatcher::makeNewMetadata(TreeDiff &Diff, VirtualNode *Root) {
  Root->VirtualHeight = 0;
  Root->VirtualSize = 1;
  if (!Diff.findNewToOldMapping(Root))
    UncommonNewNodes.insert(Root);
  else if (!Root->isLeaf())
    for (auto Child : Root->Children) {
      makeNewMetadata(Diff, Child);
      Root->VirtualHeight =
          std::max(Root->VirtualHeight, Child->VirtualHeight + 1);
      Root->VirtualSize += Child->VirtualSize;
    }
}

void DiffInkMatcher::makeHash(TreeDiff &Diff) {
  clear();
  makeOldHash(Diff, Diff.getOldRoot());
  makeNewHash(Diff, Diff.getNewRoot());
}

void DiffInkMatcher::makeOldHash(TreeDiff &Diff, VirtualNode *Root) {
  Root->VirtualHeight = 0;
  Root->VirtualSize = 1;
  if (!Diff.findOldToNewMapping(Root)) {
    UncommonOldNodes.insert(Root);
    VirtualHash.emplace(Root, UncommonTypeHash);
  } else if (Root->isLeaf())
    VirtualHash.emplace(Root, Root->Original.getTypeHash());
  else {
    CommonOldAncestors.push_back(Root);
    std::vector<XXH64_hash_t> Hashes;
    Hashes.reserve(Root->Children.size());
    for (auto Child : Root->Children) {
      makeOldHash(Diff, Child);
      Root->VirtualHeight =
          std::max(Root->VirtualHeight, Child->VirtualHeight + 1);
      Root->VirtualSize += Child->VirtualSize;
      Hashes.push_back(VirtualHash[Child]);
    }
    VirtualHash.emplace(
        Root, xxhVector64({Root->Original.getTypeHash(), xxhVector64(Hashes)}));
  }
}

void DiffInkMatcher::makeNewHash(TreeDiff &Diff, VirtualNode *Root) {
  Root->VirtualHeight = 0;
  Root->VirtualSize = 1;
  if (!Diff.findNewToOldMapping(Root)) {
    UncommonNewNodes.insert(Root);
    VirtualHash.emplace(Root, UncommonTypeHash);
  } else if (Root->isLeaf())
    VirtualHash.emplace(Root, Root->Original.getTypeHash());
  else {
    std::vector<XXH64_hash_t> Hashes;
    Hashes.reserve(Root->Children.size());
    for (auto Child : Root->Children) {
      makeNewHash(Diff, Child);
      Root->VirtualHeight =
          std::max(Root->VirtualHeight, Child->VirtualHeight + 1);
      Root->VirtualSize += Child->VirtualSize;
      Hashes.push_back(VirtualHash[Child]);
    }
    VirtualHash.emplace(
        Root, xxhVector64({Root->Original.getTypeHash(), xxhVector64(Hashes)}));
  }
}

bool DiffInkMatcher::resolveFragmentation(TreeDiff &Diff) {
  makeMetadata(Diff);
  if (existsEmptyUncommons())
    return false;

  std::unordered_set<VirtualNode *> UnrollingOldNodes, UnrollingNewNodes;
  for (auto [Target, Partner, Unrolling] :
       {std::make_tuple(Diff.getOldRoot(), &UncommonNewNodes,
                        &UnrollingOldNodes),
        std::make_tuple(Diff.getNewRoot(), &UncommonOldNodes,
                        &UnrollingNewNodes)}) {
    decltype(&TreeDiff::findOldToNewMapping) findMapping =
        Target == Diff.getOldRoot() ? &TreeDiff::findOldToNewMapping
                                    : &TreeDiff::findNewToOldMapping;

    std::map<uint32_t, std::vector<VirtualNode *>> TargetQueue;
    for (auto Node : Target->Children)
      if (Node->VirtualHeight != 0)
        TargetQueue[Node->Original.getHeight()].push_back(Node);
    std::map<uint32_t, std::vector<const HashNode *>> UncommonQueue;
    for (auto Node : *Partner)
      if (!Node->Original.isLeaf())
        UncommonQueue[Node->Original.getHeight()].push_back(&Node->Original);

    while (!TargetQueue.empty() && !UncommonQueue.empty()) {
      const auto &[TargetHeight, Targets] = *TargetQueue.rbegin();
      const auto &[UncommonHeight, Uncommons] = *UncommonQueue.rbegin();
      if (TargetHeight > UncommonHeight) {
        for (auto Node : Targets)
          for (auto Child : Node->Children)
            if (Child->VirtualHeight != 0)
              TargetQueue[Child->Original.getHeight()].push_back(Child);
        TargetQueue.erase(std::prev(TargetQueue.end()));
      } else if (TargetHeight != UncommonHeight) {
        for (auto Node : Uncommons)
          for (auto &Child : Node->getChildren())
            if (!Child.isLeaf())
              UncommonQueue[Child.getHeight()].push_back(&Child);
        UncommonQueue.erase(std::prev(UncommonQueue.end()));
      }

      else /*TargetHeight == UncommonHeight*/ {
        std::unordered_set<XXH64_hash_t> Hashes, CheckedHashes;
        for (auto Node : Uncommons)
          Hashes.insert(Node->getExactHash());
        for (auto Node : Targets)
          if (Hashes.contains(Node->Original.getExactHash())) {
            Unrolling->insert(Node);
            CheckedHashes.insert(Node->Original.getExactHash());
          } else
            for (auto Child : Node->Children)
              if (Child->VirtualHeight != 0)
                TargetQueue[Child->Original.getHeight()].push_back(Child);
        for (auto Node : Uncommons)
          if (!CheckedHashes.contains(Node->getExactHash()))
            for (auto &Child : Node->getChildren())
              if (!Child.isLeaf())
                UncommonQueue[Child.getHeight()].push_back(&Child);
        TargetQueue.erase(std::prev(TargetQueue.end()));
        UncommonQueue.erase(std::prev(UncommonQueue.end()));
      }
    }
  }

  for (auto Node : UnrollingOldNodes)
    if (Diff.findOldToNewMapping(Node))
      VirtualNode::traversePostOrder(Node, [&Diff](VirtualNode *Node) {
        Diff.eraseOldMapping(Node);
        if (Node->isLeaf() && !Node->Original.isLeaf())
          Diff.buildOldSubtree(Node);
      });
  for (auto Node : UnrollingNewNodes)
    if (Diff.findNewToOldMapping(Node))
      VirtualNode::traversePostOrder(Node, [&Diff](VirtualNode *Node) {
        Diff.eraseNewMapping(Node);
        if (Node->isLeaf() && !Node->Original.isLeaf())
          Diff.buildNewSubtree(Node);
      });
  return true;
}

bool DiffInkMatcher::swapCrossMapping(TreeDiff &Diff) {
  makeHash(Diff);
  if (existsEmptyUncommons()) {
    CommonOldAncestors.clear();
    VirtualHash.clear();
    return false;
  }

  std::vector<std::unordered_set<VirtualNode *>> CrossMappingCandidates(
      Diff.getOldRoot()->VirtualHeight + 1);
  for (auto Node : CommonOldAncestors)
    if (equal(Node, Diff.findOldToNewMapping(Node)))
      CrossMappingCandidates[Node->VirtualHeight].insert(Node);
  for (auto Candidates = CrossMappingCandidates.crbegin();
       Candidates != CrossMappingCandidates.crend(); ++Candidates) {
    if (Candidates->size() < 2)
      continue;

    std::unordered_map<VirtualNode *, std::pair<std::size_t, std::size_t>>
        SimilarityCache;
    SimilarityCache.reserve(Candidates->size());
    for (auto Node : *Candidates)
      SimilarityCache.emplace(
          Node, countCommons(Node, Diff.findOldToNewMapping(Node)));

    for (auto Iter = Candidates->cbegin(); Iter != Candidates->cend(); ++Iter) {
      auto Node = *Iter;
      auto Partner = Diff.findOldToNewMapping(Node);
      auto MaxIndex = Node;
      auto MaxSim = SimilarityCache[Node];
      for (auto AltIter = std::next(Iter); AltIter != Candidates->cend();
           ++AltIter)
        if (auto AltNode = *AltIter; equal(Node, AltNode)) {
          auto AltPartner = Diff.findOldToNewMapping(AltNode);
          auto AltMaxSim = countCommons(Node, AltPartner);
          if (AltMaxSim > MaxSim &&
              countCommons(AltNode, Partner) > SimilarityCache[AltNode]) {
            MaxSim = AltMaxSim;
            MaxIndex = *AltIter;
          }
        }
      if (MaxIndex != Node) {
        SimilarityCache[MaxIndex] = countCommons(MaxIndex, Partner);
        VirtualNode::traversePostOrder(
            Node, MaxIndex, [&Diff](VirtualNode *Left, VirtualNode *Right) {
              Diff.swapMapping(Left, Right);
            });
      }
    }
  }
  CommonOldAncestors.clear();
  VirtualHash.clear();
  return true;
}

std::pair<std::size_t, std::size_t>
DiffInkMatcher::countCommons(VirtualNode *Left, VirtualNode *Right) const {
  std::size_t Exact{0}, Struct{0};
  VirtualNode::traversePostOrder(
      Left, Right, [&Exact, &Struct](VirtualNode *Left, VirtualNode *Right) {
        if (HashNode::equalExactly(Left->Original, Right->Original))
          ++Exact;
        if (HashNode::equalStructurally(Left->Original, Right->Original))
          ++Struct;
      });
  return {Exact, Struct};
}

void DiffInkMatcher::match(TreeDiff &Diff) {
  if (Diff.isIncparsed()) {
    if (resolveFragmentation(Diff))
      swapCrossMapping(Diff);
    VirtualTree::makeMetadata(Diff.getOldRoot());
    VirtualTree::makeMetadata(Diff.getNewRoot());
    if (!existsEmptyUncommons()) {
      gumtree::GreedyTopDown().match(Diff, UncommonOldNodes, UncommonNewNodes);
      BottomUp->match(Diff);
      std::unordered_set<VirtualNode *> OldUncommonParents;
      for (auto Node : UncommonOldNodes)
        OldUncommonParents.insert(Node->Parent);
      for (auto Node : OldUncommonParents)
        Rec->match(Diff, Node, Diff.findOldToNewMapping(Node));
    }
  } else {
    clear();
    gumtree::GreedyTopDown().match(Diff, {Diff.getOldRoot()},
                                   {Diff.getNewRoot()});
    BottomUp->match(Diff);
  }
  clear();
}

std::unique_ptr<DiffInkMatcher> makeDiffinkSimple() noexcept {
  return std::make_unique<DiffInkMatcher>(
      std::make_unique<gumtree::SimpleBottomUp>(),
      std::make_unique<recovery::SimpleRecovery>());
}

std::unique_ptr<DiffInkMatcher> makeDiffinkOptimal() noexcept {
  return std::make_unique<DiffInkMatcher>(
      std::make_unique<gumtree::GreedyBottomUp>(),
      std::make_unique<recovery::OptimalRecovery>(gumtree::DefaultMaxSize));
}

std::unique_ptr<DiffInkMatcher> makeDiffinkOptimalInf() noexcept {
  return std::make_unique<DiffInkMatcher>(
      std::make_unique<gumtree::GreedyBottomUp>(
          std::numeric_limits<uint32_t>::max()),
      std::make_unique<recovery::OptimalRecovery>(
          std::numeric_limits<uint32_t>::max()));
}

} // namespace diffink
