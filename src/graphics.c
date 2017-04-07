#include <math.h>
#include <stdlib.h>
#include "mybool.h"
#include "graphics.h"

void setPixel(Graph* G, int x, int y, unsigned char* color){
    G->data[G->width*y*3+x*3+0]=color[0];//R
    G->data[G->width*y*3+x*3+1]=color[1];//G
    G->data[G->width*y*3+x*3+2]=color[2];//B
}

//Bresenham's line algorithm
void drawLine(Graph* G, int x0, int y0, int x1, int y1, unsigned char* color){
    int dx  =  abs(x1-x0);
    int sx  =  x0<x1 ? 1 : -1;
    int dy  =  abs(y1-y0);
    int sy  =  y0<y1 ? 1 : -1;
    int err =  (dx>dy ? dx : -dy)/2;
    int e2;

    while(true){
        if(x0<0 || x0>=G->width || y0<0 || y0>=G->height) break; //out of bounds!
        setPixel(G, x0, y0, color);
        if(x0==x1 && y0==y1) break;
        e2=err;
        if(e2 > -dx) {err -= dy; x0 += sx; }
        if(e2 <  dy) {err += dx; y0 += sy; }
    }
}

int initGraph(Graph* G, int width, int height){
		int i;    
		G->data=(unsigned char*) malloc(width*height*3);
    G->width=width;
    G->height=height;
    for(i=0;i<width*height*3;i++){
        G->data[i]=0xFF;
    }

    return 0;
}

int destroyGraph(Graph* G){
    free(G->data);
    return 0;
}

int plotTrend(Graph* G, float* arr, int arrSize, float ymin, float ymax, unsigned char* color){
    float dx = ((float)(G->width))/((float)arrSize);
    float dy = ((float)(G->height-1))/(ymax-ymin);
		int i;

    //draw data, in black for now
    for(i=0;i<arrSize-1;i++){
        drawLine(G,floor(i*dx),G->height-1-floor(arr[i]*dy),floor((i+1)*dx), G->height-1-floor(arr[i+1]*dy), color);
    }

    //possibly add some kind of grid later...

    return 0;
}
