#include "DiffInk/SyntaxTree/HashNode.h"

namespace diffink {

std::string HashNode::UTF8Range::stringify() const {
  return std::format("({},{})-({},{})", StartPos.row + 1, StartPos.column + 1,
                     EndPos.row + 1, EndPos.column + 1);
}

HashNode::HashNode(const TSNode &RawNode, std::string &&Type,
                   const SourceCode &Code, bool IsLeaf) noexcept
    : Type(std::move(Type)),
      ByteRange{ts_node_start_byte(RawNode), ts_node_end_byte(RawNode)},
      Label(IsLeaf ? Code.getSubstring(ts_node_start_byte(RawNode),
                                       ts_node_end_byte(RawNode))
                   : ""),
      PosRange(UTF8Range{Code.getUTF8Position(ts_node_start_byte(RawNode)),
                         Code.getUTF8Position(ts_node_end_byte(RawNode))}) {}

std::vector<const HashNode *> HashNode::makePostOrder() const {
  std::vector<const HashNode *> PostOrder;
  makePostOrder(PostOrder, this);
  return PostOrder;
}

void HashNode::stringifySubtree(std::string &Buffer, std::size_t Depth,
                                std::size_t Indent) const {
  Buffer.append(Depth * Indent, ' ').append(stringifyNode()).push_back('\n');
  for (auto &Child : Children)
    Child.stringifySubtree(Buffer, Depth + 1, Indent);
}

void HashNode::makeMetadata() {
  Height = 0;
  auto TypeHash = getTypeHash();
  if (isLeaf()) {
    ExactHash = xxhVector64({TypeHash, xxhString64(Label)});
    StructuralHash = TypeHash;
    return;
  }

  std::vector<XXH64_hash_t> ExactHashes, StructuralHashes;
  ExactHashes.reserve(Children.size());
  StructuralHashes.reserve(Children.size());
  for (auto &Child : Children) {
    Child.makeMetadata();
    Height = std::max(Height, Child.Height + 1);
    ExactHashes.push_back(Child.ExactHash);
    StructuralHashes.push_back(Child.StructuralHash);
  }
  ExactHash = xxhVector64({TypeHash, xxhVector64(ExactHashes)});
  StructuralHash = xxhVector64({TypeHash, xxhVector64(StructuralHashes)});
}

void HashNode::makePostOrder(std::vector<const HashNode *> &PostOrder,
                             const HashNode *Iter) const {
  for (const auto &Child : Iter->Children)
    makePostOrder(PostOrder, &Child);
  PostOrder.push_back(Iter);
}

void HashNode::build(const SourceCode &Code, TSTreeCursor &Cursor,
                     HashNode &Parent) {
  auto Node = ts_tree_cursor_current_node(&Cursor);
  if (ts_node_is_missing(Node))
    return;
  std::string NodeType{ts_node_type(Node)};

  if (!ts_tree_cursor_goto_first_child(&Cursor))
    Parent.Children.emplace_back(Node, std::move(NodeType), Code, true);
  else {
    auto &CurNode =
        Parent.Children.emplace_back(Node, std::move(NodeType), Code, false);
    do
      build(Code, Cursor, CurNode);
    while (ts_tree_cursor_goto_next_sibling(&Cursor));
    ts_tree_cursor_goto_parent(&Cursor);
  }
}

void HashNode::build(const SourceCode &Code, TSTreeCursor &Cursor,
                     HashNode &Parent, const BuildConfig &Config) {
  auto Node = ts_tree_cursor_current_node(&Cursor);
  std::string NodeType{ts_node_type(Node)};
  if (ts_node_is_missing(Node) || Config.Ignored.contains(NodeType))
    return;
  if (auto It = Config.Aliased.find(NodeType); It != Config.Aliased.end())
    NodeType = It->second;

  if (Config.Flattened.contains(NodeType))
    Parent.Children.emplace_back(Node, std::move(NodeType), Code, true);
  else if (!ts_tree_cursor_goto_first_child(&Cursor))
    Parent.Children.emplace_back(Node, std::move(NodeType), Code, true);
  else {
    auto &CurNode =
        Parent.Children.emplace_back(Node, std::move(NodeType), Code, false);
    do
      build(Code, Cursor, CurNode);
    while (ts_tree_cursor_goto_next_sibling(&Cursor));
    ts_tree_cursor_goto_parent(&Cursor);
  }
}

std::string HashNode::stringifyNode() const {
  if (Label.empty())
    return std::format("{} {}", Type, PosRange.stringify());
  return std::format("{}: '{}' {}", Type, Label, PosRange.stringify());
}

std::string HashNode::stringifySubtree(std::size_t Indent) const {
  std::string Buffer;
  stringifySubtree(Buffer, 0, Indent);
  Buffer.pop_back();
  return Buffer;
}

std::unique_ptr<HashNode> HashNode::build(TSNode RootNode,
                                          const SourceCode &Code,
                                          const BuildConfig *Config) {
  if (ts_node_is_null(RootNode))
    return nullptr;
  auto Root =
      std::make_unique<HashNode>(RootNode, ts_node_type(RootNode), Code, false);
  auto Cursor = ts_tree_cursor_new(RootNode);
  if (ts_tree_cursor_goto_first_child(&Cursor))
    do
      Config ? build(Code, Cursor, *Root, *Config) : build(Code, Cursor, *Root);
    while (ts_tree_cursor_goto_next_sibling(&Cursor));
  ts_tree_cursor_delete(&Cursor);
  Root->makeMetadata();
  return Root;
}

} // namespace diffink