#include <seqan/arg_parse.h>

#include "common.h"
#include "paper_optimum_schemes.h"

using namespace seqan;

template <uint8_t errors, typename TDistanceTag>
void output_count(unsigned const read_length, TDistanceTag /**/)
{
    std::cout << "K = " << (unsigned)errors << ", Trivial: ";
    std::cout << countTrivialSearch<Dna>(read_length, errors, TDistanceTag()) << '\n';

    // K+1 parts
    auto scheme1 = PaperOptimumSearchSchemes<errors>::VALUE_plus_one;
    _optimalSearchSchemeComputeFixedBlocklength(scheme1, read_length);
    std::cout << "K = " << (unsigned)errors << ", P = K+" << 1 << ": "
              << countSearchScheme<Dna>(read_length, scheme1, TDistanceTag()) << '\n';
    // check_OSS(scheme1);

    // K+2 parts
    auto scheme2 = PaperOptimumSearchSchemes<errors>::VALUE_plus_two;
    _optimalSearchSchemeComputeFixedBlocklength(scheme2, read_length);
    std::cout << "K = " << (unsigned)errors << ", P = K+" << 2 << ": "
              << countSearchScheme<Dna>(read_length, scheme2, TDistanceTag()) << '\n';
    // check_OSS(scheme2);

    // K+3 parts
    auto scheme3 = PaperOptimumSearchSchemes<errors>::VALUE_plus_three;
    _optimalSearchSchemeComputeFixedBlocklength(scheme3, read_length);
    std::cout << "K = " << (unsigned)errors << ", P = K+" << 3 << ": "
              << countSearchScheme<Dna>(read_length, scheme3, TDistanceTag()) << '\n';
    // check_OSS(scheme3);
}

int main(int argc, char *argv[])
{
    // Argument Parser
    ArgumentParser parser("Counting edges");
    addDescription(parser, "App for couting edges for trivial backtracking and Optimum Search Schemes.");

    addOption(parser, ArgParseOption("R", "read-length", "Length of the read", ArgParseArgument::INTEGER, "INT"));
    setDefaultValue(parser, "read-length", 101);

    ArgumentParser::ParseResult res = parse(parser, argc, argv);
    if (res != ArgumentParser::PARSE_OK)
        return res == ArgumentParser::PARSE_ERROR;

    // Retrieve input parameters
    unsigned read_length;
    getOptionValue(read_length, parser, "read-length");

    std::cout << "*** Hamming Distance ***\n";
    output_count<1>(read_length, HammingDistance());
    output_count<2>(read_length, HammingDistance());
    output_count<3>(read_length, HammingDistance());
    output_count<4>(read_length, HammingDistance());
    std::cout << "*** Edit Distance ***\n";
    output_count<1>(read_length, EditDistance());
    output_count<2>(read_length, EditDistance());
    output_count<3>(read_length, EditDistance());
    output_count<4>(read_length, EditDistance());

    return 0;
}
