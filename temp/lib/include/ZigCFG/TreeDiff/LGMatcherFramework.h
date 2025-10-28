#ifndef DIFFINK_TREEDIFF_LGMATCHERFRAMEWORK_H
#define DIFFINK_TREEDIFF_LGMATCHERFRAMEWORK_H

#include "DiffInk/TreeDiff/GumTree/GreedyBottomUp.h"
#include "DiffInk/TreeDiff/GumTree/GreedyTopDown.h"
#include "DiffInk/TreeDiff/GumTree/SimpleBottomUp.h"

namespace diffink {

class LGMatcherFramework : public TreeDiff::Matcher {
private:
  std::unique_ptr<Matcher> BottomUp;
  const SourceCode &OldCode;
  const SourceCode &NewCode;

public:
  LGMatcherFramework(std::unique_ptr<Matcher> BottomUp,
                     const SourceCode &OldCode,
                     const SourceCode &NewCode) noexcept
      : BottomUp(std::move(BottomUp)), OldCode{OldCode}, NewCode{NewCode} {}

  ~LGMatcherFramework() final = default;

  void match(TreeDiff &Diff) final;
};

std::unique_ptr<LGMatcherFramework>
makeLGMatcherSimple(const SourceCode &OldCode,
                    const SourceCode &NewCode) noexcept;

} // namespace diffink

#endif // DIFFINK_TREEDIFF_LGMATCHERFRAMEWORK_H