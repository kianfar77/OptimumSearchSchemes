#ifndef COMMONOBJECTS_H
#define COMMONOBJECTS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <cplex.h>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <chrono>

using namespace std;

const int NAMELENGTH = 32;

struct RowRec
{
	 int nzcnt;
     double rhs[1];
	 char sense[1];
	 int rmatbeg[1];
	 int* rmatind;
	 double* rmatval;

	 void Allocate(int noneZero){
		rmatind = new (nothrow) int [noneZero];
		if (rmatind  == 0)
			cout<<"error in allocating memory to row.rmatind."<<endl;
		rmatval = new (nothrow) double [noneZero];
		if (rmatval  == 0)
			cout<<"error in allocating memory to row.rmatval."<<endl;
	 }
	 void Dellocate(){
		delete[] rmatind;
		delete[] rmatval;
	 }

};

struct IndicesRec
{
	int s;
	int l;
	int d;
	int i;
	int q;
};

struct searchParameter
{
	int64_t T;
	int R;
	int K;
	int sigma;
	char **colnames;
    int lowerK; //
    int Verbose;
    double Time;
    int isLP;
};

class RowFormat{

public:

	void openCplex();
	void createProblem(char* );
	void addVariable();
	void addConstraint();
	void addConstraint(RowRec );
	void removeConstraint(int );
	void removeConstraint(int , int );
	void optimize(bool );
	void getSolution();
	void printProblem(const char* );
	void printX(ofstream&  , const searchParameter& , const IndicesRec& , double , const char* );
	void rowName( char* , char , char , char , int , int , int , bool , bool , bool);
	void rowName( char* , char , char , int , int , bool , bool );
	void rowName( char* , char , int , bool);
	void rowName(char* );
	void Obj( const IndicesRec& , const searchParameter& , bool );
	double time() const;
	void terminate();
	void PrintObjValue( const IndicesRec& , const searchParameter& ) ;
	int Status() const;
	int Solstat() const;
	bool CutLoop() const;
    void Param(int, double) const;
	RowFormat(bool , int , int , const searchParameter& , IndicesRec , const int* , int , bool);
	~RowFormat();


private:

	int status;
	CPXENVptr env;
	CPXLPptr problem;
	bool min;
	int ncols;
	int nrows;
	char *ctype;
	char **colnames;
	char **rownames;
	int nzcntA;
	int max;
	char *sense;
	double *obj;
	double *x;
	double* lb;
	double* ub;
	double *rhs;
	int *rmatbegA;
	int *rmatcntA;
	int *rmatindA;
	double *rmatvalA;
	int rmatspaceA;
	int spacesizeA;
	double *soln;
	double objval;
	int adj;
	int solstat;
	double runTime;
};

int64_t IntegerPower(int64_t , uint8_t );
void readInput(ifstream& , char* , IndicesRec& , searchParameter&);
int NumDigit(int);
int nCk(int, int);
void DynamicNestedLoopStriclyLess(const size_t, const size_t , size_t , size_t*  , size_t&);
int64_t Round(double);


#endif
