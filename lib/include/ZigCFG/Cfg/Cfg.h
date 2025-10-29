#ifndef ZIGCFG_CFG_CFG_H
#define ZIGCFG_CFG_CFG_H

#include "ZigCFG/Cfg/Elements.h"
#include <list>
#include <stack>
#include <tree-sitter-zig.h>
#include <tree_sitter/api.h>
#include <unordered_map>

namespace zigcfg {

class Cfg {
private:
  TSTree *Tree;
  std::vector<Function> Functions;
  std::vector<BasicBlock> BasicBlocks;
  std::vector<std::string> VarNames;

  const char *SrcPtr;
  BasicBlockId Predecessor;
  bool IsPredJump;
  std::stack<BasicBlockId> ScopeStack;
  std::stack<BasicBlockId> LoopStack;
  std::list<std::unordered_map<std::string, VarId>> VarScopes;
  std::unordered_map<std::string, VarId> GlobalVars;

private:
  void clear();

  void findFunctions(TSTreeCursor &Cursor);

  void buildFunction(Function &Func);

  void buildBlock(Function &Func, TSTreeCursor &Cursor);

  void buildStatement(Function &Func, TSTreeCursor &Cursor);

  void buildIfStmt(Function &Func, TSTreeCursor &Cursor);

  void buildLabeledStmt(Function &Func, TSTreeCursor &Cursor);

  void buildSwitchStmt(Function &Func, TSTreeCursor &Cursor);

  void buildDeferExprStmt(Function &Func, TSTreeCursor &Cursor);

  void buildErrDeferExprStmt(Function &Func, TSTreeCursor &Cursor);

  void buildBlockExprStmt(Function &Func, TSTreeCursor &Cursor);

  void buildAssignExpr(Function &Func, TSTreeCursor &Cursor);

  void buildBlockExpr(Function &Func, TSTreeCursor &Cursor);

  BasicBlockId addBasicBlock(Command Cmd);

  VarId addVar(const std::string &Name);

  VarId getVar(const std::string &Name);

  std::string makeVarName(TSTreeCursor &Cursor);

  static bool equalNodeType(TSTreeCursor &Cursor, const char *Type);

  static bool equalFieldName(TSTreeCursor &Cursor, const char *Name);

public:
  Cfg(const std::string &Src);

  ~Cfg();
};

} // namespace zigcfg

#endif // ZIGCFG_CFG_CFG_H