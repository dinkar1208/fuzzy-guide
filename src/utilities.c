#include "stdio.h"
#include <stdlib.h>
#include "utilities.h"
#ifdef MATLAB_FILE_SUPPORT
#include <zlib.h>
#endif

typedef enum{
    miINT8          =1,
    miUINT8         =2,
    miINT16         =3,
    miUINT16        =4,
    miINT32         =5,
    miUINT32        =6,
    miSINGLE        =7,
    //--
    miDOUBLE        =9,
    //--
    //--
    miINT64         =12,
    miUINT64        =13,
    miMATRIX        =14,
    miCOMPRESSED    =15,
    miUTF8          =16,
    miUTF16         =17,
    miUTF32         =18
}MatlabDataType;

typedef enum{
mxCELL_CLASS        = 1,
mxSTRUCT_CLASS      = 2,
mxOBJECT_CLASS      = 3,
mxCHAR_CLASS        = 4,
mxSPARSE_CLASS      = 5,
mxDOUBLE_CLASS      = 6,
mxSINGLE_CLASS      = 7,
mxINT8_CLASS        = 8,
mxUINT8_CLASS       = 9,
mxINT16_CLASS       = 10,
mxUINT16_CLASS      = 11,
mxINT32_CLASS       = 12,
mxUINT32_CLASS      = 13,
mxINT64_CLASS       = 14,
mxUINT64_CLASS      = 15
}MatlabMatrixClass;


int countLines(FILE* fp){
  //save positionf of fp
  long int pos = ftell(fp);
	int lines=0;
	int ch;

  //start at top of file
  fseek(fp, 0, SEEK_SET);
 
  for(; EOF != (ch=fgetc(fp)); )
    if (ch==(int)'\n')
        lines++;

  //set fp back to where it came from
  fseek(fp, pos, SEEK_SET); 

  return lines;
}

//load newline separated array of floats
int loadFloatArray(char* filename, FloatArray* arr){
  FILE* fp=NULL;
	int ret;
  int entries;
  
  //open file for reading
  fp=fopen(filename, "rt");
  if (fp==NULL){
    printf("Unable to open file %s!\n", filename);
    return -1;
  }

  //count number of newlines (==entries)
  entries=countLines(fp);

  //allocate float array
  arr->data = (float*) malloc(entries*sizeof(float));

  //read float array in
  arr->size=0;
  do{
    ret=fscanf(fp,"%f", &(arr->data[arr->size]));
    arr->size++;
  }while( ((arr->size)<entries) &&  ret!=EOF );
  //close file
  fclose(fp);

  //return read entries
  return arr->size;
}

void byteswap(float* x){
  char* px=(char*)x;
	int i;
  for(i=0;i<sizeof(float)/2;i++){
    char temp=px[i];
    px[i]=px[sizeof(float)-1-i];
    px[sizeof(float)-1-i]=temp;
  } 

}

//load newline separated array of floats
int loadBinFloatArray(char* filename, FloatArray* arr){
  FILE* fp=NULL;
	int entries;
	int bytes;  

  //open file for reading
  fp=fopen(filename, "rb");
  if (fp==NULL){
    printf("Unable to open file %s!\n", filename);
    return -1;
  }

  //count number of newlines (==entries)
  fseek(fp,0,SEEK_END);
  bytes=ftell(fp);
  fseek(fp,0,SEEK_SET);

  //allocate float array
  entries=bytes/sizeof(float);
  arr->data = (float*) malloc(entries*sizeof(float));

  //read float array in
  arr->size =fread( arr->data,sizeof(float),entries,fp);

  //close file
  fclose(fp);

  //return read entries
  return arr->size;
}


byte* parseFieldToArray(byte* input, byte** arr, int* size){
  unsigned int dataType=((unsigned int*)input)[0];
  unsigned int numBytes=((unsigned int*)input)[1];
	int allignedNumBytes=numBytes;
	int byteSize;  
  byte* endOfArray;

	*arr=(byte*)&((unsigned int*)input)[2];
	
  
  //allign to word boundaries
  
  if(allignedNumBytes%4!=0)
      allignedNumBytes+=(4-allignedNumBytes%4);
  endOfArray = &(input[8+allignedNumBytes]);


  if ((dataType&0xFFFF0000) != 0){
    //printf("short notation\n");
    dataType=(unsigned int)(((short unsigned int*)input)[1]);
    numBytes=(unsigned int)(((short unsigned int*)input)[0]);
    (*arr)=(byte*)&((short unsigned int*)input)[2];
    //allign to word boundaries
    allignedNumBytes=numBytes;
    if(allignedNumBytes%4!=0)
      allignedNumBytes+=(4-allignedNumBytes%4);
    endOfArray = &(input[4+allignedNumBytes]);
  }


  
  switch(dataType){
    case miINT8:
    case miUINT8:
    case miUTF8:
      byteSize=1;
    break;

    case miINT16:
    case miUINT16:
    case miUTF16:
      byteSize=2;
    break;
    
    case miINT32:
    case miUINT32:
    case miUTF32:
      byteSize=4;
    break;
    
    case miSINGLE:
      byteSize=4;
    break;
    
    case miDOUBLE:
      byteSize=8;
    break;
  
    case miINT64:
    case miUINT64:
      byteSize=8;
    break;
    
    default:
      //compressed, matrix or reserved
      return NULL;
  }
  (*size)=numBytes/byteSize;

  return endOfArray;
}


#ifdef MATLAB_FILE_SUPPORT
int loadMATFile(char* filename, MatlabMatrix* ma){
  FILE* fp=NULL;
 
  //open file for reading
  fp=fopen(filename, "rb");
  if (fp==NULL){
    printf("Unable to open file %s!\n",filename);
    return -1;
  }

  //skip human readable header
  fseek(fp, 116, SEEK_SET);
  
  //parse subsystem data offset
  long long int subsystemDataOffset; //8 bytes
  if (fread((void*)&subsystemDataOffset,1,8,fp)!=8) return -1;

  //version and endianess
  short int version, endian;
  if (fread((void*)&version,1,2,fp)!=2) return -1;
  if (fread((void*)&endian,1,2,fp)!=2) return -1;

  if( ((char*)&endian)[0]=='M'){
    //need to perform byte swapping!
    printf("byteSwapping needed (?)!\n");
    return -1;
  }

  //parse elements
  while(!feof(fp)){

    unsigned int dataType, numBytes;
    if (fread((void*)&dataType,1,4,fp)!=4) break;
    if (fread((void*)&numBytes,1,4,fp)!=4) break;

    //check if small data element format
    if ( (dataType&0xFFFF0000) != 0){
      //small format....reparse
      fseek(fp, -8, SEEK_CUR);
      short unsigned int sh_numBytes, sh_dataType;
      if (fread((void*)&sh_numBytes, 1,2,fp)!=2) break;
      if (fread((void*)&sh_dataType, 1,2,fp)!=2) break;
      dataType=(unsigned int) sh_dataType;
      numBytes=(unsigned int) sh_numBytes;
    }
    
    //alloc room for data and read in
    byte* data= (byte*)malloc(numBytes);
    byte* free_data_ptr=data;//pointer to data to be freed at some point
    if (fread((void*)data,1,numBytes,fp)!=numBytes) break;

    


    //if data was compressed, we now need to decompress it!
    if (dataType==miCOMPRESSED){
      //create stream for gzip
      z_stream strm;

      // allocate inflate state and set defaults
      strm.zalloc = Z_NULL;
      strm.zfree  = Z_NULL;
      strm.opaque = Z_NULL;

      //set input
      strm.avail_in = numBytes;
      strm.next_in = (byte*) data;

      //init inflate
      if (inflateInit(&strm) != Z_OK){
        printf("failure while trying to inflate compressed MATLAB data in file %s\n", filename);
        return -1;
      }

      //keep inflating untill end of input is reached
      int outputPtr=0;
      byte* out_data=NULL; //initially just allocate with size 0

      do{
        //(re)alloc out_data to make room
        out_data=(byte*)realloc(out_data, outputPtr+numBytes);              //grow by num_bytes

        //set output buffer
        strm.avail_out = numBytes;                               //we now have num_bytes space in output
        strm.next_out = &(out_data[outputPtr]); //set output buffer

        //inflate and check for errors
        switch (inflate(&strm, Z_NO_FLUSH)) {
          case Z_NEED_DICT:
          case Z_DATA_ERROR:
          case Z_MEM_ERROR:
              (void)inflateEnd(&strm);
              printf("failure while trying to inflate compressed MATLAB data in file %s\n", filename);
              return -1;
        }

        //avail out is a measure for how many bytes still free in the buffer!
        int decompressed = numBytes - strm.avail_out; 

        //increase outputPtr by bytes processed
        outputPtr+=decompressed;                  
      
      }while (strm.avail_out == 0); //when 0 bytes left, there is still data to be deflated

      //free compressed data
      free(data);

      //data is now the decompressed data
      data=out_data;
      free_data_ptr=out_data;

      //outputPtr now tells us how many bytes we have in 'data'
      numBytes=outputPtr; 

      //now parse the header of our uncompressed data
      dataType=((unsigned int*)data)[0];
      numBytes=((unsigned int*)data)[1]-2*4;
      data=(byte*)&(((unsigned int*)data)[2]); //adjust data accordingly
    }

   
    switch(dataType){

      case miMATRIX:
        //printf("handle miMatrix\n");
        
       
        ma->freeMe=free_data_ptr;//pointer to data to be freed at some point

        byte* end = data+numBytes;

        //parse header of array flags
        dataType=((unsigned int*)data)[0];
        numBytes=((unsigned int*)data)[1];

        if(dataType!=miUINT32){
          printf("first sub of matrix is not array flags?\n");
          return -1;
        }

        //get array flags
        byte* flags = (byte*)&(((unsigned int*)data)[2]);
        if (flags[1] & (1<<4))
          printf("complex\n");
        if (flags[1] & (1<<5))
          printf("global\n");
        if (flags[1] & (1<<6))
          printf("logical\n");

        //class
        //flags[0]; //?

        //increase data ptr
        data=&(flags[8]);

        //get dimensions
        dataType=((unsigned int*)data)[0];
        numBytes=((unsigned int*)data)[1];
        if(dataType!=miINT32){
          printf("second sub of matrix is not array flags?\n");
          return -1;
        }
        data=parseFieldToArray(data,(byte**)&(ma->dim),&(ma->nDim));


        //get name
        data=parseFieldToArray(data,(byte**)&(ma->name),&(ma->nameLen));
        //NB should terminate name string!

        //get real part
        int len;
        data=parseFieldToArray(data,(byte**)&(ma->re_part.data),&len);

        
        //check for optional imaginary part
        if(data<end){
          data=parseFieldToArray(data,(byte**)&(ma->im_part.data),&len);  
        }

        //return succes (TODO: parse other contents!)
        fclose(fp);
        return 0;
      break;


      default:
        printf("datatype not implemented yet\n");
    }


    //here we now have 'numBytes' of data in void* data

//    for(int i=0;i<numBytes/4;i++){
//      printf("%x ",((unsigned int*)data)[i]) ;

//      if (i%(16)==0)
//        printf("\n");
//    }


//    FILE* fpo =fopen("test.bin","wb");
//    fwrite(data, 1, numBytes, fpo);
//    fclose(fpo);

    

  }
  //EOF ?
  fclose(fp);
  return 0;
}
#endif
