#include "DataLoader.h"
#include <stdlib.h>
#include "utilities.h"
#include <stdio.h>

int DataLoader(         //return 0 is success
    bool Micro,         //Micro=true loads data from microrecording
    short int Num,      //Num gives the seizure number for the data
    short int Ch,       //If Micro is true then, Ch gives information on which channel to load (1...8)
    bool srate,         //srate=true implies Full sampled Micro Data loaded
    MatlabMatrix* Z     //matrix Z to be loaded
    ){

    char File[128];
    if (Micro && srate){       
        //load array of floats from file
        FloatArray arr;
        int ret;

				sprintf(File, "./Data/MicroRatData/Micro_Sz%hd_Avg%hd.bin",Num, Ch);
				ret = loadBinFloatArray(File, &arr);

        //put into matlab matrix for compatibility
        Z->re_part.data_f=arr.data;
        Z->im_part.data=NULL;
        Z->nDim=1;
        Z->dim = (int*) malloc(sizeof(int));
        Z->dim[0]=arr.size;
        Z->name="Z";
        Z->nameLen=1;
        Z->type=sfloat;
				Z->freeMe=Z->re_part.data;
        return ret;

    }else if (Micro && (~srate)){
        
        //load array of floats from file
        FloatArray arr;
				int ret;

				sprintf(File, "./Data/MicroRatData/MicroRatDownSampledData/Micro_Sz_DownSampled%hd_Avg%hd.bin",Num, Ch);
        ret = loadBinFloatArray(File, &arr);

        //put into matlab matrix for compatibility
        Z->re_part.data_f=arr.data;
        Z->im_part.data=NULL;
        Z->nDim=1;
        Z->dim = (int*) malloc(sizeof(int));
        Z->dim[0]=arr.size;
        Z->name="Z";
        Z->nameLen=1;
        Z->type=sfloat;
        Z->freeMe=Z->re_part.data;
        
        return ret;

    }else{
        sprintf(File, "./Data/MacroRatData/Macro%hd.mat",Num);
				#ifdef MATLAB_FILE_SUPPORT        
				return loadMATFile(File, Z);
				#else
				printf("Error loading Matlab file %s\n",File);
				printf("Application is compiled without Matlab supported\n");
				return -1;
				#endif
    }
}
