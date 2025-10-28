#include "DiffInk/SyntaxTree/MerkleTree.h"

namespace diffink {

void MerkleTree::clearMetadata() noexcept {
  Mappings.clear();
  decltype(Mappings)().swap(Mappings);
  Uncommons.clear();
  decltype(Uncommons)().swap(Uncommons);
  HasUncommonChild.clear();
  decltype(HasUncommonChild)().swap(HasUncommonChild);
}

void MerkleTree::swap(MerkleTree &Rhs) noexcept {
  Root.swap(Rhs.Root);
  RawTree.swap(Rhs.RawTree);
  clearMetadata();
}

void MerkleTree::parse(TSParser &Parser, const SourceCode &Code,
                       const TSTree *Tree) {
  clearMetadata();
  RawTree.reset(
      ts_parser_parse_string(&Parser, Tree, Code.getContent(), Code.getSize()));
  Root = HashNode::build(ts_tree_root_node(RawTree.get()), Code, Config.get());
}

} // namespace diffink