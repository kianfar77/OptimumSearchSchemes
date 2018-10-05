__________________________________________________________________________________________________

This program generates optimal search schemes of the following paper\
"Optimum Search Schemes for Approximate String Matching Using Bidirectional FM-Index".\
This program is meant to be used for generating the results of above paper and is not for commercial use. 

__________________________________________________________________________________________________

The MIP can have different solutions with the same objective values. 
It is also possible that multiple runs of one problem, which do not reach optimality, i.e., run time equals to run time limit)
result in slightly different solutions and objective values.

## There are two options to run the program in Linux: run the executive file or build from source code. 
For both options first execute the following:

```sh
   $ git clone https://github.com/kianfar77/OptimumSearchSchemes.git
   $ cd OptimumSearchSchemes/MIPCode/
```

### Run the executive file
* The executables have been generated using CPLEX 12.7.1.
* Before running the program, make sure there is a folder named "output" in application directory.
* Do not delete or modify parameters.txt
* In application directory, type the following commands in terminal

```sh
   $ chmod +x EqualFix
   $ ./EqualFix -s <upperbound on number of searches> -p <number of parts> -k <maximum error> -r <read length> [-t <time limit>][--lowerK= <minimum number of errors>][--lp][--verbose][--sigma][-h|--help]
```

### Build from source code
* Make sure you have the following files in "libs" folder (you can acquire the latest version on [IBM download page for students/faculties](https://developer.ibm.com/docloud/blog/2016/11/24/cos-12-7-ai/ "IBM"), see the section "required libraries" below)
1. cplex.h
2. cpxconst.h
3. libcplex.a
4. libilocplex.a
5. libcplexdistmip.a
* Before running the program, make sure there is a folder named "output" in source directory.
* In source files' directory, type the following commands in terminal

```sh
   $ make
   $ ./EqualFix -s <upperbound on number of searches> -p <number of parts> -k <maximum error> -r <read length> [-t <time limit>][--lowerK= <minimum number of errors>][--lp][--verbose][--sigma][-h|--help]
```
## Required libraries
* CPLEX 12.7.1 is used for the paper. If not avaialbele, the latest academic version, i.e., CPLEX 12.8, availble at [IBM download page for students/faculties](https://developer.ibm.com/docloud/blog/2016/11/24/cos-12-7-ai/ "IBM"), can be used with no significant difference in run times.

* Install CPLEX under super user

```sh
   $ chmod +x cplex_file_you_have_downloaded.bin
   $ ./cplex_file_you_have_downloaded.bin
```
* After installations, say, in the path /opt/ibm/ILOG/CPLEX_Studio1271,
	* cplex.h and cpxconst.h would be located at\
	  /opt/ibm/ILOG/CPLEX_Studio1271/cplex/include/ilcplex
	* libcplex.a, libilocplex.a, and libcplexdistmip.a would be located at\
	  /opt/ibm/ILOG/CPLEX_Studio1271/cplex/lib/x86-64_linux/static_pic/

## Arguments of the program:


|               |                                                                                                                          | 
|---------------|------------------------------------------------------------------------------------------------------------------------- | 
|-s:            |Upperbound on the number of searches. [required]						                   	   |
|-p:            |Number of parts (partitions). [required]								           	   |
|-k:            |Number of maximum errors. [required]	           	                                           		 	   |
|-r:            |Length of read. [required]	               							                  	   |
|-t:            |Upperbound on run time. [optional, defualt= 3.0 hours]	               					           	   |
|--lowerK:      |Number of minimum errors. [optional, defualt= 0 ]	               				                           |
|--lp:          |Prints out the MIP formulation. [optional]	               								   |
|--verbose:     |Prints out the progress of objective value, rows starting with * correspond to integer feasible solutions. [optional]	   |
|--sigma:	|Alphabet size. [optional, defualt= 4 ]	               							                   |
|-h,  --help:	|Displays help.	               							                                           |

### Usage

```sh
   $ ./EqualFix --help
   $ ./EqualFix -s 3 -p 3 -k 2 -r 12
   $ ./EqualFix -s 3 -p 3 -k 2 -r 12 -t 0.5
   $ ./EqualFix -s 3 -p 3 -k 2 -r 12 -t 0.5 --verbose --lowerK=1 --sigma=2
   $ ./EqualFix -s 3 -p 3 -k 2 -r 12 -t 0.5 --verbose --lp > ./output/verbose.txt
```

## Output:

* The objective value and execution time (only for solver) will be printed on the screen.
* The program generates "output.txt" in "output" folder.
* "output.txt" contains sigma (alphabet size), S (upper bound on number of searches), P (number of parts), 
  lowerK (minimum number of errors), K (maximum number of errors), R (read length), run time (sec), and Objective 
  (number of edges in all tries). Also for each search, there are U's (upper bounds on errors in different parts), x's 
  (the order in which a search processes the parts), and L's (lower bounds on errors in different read's parts). 
  There would be P elements in x's , L's and U's. First element of x shows the partition (part) being searched at
  iteration 1, second element of x shows the partition covered at iteration 2, and so forth. Similarly for L and U, 
  the ith elements represent the number of errors in the ith element of x, 
  the part of the read, processed at ith iteration. In other words, the ith elements of x, L, and U are associated with iteration i.
* The program generates "LP.lp" in "output" folder if --lp is recieved. "LP.lp" contains the mixed integer program.
* If detailed verbose is requested, The progress of feasible solutions toward the optimal solution will be
  printed on the screen (results in a longer run time).
  In order to save detailed verbose messages you may run the program by typing
  ./EqualFix -s <upperbound on number of searches> -p <number of parts> -k <maximum error> -r <read length>  > ./output/verbose.txt.
  In detailed verbose setting, rows starting with "*" represent a feasible integer solution. By looking at those 
  rows one can observe how fast the program converges to the optimal solution (see verboseExample.png).

### output_for_some_optimal_runs.txt

This file contains the optimal search schemes for some instances we had run for the paper. Please notice that there might be some empty searches in which
upper bound on errors is greater than lower bound on errors for some parts. 

### parameters_used_for_runs.txt

This file contains the input parameters that we had used for generating graphs and tables in the paper.










