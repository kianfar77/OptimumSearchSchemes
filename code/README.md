Search Schemes
==============

How to build the project
------------------------

```sh
   $ git clone git@github.com:cpockrandt/search_schemes.git --recursive
   $ mkdir search_schemes-build && cd search_schemes-build
   $ cmake ../search_schemes/code -DCMAKE_BUILD_TYPE=Release
   $ make -j
```

Documentation
-------------

All binaries document possible flags that you can get with --help, e.g. './benchmark_create_index --help'

How to run benchmarks
---------------------

First you need to create an index of a FASTA-file. Currently only DNA4 is supported, i.e. all characters other than A, C, G or T will be implicitly converted to A (this also holds for N). The fasta file can hold arbitrarly many sequences but the total length cannot exceed 4 gigabases. To randomly replace N by any Dna character, please run the app randomizeN.

```sh
   ./benchmark_create_index -G file_to_genome.fasta -I /path/to/index/indexname
```

The index is built using secondary memory. If you're getting a runtime error, you're most likely runing out of disk space or quota. You can change the TMPRDIR environment variable TMPRDIR on UNIX systems (and TEMP on Windows).

```sh
   $ export TMPDIR=/somewhere/else/with/more/space
```

After building the index, you can run the benchmark executable:

```sh
   $ ./benchmark -G /path/to/index/indexname -R /path/to/reads.fasta .....
```

For a complete list of all arguments, please check the help function './benchmark --help'.

