#include "Image/DGPImageIOFile.h"
#include "Image/DGPImageOFile.h"
#include "Image/RGBPallette.h"
#include <iostream.h>

#define SLICE 400 

void rotate_slice(unsigned char *slice, unsigned char *rotslice,
                  long width, long height,int direction)
{
   int i,j,k;

   k = 0;

   if (direction == 1)
   {
      for (i=0;i<width;i++)
          for (j=height-1;j>=0;j--)
          {
              rotslice[k] = slice[j*width+i];
              k++;
          }
   }
   if (direction == 2)
   {
      for (i=width-1;i>=0;i--)
          for (j=0;j<height;j++)
          {
              rotslice[k] = slice[j*width+i];
              k++;
          }
   }
}

void main(int argc, char **argv)
{
   if (argc < 4)
   {
      cerr << "usage: " << argv[0] << " <tiff> <tiff> cw|ccw" << endl;
      exit(69);
   }

   DGPImageIOFile *fin;
   DGPImageOFile *fout;
   RGBPallette *pal;
   int direction;
   long width, height;
   unsigned char *slice;
   unsigned char *rotslice;
   int i,j,k,row;
   long h;
   char updt[4] = { '|','/','-','\\' };
   int bar;

   if (strcmp(argv[3],"cw")==0)
      direction = 1;
   else if (strcmp(argv[3],"ccw")==0)
      direction = 2;
   else
   {
      cerr << "unknown direction " << argv[3] << endl;
      cerr << "usage: " << argv[0] << " <tiff> <tiff> cw|ccw" << endl;
      exit(69);
   }

   fin = new DGPImageIOFile(argv[1]);
   if (!fin->good())
   {
      cerr << "Cannot open " << argv[1] << endl;
      exit(69);
   }
   width  = fin->getWidth();   
   height = fin->getHeight();   
 
   fout = new DGPImageOFile(argv[2],height,width);
   if (!fout->good())
   {
      cerr << "Cannot open " << argv[2] << endl;
      exit(69);
   }

   pal = (RGBPallette *) fin->getPallette();
   fout->setPallette(pal);

   rotslice = new unsigned char[width*SLICE];
   row = 0;
   cout << "Rotating..." << endl;
   bar = 0;
   if (direction == 1)
   {
      for (i=0;i<height;i+=SLICE)
      {
          cout << updt[bar] << " " << i << "\r" << flush;
          bar++;
          bar = bar % 4;
          if ((i+SLICE-1)<height)
          {
             slice = (unsigned char *) fin->getRawRectangle(0,i,width-1,i+SLICE-
1);
             h = SLICE;
          }
          else
          {
             slice = (unsigned char *) fin->getRawRectangle(0,i,width-1,height-1
);
             h = height - i;
          }
          rotate_slice(slice,rotslice,width,h,direction);
          fout->putRawRectangle(rotslice,height-i-h,0,height-i,width-1);
          delete[] slice;
      }
   }
   if (direction == 2)
   {
      for (i=0;i<height;i+=SLICE)
      {
          cout << updt[bar] << " " << i << "\r" << flush;
          bar++;
          bar = bar % 4;
          if ((i+SLICE-1)<height)
          {
             slice = (unsigned char *) fin->getRawRectangle(0,i,width-1,i+SLICE-1);
             h = SLICE;
          }
          else
          {
             slice = (unsigned char *) fin->getRawRectangle(0,i,width-1,height-1);
             h = height - i;
          }
          rotate_slice(slice,rotslice,width,h,direction);
          fout->putRawRectangle(rotslice,i,0,i+h-1,width-1);
          delete[] slice;
      }
   }

   cout << "Done.          " << endl;

   delete pal;
   delete fin;
   delete fout;
   delete[] rotslice;
}

