#include "wave_bases.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

/* very simple approximation */
float st_gamma(float x)
{
  return sqrt(2.0*M_PI/x)*pow(x/M_E, x);
}
 
#define A 12
float sp_gamma(float z)
{
  const int a = A;
  static float c_space[A];
  static float *c = NULL;
  int k;
  float accm;
 
  if ( c == NULL ) {
    float k1_factrl = 1.0; /* (k - 1)!*(-1)^k with 0!==1*/
    c = c_space;
    c[0] = sqrt(2.0*M_PI);
    for(k=1; k < a; k++) {
      c[k] = exp(a-k) * pow(a-k, k-0.5) / k1_factrl;
      k1_factrl *= -k;
    }
  }
  accm = c[0];
  for(k=1; k < a; k++) {
    accm += c[k] / ( z + k );
  }
  accm *= exp(-(z+a)) * pow(z+a, z+0.5); /* Gamma(z+1) */
  return accm/z;
}

int prod(int a,int b){
    int p=a;
		int i;    
		for(i=a+1;i<=b;i++)
        p*=i;
    return p;
}

// typedef struct{
//     float* daughter;
//     float fourier_factor;
//     float coi;
//     float dofmin;
// }Wave_Bases_ret;

int wave_bases(
    Wave_Bases_ret* ret,         //return value
    char* mother,
    float* k,
    int ksize,
    float scale,
    int param
    ){

    int int_return_val=0;
   // mother = toupper(mother);
    int n = ksize;
    float norm;
    float* expnt;
		int i;

    ret->daughter = (float (*)[2]) malloc(ksize*sizeof(float)*2);
    

    expnt = (float*) malloc( ksize*sizeof(float));
    if (strcmp(mother,"MORLET")==0){  
				float k0;  
				if (param == -1){
            param = 6.0; 
        }
        k0 = param;
        for(i=0;i<ksize;i++){
            expnt[i] = (k[i]>0.0) ? -1.0*pow( (scale*k[i] - k0), 2)/2.0 : 0.0;
        }
        norm = sqrt(scale*k[2])*(pow(M_PI,-0.25))*sqrt(n);    
        
        
        for(i=0;i<ksize;i++){
            if(k[i]>0){
                ret->daughter[i][0] = norm*exp(expnt[i]);
                ret->daughter[i][1] = 0.0;
            }
        }
        
        ret->fourier_factor = (4*M_PI)/(k0 + sqrt(2.0 + pow(k0,2))); 
        ret->coi = ret->fourier_factor/sqrt(2.0);                  
        ret->dofmin = 2;                 
    }                   
    else if (strcmp(mother,"PAUL")==0){  
				int m;        
				if (param == -1)
            param = 4.;
        m = param;
       
        for(i=0;i<ksize;i++){
            expnt[i] = (k[i]>0.0) ? -1.0*(scale*k[i]) : 0.0;
        }
        norm = sqrt(scale*k[2])*( pow(2,m)/sqrt(m*prod(2,(int)(2*m-1))))*sqrt(n);
        
        for(i=0;i<ksize;i++){
            if(k[i]>0){
                ret->daughter[i][0] = norm*exp(expnt[i])*pow((scale*k[i]),m);
                ret->daughter[i][1] = 0.0;
            }
        }

        ret->fourier_factor = 4.0*M_PI/(2.0*m+1.0);
        ret->coi = ret->fourier_factor*sqrt(2);
        ret->dofmin = 2;
    }
    else if (strcmp(mother,"DOG")==0){  
				int m;        
				if (param == -1.0){
            param = 2.0;
        }
        m = param;
        for(i=0;i<ksize;i++){
            expnt[i] = -1.0*pow((scale*k[i]),2)/2.0;
        }
        
        norm = sqrt(scale*k[1]/sp_gamma(m+0.5))*sqrt(n);
       
        if(m&1){
            //imaginary
            for(i=0;i<ksize;i++){
                ret->daughter[i][0] = 0.0;
                ret->daughter[i][1] = -1.0*norm*exp(expnt[i])*pow((scale*k[i]),m); //*(i^m) !! if m==odd, daughter imaginary?
            }
        }else{
            //real
            for(i=0;i<ksize;i++){
                ret->daughter[i][0] = norm*exp(expnt[i])*pow((scale*k[i]),m);
                ret->daughter[i][1] = 0.0;
            }
        }

        ret->fourier_factor = 2.0*M_PI*sqrt(2./(2.0*m+1.0));
        ret->coi = ret->fourier_factor/sqrt(2);
        ret->dofmin = 1;
    }
    else{
        printf("wave_bases.c: Mother must be one of MORLET,PAUL,DOG\n");
        int_return_val=-1;
    }

    free(expnt);
    return int_return_val;
}
