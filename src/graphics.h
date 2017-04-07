#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef struct{
    unsigned char* data;
    int width;
    int height;
}Graph;


int initGraph(
    Graph* G, 
    int width, 
    int height
);

int destroyGraph(
    Graph* G
);

int plotTrend(
    Graph* G, 
    float* arr, 
    int arrSize, 
    float ymin, 
    float ymax,
    unsigned char* color
);


#endif
