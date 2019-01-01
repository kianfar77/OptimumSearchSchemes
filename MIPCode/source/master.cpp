#include "master.h"

void MasterObjCoefficients(RowFormat& master, const IndicesRec& indexes, const searchParameter& tag)
{
	master.Obj(indexes, tag, true);
}

void PopulateMasterProblem(RowFormat& master, const IndicesRec& indexes,const int* m ,const searchParameter& tag ) 
{

	RowRec row;

	row.nzcnt = indexes.i;
	row.rhs[0] = 1;
	row.sense[0] = 'E';
	row.rmatbeg[0] = 0;
	row.Allocate(row.nzcnt);
	for(int i=0; i<row.nzcnt; i++)
		row.rmatval[i] = 1;
	for (int s=0; s<indexes.s ; s++)
		for (int j=0; j<indexes.i; j++)
		{
			for(int i=0; i<row.nzcnt; i++)
				row.rmatind[i] =  indexes.s * indexes.l * (indexes.d+1) + s * indexes.i * indexes.i + indexes.i * i + j;
			master.rowName("4a",  's', 'j', s, j, true, true);
			master.addConstraint(row);
		}
	row.Dellocate();
	

	row.nzcnt = indexes.i;
	row.Allocate(row.nzcnt);
	row.rhs[0] = 1;
	row.sense[0] = 'E';
	row.rmatbeg[0] = 0;
	for(int i=0; i<row.nzcnt; i++)
		row.rmatval[i] = 1;
	for (int s=0; s<indexes.s ; s++)
		for(int i=0; i<indexes.i; i++)
		{
			for (int j=0; j<row.nzcnt; j++)
				row.rmatind[j] =   indexes.s * indexes.l * (indexes.d+1) + s * indexes.i * indexes.i + indexes.i * i + j;
			master.rowName("4b",  's', 'i', s, i, true, true);
			master.addConstraint(row);
		}
	row.Dellocate();


	row.rhs[0] = 0;
	row.sense[0] = 'E';
	row.rmatbeg[0] = 0;
	for(int i=1; i<indexes.i-1; i++)
	{
		row.nzcnt = (i+1)+2; 
		row.Allocate(row.nzcnt);
		for(int h=0; h<(i+1); h++)
			row.rmatval[h] = 1;
		row.rmatval[i+1] = -1;
		row.rmatval[(i+1)+1] = 1;
		for (int s=0; s<indexes.s ; s++)
		{
			for(int h=0; h<(i+1); h++)
				row.rmatind[h] =  indexes.s * indexes.l * (indexes.d+1) + s * indexes.i * indexes.i + indexes.i * h + 0 ;
			row.rmatind[i+1] =  (indexes.s * indexes.l *( indexes.d+1)) +(indexes.s * indexes.i * indexes.i) + (s * indexes.i * (indexes.i+1) + (indexes.i+1) * i + 0) ;
			row.rmatind[(i+1)+1] =  (indexes.s * indexes.l * (indexes.d+1)) +(indexes.s * indexes.i * indexes.i) + (indexes.s * indexes.i * (indexes.i+1)) +(s * indexes.i * (indexes.i+1) + (indexes.i+1) * i + 0) ;
			master.rowName("5a",  's', 'i', 'j', s, i, 0, true, true, true);
			master.addConstraint(row);
		}
		row.Dellocate();
	}


	row.rhs[0] = 0;
	row.sense[0] = 'E';
	row.rmatbeg[0] = 0;
	for(int i=1; i<indexes.i-1; i++)
	{
		row.nzcnt = (i+1)+2; 
		row.Allocate(row.nzcnt);
		for(int h=0; h<(i+1); h++)
			row.rmatval[h] = -1;
		row.rmatval[i+1] = -1;
		row.rmatval[(i+1)+1] = 1;
		for (int s=0; s<indexes.s ; s++)
		{
			for(int h=0; h<(i+1); h++)
				row.rmatind[h] = indexes.s * indexes.l * (indexes.d+1)+ s * indexes.i * indexes.i + indexes.i * h + (indexes.i)-1 ;	
			row.rmatind[i+1] =  (indexes.s * indexes.l *( indexes.d+1)) +(indexes.s * indexes.i * indexes.i) + (s * indexes.i * (indexes.i+1) + (indexes.i+1) * i + (indexes.i)) ;
			row.rmatind[(i+1)+1] =  (indexes.s * indexes.l * (indexes.d+1)) + (indexes.s * indexes.i * indexes.i) + (indexes.s * indexes.i * (indexes.i+1)) +(s * indexes.i * (indexes.i+1) + (indexes.i+1) * i + (indexes.i)) ;	
			master.rowName("5a",  's', 'i', 'j', s, i, indexes.i, true, true, true);
			master.addConstraint(row);
		}
		row.Dellocate();
	}	


	row.rhs[0] = 0;
	row.sense[0] = 'E';
	row.rmatbeg[0] = 0;
	for(int i=1; i<indexes.i-1; i++)
	{
		row.nzcnt = 2*(i+1)+2; 
		row.Allocate(row.nzcnt);
		for (int s=0; s<indexes.s ; s++)
			for (int j=1; j<indexes.i; j++)
			{
				for(int h=0; h<(i+1); h++)
				{
					row.rmatval[2*h] = -1;
					row.rmatval[2*h+1] = 1;
					row.rmatind[2*h] =  indexes.s * indexes.l * (indexes.d+1)+ s * indexes.i * indexes.i + indexes.i * h + j-1 ;
					row.rmatind[2*h+1] =  indexes.s * indexes.l * (indexes.d+1) + s * indexes.i * indexes.i + indexes.i * h + j ;
				}
				row.rmatval[2*(i+1)] = -1;
				row.rmatval[2*(i+1)+1] = 1;
				row.rmatind[2*(i+1)] =  (indexes.s * indexes.l *( indexes.d+1)) +(indexes.s * indexes.i * indexes.i) + (s * indexes.i * (indexes.i+1) + (indexes.i +1)* i + j) ;
				row.rmatind[2*(i+1)+1] =  (indexes.s * indexes.l * (indexes.d+1)) +(indexes.s * indexes.i * indexes.i) +(indexes.s * indexes.i * (indexes.i+1)) + (s * indexes.i * (indexes.i+1) + (indexes.i+1) * i + j) ;
				master.rowName("5a",  's', 'i', 'j', s, i, j, true, true, true);
				master.addConstraint(row);
			}
		row.Dellocate();
	}
	

	row.nzcnt = 2*(indexes.i+1);
	row.Allocate(row.nzcnt);
	row.rhs[0] = 2;
	row.sense[0] = 'E';
	row.rmatbeg[0] = 0;
	for(int i=0; i<row.nzcnt; i++)
		row.rmatval[i] = 1;
	for (int s=0; s<indexes.s ; s++)
		for(int i=1; i<indexes.i-1; i++)
		{
			for (int j=0; j<indexes.i+1; j++)
				row.rmatind[j] =  (indexes.s * indexes.l *( indexes.d+1)) +(indexes.s * indexes.i * indexes.i) + (s * indexes.i * (indexes.i+1) + (indexes.i+1) * i + j) ;
			for (int j=0; j<indexes.i+1; j++)
				row.rmatind[j+indexes.i+1] =  (indexes.s * indexes.l * (indexes.d+1)) +(indexes.s * indexes.i * indexes.i) +(indexes.s * indexes.i * (indexes.i+1)) + (s * indexes.i * (indexes.i+1) + (indexes.i+1) * i + j) ;
			master.rowName("5b",  's', 'i', s, i, true, true);
			master.addConstraint(row);
		}
	row.Dellocate();
	

	row.nzcnt =2;
	row.Allocate(row.nzcnt);
	row.sense[0] = 'L';
	row.rmatbeg[0] = 0;
	row.rmatval[0] = -1;
	row.rmatval[1] = -(tag.R+1);
	for(int s=0; s<indexes.s; s++)
	{		
		for(int l=0; l<indexes.l; l++)
		{
			int i = ceil(((l+1)/(double)m[0]))-1;
			row.rmatind[0] = indexes.s * indexes.l *( indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1)) + s *indexes.i + i;
			for(int d=0; d<indexes.d+1 ; d++)   
			{
				row.rhs[0] = -(d+1)-((i+1)*m[0]-(l+1)); 
				row.rmatind[1] =   indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + s * indexes.l * (indexes.d+1) +
					l * (indexes.d+1) + d;
				master.rowName("6a",  's', 'L', 'd', s, l, d, true, true, false);
				master.addConstraint(row);
			}
		}
	}
	row.Dellocate();


	row.nzcnt =2;
	row.Allocate(row.nzcnt);
	row.sense[0] = 'L';
	row.rmatbeg[0] = 0;
	row.rmatval[0] = 1;
	row.rmatval[1] = -(tag.K+1);
	for(int s=0; s<indexes.s; s++)
	{		
		for(int l=0; l<indexes.l; l++)
		{
			int i = ceil(((l+1)/(double)m[0]))-1;
			row.rmatind[0] =  indexes.s * indexes.l *( indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  +1* indexes.s *indexes.i + s *indexes.i + i;
			for(int d=0; d<(indexes.d+1); d++)   
			{
				row.rhs[0] = d-1; 
				row.rmatind[1] =   2 * indexes.s * indexes.l *( indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + s * indexes.l * (indexes.d+1) 
					+ l * (indexes.d+1) + d;
				master.rowName("6b",  's', 'L', 'd', s, l, d, true, true, false);
				master.addConstraint(row);
			}
		}
	}
	row.Dellocate();
	

	row.nzcnt =4;
	row.Allocate(row.nzcnt);
	row.sense[0] = 'L';
	row.rmatbeg[0] = 0;
	row.rmatval[0] = 1;
	row.rmatval[1] = -1;												
	for(int s=0; s<indexes.s; s++)
	{		
		for(int l=1; l<indexes.l; l++)
		{
			row.rhs[0] = 2 * 1;                                                      
			row.rmatval[2] = 1;
			row.rmatval[3] = 1;
			row.rmatind[3] = 2 * indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + s * indexes.l * (indexes.d+1)
				+ l * (indexes.d+1) + 0;
			row.rmatind[2] = indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + s * indexes.l * (indexes.d+1) 
				+ l * (indexes.d+1) + 0;
			row.rmatind[1]= s * indexes.l * (indexes.d+1) + l * (indexes.d+1) + 0;
			row.rmatind[0]= s * indexes.l * (indexes.d+1) + (l-1) * (indexes.d+1) + 0;
			master.rowName("6c",  's', 'L', 'd', s, l, 0, true, true, false);
			master.addConstraint(row);
		}
	}		
	row.Dellocate();
	

	row.nzcnt =5;
	row.Allocate(row.nzcnt);
	row.sense[0] = 'L';
	row.rmatbeg[0] = 0;
	row.rmatval[0] = tag.sigma-1;
	row.rmatval[1] = 1;
	row.rmatval[2] = -1;
	double product=0;												
	for(int s=0; s<indexes.s; s++)
	{		
		for(int l=1; l<indexes.l; l++)
		{
			for(int d=1; d<(indexes.d+1); d++)   
			{
				product = nCk(l+1,d) *  (double) IntegerPower(tag.sigma-1,d);   
				row.rhs[0] = 2 * product;                                                  
				row.rmatval[3] = product;
				row.rmatval[4] = product;

				row.rmatind[4] = 2 * indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + s * indexes.l * (indexes.d+1)
					+ l * (indexes.d+1) + d;
				row.rmatind[3] = indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + s * indexes.l * (indexes.d+1)
					+ l * (indexes.d+1) + d;
				row.rmatind[2]= s * indexes.l * (indexes.d+1) + l * (indexes.d+1) + d;
				row.rmatind[1]= s * indexes.l * (indexes.d+1) + (l-1) * (indexes.d+1) + d;
				row.rmatind[0]= s * indexes.l * (indexes.d+1) + (l-1) * (indexes.d+1) + d-1;
				master.rowName("6c",  's', 'L', 'd', s, l, d, true, true, false);
				master.addConstraint(row);
			}
		}
	}		
	row.Dellocate();


	row.nzcnt =3;
	row.Allocate(row.nzcnt);
	row.sense[0] = 'L';
	row.rmatbeg[0] = 0;
	row.rhs[0] = 2 * (tag.sigma-1)-(tag.sigma-1);
	row.rmatval[0] = -1;
	row.rmatval[1] = (tag.sigma-1);
	row.rmatval[2] = (tag.sigma-1);												
	for(int s=0; s<indexes.s; s++)
	{		
		row.rmatind[2] = 2 * indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + s * indexes.l * (indexes.d+1)
			+ 0 * (indexes.d+1) + 1;
		row.rmatind[1] = indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + s * indexes.l * (indexes.d+1) + 0 * (indexes.d+1) + 1;
		row.rmatind[0]= s * indexes.l * (indexes.d+1) + 0 * (indexes.d+1) + 1;
		master.rowName("6c",  's', 'L', 'd', s, 0, 1, true, true, false);
		master.addConstraint(row);
	}		
	row.Dellocate();

	
	row.nzcnt =3;
	row.Allocate(row.nzcnt);
	row.sense[0] = 'L';
	row.rmatbeg[0] = 0;
	row.rhs[0] = 2 * 1-1;   
	row.rmatval[0] = -1;
	row.rmatval[1] = 1;
	row.rmatval[2] = 1;
	for(int s=0; s<indexes.s; s++)
	{		
		row.rmatind[2] = 2 * indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + s * indexes.l * (indexes.d+1) 
			+ 0 * (indexes.d+1) + 0;
		row.rmatind[1] = indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + s * indexes.l * (indexes.d+1) + 0 * (indexes.d+1) + 0;
		row.rmatind[0]= s * indexes.l * (indexes.d+1) + 0 * (indexes.d+1) + 0;
		master.rowName("6c",  's', 'L', 'd', s, 0, 0, true, true, false);
		master.addConstraint(row);
	}		
	row.Dellocate();

	
	row.nzcnt = 2;
	row.rhs[0] = 0;
	row.sense[0] = 'L';
	row.rmatbeg[0] = 0;
	row.Allocate(row.nzcnt);
	row.rmatval[0] = 1;
	row.rmatval[1] = -1;
	for (int s=0; s<indexes.s ; s++)
		for(int i=0; i<indexes.i-1; i++)
		{
			row.rmatind[0] =  indexes.s * indexes.l *( indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1)) + s *indexes.i + i;
			row.rmatind[1] =  indexes.s * indexes.l *( indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1)) + s *indexes.i + i+1;
			master.rowName("7a",  's', 'i', s, i, true, true);
			master.addConstraint(row);
		}
	row.Dellocate();
	

	row.nzcnt = 2;
	row.rhs[0] = 0;
	row.sense[0] = 'L';
	row.rmatbeg[0] = 0;
	row.Allocate(row.nzcnt);
	row.rmatval[0] = 1;
	row.rmatval[1] = -1;
	for (int s=0; s<indexes.s ; s++)
		for(int i=0; i<indexes.i-1; i++)
		{
			row.rmatind[0] =  indexes.s * indexes.l *( indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  +1* indexes.s *indexes.i + s *indexes.i + i;
			row.rmatind[1] =  indexes.s * indexes.l *( indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  +1* indexes.s *indexes.i + s *indexes.i + i+1;
			master.rowName("7b",  's', 'i', s, i, true, true);
			master.addConstraint(row);
		}
	row.Dellocate();


	
	row.rhs[0] = 1;
	row.sense[0] = 'E';
	row.rmatbeg[0] = 0;
	for (int i=(floor(indexes.i/2.0)+1); i<indexes.i; ++i)
	{
		row.nzcnt = 2*(indexes.i-i);
		row.Allocate(row.nzcnt);
		for (int h=0; h< row.nzcnt; ++h)
			row.rmatval[h] = 1;
		for (int s=0; s< indexes.s; ++s)
		{
			for (int k=0; k< indexes.i-i; ++k)
				row.rmatind[k] =  indexes.s * indexes.l *( indexes.d+1) +  s * indexes.i * indexes.i + indexes.i * (i) + (k) ;
			for (int k=0; k< indexes.i-i; ++k)
				row.rmatind[indexes.i-i+k] =  indexes.s * indexes.l *( indexes.d+1) +  s * indexes.i * indexes.i + indexes.i * (i) + (i+k) ;
			master.rowName("12", 's', 'i', i, s, true, true);
			master.addConstraint(row);
		}
		row.Dellocate();
	}
		
	

	row.sense[0] = 'L';
	row.rmatbeg[0] = 0;
	size_t count;
		for (int s=0; s<indexes.s-1 ; s++)
		{
			row.rhs[0] = indexes.s-s;
			for (int j=1; j<indexes.i; j++)
			{
				row.nzcnt = 1+ (indexes.s-s)*(j);
				row.Allocate(row.nzcnt);
				row.rmatval[0] = indexes.s-s;
				for(int i=1; i<row.nzcnt; ++i)
					row.rmatval[i] = 1;
				count=0;
				row.rmatind[count] =  indexes.s * indexes.l *( indexes.d+1) +  s * indexes.i * indexes.i + indexes.i * (0) + (j) ;
				for(int t=s; t< (indexes.s); ++t)
					for(int k=0; k< j; ++k)
					{
						++count;
						row.rmatind[count] =  indexes.s * indexes.l *( indexes.d+1) +  (t) * indexes.i * indexes.i + indexes.i * (0) + (k) ;
					}
				master.rowName("11b",  's', 'j', s, j, true, true);
				master.addConstraint(row);

				row.Dellocate();
			}
		}
}


void AddConstraintToMaster( RowFormat& master, const IndicesRec& indexes, const searchParameter& tag, const size_t* a, size_t mismatchCursor)
{
	RowRec row;
	size_t q;


	row.rhs[0] = 0;
	row.sense[0] = 'E';
	row.rmatbeg[0] = 0;
	for(int i=0; i<indexes.i; i++)
	{
		row.nzcnt = indexes.i*(i+1)+1; 
		row.Allocate(row.nzcnt);
		q=mismatchCursor;
		{
			for(int h=0; h<(i+1); h++)
			for(int j=0; j<indexes.i; j++)
				row.rmatval[h*indexes.i+j] = -1 * (int ) (a[j+ mismatchCursor*indexes.i]); 	
			row.rmatval[indexes.i*(i+1)] = 1;
			for (int s=0; s<indexes.s ; s++)
			{
				for (int h=0; h<i+1; h++)
					for (int j=0; j<indexes.i; j++)
						row.rmatind[h*indexes.i+j] =  indexes.s * indexes.l * (indexes.d+1) + s * indexes.i * indexes.i + indexes.i * h + j ;
				row.rmatind[indexes.i*(i+1)] = 3 * indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + q * indexes.s * indexes.i
					+ s * indexes.i + i;
				master.rowName("8a_A_is_sum(a.x)",  'q', 's', 'i', q, s, i, false, true, true);
				master.addConstraint(row);
			}
		}
		row.Dellocate();
	}	


	row.nzcnt =3;
	row.Allocate(row.nzcnt);
	row.sense[0] = 'L';
	row.rmatbeg[0] = 0;
	row.rhs[0] = tag.K;					                        
	row.rmatval[0] = 1;
	row.rmatval[1] = -1;
	row.rmatval[2] = tag.K;
	q=mismatchCursor;
		for(int s=0; s<indexes.s; s++) 
			for(int i=0; i<indexes.i; i++)
			{
				row.rmatind[0] =  indexes.s * indexes.l *( indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1)) + s *indexes.i + i;
				row.rmatind[1] =  3 * indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + q * indexes.s * indexes.i
					+ s * indexes.i + i;
				row.rmatind[2] = 3 * indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1)) + 2 * indexes.s *indexes.i  + indexes.q * indexes.s * indexes.i
					+ q * indexes.s + s;
				master.rowName("8a.left",  'q', 's', 'i', q, s, i, false, true, true);
				master.addConstraint(row);
			}
	row.Dellocate();	


	row.nzcnt =3;
	row.Allocate(row.nzcnt);
	row.sense[0] = 'L';
	row.rmatbeg[0] = 0;
	row.rhs[0] = tag.K;							                        
	row.rmatval[0] = -1;
	row.rmatval[1] = 1;
	row.rmatval[2] = tag.K;
	q=mismatchCursor;
		for(int s=0; s<indexes.s; s++) 
			for(int i=0; i<indexes.i; i++)
			{
				row.rmatind[0] =  indexes.s * indexes.l *( indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  +  indexes.s *indexes.i +s *indexes.i + i;
				row.rmatind[1] =  3 * indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + q * indexes.s * indexes.i
					+ s * indexes.i + i;
				row.rmatind[2] = 3 * indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  
					+ 2 * indexes.s *indexes.i 
				+ indexes.q  * indexes.s * indexes.i + q * indexes.s + s;
				master.rowName("8a.right",  'q', 's', 'i', q, s, i, false, true, true);
				master.addConstraint(row);
			}
	row.Dellocate();


	row.nzcnt =indexes.s;
	row.Allocate(row.nzcnt);
	row.sense[0] = 'G';              
	row.rmatbeg[0] = 0;
	row.rhs[0] = 1;	
	for(int s=0; s<indexes.s; s++)
		row.rmatval[s] = 1;
	q=mismatchCursor;
	{
		for(int s=0; s<indexes.s; s++) 
			row.rmatind[s] = 3 * indexes.s * indexes.l * (indexes.d+1) + (indexes.s * indexes.i * indexes.i) +2*(indexes.s * indexes.i * (indexes.i+1))  + 2 * indexes.s *indexes.i + indexes.q * indexes.s * indexes.i
			+ q * indexes.s + s;
		master.rowName("8b", 'q', q, false);
		master.addConstraint(row);
	}
	row.Dellocate();
}
