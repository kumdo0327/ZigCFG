#ifndef DIFFINK_SYNTAXTREE_SOURCECODE_H
#define DIFFINK_SYNTAXTREE_SOURCECODE_H

#include <stdexcept>
#include <string>
#include <tree_sitter/api.h>
#include <vector>

namespace diffink {

class SourceCode {
private:
  std::string Content;
  std::vector<TSPoint> Pos;
  std::vector<TSPoint> UTF8Pos;
  std::vector<uint32_t> Lines;

public:
  SourceCode(std::string &&Content);

  const char *getContent() const noexcept { return Content.c_str(); }

  // Equivalent to "getPosition"
  TSPoint operator[](std::string::size_type Offset) const noexcept {
    return Pos[Offset];
  }

  TSPoint getPosition(std::string::size_type Offset) const noexcept {
    return Pos[Offset];
  }

  TSPoint getUTF8Position(std::string::size_type Offset) const noexcept {
    return UTF8Pos[Offset];
  }

  uint32_t getEndOfLine(std::size_t Line) const noexcept { return Lines[Line]; }

  std::string::size_type getSize() const noexcept { return Content.size(); }

  uint32_t getLOC() const noexcept {
    return Pos.empty() ? 0 : Pos.back().row + 1;
  }

  std::string getSubstring(std::string::size_type StartOffset,
                           std::string::size_type EndOffset) const noexcept {
    return std::string(getContent() + StartOffset, getContent() + EndOffset);
  }
};

} // namespace diffink

#endif // DIFFINK_SYNTAXTREE_SOURCECODE_H