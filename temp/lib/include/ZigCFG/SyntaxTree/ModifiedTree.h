#ifndef DIFFINK_SYNTAXTREE_MERKLETREE_H
#define DIFFINK_SYNTAXTREE_MERKLETREE_H

#include "DiffInk/SyntaxTree/MerkleTree.h"
#include "DiffInk/Utils/NodeComp.h"

namespace diffink {

class ModifiedTree : public OriginalTree {
private:
  struct Iterator {
    const HashNode &OldHashIter;
    TSTreeCursor &OldCursor;
    const HashNode &NewHashIter;
    TSTreeCursor &NewCursor;
  };

private:
  void identifyCommons(OriginalTree &OldTree, Iterator Iters);

public:
  void identifyCommons(OriginalTree &OldTree, TSTreeWrapper &EditedTree);

  bool appliedDiffInk() const noexcept { return Mappings.size(); }
};

} // namespace diffink

#endif // DIFFINK_SYNTAXTREE_MERKLETREE_H