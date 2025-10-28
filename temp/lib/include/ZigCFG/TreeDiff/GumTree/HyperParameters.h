#ifndef DIFFINK_TREEDIFF_GUMTREE_HYPERPARAMETERS_H
#define DIFFINK_TREEDIFF_GUMTREE_HYPERPARAMETERS_H

#include <cstdint>

namespace diffink::gumtree {

constexpr uint32_t DefaultMinHeight{1};

constexpr double DefaultMinDice{0.5};

constexpr uint32_t DefaultMaxSize{1000};

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_GUMTREE_HYPERPARAMETERS_H