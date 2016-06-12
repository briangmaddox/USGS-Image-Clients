#include "Image/TIFFImageIFile.h"
#include "Image/TIFFImageOFile.h"
#include "Image/RGBPallette.h"
#include <iostream.h>

void main(int argc, char **argv)
{
   TIFFImageIFile* fin;
   TIFFImageOFile* fout;
   RGBPallette* pal;
   unsigned char *line;
   unsigned char *new_line;
   char updt[4] = { '/','-','\\','|' };
   char foo = 0;
   int l,i,k,h,w;
   unsigned short s;

   if (argc != 3)
   {
      cerr << "usage: " << argv[0] << " <intiff> <outtiff>" << endl;
      exit(69);
   }

   fin = new TIFFImageIFile(argv[1]);
   fin->getTag(TIFFTAG_BITSPERSAMPLE,&s);
   if (s != 4)
   {
      cerr << argv[1] << " is not a 4 bit tiff." << endl;
      exit(69);
   }   
   fout = new TIFFImageOFile(argv[2],fin->getWidth(),fin->getHeight());
   pal = (RGBPallette *) fin->getPallette();
   fout->setPallette(pal);

   h = fin->getHeight();
   w = fin->getWidth();
   cout << "Converting " << h << " scanlines." << endl;
   
   new_line = new unsigned char[w];
   for (l=0;l<h;l++)
   {
       if ((l%20) == 0)
       {
          cout << updt[foo] << " " << l << "\r" << flush;
          foo++;
          foo = foo % 4;
       } 
       line = (unsigned char *) fin->getRawScanline(l);
       k = 0;
       for (i=0;i<w-1;i+=2)
       {
           new_line[i] = line[k] >> 4;
           new_line[i+1] = line[k] & 0x0f;
           k++;
       }
       fout->putRawScanline(new_line,l);
       delete[] line;
   }
   cout << "Done.                        " << endl;
    
   delete[] new_line;
   delete fin;
   delete fout;
   delete pal;
}
