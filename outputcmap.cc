#include "Image/RGBImage.h"
#include "Image/RGBPallette.h"
#include "Image/RGBPixel.h"
#include "Image/ImageCommon.h"
#include <iostream.h>

void outpal(RGBPallette *pal)
{
   int i;
   bool ret = true;

   for (i=0; i < 13; i++)
   {
     cout << "Index: " << i << ", Red: " << (*pal)[i].getRed();
     cout << " Green: " << (*pal)[i].getGreen();
     cout << " Blue: " << (*pal)[i].getBlue() << endl;
   }
}

int main(int argc,char **argv)
{
   RGBImage *fin;
   RGBPallette *pal;
   int i;

   if (argc < 2)
   {
      cerr << "usage: " << argv[0] << " file ..." << endl;
      exit(42);
   }

   for (i=1;i<argc;i++)
   {
      fin = new RGBImage(argv[i]);
      pal = (RGBPallette *) fin->getPallette();
      outpal(pal);
      delete pal;
      delete fin;
   }
}
