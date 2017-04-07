#ifndef CWAVELET_H
#define CWAVELET_H

//#include <stdbool.h>
#include "mybool.h"


typedef struct{
    float**  wave;
    int     wavesize[2];  //2 dimensional
    float*  period;
    float*  scale;
    int     scalesize;
    float*   coi;         //cone of interest
}Cwavelet_Return;


// Borrowed from Torrence paper "Practical guide to Wavelet Analysis: refer
// the paper to understand the code"
int cwavelet(
    Cwavelet_Return* ret,   //structure to hold return values
    float* Y,               // Data
    int Ysize,              // number of points in Data
    float dt,               // 1/Sampling
    bool pad,               //use padding                  
    float dj,
    float s0,
    int J1,                 //number of scales
    char* mother,           //wavelet type
    int param
    );

#endif
