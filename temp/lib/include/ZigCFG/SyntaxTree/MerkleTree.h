#ifndef DIFFINK_AST_MERKLETREE_H
#define DIFFINK_AST_MERKLETREE_H

#include "DiffInk/SyntaxTree/HashNode.h"
#include "DiffInk/Utils/Wrapper.h"

namespace diffink {

class MerkleTree {
private:
  std::unique_ptr<HashNode> Root;
  TSTreeWrapper RawTree;
  std::shared_ptr<BuildConfig> Config;
  std::vector<std::pair<const HashNode *, const HashNode *>> Mappings;
  std::unordered_set<const HashNode *> Uncommons;
  std::unordered_set<const HashNode *> HasUncommonChild;

protected:
  void clearMetadata() noexcept;

public:
  MerkleTree() noexcept : RawTree(nullptr, ts_tree_delete) {}

  void swap(MerkleTree &Rhs) noexcept;

  void parse(TSParser &Parser, const SourceCode &Code,
             const TSTree *Tree = nullptr);

  void setBuildConfig(std::shared_ptr<BuildConfig> Config) noexcept {
    this->Config = std::move(Config);
  }

  const TSTree &getRawTree() const noexcept { return *RawTree; }

  const HashNode &getRoot() const noexcept { return *Root; }

  const decltype(Mappings) &getMappings() const noexcept { return Mappings; }

  bool isUncommon(const HashNode &Node) const {
    return Uncommons.contains(&Node);
  }

  bool hasUncommonChild(const HashNode &Node) const {
    return HasUncommonChild.contains(&Node);
  }
};

} // namespace diffink

#endif // DIFFINK_AST_MERKLETREE_H