#ifndef ZIGCFG_CFG_BLOCK_H
#define ZIGCFG_CFG_BLOCK_H

#include <cstdint>
#include <variant>
#include <vector>

namespace zigcfg {

using VarId = uint32_t;
using BlockId = uint32_t;

struct VarExpr {
  VarId Var;
};

struct DerefExpr {
  VarId Var;
};

struct NullExpr {};

struct UnknownExpr {};

using Expression = std::variant<VarExpr, DerefExpr, NullExpr, UnknownExpr>;

struct SetCmd {
  Expression Lhs;
  Expression Rhs;
};

struct AllocCmd {
  Expression Expr;
};

struct FreeCmd {
  Expression Expr;
};

struct AssertEqualCmd {
  Expression Lhs;
  Expression Rhs;
};

struct AssertNotEqualCmd {
  Expression Lhs;
  Expression Rhs;
};

using Command =
    std::variant<SetCmd, AllocCmd, FreeCmd, AssertEqualCmd, AssertNotEqualCmd>;

struct Block {
  Command Cmd;
  BlockId Id;
  std::vector<BlockId> Successors;
  std::vector<BlockId> Predecessors;

  uint32_t StartOffset;
  uint32_t EndOffset;
  uint32_t StartRow;
  uint32_t StartCol;
  uint32_t EndRow;
  uint32_t EndCol;
};

} // namespace zigcfg

#endif // ZIGCFG_CFG_BLOCK_H