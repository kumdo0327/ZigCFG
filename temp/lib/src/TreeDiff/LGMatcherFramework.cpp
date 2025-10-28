#include "DiffInk/TreeDiff/LGMatcherFramework.h"

namespace diffink {

void LGMatcherFramework::match(TreeDiff &Diff) {
  std::unordered_map<uint32_t, uint32_t> OldToNewLine;
  uint32_t OldLine{0}, NewLine{0};
  for (const auto &Edit : makeLineDiff(OldCode, NewCode))
    if (Edit.operation == LineLevel::Operation::EQUAL)
      for (uint32_t i{0}; i != Edit.text.length(); ++i) {
        OldToNewLine.emplace(OldLine, NewLine);
        ++OldLine;
        ++NewLine;
      }
    else if (Edit.operation == LineLevel::Operation::INSERT)
      NewLine += Edit.text.length();
    else /* Edit.operation == LineLevel::Operation::DELETE */
      OldLine += Edit.text.length();

  std::unordered_map<uint32_t, std::vector<VirtualNode *>> Candidates;
  {
    std::queue<VirtualNode *> Queue;
    for (auto Node : Diff.getNewRoot()->Children)
      Queue.push(Node);
    while (!Queue.empty()) {
      auto Node = Queue.front();
      Queue.pop();
      Candidates[Node->Original.getUTF8PosRange().StartPos.row].push_back(Node);
      for (auto Child : Node->Children)
        Queue.push(Child);
    }
  }

  std::unordered_set<VirtualNode *> UncommonOldNodes;
  {
    std::queue<VirtualNode *> Queue;
    for (auto Node : Diff.getOldRoot()->Children)
      Queue.push(Node);
    while (!Queue.empty()) {
      auto Node = Queue.front();
      Queue.pop();
      auto Line = Node->Original.getUTF8PosRange().StartPos.row;
      if (OldToNewLine.contains(Line))
        for (auto Candidate : Candidates[OldToNewLine[Line]])
          if (OldToNewLine[Node->Original.getUTF8PosRange().EndPos.row] ==
                  Candidate->Original.getUTF8PosRange().EndPos.row &&
              Node->Original.getTypeHash() ==
                  Candidate->Original.getTypeHash() &&
              Diff.findNewToOldMapping(Candidate) == nullptr) {
            Diff.insertMapping(Node, Candidate);
            break;
          }
      if (Diff.findOldToNewMapping(Node))
        for (auto Child : Node->Children)
          Queue.push(Child);
      else
        UncommonOldNodes.insert(Node);
    }
  }

  std::unordered_set<VirtualNode *> UncommonNewNodes;
  {
    std::queue<VirtualNode *> Queue;
    for (auto Node : Diff.getNewRoot()->Children)
      Queue.push(Node);
    while (!Queue.empty()) {
      auto Node = Queue.front();
      Queue.pop();
      if (Diff.findNewToOldMapping(Node))
        for (auto Child : Node->Children)
          Queue.push(Child);
      else
        UncommonNewNodes.insert(Node);
    }
  }
  gumtree::GreedyTopDown().match(Diff, UncommonOldNodes, UncommonNewNodes);
  BottomUp->match(Diff);
}

std::unique_ptr<LGMatcherFramework>
makeLGMatcherSimple(const SourceCode &OldCode,
                    const SourceCode &NewCode) noexcept {
  return std::make_unique<LGMatcherFramework>(
      std::make_unique<gumtree::SimpleBottomUp>(), OldCode, NewCode);
}

} // namespace diffink