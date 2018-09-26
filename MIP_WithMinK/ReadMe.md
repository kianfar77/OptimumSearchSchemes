Search Schemes
==============

How to build the project
------------------------

```sh
   $ cd libs
   $ tar -zxf libcplex.a.tar.gz
   $ cd ..
   $ make
   $ ./EqualFix
```

This code generates optimal search schemes. To  run different cases, modify the numbers (S, P, minK, K, R, and sigma) in the input file. 
The input file is a text file (default: input.txt).There is no need to change variable names.
There should be one number per line and the first number represents S, the second, third, forth, fifth , sisxth, and seventh represent P, minK, K, R, sigma, and T, respectively. T is the length of genome and there is no need to change that. The last number shows the number of variables and it should be 10. The rest of the file is the name of varibles. ReadMe.xlsx contains more detial on the input file.
Output:
The objective value and wall time (only for the solver) will be printed on the screen. Press "y" to exit.
Program generates an output text file (default: outX.txt) which contains: sigma, S, P, K, R, Obj, time(sec), input file, x's, L's, and U's. 
There are S of x's , S of L's and S of U's. First x, first L , and first U are corrosponding x , L, and U for search 1.

