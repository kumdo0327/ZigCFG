#include "DiffInk/SyntaxTree/ModifiedTree.h"

namespace diffink {

void ModifiedTree::identifyCommons(OriginalTree &OldTree, Iterator Iters) {
  auto insertStructuralChange =
      [](OriginalTree &Tree, const HashNode &Iter,
         std::list<HashNode>::const_iterator Child) -> void {
    Tree.refUncommons().insert(&*Child);
    Tree.HasUncommonChild.insert(&Iter);
  };
  auto detectUncommonChild = [this, &OldTree](const HashNode *OldIter,
                                              const HashNode *NewIter) {
    OldTree.HasUncommonChild.insert(OldIter);
    HasUncommonChild.insert(NewIter);
  };

  bool HasEditedChild = ts_tree_cursor_goto_first_child(&Iters.OldCursor);
  bool HasNewChild = ts_tree_cursor_goto_first_child(&Iters.NewCursor);
  if (!HasEditedChild && !HasNewChild)
    return;

  if (HasEditedChild && HasNewChild) {
    for (auto OldHashChild = Iters.OldHashIter.getChildren().cbegin(),
              NewHashChild = Iters.NewHashIter.getChildren().cbegin(),
              OldHashEnd = Iters.OldHashIter.getChildren().cend(),
              NewHashEnd = Iters.NewHashIter.getChildren().cend();
         ;) {

      if (OldHashChild == OldHashEnd) {
        for (; NewHashChild != NewHashEnd; ++NewHashChild)
          refUncommons().insert(&*NewHashChild);
        detectUncommonChild(&Iters.OldHashIter, &Iters.NewHashIter);
        break;
      }

      if (NewHashChild == NewHashEnd) {
        for (; OldHashChild != OldHashEnd; ++OldHashChild)
          OldTree.refUncommons().insert(&*OldHashChild);
        detectUncommonChild(&Iters.OldHashIter, &Iters.NewHashIter);
        break;
      }

      auto OldRawNode = ts_tree_cursor_current_node(&Iters.OldCursor);
      while (ts_node_is_missing(OldRawNode)) {
        ts_tree_cursor_goto_next_sibling(&Iters.OldCursor);
        OldRawNode = ts_tree_cursor_current_node(&Iters.OldCursor);
      }
      auto NewRawNode = ts_tree_cursor_current_node(&Iters.NewCursor);
      while (ts_node_is_missing(NewRawNode)) {
        ts_tree_cursor_goto_next_sibling(&Iters.NewCursor);
        NewRawNode = ts_tree_cursor_current_node(&Iters.NewCursor);
      }

      switch (compareNodes(OldRawNode, NewRawNode)) {
      case NodeComp::Equal:
        if (OldHashChild->getType() == NewHashChild->getType()) {
          if (HashNode::equalExactly(*OldHashChild, *NewHashChild))
            Mappings.emplace_back(&*OldHashChild, &*NewHashChild);
          else {
            identifyCommons(OldTree, {*OldHashChild, Iters.OldCursor,
                                      *NewHashChild, Iters.NewCursor});
            bool AreChildrenUncommon = !OldHashChild->getChildren().empty();
            for (auto &Child : OldHashChild->getChildren())
              if (!OldTree.isUncommon(Child)) {
                AreChildrenUncommon = false;
                break;
              }
            if (AreChildrenUncommon) {
              for (auto &Child : OldHashChild->getChildren())
                OldTree.refUncommons().erase(&Child);
              for (auto &Child : NewHashChild->getChildren())
                refUncommons().erase(&Child);
              insertStructuralChange(OldTree, Iters.OldHashIter, OldHashChild);
              insertStructuralChange(*this, Iters.NewHashIter, NewHashChild);
            } else {
              Mappings.emplace_back(&*OldHashChild, &*NewHashChild);
              detectUncommonChild(&Iters.OldHashIter, &Iters.NewHashIter);
            }
          }
        } else {
          insertStructuralChange(OldTree, Iters.OldHashIter, OldHashChild);
          insertStructuralChange(*this, Iters.NewHashIter, NewHashChild);
        }

        ++OldHashChild;
        ++NewHashChild;
        ts_tree_cursor_goto_next_sibling(&Iters.OldCursor);
        ts_tree_cursor_goto_next_sibling(&Iters.NewCursor);
        break;

      case NodeComp::Inequal:
        if (HashNode::equalExactly(*OldHashChild, *NewHashChild))
          Mappings.emplace_back(&*OldHashChild, &*NewHashChild);
        else {
          insertStructuralChange(OldTree, Iters.OldHashIter, OldHashChild);
          insertStructuralChange(*this, Iters.NewHashIter, NewHashChild);
        }

        ++OldHashChild;
        ++NewHashChild;
        ts_tree_cursor_goto_next_sibling(&Iters.OldCursor);
        ts_tree_cursor_goto_next_sibling(&Iters.NewCursor);
        break;

      case NodeComp::Precedes:
        insertStructuralChange(OldTree, Iters.OldHashIter, OldHashChild);
        HasUncommonChild.insert(&Iters.NewHashIter);
        ++OldHashChild;
        ts_tree_cursor_goto_next_sibling(&Iters.OldCursor);
        break;

      case NodeComp::Succeeds:
        insertStructuralChange(*this, Iters.NewHashIter, NewHashChild);
        OldTree.HasUncommonChild.insert(&Iters.OldHashIter);
        ++NewHashChild;
        ts_tree_cursor_goto_next_sibling(&Iters.NewCursor);
      }
    }

    ts_tree_cursor_goto_parent(&Iters.OldCursor);
    ts_tree_cursor_goto_parent(&Iters.NewCursor);
    return;
  }

  if (HasEditedChild) {
    for (auto &Child : Iters.OldHashIter.getChildren())
      OldTree.refUncommons().insert(&Child);
    detectUncommonChild(&Iters.OldHashIter, &Iters.NewHashIter);
    ts_tree_cursor_goto_parent(&Iters.OldCursor);
  }

  else /* HasNewChild */ {
    for (auto &Child : Iters.NewHashIter.getChildren())
      refUncommons().insert(&Child);
    detectUncommonChild(&Iters.OldHashIter, &Iters.NewHashIter);
    ts_tree_cursor_goto_parent(&Iters.NewCursor);
  }
}

void ModifiedTree::identifyCommons(OriginalTree &OldTree,
                                   TSTreeWrapper &EditedTree) {
  if (!EditedTree)
    return;
  auto OldCursor = ts_tree_cursor_new(ts_tree_root_node(EditedTree.get()));
  auto NewCursor = ts_tree_cursor_new(ts_tree_root_node(&getRawTree()));
  Mappings.emplace_back(&OldTree.getRoot(), &getRoot());
  identifyCommons(OldTree,
                  {OldTree.getRoot(), OldCursor, getRoot(), NewCursor});
  ts_tree_cursor_delete(&OldCursor);
  ts_tree_cursor_delete(&NewCursor);
}

} // namespace diffink