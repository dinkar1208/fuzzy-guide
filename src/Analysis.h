#ifndef ANALYSIS_H
#define ANALYSIS_H

//#include <stdbool.h>
#include "mybool.h"

typedef struct{
  int Tim;
  float* MaxFreq;
  int MaxFreqsize;
  int NonLinMeasure;
  int WhiteMeasure; 
  int RSMeasure;
}AnalysisReturn_t;

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
  );

#endif
