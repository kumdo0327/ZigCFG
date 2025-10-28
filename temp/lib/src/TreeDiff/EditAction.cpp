#include "DiffInk/TreeDiff/EditAction.h"

namespace diffink {

ExtendedEditScript simplifyEditScript(const EditScript &Script) {
  ExtendedEditScript ExScript;
  std::unordered_set<const HashNode *> InsertedNodes;
  std::unordered_set<const HashNode *> DeletedNodes;
  ExScript.reserve(Script.size());
  InsertedNodes.reserve(Script.size());
  DeletedNodes.reserve(Script.size());

  std::unordered_map<const HashNode *, std::vector<const HashNode *>> Cache;

  auto makePostOrder =
      [&Cache](const HashNode &Node) -> const std::vector<const HashNode *> & {
    auto Iter = Cache.find(&Node);
    if (Iter != Cache.cend())
      return Iter->second;
    return Cache.emplace(&Node, Node.makePostOrder()).first->second;
  };

  auto detectAggregation =
      [](const std::vector<const HashNode *> &Descendants,
         std::unordered_set<const HashNode *> &Set) -> bool {
    for (const auto Node : Descendants)
      if (!Set.contains(Node))
        return false;
    return true;
  };

  for (const auto &Action : Script) {
    std::visit(
        [&](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;
          if constexpr (std::is_same_v<T, edit_action::InsertNode>)
            InsertedNodes.insert(&Action.Leaf);
          else if constexpr (std::is_same_v<T, edit_action::DeleteNode>)
            DeletedNodes.insert(&Action.Leaf);
        },
        Action);
  }

  for (const auto &Action : Script) {
    std::visit(
        [&](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;

          if constexpr (std::is_same_v<T, edit_action::InsertNode>) {
            auto Descendants = makePostOrder(Action.Leaf);
            if (detectAggregation(Descendants, InsertedNodes)) {
              if (!InsertedNodes.contains(&Action.Parent) ||
                  !detectAggregation(makePostOrder(Action.Parent),
                                     InsertedNodes))
                if (Action.Leaf.isLeaf())
                  ExScript.push_back(Action);
                else
                  ExScript.push_back(
                      edit_action::InsertTree{.Subtree = Action.Leaf,
                                              .Parent = Action.Parent,
                                              .Index = Action.Index});
            } else
              ExScript.push_back(Action);
          }

          else if constexpr (std::is_same_v<T, edit_action::DeleteNode>) {
            auto Descendants = makePostOrder(Action.Leaf);
            if (detectAggregation(Descendants, DeletedNodes)) {
              if (!DeletedNodes.contains(&Action.Parent) ||
                  !detectAggregation(makePostOrder(Action.Parent),
                                     DeletedNodes))
                if (Action.Leaf.isLeaf())
                  ExScript.push_back(Action);
                else
                  ExScript.push_back(
                      edit_action::DeleteTree{.Subtree = Action.Leaf});
            } else
              ExScript.push_back(Action);
          } else
            ExScript.emplace_back(Action);
        },
        Action);
  }

  return ExScript;
}

} // namespace diffink