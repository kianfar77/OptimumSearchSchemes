# Benchmarking optimum search schemes for approximate string matching found by mixed intgeer programming
__________________________________________________________________________________________________

This program/code benchmarks the optimum search schemes for approximate string matching using bidirectional FM-index. The results of specific benchmarking experiments are presebted in the paper *Kianfar, K., Pockrandt, C., Torkamandi, B., Luo, H., Reinert, K., Optimum Search Schemes for Approximate String Matching Using Bidirectional FM-Index, 2018*.\
Any commercial use is prohibited.

__________________________________________________________________________________________________

How to build the project
------------------------

```sh
   $ git clone https://github.com/kianfar77/OptimumSearchSchemes.git --recurse-submodules
   $ mkdir OptimumSearchSchemes-build && cd OptimumSearchSchemes-build
   $ cmake ../OptimumSearchSchemes/BenchmarkCode -DCMAKE_BUILD_TYPE=Release
   $ make -j
```

Documentation
-------------

All binaries document possible flags that you can get with --help, e.g. './benchmark_create_index --help'

How to run benchmarks
---------------------

### Test data set

To run the benchmarks from the paper, you can download the test data set. It includes the human genome hg38 without N bases, a prebuilt index of that genome and as well as the Illumina reads.

You can find everything on http://ftp.mi.fu-berlin.de/pub/SeqAn/mappability/


### Building an index ###

First you need to create an index of a FASTA-file. Currently only DNA4 is supported, i.e. all characters other than A, C, G or T will be implicitly converted to A (this also holds for N). The fasta file can hold arbitrarly many sequences but the total length cannot exceed 4 gigabases. To randomly replace N by any Dna character, please run the app randomizeN.

```sh
   ./build_index -G file_to_genome.fasta -I /path/to/index/indexname
```

The index is built using secondary memory. If you're getting a runtime error, you're most likely runing out of disk space or quota. You can change the TMPRDIR environment variable TMPRDIR on UNIX systems (and TEMP on Windows).

```sh
   $ export TMPDIR=/somewhere/else/with/more/space
   $ SET TEMP=C:\somewhere\else\with\more\space
```

### Running the benchmark ###

After building the index, you can run the benchmark suite, which generates results of Table 2 in the paper (the run times are system dependent and may vary from system to system). The suite is based on Google Benchmark and will download and build Google Benchmark in your build directory:

```sh
   $ ./benchmark -G /path/to/index/indexname -R /path/to/reads.fasta
```

### Counting edges ###

This executable will computes the results in Table 1 of the paper, i.e., the total number of edges in the search scheme tries (the objective function values) for the optimum search schemes from the paper:

```sh
   ./count_edges
```

K is the number of errors, P is the number of parts for Optimum Search Schemes. "Trivial" refers to simple backtracking.

Detailed benchmarking
---------------------

To run single benchmarks (i.e. for a specific error number and distance metric and other search modes such as strata), you can use the detailed benchmark.

For a complete list of all arguments, please check the help function './detailed_benchmark --help'.


