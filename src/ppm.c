#include <stdio.h>
#include "ppm.h"


int writePPM(char* filename, unsigned char* data, int width, int height, int channels){

  FILE *fp = fopen(filename, "wb"); 
  if(fp==NULL){
    printf("unable to open file %s\n",filename);
    return -1;
  }

  fprintf(fp, "P6\n%d %d\n255\n", width, height);
  fwrite(data, 1, width*height*channels, fp);
  fclose(fp);
  return 0;
}