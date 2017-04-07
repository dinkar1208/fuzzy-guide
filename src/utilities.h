#ifndef UTILS_H
#define UTILS_H

//#include <stdbool.h>
#include "mybool.h"

typedef struct{
  float* data;
  int size;
}FloatArray;

typedef enum{
    int8          =1,
    uint8         =2,
    int16         =3,
    uint16        =4,
    int32         =5,
    uint32        =6,
    sfloat       =7,
    //--
    ddouble      =9,
    //--
    //--
    int64         =12,
    uin64         =13,
    UTF8          =16,
    UTF16         =17,
    UTF32         =18
}DataType_t;


typedef unsigned char byte;

typedef union{
	byte* data;
  unsigned char* data_8u;
  char* data_8;
  unsigned short int* data_16u;
  short int* data_16;
  unsigned int* data_32u;
  int* data_32;
  float* data_f;
  double* data_d;
}complex_part_t;

typedef struct{
		complex_part_t re_part;
		complex_part_t im_part;    
    int   nDim; //number of dimensions
    int*  dim;  //list of dimensions
    char* name; //non-terminated(!) name
    int   nameLen; //number of characters in name
    DataType_t type;

    byte* freeMe;

}MatlabMatrix;

//load array of float separated by newlines
int loadFloatArray(char* filename, FloatArray* arr);

//load floats from binary file
int loadBinFloatArray(char* filename, FloatArray* arr);

#ifdef MATLAB_FILE_SUPPORT
//load MAT (MATLAB) file (for now it only loads the first matlab matrix it encounters!)
int loadMATFile(char* filename, MatlabMatrix* ma);
#endif



#endif
