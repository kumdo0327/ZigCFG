#include "ZigCFG/Cfg/Cfg.h"
#include <fstream>
#include <iostream>

void printNode(TSNode node, uint32_t indent = 0) {
  for (uint32_t i = 0; i < indent; ++i)
    std::cout << "  ";
  std::cout << ts_node_type(node) << std::endl;

  uint32_t childCount = ts_node_child_count(node);
  for (uint32_t i = 0; i < childCount; ++i) {
    TSNode child = ts_node_child(node, i);
    printNode(child, indent + 1);
  }
}

int main() {
  // Read benchmark/1.zig
  std::ifstream File("benchmark/1.zig");
  std::string Src((std::istreambuf_iterator<char>(File)),
                  std::istreambuf_iterator<char>());

  TSParser *Parser = ts_parser_new();
  ts_parser_set_language(Parser, tree_sitter_zig());
  TSTree *Tree =
      ts_parser_parse_string(Parser, nullptr, Src.c_str(), Src.size());
  ts_parser_delete(Parser);

  TSNode root = ts_tree_root_node(Tree);
  printNode(root);

  ts_tree_delete(Tree);
  return 0;
}