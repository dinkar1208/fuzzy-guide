#ifndef COMPUTE_WAVELET_H
#define COMPUTE_WAVELET_H
//#include <stdbool.h>
#include "mybool.h"

typedef struct{
    float**  U;
    int     Usize[2]; //2 dimensional
    float*  scale;
    int     scalesize;
    float*  Freq;
    int     Freqsize;
}Compute_Wavelet_Return;

//Computes the Wavelet Coeff for Data Z
//Returns the sqaured normalized wavelet coeff as function of data index
//scale s=s0.2^dj*[1:J]
//example:dj=.2258, s0=.005, J=25
//dt=60/12207.03125;

int Compute_Wavelet(
    Compute_Wavelet_Return* ret,
    float* Z,
    int Zsize,
    char* mother,
    int order,
    bool Total,         //Total==true implies, All scales used for computation
    float Value,        //Total==false implies, one needs to provide the Value of scale selected (given by variable: Value) and all other parameters: dj=so=J=0
    float srate,
    float dj,
    float s0,
    int J
    );

#endif
