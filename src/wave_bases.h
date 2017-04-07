#ifndef WAVE_BASES_H
#define WAVE_BASES_H

typedef struct{
    float (*daughter)[2];
    float fourier_factor;
    float coi;
    float dofmin;
}Wave_Bases_ret;

//Function to generate one of the three mother wavelets, Complex MORLET,
//PAUL and the Derivative of Gaussian (order=2 gives the mexican hat mother wavelet)
//Derived from Torrence paper "Practical guide to Wavelet Analysis: Refer the paper to understand the code"

int wave_bases(
    Wave_Bases_ret* ret,         //return value
    char* mother,
    float* k,
    int ksize,
    float scale,
    int param
    );


#endif
