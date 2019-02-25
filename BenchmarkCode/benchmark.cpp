#include <benchmark/benchmark.h>
#include <seqan/arg_parse.h>
#include <seqan/index.h>
#include <seqan/seq_io.h>

#include <type_traits>

#include "common.h"
#include "paper_optimum_schemes.h"

using namespace seqan;

typedef Index<StringSet<String<Dna, Alloc<>>, Owner<ConcatDirect<> > >, TIndexConfig> TIndex;
typedef Iter<TIndex, VSTree<TopDown<> > > TIter;

TIndex fm_index;
StringSet<DnaString> reads;

void BM_HammingDistance(benchmark::State& state, uint8_t const maxErrors)
{
    typedef HammingDistance TDistanceTag;

    TIter it(fm_index);

    uint64_t hitsNbr, uniqueHits;
    auto delegate = [&hitsNbr](auto const &it, DnaString const & /*read*/, unsigned const errors = 0) {
        ++hitsNbr;
        unsigned x = 0;
        for (unsigned i = 0; i < length(getOccurrences(it)); ++i)
            x += getOccurrences(it)[i].i2;
    };

    for (auto _ : state)
    {
        hitsNbr = 0;
        uniqueHits = 0;
        for (unsigned i = 0; i < length(reads); ++i)
        {
            uint64_t oldHits = hitsNbr;
            _findBacktracking(it.revIter, reads[i], begin(reads[i], Standard()), 0, (signed) maxErrors, delegate,
                              TDistanceTag());
            reverseComplement(reads[i]);
            _findBacktracking(it.revIter, reads[i], begin(reads[i], Standard()), 0, (signed) maxErrors, delegate,
                              TDistanceTag());
            benchmark::DoNotOptimize(uniqueHits += oldHits != hitsNbr);
        }
        // std::cout << "Backtracking: " << ((double)((time*100)/CLOCKS_PER_SEC)/100) << " s. "
        //           << "Hits: " << uniqueHits << " (" << hitsNbr << ")" << std::endl;
    }
}

template <class TSearchScheme>
void BM_HammingDistance(benchmark::State& state, TSearchScheme scheme)
{
    typedef HammingDistance TDistanceTag;

    TIter it(fm_index);
    _optimalSearchSchemeComputeFixedBlocklength(scheme, length(reads[0]));

    uint64_t hitsNbr, uniqueHits;
    auto delegate = [&hitsNbr](auto const &it, DnaString const & /*read*/, unsigned const errors = 0) {
        ++hitsNbr;
        unsigned x = 0;
        for (unsigned i = 0; i < length(getOccurrences(it)); ++i)
            x += getOccurrences(it)[i].i2;
    };

    for (auto _ : state)
    {
        hitsNbr = 0;
        uniqueHits = 0;
        for (unsigned i = 0; i < length(reads); ++i)
        {
            uint64_t oldHits = hitsNbr;
            _optimalSearchScheme(delegate, it, reads[i], scheme, TDistanceTag());
            reverseComplement(reads[i]);
            _optimalSearchScheme(delegate, it, reads[i], scheme, TDistanceTag());
            benchmark::DoNotOptimize(uniqueHits += oldHits != hitsNbr);
        }
        // std::cout << "Opt.-Schemes: " << ((double)((time*100)/CLOCKS_PER_SEC)/100) << " s. "
        //           << "Hits: " << uniqueHits << " (" << hitsNbr << ")" << std::endl;
    }
}

void BM_EditDistance(benchmark::State& state, uint8_t const maxErrors)
{
    typedef EditDistance TDistanceTag;

    TIter it(fm_index);

    uint64_t hitsNbr, uniqueHits;
    auto delegate = [&hitsNbr](auto const &it, DnaString const & /*read*/, unsigned const errors = 0) {
        ++hitsNbr;
        unsigned x = 0;
        for (unsigned i = 0; i < length(getOccurrences(it)); ++i)
            x += getOccurrences(it)[i].i2;
    };

    for (auto _ : state)
    {
        hitsNbr = 0;
        uniqueHits = 0;
        for (unsigned i = 0; i < length(reads); ++i)
        {
            uint64_t oldHits = hitsNbr;
            _findBacktracking(it.revIter, reads[i], begin(reads[i], Standard()), 0, (signed) maxErrors, delegate,
                              TDistanceTag());
            reverseComplement(reads[i]);
            _findBacktracking(it.revIter, reads[i], begin(reads[i], Standard()), 0, (signed) maxErrors, delegate,
                              TDistanceTag());
            benchmark::DoNotOptimize(uniqueHits += oldHits != hitsNbr);
        }
        // std::cout << "Backtracking: " << ((double)((time*100)/CLOCKS_PER_SEC)/100) << " s. "
        //           << "Hits: " << uniqueHits << " (" << hitsNbr << ")" << std::endl;
    }
}

template <class TSearchScheme>
void BM_EditDistance(benchmark::State& state, TSearchScheme scheme)
{
    typedef EditDistance TDistanceTag;

    TIter it(fm_index);
    _optimalSearchSchemeComputeFixedBlocklength(scheme, length(reads[0]));

    uint64_t hitsNbr, uniqueHits;
    auto delegate = [&hitsNbr](auto const &it, DnaString const & /*read*/, unsigned const errors = 0) {
        ++hitsNbr;
        unsigned x = 0;
        for (unsigned i = 0; i < length(getOccurrences(it)); ++i)
            x += getOccurrences(it)[i].i2;
    };

    for (auto _ : state)
    {
        hitsNbr = 0;
        uniqueHits = 0;
        for (unsigned i = 0; i < length(reads); ++i)
        {
            uint64_t oldHits = hitsNbr;
            _optimalSearchScheme(delegate, it, reads[i], scheme, TDistanceTag());
            reverseComplement(reads[i]);
            _optimalSearchScheme(delegate, it, reads[i], scheme, TDistanceTag());
            benchmark::DoNotOptimize(uniqueHits += oldHits != hitsNbr);
        }
        // std::cout << "Opt.-Schemes: " << ((double)((time*100)/CLOCKS_PER_SEC)/100) << " s. "
        //           << "Hits: " << uniqueHits << " (" << hitsNbr << ")" << std::endl;
    }
}

template <typename TIter, typename TText, typename TOptimalScheme, typename TDistanceTag>
inline bool runStrataBenchmarkStep(TIter & it, TText & read, TOptimalScheme const & scheme, TDistanceTag const & /**/)
{
    unsigned hitsNbr = 0;
    auto delegate = [&hitsNbr](TIter const & it, TText const & /*read*/, unsigned const /*errors*/) {
        ++hitsNbr;
        unsigned x = 0;
        for (unsigned i = 0; i < length(getOccurrences(it)); ++i)
            x += getOccurrences(it)[i].i2;
    };

    _optimalSearchScheme(delegate, it, read, scheme, TDistanceTag());
    reverseComplement(read);
    _optimalSearchScheme(delegate, it, read, scheme, TDistanceTag());

    return hitsNbr != 0;
}

void BM_EditDistanceStrata0(benchmark::State& state, uint8_t const maxErrors)
{
    typedef EditDistance TDistanceTag;

    TIter it(fm_index);

    auto s0 = OptimalSearchSchemes<0, 0>::VALUE;
    auto s1 = OptimalSearchSchemes<1, 1>::VALUE;
    auto s2 = OptimalSearchSchemes<2, 2>::VALUE;
    auto s3 = OptimalSearchSchemes<3, 3>::VALUE;
    auto s4 = OptimalSearchSchemes<4, 4>::VALUE;
    _optimalSearchSchemeComputeFixedBlocklength(s0, length(reads[0]));
    _optimalSearchSchemeComputeFixedBlocklength(s1, length(reads[0]));
    _optimalSearchSchemeComputeFixedBlocklength(s2, length(reads[0]));
    _optimalSearchSchemeComputeFixedBlocklength(s3, length(reads[0]));
    _optimalSearchSchemeComputeFixedBlocklength(s4, length(reads[0]));

    unsigned hits[5] = {};
    for (auto _ : state)
    {
        for (unsigned i = 0; i < length(reads); ++i)
        {
            if (runStrataBenchmarkStep(it, reads[i], s0, TDistanceTag()))
                ++hits[0];
            else if (maxErrors >= 1 && runStrataBenchmarkStep(it, reads[i], s1, TDistanceTag()))
                ++hits[1];
            else if (maxErrors >= 2 && runStrataBenchmarkStep(it, reads[i], s2, TDistanceTag()))
                ++hits[2];
            else if (maxErrors >= 3 && runStrataBenchmarkStep(it, reads[i], s3, TDistanceTag()))
                ++hits[3];
            else if (maxErrors >= 4 && runStrataBenchmarkStep(it, reads[i], s4, TDistanceTag()))
                ++hits[4];
        }
    }

    for (unsigned e = 0; e <= maxErrors; ++e)
    {
        if (hits[e] > 0)
            std::cout << "hits[" << e << "] = " << hits[e] << std::endl;
    }
}

template <typename TIter, typename TText, typename TOptimalScheme, typename TDistanceTag>
inline unsigned runStrataBenchmarkStep1(TIter & it, TText & read, TOptimalScheme const & scheme, TDistanceTag const & /**/)
{
    unsigned bestErrors = 10;
    auto delegate = [&bestErrors](TIter const & it, TText const & /*read*/, unsigned const errors) {
        bestErrors = std::min(errors, bestErrors);
        unsigned x = 0;
        for (unsigned i = 0; i < length(getOccurrences(it)); ++i)
            x += getOccurrences(it)[i].i2;
    };

    _optimalSearchScheme(delegate, it, read, scheme, TDistanceTag());
    reverseComplement(read);
    _optimalSearchScheme(delegate, it, read, scheme, TDistanceTag());

    return bestErrors;
}

void BM_EditDistanceStrata1(benchmark::State& state, uint8_t const maxErrors)
{
    typedef EditDistance TDistanceTag;

    TIter it(fm_index);

    auto s0_0 = OptimalSearchSchemes<0, 0>::VALUE;
    auto s0_1 = OptimalSearchSchemes<0, 1>::VALUE;
    auto s0_2 = OptimalSearchSchemes<0, 2>::VALUE;
    auto s2_2 = OptimalSearchSchemes<2, 2>::VALUE;
    auto s2_3 = OptimalSearchSchemes<2, 3>::VALUE;
    auto s4_4 = OptimalSearchSchemes<4, 4>::VALUE;
    _optimalSearchSchemeComputeFixedBlocklength(s0_0, length(reads[0]));
    _optimalSearchSchemeComputeFixedBlocklength(s0_1, length(reads[0]));
    _optimalSearchSchemeComputeFixedBlocklength(s0_2, length(reads[0]));
    _optimalSearchSchemeComputeFixedBlocklength(s2_2, length(reads[0]));
    _optimalSearchSchemeComputeFixedBlocklength(s2_3, length(reads[0]));
    _optimalSearchSchemeComputeFixedBlocklength(s4_4, length(reads[0]));

    unsigned tmp = 0;

    unsigned hits[5] = {};
    for (auto _ : state)
    {
        for (unsigned i = 0; i < length(reads); ++i)
        {
            unsigned best = runStrataBenchmarkStep1(it, reads[i], s0_1, TDistanceTag());
            if (best == 0)
                ++hits[0];
            else if (best == 1)
            {
                ++hits[1];
                if (maxErrors > 1)
                    tmp += runStrataBenchmarkStep1(it, reads[i], s2_2, TDistanceTag());
            }
            else if (maxErrors > 1)
            {
		if (maxErrors == 2)
                {
                    best = runStrataBenchmarkStep1(it, reads[i], s2_2, TDistanceTag());
                    if (best == 2)
                        ++hits[2];
                }
                else
                {
                    best = runStrataBenchmarkStep1(it, reads[i], s2_3, TDistanceTag());
                    if (best == 2)
                        ++hits[2];
                    else if (best == 3)
                    {
                        ++hits[3];
                        if (maxErrors > 3)
                            tmp += runStrataBenchmarkStep1(it, reads[i], s4_4, TDistanceTag());
                    }
                    else if (maxErrors == 4)
                    {
                        best = runStrataBenchmarkStep1(it, reads[i], s4_4, TDistanceTag());
                        if (best == 4)
                            ++hits[4];
                    }
                }
            }
        }
    }
    std::cout << "tmp: " << tmp << '\n';

    for (unsigned e = 0; e <= maxErrors; ++e)
    {
        if (hits[e] > 0)
            std::cout << "hits[" << e << "] = " << hits[e] << std::endl;
    }
}

BENCHMARK_CAPTURE(BM_EditDistanceStrata1, errors_1_strata_1, (uint8_t)1)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_EditDistanceStrata1, errors_2_strata_1, (uint8_t)2)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_EditDistanceStrata1, errors_3_strata_1, (uint8_t)3)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_EditDistanceStrata1, errors_4_strata_1, (uint8_t)4)->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_EditDistanceStrata0, errors_0_strata_0, (uint8_t)0)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_EditDistanceStrata0, errors_1_strata_0, (uint8_t)1)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_EditDistanceStrata0, errors_2_strata_0, (uint8_t)2)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_EditDistanceStrata0, errors_3_strata_0, (uint8_t)3)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_EditDistanceStrata0, errors_4_strata_0, (uint8_t)4)->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_HammingDistance, errors_1_backtracking  , (uint8_t)1)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_HammingDistance, errors_1_parts_k_plus_1, PaperOptimumSearchSchemes<1>::VALUE_plus_one)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_HammingDistance, errors_1_parts_k_plus_2, PaperOptimumSearchSchemes<1>::VALUE_plus_two)->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_HammingDistance, errors_2_backtracking  , (uint8_t)2)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_HammingDistance, errors_2_parts_k_plus_1, PaperOptimumSearchSchemes<2>::VALUE_plus_one)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_HammingDistance, errors_2_parts_k_plus_2, PaperOptimumSearchSchemes<2>::VALUE_plus_two)->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_HammingDistance, errors_3_backtracking  , (uint8_t)3)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_HammingDistance, errors_3_parts_k_plus_1, PaperOptimumSearchSchemes<3>::VALUE_plus_one)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_HammingDistance, errors_3_parts_k_plus_2, PaperOptimumSearchSchemes<3>::VALUE_plus_two)->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_EditDistance, errors_1_backtracking  , (uint8_t)1)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_EditDistance, errors_1_parts_k_plus_1, PaperOptimumSearchSchemes<1>::VALUE_plus_one)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_EditDistance, errors_1_parts_k_plus_2, PaperOptimumSearchSchemes<1>::VALUE_plus_two)->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_EditDistance, errors_2_backtracking  , (uint8_t)2)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_EditDistance, errors_2_parts_k_plus_1, PaperOptimumSearchSchemes<2>::VALUE_plus_one)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_EditDistance, errors_2_parts_k_plus_2, PaperOptimumSearchSchemes<2>::VALUE_plus_two)->Unit(benchmark::kMillisecond);

BENCHMARK_CAPTURE(BM_EditDistance, errors_3_backtracking  , (uint8_t)3)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_EditDistance, errors_3_parts_k_plus_1, PaperOptimumSearchSchemes<3>::VALUE_plus_one)->Unit(benchmark::kMillisecond);
BENCHMARK_CAPTURE(BM_EditDistance, errors_3_parts_k_plus_2, PaperOptimumSearchSchemes<3>::VALUE_plus_two)->Unit(benchmark::kMillisecond);

// BENCHMARK_MAIN();
int main(int argc, char** argv)
{
    // Argument parser
    ArgumentParser parser("SearchSchemes - Benchmarking");
    addDescription(parser,
        "App for creating the benchmark of Optimum Search Schemes from the paper.");

    addOption(parser, ArgParseOption("G", "genome", "Path to the indexed genome", ArgParseArgument::INPUT_FILE, "IN"));
	setRequired(parser, "genome");

    addOption(parser, ArgParseOption("R", "reads", "Path to the reads", ArgParseArgument::INPUT_FILE, "IN"));
	setValidValues(parser, "reads", "fa fasta fastq");
	setRequired(parser, "reads");

    ArgumentParser::ParseResult res = parse(parser, argc, argv);
    if (res != ArgumentParser::PARSE_OK)
        return res == ArgumentParser::PARSE_ERROR;

    // Retrieve input parameters
    CharString indexPath, readsPath;
    getOptionValue(indexPath, parser, "genome");
    getOptionValue(readsPath, parser, "reads");

    open(fm_index, toCString(indexPath), OPEN_RDONLY);
    StringSet<CharString> ids;
    SeqFileIn seqFileIn(toCString(readsPath));
    readRecords(ids, reads, seqFileIn);

    for (unsigned i = 1; i < length(reads); ++i)
    {
        if (length(reads[i]) != length(reads[0]))
        {
            std::cerr << "ERROR: Not all reads have the same length." << std::endl;
            return 1;
        }
    }

    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
}
