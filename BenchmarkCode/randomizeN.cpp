#include <seqan/arg_parse.h>
#include <seqan/seq_io.h>

using namespace std;
using namespace seqan;

inline void randomizeNs(Dna5String & str)
{
    mt19937 rng(42);

    typedef typename Value<Dna5String>::Type               TAlphabet;
    typedef typename Iterator<Dna5String, Standard>::Type  TIter;

    TIter it = begin(str, Standard());
    TIter itEnd = end(str, Standard());

    while (it != itEnd)
    {
        if (value(it) == Dna5('N'))
            value(it) = rng() % ValueSize<Dna>::VALUE;
        ++it;
    }
}
template <typename TSpec, typename TString, typename TSpec2>
inline void addReverseReads(StringSet<CharString, TSpec> & ids, StringSet<TString, TSpec2> & s)
{
    unsigned seqTotal = length(s);
    for (unsigned seqNo = 0; seqNo < seqTotal; ++seqNo)
    {
        appendValue(ids, ids[seqNo]);
        appendValue(s, s[seqNo]);
        reverseComplement(s[seqNo]);
    }
}

int main(int argc, char *argv[])
{
    // Argument Parser
    ArgumentParser parser("SearchSchemes - Fasta Randomization of Ns");
    addDescription(parser, "App for randomly replacing N's by A, C, G T in fasta files.");

    addOption(parser, ArgParseOption("I", "in", "Path to fasta (.fa, .fasta)", ArgParseArgument::INPUT_FILE, "IN"));
	setValidValues(parser, "in", "fa fasta fastq");
	setRequired(parser, "in");

    addOption(parser, ArgParseOption("O", "out", "Path to fasta (.fa, .fasta)", ArgParseArgument::OUTPUT_FILE, "OUT"));
	setValidValues(parser, "out", "fa fasta fastq");
	setRequired(parser, "out");

    addOption(parser, ArgParseOption("R", "reverse", "Add reversed sequences to fasta"));

    ArgumentParser::ParseResult res = parse(parser, argc, argv);
    if (res != ArgumentParser::PARSE_OK)
        return res == ArgumentParser::PARSE_ERROR;

    typedef Dna5String TString;

    // Retrieve input parameters
    CharString in, out;
    getOptionValue(in, parser, "in");
    getOptionValue(out, parser, "out");
    bool reverseSequences = isSet(parser, "reverse");

    // Read fasta input file
    StringSet<CharString> ids;
    StringSet<TString> genome;
    SeqFileIn seqFileIn(toCString(in));
    readRecords(ids, genome, seqFileIn);
    close(seqFileIn);

    cout << "Randomization started." << endl;
    #pragma omp parallel for
    for (unsigned i = 0; i < length(genome); ++i)
        randomizeNs(genome[i]);
    cout << "Randomization finished." << endl;

    if (reverseSequences)
    {
        cout << "Reversing started." << endl;
        addReverseReads(ids, genome);
        cout << "Reversing finished." << endl;
    }

    SeqFileOut seqFileOut(toCString(out));
    writeRecords(seqFileOut, ids, genome);
    close(seqFileOut);

    cout << "Done." << endl;

    return 0;
}
