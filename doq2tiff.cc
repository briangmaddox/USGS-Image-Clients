#include "Image/DOQImageIFile.h"
#include "Image/TIFFImageOFile.h"
#include "Image/RGBPallette.h"
#include "Image/RGBPixel.h"
#include "FrodoLib/SpinnerThingy.h"
#include <iostream.h>

int main(int argc, char **argv)
{
   SpinnerThingy thingy;
   DOQImageIFile *fin;
   TIFFImageOFile *fout;
   int i;
   RGBPallette *pal;
   RGBPixel *colors = new RGBPixel[256];
   unsigned char *buf;

   if (argc != 3)
   {
      cout << "usage: " << argv[0] << " <grey-file> <tiff>" << endl;
      exit(69);
   }
   fin = new DOQImageIFile(argv[1]);
   if (!fin->good())
   {
      cout << "Can't open " << argv[1] << endl;
      exit(69);
   } 
   fout = new TIFFImageOFile(argv[2],fin->getSamples(),fin->getLines());
   if (!fout->good())
   {
      cout << "Can't open " << argv[2] << endl;
      exit(69);
   } 

   for (i=0;i<256;i++)
   {
       colors[i].setRed(i);
       colors[i].setGreen(i);
       colors[i].setBlue(i);
   }
   pal = new RGBPallette(256,colors);

   fout->setPallette(pal);

   cout << "Converting " << fin->getLines() << " scanlines." << endl;

   for (i=0;i<fin->getLines();i++)
   {
       if ((i%20)==0)
          thingy.update(i);
       buf = (unsigned char *) fin->getRawScanline(i);
       fout->putRawScanline(buf,i);
       delete[] buf;
   }
 
   thingy.done("Done.");
   delete fin;
   delete fout;
   delete pal;
   delete[] colors;
} 
