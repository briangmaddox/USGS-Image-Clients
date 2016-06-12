#include "Image/DGPImageIOFile.h"
#include "Image/RGBPixel.h"
#include "Image/RGBPallette.h"
#include <iostream.h>

#define PINK           9 
#define PURPLE         6
#define GREEN          5
#define BLUE           2
#define LTBLUE         8
#define WHITE          1
#define THRESHOLD      8
#define GRANULARITY   16 


int pink_t;
int purple_t;
int green_t;
int blue_t;
int ltblue_t;

int pink_count = 0;
int purple_count = 0;
int green_count = 0;
int blue_count = 0;
int ltblue_count = 0;

void den_count(int &pink, int& purple, int& green, int& blue, int& ltblue,
           unsigned char* rect, 
           long address,long width,long columns, long rows)
{
    long i,j;
    pink=0;
    purple=0;
    green=0;
    blue=0;
    ltblue=0;
    for (i=0;i<columns;i++)
        for (j=0;j<rows;j++)
        {
            if (rect[j*width+address+i] == PINK)
               pink++;
            else if (rect[j*width+address+i] == PURPLE)
               purple++;
            else if (rect[j*width+address+i] == GREEN)
               green++;
            else if (rect[j*width+address+i] == BLUE)
               blue++;
            else if (rect[j*width+address+i] == LTBLUE)
               ltblue++;
        }
    pink_count   += pink;
    purple_count += purple;
    green_count  += green;
    blue_count   += blue;
    ltblue_count += ltblue;
}

void change_from(unsigned char color, unsigned char* rect,
               long address, long width,long columns,long rows)
{
    long i,j;
    for (i=0;i<columns;i++)
        for (j=0;j<rows;j++)
        {
            if (rect[j*width+address+i] == color)
               rect[j*width+address+i] = WHITE;
        }
} 

void den_proc(unsigned char* rect,long width,
             long rows, long granularity)
{
    long i;
    long columns;
    int pink, purple, green,blue,ltblue;

    for (i=0;i<width;i+=granularity)
    {
        if (i+granularity>=width)
        {
           columns = width - i;
           den_count(pink,purple,green,blue,ltblue,rect,i,width,columns,rows);
           if (pink<pink_t)
              change_from(PINK,rect,i,width,columns,rows);
           if (purple<purple_t)
              change_from(PURPLE,rect,i,width,columns,rows);
           if (green<green_t)
              change_from(GREEN,rect,i,width,columns,rows);
           if (blue<blue_t)
              change_from(BLUE,rect,i,width,columns,rows);
           if (ltblue<ltblue_t)
              change_from(LTBLUE,rect,i,width,columns,rows);
        }
        else 
        {
           den_count(pink,purple,green,blue,ltblue,rect,i,width,granularity,rows);
           if (pink<pink_t)
              change_from(PINK,rect,i,width,granularity,rows);
           if (purple<purple_t)
              change_from(PURPLE,rect,i,width,granularity,rows);
           if (green<green_t)
              change_from(GREEN,rect,i,width,granularity,rows);
           if (blue<blue_t)
              change_from(BLUE,rect,i,width,granularity,rows);
           if (ltblue<ltblue_t)
              change_from(LTBLUE,rect,i,width,granularity,rows);
        }
    }
}

void denoise(DGPImageIOFile* foo, int granularity)
{
     long height;
     long i;
     unsigned char* slice;
     int num_rows_left;
       
     height = foo->getHeight();

     for (i=0;i<height;i+=granularity)
     {
         if (i+granularity>=height)
         {
            num_rows_left = height-i;
            
            slice=(unsigned char*)foo->getRawRectangle(0,i,
                                  foo->getWidth()-1, i+num_rows_left-1);
            den_proc(slice,foo->getWidth(),num_rows_left,granularity);
            foo->putRawRectangle(slice,0,i,
                                    foo->getWidth()-1,i+num_rows_left-1);
         }
         else
         {
            slice=(unsigned char*)foo->getRawRectangle(0,i,
                                  foo->getWidth()-1, i+granularity-1);
            den_proc(slice,foo->getWidth(),granularity,granularity);
            foo->putRawRectangle(slice,0,i,
                                    foo->getWidth()-1,i+granularity-1);
         }
         delete[] slice;
     }   
}

void main(int argc, char** argv)
{
     DGPImageIOFile* finout;
     long granularity;
    

     if (argc<7)
     {
        cerr << "usage: denoise <dgp> <pink> <purple> <green> <blue> <green>" << endl;
        exit(69);
     }

     finout = new DGPImageIOFile(argv[1]);
     granularity = 20;
     pink_t = atoi(argv[2]); 
     purple_t = atoi(argv[3]);
     green_t = atoi(argv[4]);
     blue_t = atoi(argv[5]);
     ltblue_t = atoi(argv[6]);

     denoise(finout,granularity);

     cout << "Pink count:   " << pink_count << endl;
     cout << "Purple count: " << purple_count << endl;
     cout << "Green count:  " << green_count << endl;
     cout << "Blue count:   " << blue_count << endl;
     cout << "LtBlue count: " << ltblue_count << endl;
     delete finout;
}
