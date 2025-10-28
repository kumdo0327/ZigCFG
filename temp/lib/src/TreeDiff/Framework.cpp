#include "DiffInk/TreeDiff/Framework.h"

namespace diffink {

void Framework::incbuild(const MerkleTree &OriginalTree,
                         VirtualTree &TreeCopy) {
  auto Root = TreeCopy.pushBack(OriginalTree.getRoot(), nullptr);
  TreeCopy.setRoot(Root);
  Copies.emplace(&OriginalTree.getRoot(), Root);
  _incbuild(OriginalTree, TreeCopy, OriginalTree.getRoot(), Root);
}

void Framework::_incbuild(const MerkleTree &OriginalTree, VirtualTree &TreeCopy,
                          const HashNode &OriginalNode, VirtualNode *NodeCopy) {
  if (!OriginalNode.getChildren().empty())
    for (auto &Child : OriginalNode.getChildren()) {
      auto Temp = TreeCopy.pushBack(Child, NodeCopy);
      Copies.emplace(&Child, Temp);
      if (OriginalTree.isUncommon(Child))
        TreeCopy.buildSubtree(Temp);
      else if (OriginalTree.hasUncommonChild(Child))
        _incbuild(OriginalTree, TreeCopy, Child, Temp);
    }
}

void Framework::matchCommons(const MerkleTree &New) {
  for (auto [OldNode, NewNode] : New.getMapping())
    insertMapping(Copies[OldNode], Copies[NewNode]);
  Copies.clear();
}

EditScript Framework::makeEditScript() {
  EditScript Script;
  std::queue<VirtualNode *> Queue;
  Queue.push(NewTree.getRoot());

  while (!Queue.empty()) {
    auto Node = Queue.front();
    Queue.pop();
    for (auto Child : Node->Children)
      Queue.push(Child);
    auto Partner = findNewToOldMapping(Node);
    auto ParentPartner = findNewToOldMapping(Node->Parent);

    if (!Partner) {
      auto Index = findPosition(Node);
      Script.emplace_back(
          edit_action::InsertNode{.Leaf = Node->Original,
                                  .Parent = Node->Parent->Original,
                                  .Index = Index});
      Partner = OldTree.insert(Node->Original, ParentPartner, Index);
      insertMapping(Partner, Node);
    }

    else if (Node->Parent) {
      if (Partner->Original.getLabel() != Node->Original.getLabel())
        Script.emplace_back(edit_action::UpdateNode{
            .Leaf = Partner->Original, .UpdatedLeaf = Node->Original});

      if (ParentPartner != Partner->Parent) {
        auto Index = findPosition(Node);
        Script.emplace_back(
            edit_action::MoveTree{.Subtree = Partner->Original,
                                  .Parent = ParentPartner->Original,
                                  .Index = Index,
                                  .MovedSubtree = Node->Original});
        OldTree.move(Partner, ParentPartner, Index);
      }
    }

    std::vector<VirtualNode *> OutOfOrder;
    std::size_t Index{0};
    for (const auto [Old, New] : findCommons(
             Partner->Children.size(), Node->Children.size(),
             [&, this](std::size_t Old, std::size_t New) {
               return areContained(Partner->Children[Old], Node->Children[New]);
             })) {
      Partner->Children[Old]->Marker = true;
      Node->Children[New]->Marker = true;
      for (; Index != New; ++Index)
        OutOfOrder.push_back(Node->Children[Index]);
      ++Index;
    }
    for (; Index != Node->Children.size(); ++Index)
      OutOfOrder.push_back(Node->Children[Index]);
    if (OutOfOrder.empty())
      continue;

    std::unordered_set<VirtualNode *> PartnerChildren(Partner->Children.begin(),
                                                      Partner->Children.end());
    for (auto Iter = OutOfOrder.crbegin(); Iter != OutOfOrder.crend(); ++Iter) {
      auto Child = *Iter;
      auto ChildPartner = findNewToOldMapping(Child);
      if (PartnerChildren.contains(ChildPartner)) {
        auto Index = findPosition(Child);
        Script.emplace_back(
            edit_action::MoveTree{.Subtree = ChildPartner->Original,
                                  .Parent = Partner->Original,
                                  .Index = Index,
                                  .MovedSubtree = Child->Original});
        OldTree.move(ChildPartner, Partner, Index);
        ChildPartner->Marker = true;
        Child->Marker = true;
      }
    }
  }

  VirtualNode::traversePostOrder(
      OldTree.getRoot(), [&Script, this](VirtualNode *Node) {
        if (!findOldToNewMapping(Node))
          Script.emplace_back(edit_action::DeleteNode{
              .Leaf = Node->Original, .Parent = Node->Parent->Original});
      });
  return Script;
}

std::size_t Framework::findPosition(VirtualNode *Node) const {
  VirtualNode *Predecessor{nullptr};
  for (const auto Child : Node->Parent->Children) {
    if (Child == Node)
      break;
    if (Child->Marker)
      Predecessor = Child;
  }
  if (!Predecessor)
    return 0;
  auto OldPredecessor = findNewToOldMapping(Predecessor);
  return OldPredecessor->Parent->findChild(OldPredecessor) + 1;
}

VirtualNode *Framework::findOldToNewMapping(VirtualNode *Node) const {
  auto Iter = OldToNewMapping.find(Node);
  return Iter != OldToNewMapping.cend() ? Iter->second : nullptr;
}

VirtualNode *Framework::findNewToOldMapping(VirtualNode *Node) const {
  auto Iter = NewToOldMapping.find(Node);
  return Iter != NewToOldMapping.cend() ? Iter->second : nullptr;
}

bool Framework::areContained(VirtualNode *OldNode, VirtualNode *NewNode) const {
  auto Temp = findNewToOldMapping(NewNode);
  return Temp == OldNode;
}

void Framework::insertMapping(VirtualNode *OldNode, VirtualNode *NewNode) {
  OldToNewMapping.emplace(OldNode, NewNode);
  NewToOldMapping.emplace(NewNode, OldNode);
}

void Framework::overrideMapping(VirtualNode *OldNode, VirtualNode *NewNode) {
  auto AltOld = findNewToOldMapping(NewNode);
  auto AltNew = findOldToNewMapping(OldNode);
  if (AltOld && AltOld != OldNode)
    OldToNewMapping.erase(AltOld);
  if (AltNew && AltNew != NewNode)
    NewToOldMapping.erase(AltNew);
  OldToNewMapping[OldNode] = NewNode;
  NewToOldMapping[NewNode] = OldNode;
}

void Framework::eraseOldMapping(VirtualNode *OldNode) {
  auto NewNode = findOldToNewMapping(OldNode);
  if (NewNode) {
    OldToNewMapping.erase(OldNode);
    NewToOldMapping.erase(NewNode);
  }
}

void Framework::eraseNewMapping(VirtualNode *NewNode) {
  auto OldNode = findNewToOldMapping(NewNode);
  if (OldNode) {
    NewToOldMapping.erase(NewNode);
    OldToNewMapping.erase(OldNode);
  }
}

void Framework::swapMapping(VirtualNode *OldNode, VirtualNode *AltOldNode) {
  auto NewNode = findOldToNewMapping(OldNode);
  auto AltNewNode = findOldToNewMapping(AltOldNode);
  if (NewNode && AltNewNode) {
    OldToNewMapping[OldNode] = AltNewNode;
    OldToNewMapping[AltOldNode] = NewNode;
    NewToOldMapping[NewNode] = AltOldNode;
    NewToOldMapping[AltNewNode] = OldNode;
  } else if (NewNode /* AltNewNode == nullptr */) {
    OldToNewMapping[AltOldNode] = NewNode;
    NewToOldMapping[NewNode] = AltOldNode;
    OldToNewMapping.erase(OldNode);
  } else if (AltNewNode /* NewNode == nullptr */) {
    OldToNewMapping[OldNode] = AltNewNode;
    NewToOldMapping[AltNewNode] = OldNode;
    OldToNewMapping.erase(AltOldNode);
  }
}

std::pair<Framework::Mappings, EditScript>
Framework::run(Matcher *Framework, const MerkleTree &Old,
               const MerkleTree &New) {
  TreeDiff Diff;
  if (New.isIncparsed()) {
    Diff.IsIncparsed = true;
    Diff.incbuild(Old, Diff.OldTree);
    Diff.incbuild(New, Diff.NewTree);
    Diff.matchCommons(New);
    Framework->match(Diff);
  } else {
    Diff.OldTree.build(Old.getRoot());
    Diff.NewTree.build(New.getRoot());
    Framework->match(Diff);
  }

  Mappings Map;
  Map.reserve(Diff.OldToNewMapping.size());
  for (const auto [OldNode, NewNode] : Diff.OldToNewMapping)
    Map.emplace_back(&OldNode->Original, &NewNode->Original);
  return {std::move(Map), Diff.makeEditScript()};
}

} // namespace diffink