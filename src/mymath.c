#include "mymath.h"
#include <math.h>

//mean is a reserved name ?
float fmean(float* arr, int cnt){
  float mean=0.0;
  int i;
  for(i=0;i<cnt;i++)
    mean+=arr[i];
  return (mean/(float)cnt);
}

//round to nearest integer towards zero
int fix(float a){
    return (a<0)?(-1*floor(-1.0*a)):floor(a);
}
