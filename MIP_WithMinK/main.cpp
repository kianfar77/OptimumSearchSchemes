#include "commonObjects.h"
#include "master.h"

using namespace std;

int main(int argc, char* argv[])
{
	bool isTrue;
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
	ofstream AllFILE;
	IndicesRec index;
	searchParameter masterGeneralParameters, subGeneralParameters;

	switch (argc)
	{
		case 3:
			fileAddressI= argv[1];
			fileAddressX=argv[3];
			break;

		default:
			fileAddressI= "input.txt";
			fileAddressX="./output/outX.txt";
	}
	readInput(inMYFILE, fileAddressI, index, masterGeneralParameters, subGeneralParameters);
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
		isTrue=true;
	else
	{
		isTrue=false;
		cout<<"coefficient is too big"<<endl;
	}

	cout<<"Optimizing ..."<<endl<<endl;
	masterProblem.optimize(false);
	runTime = masterProblem.time();
	masterProblem.getSolution();
	cout<<"====================================================================="<<endl;
	masterProblem.PrintObjValue(index, masterGeneralParameters);
	cout<<"Run time (sec): "<<runTime<<endl;
	cout<<"====================================================================="<<endl;
	AllFILE.open(fileAddressX);
	masterProblem.printX(AllFILE, masterGeneralParameters, index,runTime, fileAddressI);
	AllFILE.close();
	masterProblem.terminate();

	if(argc == 1)
	{
		cout<<"Quit? (y/n)" <<endl;
		char _check_='n';
		while (!(_check_=='y' || _check_=='Y' ))
		cin.get(_check_);
	}
	return 0;
}
