#ifndef DATALOADER_H
#define DATALOADER_H


#include "utilities.h"
//#include <stdbool.h>
#include "mybool.h"

int DataLoader(         //return 0 is success
    bool Micro,         //Micro=true loads data from microrecording
    short int Num,      //Num gives the seizure number for the data
    short int Ch,       //If Micro is true then, Ch gives information on which channel to load (1...8)
    bool srate,         //srate=true implies Full sampled Micro Data loaded     
    MatlabMatrix* Z     //matrix Z to be loaded
    ); 

#endif
