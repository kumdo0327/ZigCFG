#ifndef DIFFINK_TREEDIFF_DIFFINKFRAMEWORK_H
#define DIFFINK_TREEDIFF_DIFFINKFRAMEWORK_H

#include "DiffInk/TreeDiff/Framework.h"

namespace diffink {

class DiffInkFramework {
public:
  class Matcher {
  public:
    virtual ~Matcher() = default;
    virtual void match(DiffInkFramework &Diff) = 0;
  };

  class Recovery {
  public:
    virtual ~Recovery() = default;
    virtual void match(DiffInkFramework &Diff, VirtualNode *Old,
                       VirtualNode *New) = 0;
  };

  using Mappings = std::vector<std::pair<const HashNode *, const HashNode *>>;

private:
  VirtualTree OldTree;
  VirtualTree NewTree;
  std::unordered_map<VirtualNode *, VirtualNode *> OldToNewMapping;
  std::unordered_map<VirtualNode *, VirtualNode *> NewToOldMapping;
  std::unordered_map<const HashNode *, VirtualNode *> Copies;
  bool IsIncparsed{false};

private:
  DiffInkFramework() noexcept = default;

  ~DiffInkFramework() = default;

  void incbuild(const MerkleTree &OriginalTree, VirtualTree &TreeCopy);

  void _incbuild(const MerkleTree &OriginalTree, VirtualTree &TreeCopy,
                 const HashNode &OriginalNode, VirtualNode *NodeCopy);

  void matchCommons(const MerkleTree &New);

  std::size_t findPosition(VirtualNode *Node) const;

public:
  EditScript makeEditScript();

  VirtualNode *findOldToNewMapping(VirtualNode *Node) const;

  VirtualNode *findNewToOldMapping(VirtualNode *Node) const;

  VirtualNode *getOldRoot() const noexcept { return OldTree.getRoot(); }

  VirtualNode *getNewRoot() const noexcept { return NewTree.getRoot(); }

  bool isIncparsed() const noexcept { return IsIncparsed; }

  bool areContained(VirtualNode *OldNode, VirtualNode *NewNode) const;

  void buildOldSubtree(VirtualNode *Root) { OldTree.buildSubtree(Root); }

  void buildNewSubtree(VirtualNode *Root) { NewTree.buildSubtree(Root); }

  void insertMapping(VirtualNode *OldNode, VirtualNode *NewNode);

  void overrideMapping(VirtualNode *OldNode, VirtualNode *NewNode);

  void eraseOldMapping(VirtualNode *OldNode);

  void eraseNewMapping(VirtualNode *NewNode);

  void swapMapping(VirtualNode *OldNode, VirtualNode *AltOldNode);

  static std::pair<Mappings, EditScript>
  run(Matcher *Matcher, const MerkleTree &Old, const MerkleTree &New);
};

} // namespace diffink

#endif // DIFFINK_TREEDIFF_DIFFINKFRAMEWORK_H