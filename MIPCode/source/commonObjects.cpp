#include "commonObjects.h"

void RowFormat::openCplex()
{
	env = CPXopenCPLEX (&status);
	if ( env == NULL )
	{
		char errmsg[1024];
		fprintf (stderr, "Could not open CPLEX environment.\n");
		CPXgeterrorstring (env, status, errmsg);
		fprintf (stderr, "%s", errmsg);
		RowFormat::terminate();
	}
}

void RowFormat::createProblem(char* problemName)
{
    problem = CPXcreateprob (env, &status, problemName);
	if ( problem == NULL )
	{
		fprintf (stderr, "Failed to create problem.\n");
		RowFormat::terminate();
	}

	if (min)
		CPXchgobjsen (env, problem, CPX_MIN);
	else
		CPXchgobjsen (env, problem, CPX_MAX);
}

void RowFormat::addVariable()
{
	status = CPXnewcols (env, problem, ncols, obj, lb, ub, ctype, colnames);
	if ( status )
	{
		fprintf (stderr, "Failed to add variable.\n");
		RowFormat::terminate();
	}
}

void RowFormat::addConstraint()
{
	status = CPXaddrows (env, problem, 0, nrows, nzcntA, rhs, sense, rmatbegA, rmatindA, rmatvalA, NULL, rownames); //rownames
	if ( status )
	{
		fprintf (stderr, "Failed to add constraint.\n");
		RowFormat::terminate();
	}
}

void RowFormat::addConstraint(RowRec row)
{
	status = CPXaddrows (env, problem, 0, 1, row.nzcnt, row.rhs, row.sense, row.rmatbeg /* 0 */, row.rmatind, row.rmatval, NULL, rownames); //rownames
	if ( status )
	{
		fprintf (stderr, "Failed to add a constraint.\n");
		RowFormat::terminate();
	}
}

void RowFormat::removeConstraint(int row)
{
	status = CPXdelrows( env, problem, row, row);
	if ( status )
	{
		fprintf (stderr, "Failed to delete constraints.\n");
		RowFormat::terminate();
	}
}

void RowFormat::removeConstraint(int begin, int end)
{
	status = CPXdelrows( env, problem, begin, end);
	if ( status )
	{
		fprintf (stderr, "Failed to delete the constraint.\n");
		RowFormat::terminate();
	}
}

void RowFormat::optimize(bool isLinearProgramming)
{
   	using namespace std::chrono;

	steady_clock::time_point t1, t2;
	if(isLinearProgramming)
	{

		t1=steady_clock::now();
		status = CPXlpopt (env, problem);
		t2=steady_clock::now();
		if ( status )
		{
			fprintf (stderr, "Failed to optimize LP.\n");
			RowFormat::terminate();
		}
	}
	else
	{
		t1=steady_clock::now();
		status = CPXmipopt (env, problem);
		t2=steady_clock::now();
		if (status != 0) cout<<"Status @ CPXmipopt(optimize): "<<status<<endl;
		if ( status )
		{
			fprintf (stderr, "Failed to optimize MIP.\n");
			RowFormat::terminate();
		}
	}
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	runTime= time_span.count();

}

//
void RowFormat::getSolution()
{
	status = CPXsolution (env, problem, &solstat, &objval, x, NULL, NULL, NULL);
	if((solstat != 101) && (solstat != 102))
	{
		cout<<"solstat @ CPXsolution @ getSolution: "<<solstat<<endl;
		cout<<"check out this website: "<<endl<<"https://www.ibm.com/support/knowledgecenter/SSSA5P_12.6.3/ilog.odms.cplex.help/refcallablelibrary/macros/Solution_status_codes.html"<<endl;
	}
	if ( status ) {
	fprintf (stderr, "Failed to obtain solution.\n");
	cout<<"status @ CPXsolution @ getSolution: "<<status<<endl;
	cout<<"check out this website: "<<endl<<"https://www.ibm.com/support/knowledgecenter/SSSA5P_12.5.1/ilog.odms.cplex.help/refcallablelibrary/html/optim.cplex.errorcodes/group.html"<<endl;
	RowFormat::terminate();
	}
}
//

void RowFormat::printProblem(const char* fname)
{
	status = CPXwriteprob (env, problem, fname, NULL);
	if ( status ) {
	fprintf (stderr, "Failed to write problem.\n");
	RowFormat::terminate();
	}
}


void RowFormat::terminate()
{
	if ( problem != NULL )
	{
		status = CPXfreeprob (env, &problem);
		if ( status )
			fprintf (stderr, "CPXfreeprob failed, error code %d.\n", status);
	}
	if ( env != NULL )
	{
		status = CPXcloseCPLEX (&env);
		if ( status )
		{
			char errmsg[1024];
			fprintf (stderr, "Could not close CPLEX environment.\n");
			CPXgeterrorstring (env, status, errmsg);
			fprintf (stderr, "%s", errmsg);
		}
	}
}


RowFormat::RowFormat(bool min_, int ncols_, int nrows_, const searchParameter& parameter, IndicesRec index, const int* varTypeNum, int size, bool isMaster)
{
	size_t iteratorI, boundI,  iteratorII, boundII;

	status = 0;
	env = NULL;
	problem = NULL;
	min = min_;
	ncols = ncols_;
	nrows = nrows_;
	nzcntA = 0;
	sense = NULL;
	rhs = NULL;

	ctype = new (nothrow) char [ncols_];
	if (ctype==0)
		cout<<"error allocating ctype."<<endl;
	obj = new (nothrow) double[ncols_];
	if (obj==0)
		cout<<"error allocating obj."<<endl;
	x = new (nothrow) double [ncols_];
	if (x==0)
		cout<<"error allocating x."<<endl;
	lb = new (nothrow) double[ncols_];
	if (lb==0)
		cout<<"error allocating lb."<<endl;
	ub = new (nothrow) double[ncols_];
	if (ub==0)
		cout<<"error allocating ub."<<endl;

	colnames = new char*[ncols_];
	int tempArr[5]={index.d+1, index.i, index.l, index.q, index.s};
	sort(tempArr, tempArr + 5);
	max = NumDigit(tempArr[4]) +  NumDigit(tempArr[3]) +  NumDigit(tempArr[2]) + 1;
	for(int i = 0; i < ncols_; ++i)
		colnames[i] = new char[NAMELENGTH + 2* max];

	rownames = new char*[1];
	rownames[0] = new char[NAMELENGTH + 2* max];

	rmatbegA = NULL;
	rmatcntA = NULL;
	rmatindA = NULL;
	rmatvalA = NULL;

	soln = NULL;
	objval = 0;
	solstat = -1;
	rmatspaceA =0;
	spacesizeA=0;
	runTime=0;
	adj = 0;

	if(isMaster)
	{

		iteratorI = 0;
		boundI = index.s * index.l * (index.d+1);
		for (; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'C';
			ub [iteratorI] = CPX_INFBOUND;
			lb [iteratorI] = 0;
		}
		boundI += index.s * index.i * index.i;
		for ( ; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'B';
			ub [iteratorI] = 1;
			lb [iteratorI] = 0;
		}
		boundI += index.s * index.i * (index.i+1);
		for ( ; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'B';
			ub [iteratorI] = 1;
			lb [iteratorI] = 0;
		}
		boundI += index.s * index.i * (index.i+1);
		for ( ; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'B';
			ub [iteratorI] = 1;
			lb [iteratorI] = 0;
		}
		boundI += index.s * index.i;
		for ( ; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'I';
			ub [iteratorI] = parameter.K;
			lb [iteratorI] = 0;
		}
		boundI += index.s * index.i;
		for ( ; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'I';
			ub [iteratorI] = parameter.K;
			lb [iteratorI] = 0;
		}
		boundI += index.s * index.l * (index.d+1);
		for (; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'B';
			ub [iteratorI] = 1;
			lb [iteratorI] = 0;
		}
		boundI += index.s * index.l * (index.d+1);
		for (; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'B';
			ub [iteratorI] = 1;
			lb [iteratorI] = 0;
		}
		boundI += (index.q) * index.s * index.i;
		for (; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'C';
			ub [iteratorI] = parameter.K;
			lb [iteratorI] = 0;
		}
		boundI += (index.q) * index.s;
		for (; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'B';
			ub [iteratorI] = 1;
			lb [iteratorI] = 0;
		}


		iteratorII=0;
		for(int s=0; s<index.s; ++s)
			for(int l=0; l<index.l; ++l)
				for(int d=0; d<(index.d+1); ++d)
				{
					stringstream ss;
					string str;
					ss <<   parameter.colnames[0] << s+1 << l+1 << d;
					str = ss.str();
					strcpy (colnames[iteratorII], str.c_str());
					++iteratorII;
				}
		for(int s=0; s<index.s; ++s)
			for(int i=0; i<index.i; ++i)
				for(int j=0; j<index.i; ++j)
				{
					stringstream ss;
					string str;
					ss << parameter.colnames[1] << s+1 << i+1 << j+1;
					str =ss.str();
					strcpy (colnames[iteratorII], str.c_str());
					++iteratorII;
				}
		for(int s=0; s<index.s; ++s)
			for(int i=0; i<index.i; ++i)
				for(int j=0; j<index.i+1; ++j)
				{
					stringstream ss;
					string str;
					ss << parameter.colnames[2] << s+1 << i+1 << j+1;
					str =ss.str();
					strcpy (colnames[iteratorII], str.c_str());
					++iteratorII;
				}
		for(int s=0; s<index.s; ++s)
			for(int i=0; i<index.i; ++i)
				for(int j=0; j<index.i+1; ++j)
				{
					stringstream ss;
					string str;
					ss << parameter.colnames[3] << s+1 << i+1 << j+1;
					str =ss.str();
					strcpy (colnames[iteratorII], str.c_str());
					++iteratorII;
				}
		for(int s=0; s<index.s; ++s)
			for(int i=0; i<index.i; ++i)
			{
				stringstream ss;
				string str;
				ss << parameter.colnames[4] << s+1 << i+1 ;
				str =ss.str();
				strcpy (colnames[iteratorII], str.c_str());
				++iteratorII;
			}
		for(int s=0; s<index.s; ++s)
			for(int i=0; i<index.i; ++i)
			{
				stringstream ss;
				string str;
				ss << parameter.colnames[5] << s+1 << i+1 ;
				str =ss.str();
				strcpy (colnames[iteratorII], str.c_str());
				++iteratorII;
			}
		for(int s=0; s<index.s; ++s)
			for(int l=0; l<index.l; ++l)
				for(int d=0; d<(index.d+1); ++d)
				{
					stringstream ss;
					string str;
					ss << parameter.colnames[6] << s+1 << l+1 << d;
					str =ss.str();

					strcpy (colnames[iteratorII], str.c_str());
					++iteratorII;
				}
		for(int s=0; s<index.s; ++s)
			for(int l=0; l<index.l; ++l)
				for(int d=0; d<(index.d+1); ++d)
				{
					stringstream ss;
					string str;
					ss << parameter.colnames[7] << s+1 << l+1 << d;
					str =ss.str();
					strcpy (colnames[iteratorII], str.c_str());
					++iteratorII;
				}
		for(int q=0; q<(index.q); ++q)
			for(int s=0; s<index.s; ++s)
				for(int i=0; i<index.i; ++i)
				{
					stringstream ss;
					string str;
					ss << parameter.colnames[8] << q << s+1 << i+1;
					str =ss.str();
					strcpy (colnames[iteratorII], str.c_str());
					++iteratorII;
				}
		for(int q=0; q<(index.q); ++q)
			for(int s=0; s<index.s; ++s)
			{
				stringstream ss;
					string str;
				ss << parameter.colnames[9] << q << s+1;
				str =ss.str();
				strcpy (colnames[iteratorII], str.c_str());
				++iteratorII;
			}
	}
	else
	{
		iteratorI = 0;
		boundI =  index.i;
		for (; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'I';
			ub [iteratorI] = CPX_INFBOUND;
			lb [iteratorI] = 0;
		}
		boundI += index.s * index.i;
		for ( ; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'I';
			ub [iteratorI] = CPX_INFBOUND;
			lb [iteratorI] = 0;
		}
		boundI += index.s * index.i;
		for ( ; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'B';
			ub [iteratorI] = 1;
			lb [iteratorI] = 0;
		}
		boundI += index.s * index.i;
		for ( ; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'B';
			ub [iteratorI] = 1;
			lb [iteratorI] = 0;
		}
		boundI += 1;
		for ( ; iteratorI < boundI ; ++iteratorI)
		{
			ctype[iteratorI] = 'I';
			ub [iteratorI] = CPX_INFBOUND;
			lb [iteratorI] = 0;
		}


		iteratorII=0;
		for(int i=0; i<index.i; ++i)
		{
			stringstream ss;
			string str;
			ss <<   parameter.colnames[0] << i+1 ;
			str = ss.str();
			strcpy (colnames[iteratorII], str.c_str());
			++iteratorII;
		}
		for(int s=0; s<index.s; ++s)
			for(int i=0; i<index.i; ++i)
			{
				stringstream ss;
				string str;
				ss << parameter.colnames[1] << s+1 << i+1;
				str =ss.str();
				strcpy (colnames[iteratorII], str.c_str());
				++iteratorII;
			}
		for(int s=0; s<index.s; ++s)
			for(int i=0; i<index.i; ++i)
			{
				stringstream ss;
				string str;
				ss << parameter.colnames[2] << s+1 << i+1;
				str =ss.str();
				strcpy (colnames[iteratorII], str.c_str());
				++iteratorII;
			}
		for(int s=0; s<index.s; ++s)
			for(int i=0; i<index.i; ++i)
			{
				stringstream ss;
				string str;
				ss << parameter.colnames[3] << s+1 << i+1;
				str =ss.str();
				strcpy (colnames[iteratorII], str.c_str());
				++iteratorII;
			}
		{
			stringstream ss;
			string str;
			ss << parameter.colnames[4];
			str =ss.str();
			strcpy (colnames[iteratorII], str.c_str());
			++iteratorII;
		}
	}

}

RowFormat::~RowFormat()
{
	delete [] ctype;
	delete [] obj;
	delete [] sense;
	delete [] rhs;
	delete [] rmatindA;
	delete [] rmatvalA;
	delete [] rmatbegA;
	delete [] rmatcntA;

	for(int i = 0; i < ncols; ++i)
		delete [] colnames[i];
	delete [] colnames;

	if ( problem != NULL )
	{
		status = CPXfreeprob (env, &problem);
		if ( status )
			fprintf (stderr, "CPXfreeprob failed, error code %d.\n", status);
	}
}


int64_t IntegerPower(int64_t bNum, uint8_t power) 
 {
	if(power > 30) 
	{
	 	cout<<"over flow"<<endl;
		return 0;
	}
	int64_t output = 1;
	static const uint8_t bitSet[] = 
	{
        0, 1, 2, 2, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 32
    };

    switch (bitSet[power])
	{
		case 32:
			if (bNum == 1) 
				return 1;
			if (bNum == -1)
				return 1 - 2 * (power & 1);
			return 0;

		case 5:
			if (power & 1) 
				output *= bNum;
			power >>= 1;
			bNum *= bNum;
			
		case 4:
			if (power & 1) 
				output *= bNum;
			power >>= 1;
			bNum *= bNum;
			
		case 3:
			if (power & 1) 
				output *= bNum;
			power >>= 1;
			bNum *= bNum;
			
		case 2:
			if (power & 1) 
				output *= bNum;
			power >>= 1;
			bNum *= bNum;
			
		case 1:
			if (power & 1) 
				output *= bNum;
				
		default:
			return  output;
    }
}

void readInput(ifstream& inMYFILE, char* fileAddress, IndicesRec& index, searchParameter& masterParam)
{
	int masterNumVar, subNumVar, j;
	inMYFILE.open(fileAddress);
	if (!inMYFILE)
	{
		cerr<< " Cannot read the file."<<endl;
		exit(1);
	}
	index.d=masterParam.K;
    index.l= masterParam.R;
    
	inMYFILE>> masterParam.T;
	inMYFILE >> masterNumVar;
	inMYFILE.ignore (numeric_limits<streamsize>::max(), '\n');
	masterParam.colnames = new char* [masterNumVar];
    for (int i=0; i<masterNumVar; ++i)
	{
		masterParam.colnames[i] = new char[NAMELENGTH];
		inMYFILE.getline(masterParam.colnames[i], NAMELENGTH);
		j=0;
		while(j < NAMELENGTH && masterParam.colnames[i][j] != '\0')
            ++j;
        masterParam.colnames[i][j-1]='\0';

	}
	inMYFILE.close();
}

int NumDigit( int n)
{
    int count = 0;

    while(n != 0)
    {
        n /= 10;
        ++count;
    }

    return count;
}
void RowFormat::printX( ofstream& AllLxuFILE , const searchParameter& masterParam, const IndicesRec& index, double runTime, const char* input)
{
	int cursor =0;
	double realX;
	double* tempX = new (nothrow) double [index.i];
	double* tempL = new (nothrow) double [index.i];
	double* tempU = new (nothrow) double [index.i];
	//int nodecount=CPXgetnodecnt (env, problem);

    AllLxuFILE<< "sigma "<<'\t'<< "S "<<'\t'<< "P "<<'\t'<< "lowerK "<<'\t'<< "K "<<'\t'<< "R "<<'\t'<< "run time(sec) "<<'\t'<< "objective "<<endl;
	AllLxuFILE<<masterParam.sigma<<'\t'<< index.s<<'\t'<< index.i<<'\t'<< masterParam.lowerK<<'\t'<< masterParam.K<<'\t'<< masterParam.R<<'\t'<<runTime<<'\t'<<'\t'<< objval<<endl<<endl;
	for(int s=0; s<index.s; ++s)
	{
		for (int i=0; i<index.i; ++i)
		{
			realX=0;
			for (int j=0; j<index.i; ++j)
			{
				cursor = index.s * index.l * (index.d+1) /*for n_sld*/+ s * index.i * index.i + index.i * i + j;
				realX += (j+1)*x[cursor];
			}
			tempX[i]=realX;
			cursor =  index.s * index.l *( index.d+1) /*for n_sld*/+ (index.s * index.i * index.i) /*for x_sij*/+2*(index.s * index.i * (index.i+1)) /*for t+_sij, t-_sij*/ + s *index.i + i;
			tempL[i]=x[cursor];
			cursor = index.s * index.l *( index.d+1) /*for n_sld*/+ (index.s * index.i * index.i) /*for x_sij*/+2*(index.s * index.i * (index.i+1)) /*for t+_sij, t-_sij*/ +1* index.s *index.i /*for , L_si*/+ s *index.i + i;
			tempU[i]=x[cursor];
		}
		AllLxuFILE<< "s	"<< s <<"	"<<endl;
		AllLxuFILE<< "U	"<<"	";
		for (int i=0; i<index.i; ++i)
			AllLxuFILE<<tempU[i]<<"	";
		AllLxuFILE<< endl;
		AllLxuFILE<< "X	"<<"	";
		for (int i=0; i<index.i; ++i)
			AllLxuFILE<<tempX[i]<<"	";
		AllLxuFILE<< endl;
		AllLxuFILE<< "L	"<<"	";
		for (int i=0; i<index.i; ++i)
			AllLxuFILE<<tempL[i]<<"	";
		AllLxuFILE<< endl<< endl;
	}

	delete[] tempX;
	delete[] tempL;
	delete[] tempU;
}


void RowFormat::rowName( char* groupName, char ic, char jc, char kc, int i, int j, int k, bool a, bool b, bool c)
{
	int ii=i;
	int jj=j;
	int kk=k;
	if (a) ii=i+1;
	if (b) jj=j+1;
	if (c) kk=k+1;

	stringstream ss;
	string str;
	ss << groupName<<','<< ic <<jc<<kc<<','<<  ii <<','<< jj <<','<< kk;
	str = ss.str();
	strcpy (rownames[0], str.c_str());

}

void RowFormat::rowName( char* groupName, char ic, char jc, int i, int j, bool a, bool b)
{
	int ii=i;
	int jj=j;
	if (a) ii=i+1;
	if (b) jj=j+1;

	stringstream ss;
	string str;
	ss << groupName<<','<< ic <<jc<<','<<  ii <<','<< jj;
	str = ss.str();
	strcpy (rownames[0], str.c_str());
}

void RowFormat::rowName( char* groupName, char ic, int i, bool a)
{
	int ii=i;
	if (a) ii=i+1;

	stringstream ss;
	string str;
	ss << groupName<<','<<ic<<','<< ii;
	str = ss.str();
	strcpy (rownames[0], str.c_str());
}

void RowFormat::rowName(char* groupName)
{

	stringstream ss;
	string str;
	ss << groupName;
	str = ss.str();
	strcpy (rownames[0], str.c_str());
}

void RowFormat::Obj(const IndicesRec& index, const searchParameter& parameter, bool isMaster)
{
	double temp;
	int count=0;
	double sigmaToL=1;

	if(isMaster)
	{
		for (int l=0; l< index.l; l++)
		{

			for (int s=0; s< index.s; s++)
			{
				for (int d=0; d< index.d+1 ; d++)
					obj[s * index.l *(index.d+1) + l * (index.d+1) + d]= 1;
			}
		}
		for (int i = index.s * index.l * (index.d +1); i< ncols; i++)
			obj[i]= 0;
	}
	else
	{
		count=index.i + 3 * index.s *index.i + 0;
		obj[count]= 1;
		for (int i =0; i< count; i++)
			obj[i]= 0;
	}

}
double RowFormat::time() const
{
	return runTime;
}


void RowFormat::PrintObjValue( const IndicesRec& indexes, const searchParameter& masterParam)
{
	int U,L,mu ,nullS;

	nullS=0;
	for(int s=0; s< indexes.s; ++s)
		for (int i=0;i<indexes.i; ++i)
		{
			L=indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1)) + s *indexes.i + i;
			U=indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1)) +1* indexes.s *indexes.i + s *indexes.i + i ;
			if(x[U]-x[L]< -0.0001)
			{
				++nullS;
				mu=(i+1)*(masterParam.R/indexes.i);
				adj += mu -indexes.d;
				break;
			}
		}
	cout<< std::fixed <<"Obj value = " << Round(objval-adj) <<endl;
}

int RowFormat::Status() const
{
	return status;
}
int64_t Round(double x)
{
    if (x < 0.0)
        return (int)(x - 0.5);
    else
        return (int)(x + 0.5);
}
int RowFormat::Solstat() const
{
	return solstat;
}

bool RowFormat::CutLoop() const
{
	return (objval<= 0.000001 && objval >= -0.000001);
}

int nCk(int n, int k)
{
  size_t tmp1, tmp2, tmp;
  if (k==0 || k==n)
    return 1;
  if (k>n)
	  return 0;

  if (k<13)
  {
	tmp1=1;
	  for(int i=0; i<k; ++i)
		  tmp1 *= (n-i);
	  tmp2=1;
	  for(int i=2; i<k+1; ++i)
		  tmp2 *= i;
	  tmp=tmp1/tmp2;
  }
  else
	  tmp=nCk(n-1, k-1) + nCk(n-1, k);

  return tmp ;
}

void RowFormat::Param(int SCRIND, double TILIM) const
{
    if (TILIM < 0) 
        cout<<"Time limit cannot be negative."<<endl;
    if (!(SCRIND==1 || SCRIND==0)) 
        cout<<"The value for detailed verbose must be 0 (off) or 1 (on)."<<endl;

	CPXsetintparam(env,  CPX_PARAM_PARALLELMODE, -1);
	CPXsetintparam(env,  CPX_PARAM_THREADS, 28*2); 
	CPXsetintparam(env,  CPX_PARAM_MIPDISPLAY, 2); 
	CPXsetintparam(env,  CPX_PARAM_MIPINTERVAL, 100);
	CPXsetintparam(env,  CPX_PARAM_CLOCKTYPE, 2); 
	CPXsetdblparam(env,  CPX_PARAM_TILIM, TILIM*3600); 
    CPXsetdblparam(env,  CPX_PARAM_EPMRK, 0.9);
    CPXsetintparam(env,  CPX_PARAM_SCAIND, 1); 
    
    CPXsetintparam(env,  CPX_PARAM_SCRIND, SCRIND); 

	double tmp;
	CPXgetdblparam(env,  CPX_PARAM_TILIM, &tmp);
	cout<<"CPX_PARAM_TILIM = "<< tmp<<" sec"<<endl;


}

void DynamicNestedLoopStriclyLess(const size_t lowerDepth, const size_t depth, size_t realPartition, size_t * mismatch, size_t& counter) //
{


	size_t partition = realPartition + 1;
	bool isFirst;
	size_t* IteratorsValue =  new (nothrow) size_t [depth];

	for(int i=0; i<depth; ++i)
		IteratorsValue[i]=0;

    size_t innerloop = depth-1;
	counter = 0;
	size_t* tempMismatch =  new (nothrow) size_t [partition];

    while (true)
    {
		for(int i=0; i<partition; i++)
			tempMismatch[i]= 0;

		for(int i= 0 ; i<depth; ++i)
			++tempMismatch[IteratorsValue[i]];

        if (lowerDepth == 0)                                //
        {
           for (size_t i=0; i< realPartition; ++i)
               mismatch[i + realPartition*counter] = tempMismatch[i+1];
        }
        else if (IteratorsValue[depth-lowerDepth] > 0 )
        {
           for (size_t i=0; i< realPartition; ++i)
                mismatch[i + realPartition*counter] = tempMismatch[i+1];
        }

        if (lowerDepth == 0)                                 //
            ++counter;
        else if (IteratorsValue[depth-lowerDepth] > 0 )
            ++counter;

	    ++IteratorsValue[innerloop];

		if(IteratorsValue[innerloop] >= partition)
        {

            if (IteratorsValue[0]  >= partition-1)
				return;

			isFirst=true;
			for(int i=0; i<depth; ++i)
				if (IteratorsValue[i]+1 >= partition)
				{
					if (isFirst)
						++IteratorsValue[i-1];
					isFirst=false;
					IteratorsValue[i]=IteratorsValue[i-1];
				}
        }

    }
	delete[] tempMismatch;
	delete[] IteratorsValue;
}
