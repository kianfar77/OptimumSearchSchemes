#include <type_traits>

#include <seqan/index.h>

using namespace seqan;

namespace seqan {

template <typename TChar, typename TOwner>
struct SAValue<StringSet<String<TChar>, TOwner > >
{
    typedef Pair<uint16_t, uint32_t> Type;
};

template <typename TChar, typename TOwner>
struct SAValue<String<TChar, TOwner > >
{
    typedef uint32_t Type;
};

};

// Index type
typedef FastFMIndexConfig<void, uint32_t, 2, 1> TMyFastConfig;
typedef BidirectionalIndex<FMIndex<void, TMyFastConfig> > TIndexConfig;

template <typename TChar, typename TDistanceTag>
inline void _countTrivialSearch(unsigned const needleLength,
                                unsigned const needlePos,
                                uint8_t const errorsLeft, TDistanceTag const & /**/,
                                unsigned long long & counts)
{
    if (errorsLeft == 0 || needleLength == needlePos)
    {
        counts += needleLength - needlePos;
        return;
    }

    // Match / Mismatch
    unsigned long long countsMatch = 1, countsMismatchOrInsertion = 0;
    _countTrivialSearch<TChar>(needleLength, needlePos + 1, errorsLeft, TDistanceTag(), countsMatch);
    _countTrivialSearch<TChar>(needleLength, needlePos + 1, errorsLeft - 1, TDistanceTag(), countsMismatchOrInsertion);
    counts += countsMatch + (ValueSize<TChar>::VALUE - 1) * (countsMismatchOrInsertion + 1);

    if (std::is_same<TDistanceTag, EditDistance>::value)
    {
        // Deletion
        unsigned long long countsDeletion = 0; // TODO: do we count the deletion itself?
        _countTrivialSearch<TChar>(needleLength, needlePos, errorsLeft - 1, TDistanceTag(), countsDeletion);
        counts += ValueSize<TChar>::VALUE * countsDeletion;
        // Insertion
        counts += countsMismatchOrInsertion; // TODO: do we count the deletion itself?
    }
}

template <typename TChar, typename TDistanceTag>
inline unsigned long long countTrivialSearch(unsigned int needleLength, uint8_t const errors, TDistanceTag const & /**/)
{
    unsigned long long counts = 0;
    _countTrivialSearch<TChar>(needleLength, 0, errors, TDistanceTag(), counts);
    return counts;
}

// TODO: rename to needlePos!
template <typename TChar, typename TOptimalSearch, typename TDistanceTag>
inline void _countSearch(uint32_t const needleLength, uint32_t const needleLeftIt, uint32_t const needleRightIt,
                         uint8_t const errors, TOptimalSearch const & s,
                         bool const goToRight, uint8_t const blockIndex,
                         uint64_t & count, TDistanceTag const & /**/)
{
    // Done.
    if (needleLeftIt == 0 && needleRightIt == needleLength + 1)
        return;

    // Exact search in current block.
    uint8_t maxErrorsLeftInBlock = s.u[blockIndex] - errors;
    uint8_t minErrorsLeftInBlock = (s.l[blockIndex] > errors) ? (s.l[blockIndex] - errors) : 0;
    if (maxErrorsLeftInBlock == 0)
    {
        signed infixPosLeft, infixPosRight;

        if (goToRight)
        {
            infixPosLeft = needleRightIt + - 1;
            infixPosRight = needleLeftIt + s.blocklength[blockIndex] - 1;
        }
        else
        {
            infixPosLeft = needleRightIt - s.blocklength[blockIndex] - 1;
            infixPosRight = needleLeftIt - 1;
        }

        bool goToRight2 = s.pi[blockIndex + 1] > s.pi[blockIndex];
        count += infixPosRight - infixPosLeft + 1;
        if (goToRight)
            return _countSearch<TChar>(needleLength, needleLeftIt, infixPosRight + 2, errors, s, goToRight2, blockIndex + 1, count, TDistanceTag());
        else
            return _countSearch<TChar>(needleLength, infixPosLeft, needleRightIt, errors, s, goToRight2, blockIndex + 1, count, TDistanceTag());
    }
    // Approximate search in current block.
    else
    {
        unsigned charsLeft = s.blocklength[blockIndex] - (needleRightIt - needleLeftIt - 1);

        // Insertion
        if (std::is_same<TDistanceTag, EditDistance>::value)
        {
            uint8_t blockIndex2 = blockIndex;
            bool goToRight2 = goToRight;
            if (needleRightIt - needleLeftIt == s.blocklength[blockIndex])
            {
                ++blockIndex2;
                goToRight2 = s.pi[blockIndex2] > s.pi[blockIndex];
            }

            _countSearch<TChar>(needleLength, needleLeftIt - !goToRight, needleRightIt + goToRight, errors + 1, s, goToRight2, blockIndex2, count, TDistanceTag());
        }

        for (unsigned i = 0; i < ValueSize<TChar>::VALUE; ++i)
        {
            bool delta = (i != 0);

            // Deletion
            if (std::is_same<TDistanceTag, EditDistance>::value)
                _countSearch<TChar>(needleLength, needleLeftIt, needleRightIt, errors + 1, s, goToRight, blockIndex, count, TDistanceTag());

            if (minErrorsLeftInBlock > 0 && charsLeft - 1 < minErrorsLeftInBlock - delta)
                continue;

            uint8_t blockIndex2 = blockIndex;
            bool goToRight2 = goToRight;
            if (needleRightIt - needleLeftIt == s.blocklength[blockIndex])
            {
                ++blockIndex2;
                goToRight2 = s.pi[blockIndex2] > s.pi[blockIndex];
            }

            ++count;
            if (goToRight)
                _countSearch<TChar>(needleLength, needleLeftIt, needleRightIt + 1, errors + delta, s, goToRight2, blockIndex2, count, TDistanceTag());
            else
                _countSearch<TChar>(needleLength, needleLeftIt - 1, needleRightIt, errors + delta, s, goToRight2, blockIndex2, count, TDistanceTag());
        }
    }
}

template <typename TChar, typename TOptimalSearch, typename TDistanceTag>
inline uint64_t countSearch(unsigned int needleLength, TOptimalSearch const & s, TDistanceTag const & /**/)
{
    uint64_t count = 0; // TODO: removed initialDirection. correct?
    _countSearch<TChar>(needleLength, s.startPos, s.startPos + 1, 0, s, true /*TODO:initialDirection Rrev()*/, 0, count, TDistanceTag());
    return count;
}

template <typename TChar, typename TOptimalSearchScheme, typename TDistanceTag>
inline uint64_t countSearchScheme(unsigned const needleLength, TOptimalSearchScheme const & scheme, TDistanceTag const & /**/)
{
    uint64_t count = 0;
    for (auto const & s : scheme)
        count += countSearch<TChar>(needleLength, s, TDistanceTag());
    return count;
}

template <typename TChar, typename TOptimalSearchScheme>
inline void _optimalSearchSchemeComputeOptimalBlocklength(TOptimalSearchScheme scheme,
                                                          unsigned const maxErrors, uint32_t const needleLength)
{
    // count edges for all blocklengths and choose theoretically optimal one
    uint64_t countEdges, countEdgesOptimal = static_cast<uint64_t>(-1); // maximum value of unsigned int
    std::vector<uint32_t> blocklength, blocklengthOptimal;

    for (unsigned i = 1; i < scheme[0].pi.size(); ++i)
    {
        blocklength[i-1] = i * maxErrors;
    }
    back(blocklength) = needleLength;

    while (true)
    {
        std::vector<uint32_t> _blocklengthAbsolute = blocklength;
        for (unsigned i = _blocklengthAbsolute.size(); i > 0; --i)
        {
            _blocklengthAbsolute[i] -= _blocklengthAbsolute[i - 1];
        }

        _optimalSearchSchemeSetBlockLength(scheme, _blocklengthAbsolute);
        _optimalSearchSchemeInit(scheme);
        countEdges = countSearchScheme<TChar>(needleLength, scheme, HammingDistance()); // TODO: document that indels are always ignored!
        if (countEdges < countEdgesOptimal)
        {
            countEdgesOptimal = countEdges;
            blocklengthOptimal = _blocklengthAbsolute;
        }

        // compute next blockpos
        signed i;
        for (i = blocklength.size() - 2; i >= 0; --i)
        {
            // find rightmost element in blockpos that we can increment
            if (blocklength[i] < blocklength[i + 1] - maxErrors)
            {
                ++blocklength[i];
                // reset all elements after pos i
                for (unsigned j = i + 1; j < blocklength.size() - 1; ++j)
                {
                    blocklength[j] = blocklength[j - 1] + maxErrors;
                }
                break;
            }
        }
        if (i < 0)
            break;
    }

    _optimalSearchSchemeSetBlockLength(scheme, blocklengthOptimal);
    _optimalSearchSchemeInit(scheme);
}
