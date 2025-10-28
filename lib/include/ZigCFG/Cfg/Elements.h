#ifndef ZIGCFG_CFG_ELEMENTS_H
#define ZIGCFG_CFG_ELEMENTS_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace zigcfg {

using VarId = uint32_t;
using BasicBlockId = uint32_t;

struct VarExpr {
  VarId Var;
};

struct DerefExpr {
  VarId Var;
};

struct NullExpr {};

struct UnknownExpr {};

using Expression = std::variant<VarExpr, DerefExpr, NullExpr, UnknownExpr>;

struct NopCmd {};

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

struct AssertEqCmd {
  Expression Lhs;
  Expression Rhs;
};

struct AssertNEqCmd {
  Expression Lhs;
  Expression Rhs;
};

using Command =
    std::variant<NopCmd, SetCmd, AllocCmd, FreeCmd, AssertEqCmd, AssertNEqCmd>;

struct Function {
  std::string Name;
  TSNode RawNode;
  BasicBlockId Entry;
  BasicBlockId Defer;
  BasicBlockId ErrDefer;
};

struct Block {
  std::string Label;
};

struct BasicBlock {
  Command Cmd;
  BasicBlockId Id;
  std::vector<BasicBlockId> Successors;
  std::vector<BasicBlockId> Predecessors;

  uint32_t StartByte;
  uint32_t EndByte;
  uint32_t StartRow;
  uint32_t StartCol;
  uint32_t EndRow;
  uint32_t EndCol;

  BasicBlock(Command Cmd, BasicBlockId Id)
      : Cmd{Cmd}, Id{Id}, StartByte{0}, EndByte{0}, StartRow{0}, StartCol{0},
        EndRow{0}, EndCol{0} {}

  void setLocation(TSNode RawNode) {
    StartByte = ts_node_start_byte(RawNode);
    EndByte = ts_node_end_byte(RawNode);
    StartRow = ts_node_start_point(RawNode).row;
    StartCol = ts_node_start_point(RawNode).column;
    EndRow = ts_node_end_point(RawNode).row;
    EndCol = ts_node_end_point(RawNode).column;
  }
};

} // namespace zigcfg

#endif // ZIGCFG_CFG_ELEMENTS_H