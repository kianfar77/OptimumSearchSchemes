#include "commonObjects.h"
#include "master.h"
#include <getopt.h>

using namespace std;

int parameterParser(int argc, char* argv[], IndicesRec& index , searchParameter& masterGeneralParameters)
{
    int val=-1;
    masterGeneralParameters.lowerK=0;
    masterGeneralParameters.Time=3;
    masterGeneralParameters.Verbose=0;
    masterGeneralParameters.isLP=0;
    masterGeneralParameters.sigma=4;
    
    static struct option long_options[] =
    {
        {"lowerK", optional_argument, NULL, 0},
        {"verbose", optional_argument, NULL, 0},
        {"lp", optional_argument, NULL, 0},
        {"sigma", optional_argument, NULL, 0},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    
    struct isParmeters
    {
        bool isS=false;
        bool isP=false;
        bool isK=false;
        bool isR=false;
        bool isSufficient(){if(isS&&isP&&isK&&isR) return true; else return false;};
    };
    isParmeters must;
    
    
    auto printUsage = []() {
    
        cout<<endl<<"Options:"<<endl;
        cout<<"-s:            Upperbound on the number of searches. [required]"<<endl;
        cout<<"-p:            Number of parts (partitions). [required]"<<endl;
        cout<<"-k:            Number of maximum errors. [required]"<<endl;
        cout<<"-r:            Length of read. [required]"<<endl;
        cout<<"-t:            Upperbound on run time. [optional, defualt= 3.0 hours]"<<endl;
        cout<<"--lowerK:      Number of minimum errors. [optional, defualt= 0 ]"<<endl;
        cout<<"--lp:          Prints out the MIP formulation. [optional]"<<endl;
        cout<<"--verbose:     Prints out the progress of objective value, rows starting with * are integer feasible solutions. [optional]"<<endl;
        cout<<"--sigma:       Alphabet size. [optional, defualt= 4]"<<endl;
        cout<<"-h, --help:    Displays help."<<endl;
        
        cout<<endl<<"Usage:"<<endl;
        cout<<"./EqualFix -s 3 -p 3 -k 2 -r 12"<<endl;
        cout<<"./EqualFix -s 3 -p 3 -k 2 -r 12 -t 0.5"<<endl;
        cout<<"./EqualFix -s 3 -p 3 -k 2 -r 12 -t 0.5 --verbose --lowerK=1 --sigma=2"<<endl;
        cout<<"./EqualFix -s 3 -p 3 -k 2 -r 12 -t 0.5 --verbose --lp  > ./output/verbose.txt"<<endl<<endl;
    };
    
    
    if(argc == 1)
        return 1;
    int option_index = 0;
    while ((val = getopt_long(argc, argv, "s:p:k:r:t:h", long_options, &option_index)) != -1)
    {
        switch (val)
        {
            case 's':
                index.s = atoi(optarg); 
                must.isS=true;
                break;
            case 'p':
                index.i = atoi(optarg);
                must.isP=true;
                break;
            case 'k':
                masterGeneralParameters.K = atoi(optarg);
                must.isK=true;
                break;
            case 'r':
                masterGeneralParameters.R = atoi(optarg);
                must.isR=true;
                break;
            case 't':
                masterGeneralParameters.Time = atof(optarg);
                break;
            case 0:
                if(strcmp("lowerK", long_options[option_index].name)==0)
                    masterGeneralParameters.lowerK = atoi(optarg);
                if(strcmp("verbose", long_options[option_index].name)==0)
                    masterGeneralParameters.Verbose=1;
                if(strcmp("lp", long_options[option_index].name)==0)
                    masterGeneralParameters.isLP=1;
                if(strcmp("sigma", long_options[option_index].name)==0)
                    masterGeneralParameters.sigma = atoi(optarg);
                if(strcmp("help", long_options[option_index].name)==0)
                    printUsage();
                break;
            case 'h': 
                printUsage(); 
                return 1;
            case ':': 
                printUsage(); 
                return 1;
            case '?': 
                printUsage(); 
                return 1;
            default: 
                printUsage(); 
                return 1;
        }
    }
    if(must.isSufficient())
        return 0;
    else 
    {
        cout<<endl<<"Insufficient number of options was provided."<<endl;
        printUsage();
        return 1;
    }
}

int main(int argc, char* argv[])
{
	bool istrue;
	double runTime=0;
	int masterNumberOfVariables, masterNumberOfConstraints, temp;
	size_t seudoIndexesQ;
	size_t q;
	size_t mismatchCursor=0;
	size_t counter;
	size_t partition;
	int* m = new int [1];
	size_t* mismatch;
	char* fileAddressI=NULL;
	char* fileAddressX=NULL;
	const char *cstr;
	string str;
	ifstream inMYFILE;
	ofstream AllLxuFILE;
	IndicesRec index;
	searchParameter masterGeneralParameters;
    
    int res = parameterParser( argc, argv, index , masterGeneralParameters);
    if (res != 0) 
        return 1;

    fileAddressI= "parameters.txt";
    fileAddressX="./output/output.txt";
    
    cout<<"======================================================================================================"<<endl;
    cout<<"This program generates optimal search schemes of the following paper"<<endl;
    cout<<"\"Optimum Search Schemes for Approximate String Matching Using Bidirectional FM-Index\"."<<endl;
    cout<<"This program meant to be used for generating the results of above paper and is not for commercial use."<<endl;
    cout<<"------------------------------------------------------------------------------------------------------"<<endl;
	
	readInput(inMYFILE, fileAddressI, index, masterGeneralParameters);
	partition = index.i;
	q=0;
	for (int h=masterGeneralParameters.lowerK; h<masterGeneralParameters.K+1; ++h) 
		q += nCk(h + partition-1, h);
	index.q=q;
	m[0]=(masterGeneralParameters.R/index.i);
	masterNumberOfVariables =   3 * index.s * index.l *( index.d+1)  +   index.s * index.i * index.i + 2 * index.s * index.i *( index.i+1) + 2* index.s * index.i +( index.q) * index.s *  index.i + (index.q) * index.s ;
	int masterVarGroupsNumber[] = { index.s * index.l *( index.d+1) ,  index.s * index.i * index.i ,  index.s * index.i * ( index.i+1) , index.s * index.i *( index.i+1) , index.s * index.i ,  index.s * index.i,
		index.s * index.l *( index.d+1) , index.s * index.l *( index.d+1) , (index.q) * index.s *  index.i , ( index.q) * index.s };
	masterNumberOfConstraints = 2* index.s * index.i + index.s * (index.i-2) * (index.i+1)+ index.s * (index.i-2) +  2* index.s * index.i*index.l + 3* index.s * index.l*(index.d+1) + 2*index.s*(index.i-1) ;
	RowFormat masterProblem (true, masterNumberOfVariables, masterNumberOfConstraints,  masterGeneralParameters, index, masterVarGroupsNumber, 15, true);
	masterProblem.openCplex();
	masterProblem.createProblem("MasterSearch");
    masterProblem.Param(masterGeneralParameters.Verbose, masterGeneralParameters.Time);
	MasterObjCoefficients( masterProblem,  index, masterGeneralParameters);
	masterProblem.addVariable();
	PopulateMasterProblem( masterProblem,  index, m, masterGeneralParameters );
	mismatch =  new (nothrow) size_t [q*partition];
	DynamicNestedLoopStriclyLess(masterGeneralParameters.lowerK, masterGeneralParameters.K, partition, mismatch, counter); 
	for (int i=0; i<q; ++i)
	{
		AddConstraintToMaster(masterProblem, index, masterGeneralParameters, mismatch, mismatchCursor);
		++mismatchCursor;
	}
	delete[] mismatch;
	if( (IntegerPower(masterGeneralParameters.sigma-1, masterGeneralParameters.K) * nCk(index.l , masterGeneralParameters.K) * 2) < ULLONG_MAX )
		istrue=true;
	else
	{
		istrue=false;
		cout<<"coefficient is too big"<<endl;
	}
    if(masterGeneralParameters.isLP == 1)
        masterProblem.printProblem("./output/LP.lp");
	cout<<"Optimizing ..."<<endl<<endl;
	masterProblem.optimize(false);
	runTime = masterProblem.time();
	masterProblem.getSolution();
	cout<<"------------------------------------------------------------------------------------------------------"<<endl;
	masterProblem.PrintObjValue(index, masterGeneralParameters);
	cout<<"Run time (sec): "<<runTime<<endl;
	cout<<"======================================================================================================"<<endl;
	AllLxuFILE.open(fileAddressX);
	masterProblem.printX(AllLxuFILE, masterGeneralParameters, index,runTime, fileAddressI);
	AllLxuFILE.close();
	masterProblem.terminate();

	return 0;
}
