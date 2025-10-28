#include "DiffInk/TreeDiff/Recovery/SimpleRecovery.h"

namespace diffink::recovery {

void SimpleRecovery::matchExactly(TreeDiff &Mapping, VirtualNode *Old,
                                  VirtualNode *New) const {
  std::vector<VirtualNode *> UnmappedOldChildren, UnmappedNewChildren;
  UnmappedOldChildren.reserve(Old->Children.size());
  UnmappedNewChildren.reserve(New->Children.size());
  for (auto Child : Old->Children)
    if (!Mapping.findOldToNewMapping(Child))
      UnmappedOldChildren.push_back(Child);
  for (auto Child : New->Children)
    if (!Mapping.findNewToOldMapping(Child))
      UnmappedNewChildren.push_back(Child);

  for (const auto [Old, New] : findCommons(
           UnmappedOldChildren.size(), UnmappedNewChildren.size(),
           [&](std::size_t Old, std::size_t New) {
             return HashNode::equalExactly(UnmappedOldChildren[Old]->Original,
                                           UnmappedNewChildren[New]->Original);
           }))
    VirtualNode::traversePostOrder(
        UnmappedOldChildren[Old], UnmappedNewChildren[New],
        [&Mapping](VirtualNode *Old, VirtualNode *New) {
          Mapping.overrideMapping(Old, New);
        });
}

void SimpleRecovery::matchStructurally(TreeDiff &Mapping, VirtualNode *Old,
                                       VirtualNode *New) const {
  std::vector<VirtualNode *> UnmappedOldChildren, UnmappedNewChildren;
  UnmappedOldChildren.reserve(Old->Children.size());
  UnmappedNewChildren.reserve(New->Children.size());
  for (auto Child : Old->Children)
    if (!Mapping.findOldToNewMapping(Child))
      UnmappedOldChildren.push_back(Child);
  for (auto Child : New->Children)
    if (!Mapping.findNewToOldMapping(Child))
      UnmappedNewChildren.push_back(Child);

  for (const auto [Old, New] :
       findCommons(UnmappedOldChildren.size(), UnmappedNewChildren.size(),
                   [&](std::size_t Old, std::size_t New) {
                     return HashNode::equalStructurally(
                         UnmappedOldChildren[Old]->Original,
                         UnmappedNewChildren[New]->Original);
                   }))
    VirtualNode::traversePostOrder(
        UnmappedOldChildren[Old], UnmappedNewChildren[New],
        [&Mapping](VirtualNode *Old, VirtualNode *New) {
          Mapping.overrideMapping(Old, New);
        });
}

void SimpleRecovery::matchUnique(TreeDiff &Mapping, VirtualNode *Old,
                                 VirtualNode *New) {
  std::unordered_set<std::string> Keys;
  std::unordered_map<std::string, std::vector<VirtualNode *>> OldTypes;
  std::unordered_map<std::string, std::vector<VirtualNode *>> NewTypes;

  for (auto Child : Old->Children)
    if (!Mapping.findOldToNewMapping(Child)) {
      OldTypes[Child->Original.getType()].push_back(Child);
      Keys.insert(Child->Original.getType());
    }

  for (auto Child : New->Children)
    if (!Mapping.findNewToOldMapping(Child)) {
      NewTypes[Child->Original.getType()].push_back(Child);
      Keys.insert(Child->Original.getType());
    }

  for (auto Key : Keys) {
    auto OldIter = OldTypes.find(Key);
    auto NewIter = NewTypes.find(Key);
    if (OldIter != OldTypes.cend() && NewIter != NewTypes.cend()) {
      auto &OldCandidates = OldIter->second;
      auto &NewCandidates = NewIter->second;
      if (OldCandidates.size() == 1 && NewCandidates.size() == 1) {
        Mapping.insertMapping(OldCandidates[0], NewCandidates[0]);
        match(Mapping, OldCandidates[0], NewCandidates[0]);
      }
    }
  }
}

void SimpleRecovery::match(TreeDiff &Mapping, VirtualNode *Old,
                           VirtualNode *New) {
  matchExactly(Mapping, Old, New);
  matchStructurally(Mapping, Old, New);
  matchUnique(Mapping, Old, New);
}

} // namespace diffink::recovery