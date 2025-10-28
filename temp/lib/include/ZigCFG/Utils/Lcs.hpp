#include "DiffInk/Utils/Lcs.h"

namespace diffink {

template <class Comparator>
std::vector<std::pair<std::size_t, std::size_t>>
findCommons(std::size_t LeftSize, std::size_t RightSize, Comparator &&Equal) {
  std::vector<std::vector<std::size_t>> Table(
      LeftSize + 1, std::vector<std::size_t>(RightSize + 1, 0));
  for (std::size_t i{0}; i != LeftSize; ++i)
    for (std::size_t j{0}; j != RightSize; ++j)
      Table[i + 1][j + 1] = Equal(i, j)
                                ? Table[i][j] + 1
                                : std::max(Table[i][j + 1], Table[i + 1][j]);

  std::vector<std::pair<std::size_t, std::size_t>> Seq;
  Seq.reserve(std::min(LeftSize, RightSize));
  for (std::size_t i{LeftSize}, j{RightSize}; i != 0 && j != 0;) {
    if (Table[i][j] == Table[i - 1][j - 1] + 1 && Equal(i - 1, j - 1)) {
      --i;
      --j;
      Seq.emplace_back(i, j);
    } else if (Table[i][j] == Table[i][j - 1])
      --j;
    else
      --i;
  }
  std::reverse(Seq.begin(), Seq.end());
  return Seq;
}

template <class Comparator>
std::size_t countCommons(std::size_t LeftSize, std::size_t RightSize,
                         Comparator &&Equal) {
  std::vector<std::vector<std::size_t>> Table(
      LeftSize + 1, std::vector<std::size_t>(RightSize + 1, 0));
  for (std::size_t i{0}; i != LeftSize; ++i)
    for (std::size_t j{0}; j != RightSize; ++j)
      Table[i + 1][j + 1] = Equal(i, j)
                                ? Table[i][j] + 1
                                : std::max(Table[i][j + 1], Table[i + 1][j]);
  return Table[LeftSize][RightSize];
}

} // namespace diffink