#include "ZigCFG/Cfg/Cfg.h"

namespace zigcfg {

Cfg::Cfg(const std::string &Src) {
  TSParser *Parser = ts_parser_new();
  ts_parser_set_language(Parser, tree_sitter_zig());
  Tree = ts_parser_parse_string(Parser, nullptr, Src.c_str(), Src.size());
  ts_parser_delete(Parser);

  auto Cursor = ts_tree_cursor_new(ts_tree_root_node(Tree));
  std::stack<BlockId> ScopeStack, LoopStack;
  build(Cursor, ScopeStack, LoopStack);
  ts_tree_cursor_delete(&Cursor);
}

Cfg::~Cfg() { ts_tree_delete(Tree); }

void Cfg::build(TSTreeCursor &Cursor, std::stack<BlockId> &ScopeStack,
                std::stack<BlockId> &LoopStack) {
  auto CurrentNode = ts_tree_cursor_current_node(&Cursor);
}

} // namespace zigcfg
