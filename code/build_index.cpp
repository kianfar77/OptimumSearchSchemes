#include "common.h"

#include <seqan/arg_parse.h>
#include <seqan/index.h>
#include <seqan/seq_io.h>

using namespace std;
using namespace seqan;

int main(int argc, char *argv[])
{
    typedef Index<StringSet<DnaString, Owner<ConcatDirect<> > >, TIndexConfig> TIndex;

    // Argument Parser
    ArgumentParser parser("SearchSchemes - Index Creation for Benchmark");
    addDescription(parser, "App for building an index (that is needed for the benchmark app). Only supports Dna so far. At most 4 GB.");

    addOption(parser, ArgParseOption("G", "genome", "Path to the genome (.fa, .fasta)", ArgParseArgument::INPUT_FILE, "IN"));
	setValidValues(parser, "genome", "fa fasta");
	setRequired(parser, "genome");

    addOption(parser, ArgParseOption("I", "index", "Path to the index", ArgParseArgument::OUTPUT_FILE, "OUT"));
	setRequired(parser, "index");

    ArgumentParser::ParseResult res = parse(parser, argc, argv);
    if (res != ArgumentParser::PARSE_OK)
        return res == ArgumentParser::PARSE_ERROR;

    // Retrieve input parameters
    CharString genomePath, indexPath;
    getOptionValue(genomePath, parser, "genome");
    getOptionValue(indexPath, parser, "index");

    // Read fasta input file
    StringSet<CharString> ids;
    StringSet<DnaString, Owner<ConcatDirect<> > > genome;
    SeqFileIn seqFileIn(toCString(genomePath));
    readRecords(ids, genome, seqFileIn);

    cout << "Number of sequences: " << length(genome) << endl;

    // Build and save index
    TIndex index(genome);
    indexCreate(index, FibreSALF());
    save(index, toCString(indexPath));

    cout << "Index created successfully" << endl;

    return 0;
}
