#ifndef DIFFINK_UTILS_NODECOMP_H
#define DIFFINK_UTILS_NODECOMP_H

#include <string>
#include <tree_sitter/api.h>

namespace diffink {

enum class NodeComp {
  Equal,
  Inequal,
  Precedes,
  Succeeds,
};

NodeComp compareNodes(const TSNode &LeftNode, const TSNode &RightNode);

std::string toString(TSTree &Tree);

std::string toString(TSNode &Node, std::size_t Indent = 0);

} // namespace diffink

#endif // DIFFINK_UTILS_NODECOMP_H