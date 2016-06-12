#include "Image/DGPImageIFile.h"
#include "drg/MetFile.h"
#include "Image/GeoTIFFImageOFile.h"
#include "Image/RGBPallette.h"
#include <iostream.h>
#include <string.h>

int main(int argc, char **argv)
{
   int status;
   char updt[4] = { '|','/','-','\\' };
   char* metfn;
   char* gtiffn;
   char* dgpfn;
   char* foo;
   char* basefn;
   MetFile mf;
   DGPImageIFile* fin;
   GeoTIFFImageOFile* fout;
   RGBPallette* pal;
   int i;
   unsigned char* line;

   if (argc!=3)
   {
      cerr << "usage: dgp2gtif <dgp> <tiff>" << endl;
      exit(1);
   }

   dgpfn = new char[strlen(argv[1])+1];
   strcpy(dgpfn,argv[1]);
   basefn = new char[strlen(dgpfn)+1];
   strcpy(basefn,dgpfn);
   foo = &basefn[strlen(basefn)];
   while (*foo != '.') foo--;
   if (foo!=basefn)
      *foo = (char)0;
   metfn = new char[strlen(basefn)+5];
   strcpy(metfn,basefn);
   strcat(metfn,".met");
   gtiffn = new char[strlen(argv[2])+1];
   strcpy(gtiffn,argv[2]);
   if (!mf.readFromFile(metfn))
   {
      cerr << "Cannot open " << metfn << endl;
      exit(2);
   }
   fin = new DGPImageIFile(dgpfn);
   if (!fin->good())
   {
      cerr << "Cannot open " << dgpfn << endl;
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

   cout << "Converting " << fin->getHeight() << " scanlines to GeoTIFF..." << endl;
   status = 0;
   for (i=0;i<fin->getHeight();i++)
   {
       if ((i % 128) == 0)
       {
          cout << updt[status] << " " << i << "\r" << flush;
          status++;
          status = status % 4;
       }
       line = (unsigned char *) fin->getRawScanline(i);
       fout->putRawScanline(line,i);
       delete[] line;
   }

   cout << "Done.                     " << endl;
   delete fin;
   delete fout;
   delete[] basefn;
   delete[] dgpfn;
   delete[] gtiffn;
   delete[] metfn;
   return 0;
}
