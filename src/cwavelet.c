#include <stdlib.h>
#include <math.h>
#include "mymath.h"
#include "fft.h"
#include "cwavelet.h"
#include "wave_bases.h"
#include <stdio.h>

int cwavelet(
    Cwavelet_Return* ret,       //structure to hold return values
    float* Y,                   // Data
    int Ysize,                  // number of points in Data
    float dt,                   // 1/Sampling
    bool pad,//        =false,     //use padding                  
    float dj,//        =-1,
    float s0,//       =-1,
    int J1,//          =-1,        //number of scales
    char* mother,//    =NULL,      //wavelet type
    int param//       =-1
    ){

		int padding_size=0;
    int base2;
		int i;
		int n;
		float (*x)[2];
    float Ymean;
	  float* k;
		int ksize;
		int start;
    int k2size;
    float* k2;
 		int fsize;
    float (*f)[2];
    int scalesize;
    float* scale;
		int a1;
		float (*fd)[2]; 
    Wave_Bases_ret wb_ret;
		float (*waveX)[2];
    float* waveX_real;
		int coiSize;

    if (s0 == -1.0)
        s0=2.0*dt;
    if (dj == -1.0)
        dj = 1.0/4.0;
    if (J1 == -1)
        J1=fix((log(((double)Ysize*dt)/s0)/log((double)2))/dj);
    if (mother == NULL)
        mother = "MORLET"; //legal syntax??

    ///....construct time series to analyze, pad if necessary
    
    if (pad){
        base2 = fix(log((double)Ysize)/log(2.0) + 0.4999);   // power of 2 nearest to N
        padding_size=(int)pow(2.0,(base2))-Ysize;
    }

    n = Ysize+padding_size; //size x 

    x = (float (*)[2] ) malloc(n*sizeof(float)*2); //time2 for imaginary part!
    Ymean = fmean(Y, Ysize);
    for(i=0;i<Ysize;i++){
        x[i][0]=Y[i]-Ymean;
        x[i][1]=0; //imaginary part = 0
    }
    for(i=Ysize;i<n && pad;i++){
        x[i][0]=0;
        x[i][1]=0;
    }
    
    //....construct wavenumber array used in transform [Eqn(5)]
    ksize= fix(n/2);
    k = (float*) malloc(ksize*sizeof(float));
    for(i=0;i<ksize;i++)
        k[i]=(float)(i+1)*((2.0*M_PI)/((float)n*dt));
    
    //k = [0., k, -k(fix((n-1)/2):-1:1)]   <-- vector 0, k, reverse(-1*k)
    start = fix(( (float)n-1.0)/2.0);
    k2size=start+ksize+1;
    k2 = (float*) malloc((k2size)*sizeof(float));
    k2[0]=0.0;
    for(i=1;i<ksize+1;i++){
        k2[i]=k[i-1];
    }
    for(i=0;i<start;i++){
        k2[ksize+1+i]=-1.0*k[start-1-i];
    }
    free(k);
    k=k2; //move k2* to k
    ksize=k2size;

    //....compute FFT of the (padded) time series
    fsize=n;
    f = (float (*)[2])malloc(fsize*sizeof(float)*2); //time2 for imaginary part! 
    fft(n, x, f);    // [Eqn(3)] 
    //....construct SCALE array & empty PERIOD & WAVE arrays
    
    scalesize = J1+1;
    scale = (float*) malloc(scalesize*sizeof(float));
    ret->scale=scale;
    ret->scalesize = scalesize;
    for(i=0;i<scalesize;i++){
       scale[i]=s0*pow(2.0,(float)i*dj); 
    }

   
    //DO we need this, or does iFFt build it???
    //wave = zeros(J1+1,n);  // define the wavelet array
    //wave = wave + i*wave;  // make it complex

    // loop through all scales and compute transform
    //(waveX is pure real again, at least, for current test cases :P )
    ret->wave = (float**) malloc((J1+1)*sizeof( float*));
    
    fd = (float (*)[2]) malloc(fsize*sizeof(float)*2); //to hold f*daughter   
  
		for(a1 = 0; a1<J1+1;a1++){
				printf("\rifft %3d/%3d",a1+1,J1+1);   
				fflush(stdout);     

				wave_bases(&wb_ret, mother, k, ksize , scale[a1], param);

        //multiply f with daughter
        for(i=0;i<fsize;i++){
            //complex multiplication
            //http://mathworld.wolfram.com/ComplexMultiplication.html
            
            //NOTE, daughter is either completely real or imaginary,
            //hence it is possible to take a few shortcuts when
            //optimizing this multiplication!

            float a=f[i][0];//re
            float b=f[i][1];//im
            float c=wb_ret.daughter[i][0];//re
            float d=wb_ret.daughter[i][1];//im

            float ac=a*c;
            float bd=b*d;

            fd[i][0]=ac-bd;
            fd[i][1]=(a+b)*(c+d)-ac-bd;
            
        }
        free(wb_ret.daughter);
        //do iFFT
        waveX = (float (*)[2]) malloc(fsize*sizeof(float)*2);
        ifft(fsize, waveX ,fd);  // wavelet transform[Eqn(4)]

        
        //NOTE: observation, waveX is pure real, hence I convert here, matlab may do otherwise for other inputs??
        waveX_real = (float*) malloc(fsize*sizeof(float));
        for(i=0;i<fsize;i++){
            waveX_real[i]=waveX[i][0];
            
            /*
            if(waveX[0][1]>0.01){
                printf("warning!! truncating imaginary part, update the algorithm!!\n");
            }*/
        }
        free(waveX);
        
        //store calculated time wave at a1
        ret->wave[a1] = waveX_real;

        //TODO: insert return value
    }
		printf("\n");

    //set dimensions of wave
    ret->wavesize[0]=J1+1;
    ret->wavesize[1]=fsize;


    ret->period = (float*) malloc(scalesize*sizeof(float));
    for(i=0;i<scalesize;i++){
        ret->period[i] = wb_ret.fourier_factor*scale[i];
    }

    //ret->coi = coi*dt* [0.00001, 1:((Ysize+1)/2-1) ,fliplr((1:(Ysize/2-1))), 0.00005];  // COI [Sec.3g]
    coiSize=(1+ ((Ysize+1)/2-1)) *2;
    ret->coi = (float*) malloc(coiSize*sizeof(float));

    ret->coi[0]=0.00005*wb_ret.coi*dt;
    ret->coi[coiSize-1]=0.00005*wb_ret.coi*dt;
    for(i=1;i<=((Ysize+1)/2-1);i++){
        ret->coi[i]= (float)i*wb_ret.coi*dt;
    }
    for(i=0;i<((Ysize+1)/2-1);i++){
        ret->coi[i+((Ysize+1)/2-1)+1]= (float)(((Ysize+1)/2-1)-i)*wb_ret.coi*dt;
    }
    
    //N.B.! wave size = Ysize which comes from input parameter, hence don't need to tell callee
    //wave = wave(:,1:Ysize);  // get rid of padding before returning

    //in c padding will be gone by just ignoring the tail ;)
    free(fd);
    free(x);
    free(k);
    free(f);
    //success
    return 0;
}
