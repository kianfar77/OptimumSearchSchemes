#include "common.h"

#include <seqan/arg_parse.h>
#include <seqan/index.h>
#include <seqan/seq_io.h>

#include <ctime>

using namespace seqan;

struct Options
{
    unsigned minErrors;
    unsigned maxErrors;
    unsigned iterations;
    bool indels;
    bool fullBenchmark;
    bool strata;
    bool countEdges;
    bool optimalBlocklengths;
    CharString indexPath;
    CharString readsPath;
};

template <typename TIter, typename TText, typename TOptimalScheme, typename TDistanceTag>
inline bool runStrataBenchmarkStep(TIter & it, TText & read, TOptimalScheme & scheme, TDistanceTag const & /**/)
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

template <typename TIter, typename TText, typename TDistanceTag>
inline void runStrataBenchmark(TIter & it, StringSet<TText> & reads, unsigned const maxErrors, TDistanceTag const & /**/)
{
    // TODO: choose correct lower error bound!
    auto s0 = OptimalSearchSchemes<0, 0>::VALUE;
    auto s1 = OptimalSearchSchemes<0, 1>::VALUE;
    auto s2 = OptimalSearchSchemes<0, 2>::VALUE;
    auto s3 = OptimalSearchSchemes<0, 3>::VALUE;
    auto s4 = OptimalSearchSchemes<0, 4>::VALUE;
    _optimalSearchSchemeComputeFixedBlocklength(s0, length(reads[0]));
    _optimalSearchSchemeComputeFixedBlocklength(s1, length(reads[0]));
    _optimalSearchSchemeComputeFixedBlocklength(s2, length(reads[0]));
    _optimalSearchSchemeComputeFixedBlocklength(s3, length(reads[0]));
    _optimalSearchSchemeComputeFixedBlocklength(s4, length(reads[0]));

    std::cout << "Errors: " << maxErrors << ", "
              << "Indels: " << (std::is_same<TDistanceTag, EditDistance>::value ? "True" : "False") << std::endl;

    unsigned hits[5] = {};
    clock_t time = clock();
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
    time = clock() - time;
    std::cout << "Schemes: " << ((double)((time*100)/CLOCKS_PER_SEC)/100) << " s." << std::endl;
    for (unsigned e = 0; e <= maxErrors; ++e)
    {
        std::cout << "hits[" << e << "] = " << hits[e] << std::endl; // TODO: values differ from paper after refactoring
    }
}

template <typename TIter,
          typename TText,
          size_t nbrBlocks, size_t N,
          typename TDistanceTag>
inline void runBenchmark(TIter & it,
                         StringSet<TText> & reads,
                         std::array<OptimalSearch<nbrBlocks>, N> scheme,
                         unsigned const minErrors,
                         unsigned const maxErrors,
                         bool const countEdges,
                         bool const optimalBlocklengths,
                         TDistanceTag const & /**/)
{
    std::cout << "Errors: " << minErrors << "-" << maxErrors << ", " <<
                 "Indels: " << (std::is_same<TDistanceTag, EditDistance>::value ? "True" : "False");

    if (optimalBlocklengths)
        _optimalSearchSchemeComputeOptimalBlocklength<typename Value<TText>::Type>(scheme, maxErrors, length(reads[0]));
    else
        _optimalSearchSchemeComputeFixedBlocklength(scheme, length(reads[0]));

    if (countEdges)
    {
        std::cout << ", Edges (Backtracking vs. Scheme): "
                  << countTrivialSearch<typename Value<TText>::Type>(length(reads[0]), maxErrors, TDistanceTag())
                  << " vs. "
                  << countSearchScheme<typename Value<TText>::Type>(length(reads[0]), scheme, TDistanceTag());
    }
    std::cout << std::endl;

    unsigned oldHits, hitsNbr = 0;
    unsigned uniqueHits = 0;
    auto delegate = [&hitsNbr](auto const &it, TText const & /*read*/, unsigned const errors = 0) {
        ++hitsNbr;
        unsigned x = 0;
        for (unsigned i = 0; i < length(getOccurrences(it)); ++i)
            x += getOccurrences(it)[i].i2;
    };

    clock_t time = clock(); // TODO: don't use clock()
    for (unsigned i = 0; i < length(reads); ++i)
    {
        oldHits = hitsNbr;
        _optimalSearchScheme(delegate, it, reads[i], scheme, TDistanceTag());
        reverseComplement(reads[i]);
        _optimalSearchScheme(delegate, it, reads[i], scheme, TDistanceTag());
        uniqueHits += oldHits != hitsNbr;
    }
    time = clock() - time;
    std::cout << "Opt.-Schemes: " << ((double)((time*100)/CLOCKS_PER_SEC)/100) << " s. "
              << "Hits: " << uniqueHits << " (" << hitsNbr << ")" << std::endl;

    if (minErrors == 0)
    {
        hitsNbr = 0;
        uniqueHits = 0;
        time = clock();
        for (unsigned i = 0; i < length(reads); ++i)
        {
            oldHits = hitsNbr;
            _findBacktracking(it.revIter, reads[i], begin(reads[i], Standard()), 0, (signed) maxErrors, delegate,
                              TDistanceTag());
            reverseComplement(reads[i]);
            _findBacktracking(it.revIter, reads[i], begin(reads[i], Standard()), 0, (signed) maxErrors, delegate,
                              TDistanceTag());
            uniqueHits += oldHits != hitsNbr;
        }
        time = clock() - time;
        std::cout << "Backtracking: " << ((double)((time*100)/CLOCKS_PER_SEC)/100) << " s. "
                  << "Hits: " << uniqueHits << " (" << hitsNbr << ")" << std::endl;
    }
}

template <typename TIter, typename TStringSet,
          typename TDistanceTag>
inline int runIndels(TIter & it, TStringSet & reads, unsigned const minErrors, unsigned const maxErrors,
                     bool const countEdges, bool const optimalBlocklengths, TDistanceTag const & /**/)
{
    switch (minErrors)
    {
        case 0: {
            switch (maxErrors)
            {
                case 0: runBenchmark(it, reads, OptimalSearchSchemes<0, 0>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                case 1: runBenchmark(it, reads, OptimalSearchSchemes<0, 1>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                case 2: runBenchmark(it, reads, OptimalSearchSchemes<0, 2>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                case 3: runBenchmark(it, reads, OptimalSearchSchemes<0, 3>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                case 4: runBenchmark(it, reads, OptimalSearchSchemes<0, 4>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                default: std::cerr << "Optimal Search Schemes only support 0-4 maximum errors." << std::endl; return 1; break;
            }
        }
        case 1: {
            switch (maxErrors)
            {
                case 1: runBenchmark(it, reads, OptimalSearchSchemes<1, 1>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                case 2: runBenchmark(it, reads, OptimalSearchSchemes<1, 2>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                case 3: runBenchmark(it, reads, OptimalSearchSchemes<1, 3>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                case 4: runBenchmark(it, reads, OptimalSearchSchemes<1, 4>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                default: std::cerr << "Optimal Search Schemes only support 0-4 maximum errors." << std::endl; return 1; break;
            }
        }
        case 2: {
            switch (maxErrors)
            {
                case 2: runBenchmark(it, reads, OptimalSearchSchemes<2, 2>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                case 3: runBenchmark(it, reads, OptimalSearchSchemes<2, 3>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                case 4: runBenchmark(it, reads, OptimalSearchSchemes<2, 4>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                default: std::cerr << "Optimal Search Schemes only support 0-4 maximum errors." << std::endl; return 1; break;
            }
        }
        case 3: {
            switch (maxErrors)
            {
                case 3: runBenchmark(it, reads, OptimalSearchSchemes<3, 3>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                case 4: runBenchmark(it, reads, OptimalSearchSchemes<3, 4>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                default: std::cerr << "Optimal Search Schemes only support 0-4 maximum errors." << std::endl; return 1; break;
            }
        }
        case 4: {
            switch (maxErrors)
            {
                case 4: runBenchmark(it, reads, OptimalSearchSchemes<4, 4>::VALUE, minErrors, maxErrors, countEdges, optimalBlocklengths, TDistanceTag()); return 0; break;
                default: std::cerr << "Optimal Search Schemes only support 0-4 maximum errors." << std::endl; return 1; break;
            }
        }
        default: std::cerr << "Optimal Search Schemes only support 0-4 minimum errors" << std::endl; return 1; break;
    }
}

template <typename TChar, typename TAlloc>
inline int run(Options const & opt)
{
    // Index configuration
    typedef StringSet<String<TChar, TAlloc>, Owner<ConcatDirect<> > > TText;
    typedef Index<TText, TIndexConfig> TIndex;
    typedef Iter<TIndex, VSTree<TopDown<> > > TIter;

    // Open index
    StringSet<CharString> ids;
    StringSet<String<TChar> > reads;
    SeqFileIn seqFileIn(toCString(opt.readsPath));
    readRecords(ids, reads, seqFileIn);
    clear(ids);

    for (unsigned i = 1; i < length(reads); ++i)
    {
        if (length(reads[i]) != length(reads[0]))
        {
            std::cerr << "ERROR: Not all reads have the same length." << std::endl;
            return 1;
        }
    }
    std::cout << "Reads loaded (total: " << length(reads) << " of length " << length(reads[0]) << ")." << std::endl;

    // TODO: use this code for measuring times (works for multiple threads as well)
    // struct timeval t1, t2;
    // gettimeofday(&t1, NULL);
    // ...
    // gettimeofday(&t2, NULL);
    // auto time = ((t2.tv_sec - t1.tv_sec) * 1000000u + t2.tv_usec - t1.tv_usec) / 1.e6;

    clock_t time = clock();
    TIndex index;
    open(index, toCString(opt.indexPath), OPEN_RDONLY);
    TIter it(index);
    time = clock() - time;
    std::cout << "Index loaded in " << ((double)((time*10)/CLOCKS_PER_SEC)/10) << "s." << std::endl << std::endl;

    if (opt.strata)
    {
        std::cout << "Explanation (Strata-Mode):" << std::endl
                  << "Hits = number of reads mapped"
                  << std::endl << std::endl;

        for (unsigned iterations = 1; iterations <= opt.iterations; ++iterations)
        {
            std::cout << "Iteration #" << iterations << std::endl;
            if (opt.indels)
                runStrataBenchmark(it, reads, opt.maxErrors, EditDistance());
            else
                runStrataBenchmark(it, reads, opt.maxErrors, HammingDistance());
        }

        return 0;
    }

    std::cout << "Explanation:" << std::endl
              << "Hits = number of reads mapped" << std::endl
              << "Values in brackets = number of calls of the delegate function (not really of interest)"
              << std::endl << std::endl;

    for (unsigned iterations = 1; iterations <= opt.iterations; ++iterations)
    {
        std::cout << "Iteration #" << iterations << std::endl;
        for (unsigned maxErrors = !opt.fullBenchmark * opt.maxErrors; maxErrors <= opt.maxErrors; ++maxErrors)
        {
            for (unsigned minErrors = !opt.fullBenchmark * opt.minErrors; minErrors <= opt.minErrors; ++minErrors)
            {
                if (!opt.indels || opt.fullBenchmark)
                {
                    if (runIndels(it, reads, minErrors, maxErrors, opt.countEdges, opt.optimalBlocklengths,
                        HammingDistance()))
                            return 1;
                }
                if (opt.indels)
                {
                    if (runIndels(it, reads, minErrors, maxErrors, opt.countEdges, opt.optimalBlocklengths,
                        EditDistance()))
                            return 1;
                }
            }
        }
        std::cout << ". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ." << std::endl << std::endl;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    // Argument parser
    ArgumentParser parser("SearchSchemes - Benchmarking");
    addDescription(parser,
        "App for benchmarking the running time of search schemes. Only supports Dna4 so far "
        "(everything else than ACGT will be converted to A). All reads must have the same length.");

    addOption(parser, ArgParseOption("G", "genome", "Path to the indexed genome", ArgParseArgument::INPUT_FILE, "IN"));
	setRequired(parser, "genome");

    addOption(parser, ArgParseOption("R", "reads", "Path to the reads", ArgParseArgument::INPUT_FILE, "IN"));
	setValidValues(parser, "reads", "fa fasta fastq");
	setRequired(parser, "reads");

    addOption(parser, ArgParseOption("maxE", "maxErrors", "Maximum number of errors.", ArgParseArgument::INTEGER,
        "INT"));

    addOption(parser, ArgParseOption("minE", "minErrors",
        "Minimum number of errors (not used by trivial backtracking).", ArgParseArgument::INTEGER, "INT"));
    setDefaultValue(parser, "minErrors", "0");

    addOption(parser, ArgParseOption("it", "iterations", "Number of times the entire benchmark is performed.",
        ArgParseArgument::INTEGER, "INT"));
    setDefaultValue(parser, "iterations", "1");

    addOption(parser, ArgParseOption("i", "indels", "Turns on indels (EditDistance)."
        "If not selected, only mismatches will be considered."));

    addOption(parser, ArgParseOption("f", "full",
        "Performs extensive benchmark. --maxErrors, --minErrors, --indels and --optimal will be treated as upper bounds"
        " and all possible combinations of these values will be benchmarked."));

    addOption(parser, ArgParseOption("c", "count",
        "Counts edges in theoretical model (based on read length and alphabet type from input file). "
        "Caution: Results for indels are still experimental!"));

    addOption(parser, ArgParseOption("o", "optimal",
        "Chooses theoretically optimal blocklengths to minimize the number of edges!"));

    addOption(parser, ArgParseOption("s", "strata",
        "Strata-Mode. Ignores the following arguments: -f -c -o -minE"));

    addOption(parser, ArgParseOption("m", "memmap",
        "Turns memory-mapping on, i.e. the index is not loaded into RAM before the benchmark but accessed directly in "
        "secondary-memory during the benchmark. This makes the benchmark only slightly slower but the index does not "
        "have to be loaded into main memory (which takes some time),"));

    ArgumentParser::ParseResult res = parse(parser, argc, argv);
    if (res != ArgumentParser::PARSE_OK)
        return res == ArgumentParser::PARSE_ERROR;

    // Retrieve input parameters
    unsigned minErrors, maxErrors, iterations;
    getOptionValue(maxErrors, parser, "maxErrors");
    getOptionValue(minErrors, parser, "minErrors");
    getOptionValue(iterations, parser, "iterations");
    bool indels = isSet(parser, "indels");
    bool fullBenchmark = isSet(parser, "full");
    bool countEdges = isSet(parser, "count");
    bool optimalBlocklengths = isSet(parser, "optimal");
    bool memoryMapping = isSet(parser, "memmap");
    bool strata = isSet(parser, "strata");
    CharString indexPath, readsPath;
    getOptionValue(indexPath, parser, "genome");
    getOptionValue(readsPath, parser, "reads");

    if (minErrors > maxErrors)
    {
        std::cerr << "The number of minimum errors cannot exceed the number of maximum errors." << std::endl;
        return 1;
    }

    Options opt {minErrors, maxErrors, iterations, indels, fullBenchmark, strata, countEdges, optimalBlocklengths, indexPath,
        readsPath};

    if (memoryMapping)
        return run<Dna, MMap<> >(opt);
    else
        return run<Dna, Alloc<> >(opt);
}
