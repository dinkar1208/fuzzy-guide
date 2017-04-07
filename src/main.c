#include "Analysis.h"
#include <stdio.h>
#include <stdlib.h>
#include "graphics.h"
#include "ppm.h"

//normal values
//#define WINDOW_SEC 10
//#define WINDOW_OVERLAP_SEC 8


//adjusted to reduce simple scalar running time
#define WINDOW_SEC 2
#define WINDOW_OVERLAP_SEC 1


int main(void){

  //set the number of threads for openmp to use
  //by default we set this to the number of processors 
  //but of course you are free to change this number
  omp_set_num_threads(N_PROC);

  //you can uncomment the code below to verify 
  //openMP is working correctly
  /*
  #pragma omp parallel
  {
    // Obtain thread number
    int tid = omp_get_thread_num();
    printf("Hello World from thread = %d\n", tid);

    // Only master thread does this
    if (tid == 0){
      int nthreads = omp_get_num_threads();
      printf("Number of threads = %d\n", nthreads);
    }
  }*/

  //define colors to use for graph
  unsigned char colors[8][3]={
    {0xFF,0x00,0x00}, //red
    {0x00,0xFF,0x00}, //green
    {0x00,0x00,0xFF}, //blue
    {0xFF,0xFF,0x00}, //yellow
    {0x00,0xFF,0xFF}, //turqiose
    {0xFF,0x00,0xFF}, //magenta
    {0x00,0x00,0x00}, //black
    {0x80,0x80,0x80}  //grey
  };

  //return structure for analysis function
  AnalysisReturn_t A_ret;

  //channels
	int ch;

	//for min, max determination
	float max, min;
	int i;

	//initialize an empty graph
  Graph G;
  initGraph(&G, 1080, 720);
  
  //loop over all 8 available channels
	//N.B. only first channel for simple scalar!
  for(ch=1;ch<=1;ch++){

    printf("Micro 200Hz rat1 channel %d\n", ch);
  
    Analysis(
    	&A_ret,        			//structure to hold return values
    	true,          			// true/false implies the data is obtained with micro/macro recordings
		  false,        			// If Micro is true, then samplingbool=true/false implies the micro data considered at 12KHz/200Hz
		  1,             			// Gives the Seizure number considered for analysis (1..22 for Micro Data and 1..16 for Macro Data)
		  ch,             		// If Micro is true, then CHNum gives which channel numbers are considered (1..8)
		  WINDOW_SEC,    			// Time in seconds of windowed data considered for analysis
		  WINDOW_OVERLAP_SEC,	// Time in seconds for the overlap considered for moving average analysis
		  0,             			// subwindow size for calculation the direct energy measures
		  0										// Subwindow time in seconds considered for WhiteAutocorrelation measures
		);             

    //thresholds (based on electrode placement)
    //Ipsi-CA 8.5
    //Ipsi-DG 10.0
    //Contra-DG 9.0
    //Contra-CA 9.0 
    
    //plot graph as ascii 
    //asciiGraph(A_ret.MaxFreq, A_ret.MaxFreqsize, 10, "outputGraph.txt", 8.5);
    
		max=A_ret.MaxFreq[0];
		min=max;
		for(i=1;i<A_ret.MaxFreqsize;i++){
			if(A_ret.MaxFreq[i]>max)
				max=A_ret.MaxFreq[i];
			if(A_ret.MaxFreq[i]<min)
				min=A_ret.MaxFreq[i];
			printf("%f\n",A_ret.MaxFreq[i]);
		}

    //add trend to graph
    plotTrend(&G, A_ret.MaxFreq, A_ret.MaxFreqsize, min, (min+max)*1.1, colors[ch]);

    //free max frequency (each round!)
    free(A_ret.MaxFreq);
  }

  //write graph to ppm figure
  writePPM("wavelet.ppm",G.data, G.width, G.height, 3);

  //clean up
  destroyGraph(&G);

  return 0;
}





/*
//legacy function
//plot graph as ascii
int asciiGraph(float* data, int size, int levels, char* filename, float threshold){
  float max=0.0;
  for(int i=0;i<size;i++){
    if( data[i]>max )
      max=data[i];
  }

  FILE* fp = fopen(filename,"wt");
  if(fp==NULL){
    printf("failed to open file %s\n",filename);
    return -1;
  }

  float dLevel=max/(float)levels;
  for(int row=0;row<levels;row++){
    for(int i=0;i<size;i++){
      if(data[i]>=(levels-row)*dLevel && data[i]<(levels-row+1)*dLevel)
        if(data[i]>=threshold)
          fprintf(fp,"o");
        else
          fprintf(fp,"+");
      else
        fprintf(fp," ");
    }
    fprintf(fp,"\n");
  }
  fclose(fp);
  return 0;
}*/
