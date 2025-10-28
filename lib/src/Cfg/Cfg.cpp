#include "ZigCFG/Cfg/Cfg.h"

namespace zigcfg {

Cfg::Cfg(const std::string &Src) {
  TSParser *Parser = ts_parser_new();
  ts_parser_set_language(Parser, tree_sitter_zig());
  Tree = ts_parser_parse_string(Parser, nullptr, Src.c_str(), Src.size());
  ts_parser_delete(Parser);

  SrcPtr = Src.c_str();
  auto Cursor = ts_tree_cursor_new(ts_tree_root_node(Tree));
  if (ts_tree_cursor_goto_first_child(&Cursor))
    findFunctions(Cursor);
  ts_tree_cursor_delete(&Cursor);
  for (auto &Iter : Functions)
    buildFunction(Iter);
}

Cfg::~Cfg() { ts_tree_delete(Tree); }

void Cfg::clear() {
  while (!ScopeStack.empty())
    ScopeStack.pop();
  while (!LoopStack.empty())
    LoopStack.pop();
  VarScopes.clear();
}

void Cfg::findFunctions(TSTreeCursor &Cursor) {
  if (equalNodeType(Cursor, "FnProto")) {
    auto EntryNode = ts_tree_cursor_current_node(&Cursor);
    ts_tree_cursor_goto_next_sibling(&Cursor);
    if (equalNodeType(Cursor, "BasicBlock"))
      Functions.push_back({"", EntryNode, addBasicBlock(NopCmd{}),
                           addBasicBlock(NopCmd{}), addBasicBlock(NopCmd{})});
  }

  if (ts_tree_cursor_goto_first_child(&Cursor)) {
    do
      findFunctions(Cursor);
    while (ts_tree_cursor_goto_next_sibling(&Cursor));
    ts_tree_cursor_goto_parent(&Cursor);
  }
}

void Cfg::buildFunction(Function &Func) {
  Predecessor = Func.Entry;
  ScopeStack.push(Func.Entry);
  VarScopes.push_back({});
  auto Cursor = ts_tree_cursor_new(Func.RawNode);
  ts_tree_cursor_goto_first_child(&Cursor);

  while (!equalNodeType(Cursor, "ParamDeclList"))
    ts_tree_cursor_goto_next_sibling(&Cursor);
  ts_tree_cursor_goto_first_child(&Cursor);
  while (ts_tree_cursor_goto_next_sibling(&Cursor))
    if (equalNodeType(Cursor, "ParamDecl")) {
      ts_tree_cursor_goto_first_child(&Cursor);
      do
        if (equalNodeType(Cursor, "IDENTIFIER"))
          addVar(makeVarName(Cursor));
      while (ts_tree_cursor_goto_next_sibling(&Cursor));
      ts_tree_cursor_goto_parent(&Cursor);
    }
  ts_tree_cursor_goto_parent(&Cursor);

  ts_tree_cursor_goto_parent(&Cursor);
  ts_tree_cursor_goto_next_sibling(&Cursor);
  buildBasicBlock(Func, Cursor);
  ts_tree_cursor_delete(&Cursor);
}

void Cfg::buildBasicBlock(Function &Func, TSTreeCursor &Cursor) {
  ts_tree_cursor_goto_first_child(&Cursor);
  ts_tree_cursor_goto_next_sibling(&Cursor);
  while (equalNodeType(Cursor, "Statement")) {
    buildStatement(Func, Cursor);
    ts_tree_cursor_goto_next_sibling(&Cursor);
  }
  ts_tree_cursor_goto_parent(&Cursor);

  auto BasicBlockExit = addBasicBlock(NopCmd{});
  BasicBlocks[Predecessor].Successors.push_back(BasicBlockExit);
  Predecessor = BasicBlockExit;
}

BasicBlockId Cfg::addBasicBlock(Command Cmd) {
  BasicBlockId NewId = BasicBlocks.size();
  BasicBlocks.emplace_back(BasicBlock(Cmd, NewId));
  return NewId;
}

VarId Cfg::addVar(const std::string &Name) {
  VarId NewId = VarNames.size();
  VarNames.push_back(Name);
  if (!Name.empty())
    VarScopes.back()[Name] = NewId;
  return NewId;
}

VarId Cfg::getVar(const std::string &Name) {
  for (auto &Scope : VarScopes) {
    auto It = Scope.find(Name);
    if (It != Scope.end())
      return It->second;
  }
  auto It = GlobalVars.find(Name);
  if (It != GlobalVars.end())
    return It->second;

  VarId NewId = VarNames.size();
  VarNames.push_back(Name);
  GlobalVars[Name] = NewId;
  return NewId;
}

std::string Cfg::makeVarName(TSTreeCursor &Cursor) {
  TSNode Node = ts_tree_cursor_current_node(&Cursor);
  uint32_t StartByte = ts_node_start_byte(Node);
  uint32_t EndByte = ts_node_end_byte(Node);
  return std::string(SrcPtr + StartByte, EndByte - StartByte);
}

bool Cfg::equalNodeType(TSTreeCursor &Cursor, const char *Type) {
  return ts_node_type(ts_tree_cursor_current_node(&Cursor)) ==
         std::string(Type);
}

bool Cfg::equalFieldName(TSTreeCursor &Cursor, const char *Name) {
  return ts_tree_cursor_current_field_name(&Cursor) == std::string(Name);
}

} // namespace zigcfg
