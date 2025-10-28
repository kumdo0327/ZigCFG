#include "DiffInk/Utils/NodeComp.h"

namespace diffink {

NodeComp compareNodes(const TSNode &LeftNode, const TSNode &RightNode) {
  auto LeftStart = ts_node_start_byte(LeftNode);
  auto LeftEnd = ts_node_end_byte(LeftNode);
  // if (LeftStart == LeftEnd)
  //   return NodeComp::Precedes;
  auto RightStart = ts_node_start_byte(RightNode);
  auto RightEnd = ts_node_end_byte(RightNode);

  if (LeftStart == RightStart) {
    if (LeftEnd == RightEnd)
      return NodeComp::Equal;
    else
      return NodeComp::Inequal;
  }
  if (LeftEnd < RightStart)
    return NodeComp::Precedes;
  if (LeftStart > RightEnd)
    return NodeComp::Succeeds;
  return NodeComp::Inequal;
}

std::string toString(TSTree &Tree) {
  TSNode Root = ts_tree_root_node(&Tree);
  return toString(Root);
}

std::string toString(TSNode &Node, std::size_t Indent) {
  std::string Buffer(Indent * 2, ' ');
  Buffer.append(ts_node_type(Node)).append(" (");
  Buffer.append(std::to_string(ts_node_start_byte(Node))).append(", ");
  Buffer.append(std::to_string(ts_node_end_byte(Node))).append(")\n");

  auto ChildCount = ts_node_child_count(Node);
  for (uint32_t i = 0; i < ChildCount; ++i) {
    TSNode Child = ts_node_child(Node, i);
    Buffer.append(toString(Child, Indent + 1));
  }
  return Buffer;
}

} // namespace diffink