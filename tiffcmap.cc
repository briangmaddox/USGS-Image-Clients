#include "Image/TIFFImageIFile.h"
#include "Image/RGBPallette.h"
#include <iostream.h>

void main(int argc, char **argv)
{
   TIFFImageIFile *fin;
   RGBPallette *pal;
   int i;

   fin = new TIFFImageIFile(argv[1]);

   pal = (RGBPallette *)fin->getPallette();

   
   for (i=0;i<pal->getNoEntries();i++)
   {
      cout << (*pal)[i].getRed() << " ";
      cout << (*pal)[i].getGreen() << " ";
      cout << (*pal)[i].getBlue() << endl;
   }

   for (i=0;i<pal->getNoEntries();i++)
      cout << i << " " << i << endl;
    
   delete fin;
   delete pal;
}


