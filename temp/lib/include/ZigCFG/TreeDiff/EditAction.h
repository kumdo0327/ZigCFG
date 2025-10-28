#ifndef DIFFINK_TREEDIFF_EDITACTION_H
#define DIFFINK_TREEDIFF_EDITACTION_H

#include "DiffInk/SyntaxTree/HashNode.h"
#include <variant>

namespace diffink {

namespace edit_action {

struct InsertNode {
  const HashNode &Leaf;
  const HashNode &Parent;
  std::size_t Index;
};

struct DeleteNode {
  const HashNode &Leaf;
  const HashNode &Parent;
};

struct MoveTree {
  const HashNode &Subtree;
  const HashNode &Parent;
  std::size_t Index;
  const HashNode &MovedSubtree;
};

struct UpdateNode {
  const HashNode &Leaf;
  const HashNode &UpdatedLeaf;
};

struct InsertTree {
  const HashNode &Subtree;
  const HashNode &Parent;
  std::size_t Index;
};

struct DeleteTree {
  const HashNode &Subtree;
};

} // namespace edit_action

using EditAction =
    std::variant<edit_action::InsertNode, edit_action::DeleteNode,
                 edit_action::MoveTree, edit_action::UpdateNode>;

using ExtendedEditAction =
    std::variant<edit_action::InsertNode, edit_action::DeleteNode,
                 edit_action::MoveTree, edit_action::UpdateNode,
                 edit_action::InsertTree, edit_action::DeleteTree>;

using EditScript = std::vector<EditAction>;

using ExtendedEditScript = std::vector<ExtendedEditAction>;

ExtendedEditScript simplifyEditScript(const EditScript &Script);

} // namespace diffink

#endif // DIFFINK_TREEDIFF_EDITACTION_H