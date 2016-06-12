#include "Image/TIFFImageIFile.h"
#include "drg/MetFile.h"
#include "Image/GeoTIFFImageOFile.h"
#include "Image/RGBPallette.h"
#include <iostream.h>
#include <string.h>

int main(int argc, char **argv)
{
   char* metfn;
   char* gtiffn;
   char* tiffn;
   char* foo;
   char* basefn;
   MetFile mf;
   TIFFImageIFile* fin;
   GeoTIFFImageOFile* fout;
   RGBPallette* pal;
   int i;
   unsigned char* line;

   if (argc!=2)
   {
      cerr << "usage: tiff2gtiff <tiff>" << endl;
      exit(1);
   }

   tiffn = new char[strlen(argv[1])+1];
   strcpy(tiffn,argv[1]);
   basefn = new char[strlen(tiffn)+1];
   strcpy(basefn,tiffn);
   foo = &basefn[strlen(basefn)];
   while (*foo != '.') foo--;
   if (foo!=basefn)
      *foo = (char)0;
   metfn = new char[strlen(basefn)+5];
   strcpy(metfn,basefn);
   strcat(metfn,".met");
   gtiffn = new char[strlen(tiffn)+5];
   strcpy(gtiffn,basefn);
   strcat(gtiffn,".gtf");
   if (!mf.readFromFile(metfn))
   {
      cerr << "Cannot open " << metfn << endl;
      exit(2);
   }
   fin = new TIFFImageIFile(tiffn);
   if (!fin->good())
   {
      cerr << "Cannot open " << tiffn << endl;
      exit(3);
   }
   fout = new GeoTIFFImageOFile(gtiffn,fin->getWidth(),fin->getHeight());
   if (!fout->good())
   {
      cerr << "Cannot open " << gtiffn << endl;
      exit(4);
   }
   pal = (RGBPallette *) fin->getPallette();
   fout->setPallette(pal);
   delete pal;
   fout->setUSGS_UTM(mf.getULX(),mf.getULY(),
                     mf.getXPixRes(),mf.getYPixRes(),mf.getUTMZone());
   for (i=0;i<fin->getHeight();i++)
   {
       line = (unsigned char *) fin->getRawScanline(i);
       fout->putRawScanline(line,i);
       delete[] line;
   }

   delete fin;
   delete fout;
   delete[] basefn;
   delete[] tiffn;
   delete[] gtiffn;
   delete[] metfn;
   return 0;
}
