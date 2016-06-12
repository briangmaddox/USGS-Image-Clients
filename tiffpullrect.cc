#include <iostream.h>
#include "Image/TIFFImageIFile.h"
#include "Image/TIFFImageOFile.h"
#include "Image/RGBPallette.h"
#include "Image/RGBPixel.h"


void main(int argc, char **argv)
{
   TIFFImageIFile *fin;
   TIFFImageOFile *fout;
   unsigned char* rect;
   unsigned char* line;
   RGBPallette* pal;
   long i;


   long startx, starty, length, width;

   if (argc<7)
   {
      cerr << "usage: pullcrlrect <intiff> <x> <y> <w> <l> <outtiff>" << endl;
      exit(69);
   }

   startx = atol(argv[2]);
   starty = atol(argv[3]);
   width  = atol(argv[4]);
   length = atol(argv[5]);

   fin = new TIFFImageIFile(argv[1]);
   if (!fin->good())
   {
      cerr << "Cannot open " << argv[1] << endl;
      exit(69);
   }
   pal = (RGBPallette*)fin->getPallette();
   fout = new TIFFImageOFile(argv[6],width,length);
   if (!fout->good())
   {
      cerr << "Cannot open " << argv[6] << endl;
      exit(69);
   }
   fout->setPallette(pal);

   rect = (unsigned char*) fin->getRawRectangle(startx, starty,
                                        startx+width-1,starty+length-1);
   for (i=0;i<length;i++)
   {
      line = &(rect[i*width]);
      fout->putRawScanline(line,i); 
   }

   delete[] rect;
   delete pal;
   delete fin;
   delete fout;
}
