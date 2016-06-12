#include "Image/TIFFImageIFile.h"
#include "Image/TIFFImageOFile.h"
#include "Image/RGBPallette.h"
#include "Image/RGBPixel.h"
#include <iostream.h>


void main(int argc, char **argv)
{
   TIFFImageIFile *fin;
   TIFFImageOFile *fout;
   RGBPallette* pal;
   RGBPixel *pix;
   int i,h;
   unsigned char *line;


   if (argc!=3)
   {
      cerr << "usage: " << argv[0] << " <tiffin> <tiffout>" << endl;
      exit(69);
   }
    
   fin = new TIFFImageIFile(argv[1]);
   fout = new TIFFImageOFile(argv[2],fin->getWidth(),fin->getHeight(),2);
  
   pix = new RGBPixel[13];

   pix[0].setRed(0);
   pix[0].setGreen(0);
   pix[0].setBlue(0);


   pix[1].setRed(255);
   pix[1].setGreen(255);
   pix[1].setBlue(255);

   pix[2].setRed(0);
   pix[2].setGreen(151);
   pix[2].setBlue(164);

   pix[3].setRed(203);
   pix[3].setGreen(0);
   pix[3].setBlue(23);

   pix[4].setRed(131);
   pix[4].setGreen(66);
   pix[4].setBlue(37);

   pix[5].setRed(201);
   pix[5].setGreen(234);
   pix[5].setBlue(157);

   pix[6].setRed(137);
   pix[6].setGreen(51);
   pix[6].setBlue(128);

   pix[7].setRed(255);
   pix[7].setGreen(234);
   pix[7].setBlue(0);

   pix[8].setRed(167);
   pix[8].setGreen(226);
   pix[8].setBlue(226);

   pix[9].setRed(255);
   pix[9].setGreen(184);
   pix[9].setBlue(184);

   pix[10].setRed(218);
   pix[10].setGreen(179);
   pix[10].setBlue(214);

   pix[11].setRed(209);
   pix[11].setGreen(209);
   pix[11].setBlue(209);

   pal = new RGBPallette(12,pix);

   fout->setPallette(pal);

   h = fin->getHeight();

   for (i=0;i<h;i++)
   {
       line = (unsigned char *) fin->getRawScanline(i);
       fout->putRawScanline(line,i);
       delete[] line;
   }

   delete pal;
   delete[] pix;
   delete fin;
   delete fout;
}
