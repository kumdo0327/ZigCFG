#ifndef ZIGCFG_CFG_CFG_H
#define ZIGCFG_CFG_CFG_H

#include "ZigCFG/Cfg/Block.h"
#include <stack>
#include <string>
#include <tree-sitter-zig.h>
#include <tree_sitter/api.h>

namespace zigcfg {

class Cfg {
private:
  TSTree *Tree;
  std::vector<Block> Blocks;

private:
  void build(TSTreeCursor &Cursor, std::stack<BlockId> &ScopeStack,
             std::stack<BlockId> &LoopStack);

  void linkCalls();

public:
  Cfg(const std::string &Src);

  ~Cfg();
};

} // namespace zigcfg

#endif // ZIGCFG_CFG_CFG_H