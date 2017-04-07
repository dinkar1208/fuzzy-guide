#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Compute_Wavelet.h"
#include "cwavelet.h"

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
    ){

    float dt=1.0/srate;
		int i=0;    
	  Cwavelet_Return cv_ret;

    float* t = (float*) malloc(Zsize*sizeof(float));
    for(i=0;i<Zsize;i++){
        t[i]=(float)i*dt;
    }

    
    if(Total){ //Use all scales for computation
        cwavelet(&cv_ret,Z, Zsize, dt, true, dj, s0, J, mother, order  );

        ret->scale=cv_ret.scale;
        ret->scalesize=cv_ret.scalesize;
        ret->U=cv_ret.wave;
        ret->Usize[0]=cv_ret.wavesize[0];
        ret->Usize[1]=cv_ret.wavesize[1];
        ret->Freqsize=cv_ret.scalesize;
        ret->Freq=(float*)malloc(ret->Freqsize*sizeof(float));
        for(i=0;i<cv_ret.scalesize;i++){
            ret->Freq[i]=(sqrt((float)order+0.5))/(2.0*M_PI*ret->scale[i]);
        }
        if(strcmp(mother,"MORLET")==0){ //% Test of Morlet for phase computation with wavelets
            for(i=0;i<cv_ret.scalesize;i++){
                ret->Freq[i]=((float)order+sqrt(2+pow((double)order,2) ))/(4.0*M_PI*ret->scale[i]);
            }
        }
        
    }
    else{ //%Use the chosen scale for computation
        cwavelet(&cv_ret,Z, Zsize, dt, true, 0, Value, 0, mother, order  ) ;
        ret->scale=cv_ret.scale;
        ret->scalesize=cv_ret.scalesize;
        ret->U=cv_ret.wave;
        ret->Usize[0]=cv_ret.wavesize[0];
        ret->Usize[1]=cv_ret.wavesize[1];
        ret->Freqsize=cv_ret.scalesize;
        ret->Freq=(float*)malloc(ret->Freqsize*sizeof(float));
        for(i=0;i<cv_ret.scalesize;i++){
            ret->Freq[i]=(sqrt((float)order+0.5))/(2.0*M_PI*ret->scale[i]);
        }
        if(strcmp(mother,"MORLET")==0){ //Test
            for(i=0;i<cv_ret.scalesize;i++){
                ret->Freq[i]=((float)order+sqrt(2+pow((double)order,2) ))/(4.0*M_PI*ret->scale[i]);
            }
        }
        
    }    

    free(cv_ret.period);
    free(cv_ret.coi);

    free(t);
    return 0;
}
