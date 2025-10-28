#ifndef DIFFINK_UTILS_TEXTDIFF_H
#define DIFFINK_UTILS_TEXTDIFF_H

#include "DiffInk/SyntaxTree/SourceCode.h"
#include "DiffInk/Utils/Lcs.h"
#include "DiffInk/Utils/Wrapper.h"
#include "DiffInk/Utils/XxhStl.h"
#include <cstring>
#include <diff_match_patch.h>
#include <optional>

namespace diffink {

using LineLevel = diff_match_patch<std::wstring>;

constexpr uint32_t estimatetMyersCost(uint32_t OldLOC, uint32_t NewLOC,
                                      uint32_t Depth) noexcept {
  return (OldLOC + NewLOC) * Depth;
}

std::list<LineLevel::Diff> makeLineDiff(const SourceCode &OldCode,
                                        const SourceCode &NewCode);

class TextDiff {
private:
  struct EditedRange {
    uint32_t OldStartByte;
    uint32_t OldEndByte;
    uint32_t NewStartByte;
    uint32_t NewEndByte;
  };

  static constexpr uint32_t DefaultMaxCost{
      estimatetMyersCost(10000, 10000, 2000)};

private:
  const SourceCode &OldCode;
  const SourceCode &NewCode;

private:
  void applyEdits(TSTree &Tree, const std::vector<EditedRange> &Edits);

public:
  TextDiff(const SourceCode &OldCode, const SourceCode &NewCode)
      : OldCode(OldCode), NewCode(NewCode) {}

  TSTreeWrapper makeEditedTree(const TSTree &Tree,
                               uint32_t MaxCost = DefaultMaxCost);
};

} // namespace diffink

#endif // DIFFINK_UTILS_TEXTDIFF_H