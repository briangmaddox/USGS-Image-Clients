#include "Image/TIFFImageIFile.h"
#include "Image/TIFFImageOFile.h"
#include "Image/RGBPallette.h"
#include <iostream.h>


void main(int argc, char **argv)
{
   if (argc < 4)
   {
      cerr << "usage: " << argv[0] << " <tiff> <tiff> cw|ccw" << endl;
      exit(69);
   }

   TIFFImageIFile *fin;
   TIFFImageOFile *fout;
   RGBPallette *pal;
   int direction;
   long width, height;
   unsigned char *line;
   unsigned char *pix;
   int i,j,k,row;

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

   fin = new TIFFImageIFile(argv[1]);
   if (!fin->good())
   {
      cerr << "Cannot open " << argv[1] << endl;
      exit(69);
   }
   width  = fin->getWidth();   
   height = fin->getHeight();   
 
   fout = new TIFFImageOFile(argv[2],height,width);
   if (!fout->good())
   {
      cerr << "Cannot open " << argv[2] << endl;
      exit(69);
   }

   pal = (RGBPallette *) fin->getPallette();
   fout->setPallette(pal);

   line = new unsigned char[height];
   row = 0;
   if (direction == 1)
   {
      for (i=0;i<width;i++)
      {
          k = 0;
          for (j=height-1;j>=0;j--) 
          {
              pix = (unsigned char *) fin->getRawPixel(i,j);
              line[k] = *pix; 
              k++;
              delete pix;
          }
          fout->putRawScanline(line,row);
          row++;
      }
   }
   if (direction == 2)
   {
      for (i=width-1;i>=0;i--)
      {
          k = 0;
          for (j=0;j<height;j++) 
          {
              pix = (unsigned char *) fin->getRawPixel(i,j);
              line[k] = *pix; 
              k++;
              delete pix;
          }
          fout->putRawScanline(line,row);
          row++;
      }
   }


   delete pal;
   delete fin;
   delete fout;
   delete[] line;
}
