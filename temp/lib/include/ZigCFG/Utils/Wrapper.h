#ifndef DIFFINK_UTILS_WRAPPER_H
#define DIFFINK_UTILS_WRAPPER_H

#include <memory>
#include <tree_sitter/api.h>

namespace diffink {

using TSTreeWrapper = std::unique_ptr<TSTree, decltype(&ts_tree_delete)>;

class TSParserWrapper {
private:
  std::unique_ptr<const TSLanguage, decltype(&ts_language_delete)> Language;
  std::unique_ptr<TSParser, decltype(&ts_parser_delete)> Parser;

public:
  template <class LangAllocator>
  TSParserWrapper(LangAllocator &&Allocator) noexcept
      : Language(Allocator(), ts_language_delete),
        Parser(ts_parser_new(), ts_parser_delete) {
    ts_parser_set_language(Parser.get(), Language.get());
  }

  TSParser &get() noexcept { return *Parser; }
};

} // namespace diffink

#endif // DIFFINK_UTILS_WRAPPER_H