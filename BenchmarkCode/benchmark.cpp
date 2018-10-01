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
