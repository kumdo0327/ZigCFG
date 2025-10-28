#include "DiffInk/Utils/TextDiff.h"

namespace diffink {

std::list<LineLevel::Diff> makeLineDiff(const SourceCode &OldCode,
                                        const SourceCode &NewCode) {
  auto hashLine = [](const SourceCode &Code, uint32_t Line) -> wchar_t {
    const auto Start = Code.toByteFromLine(Line);
    const auto End = Code.toByteFromLine(Line + 1);
    return static_cast<wchar_t>(
        xxhString32(Code.getContent() + Start, End - Start));
  };
  std::wstring OldLines(OldCode.getLOC(), '\0');
  for (std::size_t i{0}; i != OldCode.getLOC(); ++i)
    OldLines[i] = hashLine(OldCode, i);
  std::wstring NewLines(NewCode.getLOC(), '\0');
  for (std::size_t i{0}; i != NewCode.getLOC(); ++i)
    NewLines[i] = hashLine(NewCode, i);

  LineLevel Diff;
  Diff.Diff_Timeout = 0.0f;
  return Diff.diff_main(OldLines, NewLines, false);
}

void TextDiff::applyEdits(TSTree &Tree, const std::vector<EditedRange> &Edits) {
  for (auto &Edit : Edits) {
    auto InputEdit =
        TSInputEdit{.start_byte = Edit.NewStartByte,
                    .old_end_byte = Edit.NewStartByte +
                                    (Edit.OldEndByte - Edit.OldStartByte),
                    .new_end_byte = Edit.NewEndByte,
                    .start_point = NewCode[Edit.NewStartByte],
                    .old_end_point = OldCode[Edit.OldEndByte],
                    .new_end_point = NewCode[Edit.NewStartByte]};

    if (InputEdit.start_point.row == OldCode[Edit.OldStartByte].row)
      InputEdit.old_end_point.column +=
          static_cast<int64_t>(InputEdit.start_point.column) -
          static_cast<int64_t>(OldCode[Edit.OldStartByte].column);
    InputEdit.old_end_point.row +=
        static_cast<int64_t>(InputEdit.start_point.row) -
        static_cast<int64_t>(OldCode[Edit.OldStartByte].row);
    ts_tree_edit(&Tree, &InputEdit);
  }
}

TSTreeWrapper TextDiff::makeEditedTree(const TSTree &Tree, uint32_t MaxCost) {
  auto MinLineDiff = std::max(OldCode.getLOC(), NewCode.getLOC()) -
                     std::min(OldCode.getLOC(), NewCode.getLOC());
  if (estimatetMyersCost(OldCode.getLOC(), NewCode.getLOC(), MinLineDiff) >
      MaxCost)
    return TSTreeWrapper(nullptr, ts_tree_delete);
  std::size_t DiffLines{0};
  for (auto &Edit : makeLineDiff(OldCode, NewCode))
    if (Edit.operation != diff_match_patch<std::wstring>::Operation::EQUAL)
      DiffLines += Edit.text.length();
  if (estimatetMyersCost(OldCode.getLOC(), NewCode.getLOC(), DiffLines) >
      MaxCost)
    return TSTreeWrapper(nullptr, ts_tree_delete);

  TSTreeWrapper EditedTree(ts_tree_copy(&Tree), ts_tree_delete);
  std::vector<EditedRange> Edits;
  diff_match_patch<std::string> Diff;
  Diff.Diff_Timeout = 0.0f;
  uint32_t OldIter{0}, NewIter{0};
  std::optional<EditedRange> CurEdit;

  for (const auto &Edit :
       Diff.diff_main(OldCode.getContent(), NewCode.getContent(), false)) {
    if (Edit.operation == diff_match_patch<std::string>::Operation::EQUAL) {
      if (CurEdit) {
        CurEdit->OldEndByte = OldIter;
        CurEdit->NewEndByte = NewIter;
        Edits.push_back(*CurEdit);
        CurEdit.reset();
      }
      OldIter += Edit.text.length();
      NewIter += Edit.text.length();
      continue;
    }
    if (!CurEdit)
      CurEdit = EditedRange{.OldStartByte = OldIter,
                            .OldEndByte = OldIter,
                            .NewStartByte = NewIter,
                            .NewEndByte = NewIter};
    if (Edit.operation == diff_match_patch<std::string>::Operation::DELETE)
      CurEdit->OldEndByte = OldIter += Edit.text.length();
    else
      CurEdit->NewEndByte = NewIter += Edit.text.length();
  }
  if (CurEdit) {
    CurEdit->OldEndByte = OldIter;
    CurEdit->NewEndByte = NewIter;
    Edits.push_back(*CurEdit);
  }

  applyEdits(*EditedTree, Edits);
  return EditedTree;
}

} // namespace diffink