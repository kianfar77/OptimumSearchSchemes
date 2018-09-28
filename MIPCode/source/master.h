#ifndef MASTER_H
#define MASTER_H
#include "commonObjects.h"

using namespace std;

void MasterObjCoefficients(RowFormat& , const IndicesRec& , const searchParameter& );
void PopulateMasterProblem(RowFormat& , const IndicesRec& ,const int* m,const searchParameter& );
void AddConstraintToMaster( RowFormat& , const IndicesRec& , const searchParameter& , const size_t* , size_t);

#endif