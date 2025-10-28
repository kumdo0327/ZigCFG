#include "DiffInk/TreeDiff/Recovery/OptimalRecovery.h"

namespace diffink::recovery {

int OptimalRecovery::computeUpdateCost(VirtualNode *Left,
                                       VirtualNode *Right) const {
  if (Left->Original.getType() != Right->Original.getType())
    return std::numeric_limits<int>::max();
  return HashNode::equalExactly(Left->Original, Right->Original) ? 0 : 1;
}

void OptimalRecovery::tryMapping(TreeDiff &Mapping, VirtualNode *Old,
                                 VirtualNode *New) const {
  if (!Mapping.findOldToNewMapping(Old) && !Mapping.findNewToOldMapping(New))
    Mapping.insertMapping(Old, New);
}

void OptimalRecovery::match(TreeDiff &Mapping, VirtualNode *Old,
                            VirtualNode *New) {
  if (std::max(Old->VirtualSize, New->VirtualSize) > MaxSize)
    return;

  auto OldPostOrder = Old->makePostOrder();
  auto NewPostOrder = New->makePostOrder();

  std::unordered_map<VirtualNode *, int> OldIndex, NewIndex;
  OldIndex.reserve(OldPostOrder.size());
  NewIndex.reserve(NewPostOrder.size());
  std::vector<int> OldLeftMost(OldPostOrder.size(), 0);
  std::vector<int> NewLeftMost(NewPostOrder.size(), 0);

  for (int i{0}; i != OldPostOrder.size(); ++i) {
    auto Node = OldPostOrder[i];
    OldIndex.emplace(Node, i);
    OldLeftMost[i] =
        Node->isLeaf() ? i : OldLeftMost[OldIndex[Node->Children[0]]];
  }
  for (int i{0}; i != NewPostOrder.size(); ++i) {
    auto Node = NewPostOrder[i];
    NewIndex.emplace(Node, i);
    NewLeftMost[i] =
        Node->isLeaf() ? i : NewLeftMost[NewIndex[Node->Children[0]]];
  }

  std::vector<int> OldKeys, NewKeys;
  {
    std::unordered_set<int> Visited;
    for (int i = OldPostOrder.size() - 1; i != -1; --i)
      if (Visited.insert(OldLeftMost[i]).second)
        OldKeys.push_back(i);
    std::reverse(OldKeys.begin(), OldKeys.end());
  }
  {
    std::unordered_set<int> Visited;
    for (int i = NewPostOrder.size() - 1; i != -1; --i)
      if (Visited.insert(NewLeftMost[i]).second)
        NewKeys.push_back(i);
    std::reverse(NewKeys.begin(), NewKeys.end());
  }

  const int N = OldPostOrder.size();
  const int M = NewPostOrder.size();
  std::vector<std::vector<int>> TreeDist(N + 1, std::vector<int>(M + 1, 0));
  std::vector<std::vector<int>> ForestDist(N + 1, std::vector<int>(M + 1, 0));

  auto LldOld = [&OldLeftMost](int Index) {
    return OldLeftMost[Index - 1] + 1;
  };
  auto LldNew = [&NewLeftMost](int Index) {
    return NewLeftMost[Index - 1] + 1;
  };

  for (auto i : OldKeys) {
    ++i;

    for (auto j : NewKeys) {
      ++j;

      ForestDist[LldOld(i) - 1][LldNew(j) - 1] = 0;
      for (int di = LldOld(i); di != i + 1; ++di) {
        ForestDist[di][LldNew(j) - 1] = ForestDist[di - 1][LldNew(j) - 1] + 1;
        for (int dj = LldNew(j); dj != j + 1; ++dj) {
          ForestDist[LldOld(i) - 1][dj] = ForestDist[LldOld(i) - 1][dj - 1] + 1;

          if (LldOld(di) == LldOld(i) && LldNew(dj) == LldNew(j)) {
            auto CostUpd =
                computeUpdateCost(OldPostOrder[di - 1], NewPostOrder[dj - 1]);
            if (CostUpd != std::numeric_limits<int>::max())
              CostUpd += ForestDist[di - 1][dj - 1];

            TreeDist[di][dj] = ForestDist[di][dj] =
                std::min({ForestDist[di - 1][dj] + 1,
                          ForestDist[di][dj - 1] + 1, CostUpd});
          }

          else
            ForestDist[di][dj] = std::min(
                {ForestDist[di - 1][dj] + 1, ForestDist[di][dj - 1] + 1,
                 ForestDist[LldOld(di) - 1][LldNew(dj) - 1] +
                     TreeDist[di][dj]});
        }
      }
    }
  }

  std::stack<std::pair<int, int>> Stack;
  Stack.emplace(N, M);

  while (!Stack.empty()) {
    const auto [LastRow, LastCol] = Stack.top();
    Stack.pop();
    const auto FirstRow = LldOld(LastRow) - 1;
    const auto FirstCol = LldNew(LastCol) - 1;

    for (auto Row = LastRow, Col = LastCol; Row > FirstRow || Col > FirstCol;) {
      if (Row > FirstRow &&
          ForestDist[Row - 1][Col] + 1 == ForestDist[Row][Col])
        --Row;
      else if (Col > FirstCol &&
               ForestDist[Row][Col - 1] + 1 == ForestDist[Row][Col])
        --Col;

      else {
        if (LldOld(Row) == LldOld(LastRow) && LldNew(Col) == LldNew(LastCol)) {
          if (OldPostOrder[Row - 1]->Original.getType() ==
              NewPostOrder[Col - 1]->Original.getType()) {
            tryMapping(Mapping, OldPostOrder[Row - 1], NewPostOrder[Col - 1]);
            --Row;
            --Col;
          } else
            throw std::runtime_error(
                "Unreachable code in OptimalRecovery::match");
        } else {
          Stack.emplace(Row, Col);
          Row = LldOld(Row) - 1;
          Col = LldNew(Col) - 1;
        }
      }
    }
  }
}

} // namespace diffink::recovery