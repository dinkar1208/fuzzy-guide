#ifndef PPM_H
#define PPM_H


//write a ppm file from a data array
int writePPM(
    char* filename, 
    unsigned char* data, 
    int width, 
    int height, 
    int channels
);

#endif