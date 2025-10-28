#ifndef DIFFINK_UTILS_LCS_H
#define DIFFINK_UTILS_LCS_H

#include <algorithm>
#include <vector>

namespace diffink {

// Equal(std::size_t LeftIndex, std::size_t RightIndex) -> bool
template <class Comparator>
std::vector<std::pair<std::size_t, std::size_t>>
findCommons(std::size_t LeftSize, std::size_t RightSize, Comparator &&Equal);

// Equal(std::size_t LeftIndex, std::size_t RightIndex) -> bool
template <class Comparator>
std::size_t countCommons(std::size_t LeftSize, std::size_t RightSize,
                         Comparator &&Equal);

} // namespace diffink

#include "DiffInk/Utils/Lcs.hpp"

#endif // DIFFINK_UTILS_LCS_H