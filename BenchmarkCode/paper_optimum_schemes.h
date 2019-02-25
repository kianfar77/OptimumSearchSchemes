#include <seqan/index.h>

using namespace seqan;

// Contains Optimum Search Schemes from the paper for errors K = 1, ..., 4 and parts P = K+1, ... K+3

template <size_t max, typename TVoidType = void>
struct PaperOptimumSearchSchemes;

template <typename TVoidType>
struct PaperOptimumSearchSchemes<1, TVoidType>
{
    static constexpr std::array<OptimalSearch<2>, 2> VALUE_plus_one
    {{
        { {{1, 2}}, {{0, 0}}, {{0, 1}}, {{0, 0}}, 0 },
        { {{2, 1}}, {{0, 1}}, {{0, 1}}, {{0, 0}}, 0 }
    }};
    static constexpr std::array<OptimalSearch<3>, 2> VALUE_plus_two
    {{
        { {{1, 2, 3}}, {{0, 0, 1}}, {{0, 0, 1}}, {{0, 0, 0}}, 0},
        { {{3, 2, 1}}, {{0, 0, 0}}, {{0, 1, 1}}, {{0, 0, 0}}, 0}
    }};
    static constexpr std::array<OptimalSearch<4>, 2> VALUE_plus_three
    {{
        { {{1, 2, 3, 4}}, {{0, 0, 0, 0}}, {{0, 0, 1, 1}}, {{0, 0, 0, 0}}, 0},
        { {{4, 3, 2, 1}}, {{0, 0, 0, 1}}, {{0, 0, 1, 1}}, {{0, 0, 0, 0}}, 0}
    }};
};

template <typename TVoidType>
constexpr std::array<OptimalSearch<2>, 2> PaperOptimumSearchSchemes<1, TVoidType>::VALUE_plus_one;

template <typename TVoidType>
constexpr std::array<OptimalSearch<3>, 2> PaperOptimumSearchSchemes<1, TVoidType>::VALUE_plus_two;

template <typename TVoidType>
constexpr std::array<OptimalSearch<4>, 2> PaperOptimumSearchSchemes<1, TVoidType>::VALUE_plus_three;

template <typename TVoidType>
struct PaperOptimumSearchSchemes<2, TVoidType>
{
    static constexpr std::array<OptimalSearch<3>, 3> VALUE_plus_one
    {{
        { {{1, 2, 3}}, {{0, 0, 2}}, {{0, 1, 2}}, {{0, 0, 0}}, 0},
        { {{3, 2, 1}}, {{0, 0, 0}}, {{0, 2, 2}}, {{0, 0, 0}}, 0},
        { {{2, 3, 1}}, {{0, 1, 1}}, {{0, 1, 2}}, {{0, 0, 0}}, 0}
    }};
    static constexpr std::array<OptimalSearch<4>, 3> VALUE_plus_two
    {{
        { {{2, 1, 3, 4}}, {{0, 0, 1, 1}}, {{0, 0, 2, 2}}, {{0, 0, 0, 0}}, 0},
        { {{3, 2, 1, 4}}, {{0, 0, 0, 0}}, {{0, 1, 1, 2}}, {{0, 0, 0, 0}}, 0},
        { {{4, 3, 2, 1}}, {{0, 0, 0, 2}}, {{0, 1, 2, 2}}, {{0, 0, 0, 0}}, 0}
    }};
    static constexpr std::array<OptimalSearch<5>, 3> VALUE_plus_three
    {{
        { {{2, 1, 3, 4, 5}}, {{0, 0, 0, 1, 1}}, {{0, 0, 2, 2, 2}}, {{0, 0, 0, 0, 0}}, 0},
        { {{4, 3, 2, 1, 5}}, {{0, 0, 0, 0, 0}}, {{0, 0, 1, 1, 2}}, {{0, 0, 0, 0, 0}}, 0},
        { {{5, 4, 3, 2, 1}}, {{0, 0, 0, 0, 2}}, {{0, 1, 1, 2, 2}}, {{0, 0, 0, 0, 0}}, 0}
    }};
};

template <typename TVoidType>
constexpr std::array<OptimalSearch<3>, 3> PaperOptimumSearchSchemes<2, TVoidType>::VALUE_plus_one;

template <typename TVoidType>
constexpr std::array<OptimalSearch<4>, 3> PaperOptimumSearchSchemes<2, TVoidType>::VALUE_plus_two;

template <typename TVoidType>
constexpr std::array<OptimalSearch<5>, 3> PaperOptimumSearchSchemes<2, TVoidType>::VALUE_plus_three;

template <typename TVoidType>
struct PaperOptimumSearchSchemes<3, TVoidType>
{
    static constexpr std::array<OptimalSearch<4>, 3> VALUE_plus_one
    {{
        { {{1, 2, 3, 4}}, {{0, 0, 0, 3}}, {{0, 2, 3, 3}}, {{0, 0, 0, 0}}, 0},
        { {{2, 3, 4, 1}}, {{0, 0, 0, 0}}, {{1, 2, 2, 3}}, {{0, 0, 0, 0}}, 0},
        { {{3, 4, 2, 1}}, {{0, 0, 2, 2}}, {{0, 0, 3, 3}}, {{0, 0, 0, 0}}, 0}
    }};
    static constexpr std::array<OptimalSearch<5>, 3> VALUE_plus_two
    {{
        { {{1, 2, 3, 4, 5}}, {{0, 0, 0, 2, 2}}, {{0, 0, 3, 3, 3}}, {{0, 0, 0, 0, 0}}, 0},
        { {{4, 3, 2, 1, 5}}, {{0, 0, 0, 0, 0}}, {{1, 1, 2, 2, 3}}, {{0, 0, 0, 0, 0}}, 0},
        { {{5, 4, 3, 2, 1}}, {{0, 0, 0, 0, 3}}, {{0, 2, 2, 3, 3}}, {{0, 0, 0, 0, 0}}, 0}
        // { {{1, 2, 3, 4, 5}}, {{0, 0, 0, 0, 3}}, {{0, 2, 2, 3, 3}}, {{0, 0, 0, 0, 0}}, 0 },
        // { {{2, 3, 4, 5, 1}}, {{0, 0, 0, 2, 2}}, {{0, 1, 2, 2, 3}}, {{0, 0, 0, 0, 0}}, 0 },
        // { {{3, 4, 5, 2, 1}}, {{0, 0, 1, 1, 1}}, {{0, 1, 1, 2, 3}}, {{0, 0, 0, 0, 0}}, 0 },
        // { {{4, 5, 3, 2, 1}}, {{0, 0, 0, 0, 0}}, {{0, 0, 3, 3, 3}}, {{0, 0, 0, 0, 0}}, 0 }
    }};
    static constexpr std::array<OptimalSearch<6>, 3> VALUE_plus_three
    {{
        { {{1, 2, 3, 4, 5, 6}}, {{0, 0, 0, 0, 0, 3}}, {{0, 2, 2, 2, 3, 3}}, {{0, 0, 0, 0, 0, 0}}, 0},
        { {{2, 3, 4, 5, 6, 1}}, {{0, 0, 0, 0, 0, 0}}, {{1, 1, 1, 2, 2, 3}}, {{0, 0, 0, 0, 0, 0}}, 0},
        { {{6, 5, 4, 3, 2, 1}}, {{0, 0, 0, 0, 2, 2}}, {{0, 0, 3, 3, 3, 3}}, {{0, 0, 0, 0, 0, 0}}, 0}
    }};
};

template <typename TVoidType>
constexpr std::array<OptimalSearch<4>, 3> PaperOptimumSearchSchemes<3, TVoidType>::VALUE_plus_one;

template <typename TVoidType>
constexpr std::array<OptimalSearch<5>, 3> PaperOptimumSearchSchemes<3, TVoidType>::VALUE_plus_two;

template <typename TVoidType>
constexpr std::array<OptimalSearch<6>, 3> PaperOptimumSearchSchemes<3, TVoidType>::VALUE_plus_three;

template <typename TVoidType>
struct PaperOptimumSearchSchemes<4, TVoidType>
{
    static constexpr std::array<OptimalSearch<5>, 3> VALUE_plus_one
    {{
        { {{1, 2, 3, 4, 5}}, {{0, 0, 0, 0, 4}}, {{0, 3, 3, 4, 4}}, {{0, 0, 0, 0, 0}}, 0},
        { {{2, 3, 4, 5, 1}}, {{0, 0, 0, 0, 0}}, {{2, 2, 3, 3, 4}}, {{0, 0, 0, 0, 0}}, 0},
        { {{5, 4, 3, 2, 1}}, {{0, 0, 0, 3, 3}}, {{0, 0, 4, 4, 4}}, {{0, 0, 0, 0, 0}}, 0}
    }};
    static constexpr std::array<OptimalSearch<6>, 3> VALUE_plus_two
    {{
        { {{1, 2, 3, 4, 5, 6}}, {{0, 0, 0, 0, 0, 4}}, {{0, 3, 3, 3, 4, 4}}, {{0, 0, 0, 0, 0, 0}}, 0},
        { {{2, 3, 4, 5, 6, 1}}, {{0, 0, 0, 0, 0, 0}}, {{2, 2, 2, 3, 3, 4}}, {{0, 0, 0, 0, 0, 0}}, 0},
        { {{6, 5, 4, 3, 2, 1}}, {{0, 0, 0, 0, 3, 3}}, {{0, 0, 4, 4, 4, 4}}, {{0, 0, 0, 0, 0, 0}}, 0}
    }};
    static constexpr std::array<OptimalSearch<7>, 3> VALUE_plus_three
    {{
        { {{1, 2, 3, 4, 5, 6, 7}}, {{0, 1, 1, 1, 1, 1, 1}}, {{3, 3, 3, 3, 3, 3, 4}}, {{0, 0, 0, 0, 0, 0, 0}}, 0},
        { {{1, 2, 3, 4, 5, 6, 7}}, {{0, 0, 0, 0, 0, 0, 0}}, {{0, 0, 4, 4, 4, 4, 4}}, {{0, 0, 0, 0, 0, 0, 0}}, 0},
        { {{7, 6, 5, 4, 3, 2, 1}}, {{0, 0, 0, 0, 0, 0, 4}}, {{0, 3, 3, 3, 3, 4, 4}}, {{0, 0, 0, 0, 0, 0, 0}}, 0}
    }};
};

template <typename TVoidType>
constexpr std::array<OptimalSearch<5>, 3> PaperOptimumSearchSchemes<4, TVoidType>::VALUE_plus_one;

template <typename TVoidType>
constexpr std::array<OptimalSearch<6>, 3> PaperOptimumSearchSchemes<4, TVoidType>::VALUE_plus_two;

template <typename TVoidType>
constexpr std::array<OptimalSearch<7>, 3> PaperOptimumSearchSchemes<4, TVoidType>::VALUE_plus_three;
