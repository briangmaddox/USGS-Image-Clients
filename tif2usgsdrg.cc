#include "Image/TIFFImageIFile.h"
#include "drg/MetFile.h"
#include "Image/GeoTIFFImageOFile.h"
#include "Image/RGBPallette.h"
#include "FrodoLib/SpinnerThingy.h"
#include <iostream.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <string>

#define SOFTWARE "USGS CD Archiver program tif2usgsdrg v.1.0"

int main(int argc, char **argv)
{
   SpinnerThingy thingy;
   char* metfn;
   char* drgfn;
   char* tiffn;
   char* foo;
   char* basefn;
   MetFile mf;
   TIFFImageIFile* fin;
   GeoTIFFImageOFile* fout;
   RGBPallette* pal;
   int i;
   unsigned char* line;
   char desc_string[100];
   char time_string[20];
   time_t cur_time;
   struct tm timv;
   int scale;
   double dscale;
   int prid;
   string* argument;

   if (argc == 2)
   {
     argument = new string(argv[1]);
     if (*argument == "-V")
     {
       cout << argv[0] << ": $Revision: 1.1.1.1 $" << endl;
       delete argument;
       exit(0);
     }
     delete argument;
   }

   if (argc!=3)
   {
      cerr << "usage: tif2usgsdrg <tif> <drg-file>" << endl;
      exit(1);
   }

// Figure out all the filename stuff

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
   drgfn = new char[strlen(argv[2])+1];
   strcpy(drgfn,argv[2]);

// Open the relevant files, etc...

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
   fout = new GeoTIFFImageOFile(drgfn,fin->getWidth(),fin->getHeight());
   if (!fout->good())
   {
      cerr << "Cannot open " << drgfn << endl;
      exit(4);
   }

// Set some crap in the output GeoTIFF
   // This checks to see if the NAD type in the metadata file is supported.
   // If not, it alerts the user that it will default to NAD27.  Note that
   // the defaulting is actually in GeoTIFFImageOFile::setUSGS_UTM and
   // any supported added for additional NAD types there should be reflected
   // in the following if statement.  BGM 26 Nov 1996
   int tDatYear = mf.getdat_year();
   if ( (tDatYear != 1927) && (tDatYear != 1983) )
   {
     cout << argv[1] << " has an unsupported NAD type of " << tDatYear;
     cout << ".  Defaulting to NAD27." << endl;
   }
   pal = (RGBPallette *) fin->getPallette();
   fout->setPallette(pal);
   delete pal;
   cur_time = time(0L);
   timv = *localtime(&cur_time);
   sprintf(time_string,"%4d:%02d:%02d %02d:%02d:%02d",
           1900+(timv.tm_year), 1+(timv.tm_mon), (timv.tm_mday),
                (timv.tm_hour),   (timv.tm_min), (timv.tm_sec));
   
   dscale = mf.getMapScale();
   scale = (int) dscale;
   prid = mf.getProductID();
   if (prid == 0)
      sprintf(desc_string,"USGS GeoTIFF DRG 1:%d Quad of %s.",
              scale,mf.getCellName());
   else
      sprintf(desc_string,"USGS GeoTIFF DRG 1:%d Quad of %s. Product:%d",
              scale,mf.getCellName(),prid);

   fout->setTag(TIFFTAG_DATETIME,time_string);
   fout->setTag(TIFFTAG_SOFTWARE,SOFTWARE);
   fout->setTag(TIFFTAG_IMAGEDESCRIPTION,desc_string);
   fout->setUSGS_UTM(mf.getULX(),mf.getULY(),
                     mf.getXPixRes(),mf.getYPixRes(),mf.getUTMZone(),
		     mf.getdat_year());

   cout << "Converting " << fin->getHeight();
   cout << " scanlines to GeoTIFF..." << endl;
   for (i=0;i<fin->getHeight();i++)
   {
       if ((i % 128) == 0)
       {
          thingy.update(i);
       }
       line = (unsigned char *) fin->getRawScanline(i);
       fout->putRawScanline(line,i);
       delete[] line;
   }

   thingy.done("Done.");
   delete fin;
   delete fout;
   delete[] basefn;
   delete[] tiffn;
   delete[] drgfn;
   delete[] metfn;
   return 0;
}
