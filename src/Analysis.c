#include "Analysis.h"
#include <stdio.h>
#include "utilities.h"
#include "DataLoader.h"
#include <math.h>
#include "Compute_Wavelet.h"
#include "mymath.h"
#include <stdlib.h>
#include "sim_api.h"

int Analysis(                 // return 0 == success
  AnalysisReturn_t* ret,      //structure to hold return values
  bool Micro,                 // true/false implies the data is obtained with micro/macro recordings
  bool samplingbool,          // If Micro is true, then samplingbool=true/false implies the micro data considered at 12KHz/200Hz
  short int SzNum,            // Gives the Seizure number considered for analysis (1..22 for Micro Data and 1..16 for Macro Data)
  short int CHNum,            // If Micro is 1, then CHNum gives which channel numbers are considered (1..8)
  int WindowTime,             // Time in seconds of windowed data considered for analysis
  int OverlapWindowTime,      // Time in seconds for the overlap considered for moving average analysis
  int smallwind,              // subwindow size for calculation the direct energy measures
  int bigwind                 // Subwindow time in seconds considered for WhiteAutocorrelation measures
  ){
  //[Tim MaxFreq NonLinMeasure WhiteMeasure RSMeasure]=Analysis...
  //...(Micro,samplingbool,SzNum,CHNum,WindowTime,OverlapWindowTime,smallwind,bigwind,ND,HR,WS,WAC,GAE)
  //Computing measures for seizure detection on the created data set
  //Output gives the 5 dynamical measures as function of time Tim

  int i, j;
  FloatArray Range;
	float Norm;
	int Zsize;
	float* Z;
	float* t0;
	float mean;
  float dt;
  float Fs;
  int Window;
  int OverlapWindow;
  int StartIndex;
  int count;
	int iterations;
	int iter;
	MatlabMatrix Zi;
	

  //select input file base on micro or not
  char RangeFile[128];
  if(Micro==true){ 
    sprintf(RangeFile, "./Data/DataFiles/MicroRange_%d.dat",CHNum);
  }else{
    sprintf(RangeFile, "./Data/DataFiles/MacroRange.dat");
  }


  if (loadFloatArray(RangeFile, &Range) == 0)
    return -1;


  //check if data is available
  if ((SzNum<Range.size) && (!isnan(Range.data[SzNum]))){

    Norm = Range.data[SzNum-1];

    //free the parsed ranges
    free(Range.data);


    if( DataLoader(Micro,SzNum,CHNum,samplingbool, &Zi)<0){
				printf("Error loading data array\n");
        return -1;
    }

    //convert Z to float and normalize
    Zsize=Zi.dim[0];
    Z = (float*)malloc(Zsize*sizeof(float));

    //ugly hack, if macro, data comes from .mat file and need to be fixed from 16 bit to float
    if(Micro==false){
      for(i=0;i<Zi.dim[0];i++)
        Z[i]=((float)Zi.re_part.data_16[i])/Norm;
    }else{
      for(i=0;i<Zsize;i++){
        Z[i]=Zi.re_part.data_f[i]/Norm;
      }
      free(Zi.dim); //free pointer to dimension
    }

    free(Zi.freeMe);

    //generate t0
    t0 = (float*)malloc(Zsize*sizeof(float));
    if (Micro && samplingbool)
      for(i=0;i<Zsize;i++)
        t0[i]=((float)i)/12207.03125; //~12kHz sampling
    
    if (Micro && !samplingbool)
      for(i=0;i<Zsize;i++)
        t0[i]=60.0*((float)i)/12207.03125; //~12kHz sampling
     
    if (!Micro)
      for(i=0;i<Zsize;i++)
        t0[i]=((float)i)/200.0; //200Hz sampling
         

    mean=0.0;
    for(i=0;i<Zsize-1;i++){
      float diff=t0[i+1]-t0[i];
      mean+=diff;
    }
    free(t0);
    dt=mean/((float)(Zsize-1));
    Fs = 1.0/dt;
    Window = floor(Fs*WindowTime);
    OverlapWindow = floor(Fs*OverlapWindowTime);

    //Binning the data
    StartIndex=0;
    count=0;

    //avoid final partial window
    iterations = floor((float)(Zsize-Window)/(float)OverlapWindow);
    ret->MaxFreq=(float*)malloc(iterations*sizeof(float));
    ret->MaxFreqsize=iterations;


		// START of ROI
    SimRoiStart();

    for(iter=0;iter<iterations;iter++){
			float meanD;
      int Xsize;
      float* X; 
			int J=35;  //J=50; //reduced for simple scalar
			float max=0.0; 
		  Compute_Wavelet_Return cw_ret;    

      //Windowed Data
      float* D=&Z[StartIndex]; 
			count++;

      StartIndex=StartIndex+OverlapWindow;
      
      //at end of input signal we shrink the window to process tail??
      if(Zsize-StartIndex<Window){
        Window=Zsize-StartIndex;
      }  

      meanD=fmean(D, Window);
      Xsize=Window;
      X = malloc(Xsize*sizeof(float));
      for(i=0;i<Xsize;i++){
        X[i]=D[i]-meanD; //The windowed Data
      }

      
			//Wavelet 
      Compute_Wavelet(&cw_ret, X,Xsize,"DOG",2,true,0.0,Fs,.1129,.005,J);
      
      for(i=0;i<J;i++){
        float sum=0.0;
        for(j=0;j<Xsize;j++){
          sum+=abs(cw_ret.U[i][j]);
        }
        sum=sum/(float)Xsize;
        if(sum>max){
          max=sum;
          ret->MaxFreq[iter]=cw_ret.Freq[i];
        }
      } 

      free(cw_ret.Freq);
      free(cw_ret.scale);
      for(i=0;i<cw_ret.Usize[0];i++){
        free(cw_ret.U[i]);
      }                  
      free(cw_ret.U);
      free(X);
    
    } //end iter loop

		// END of ROI
    SimRoiEnd();

    free(Z); //free original Z

  } //end //end of data present check loop


  return 0;
}
