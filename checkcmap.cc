#include "Image/RGBImage.h"
#include "Image/RGBPallette.h"
#include "Image/RGBPixel.h"
#include "Image/ImageCommon.h"
#include <iostream.h>

RawRGBPixel colors[13] = { {   0,  0,  0 },
                           { 255,255,255 },
                           { 0,151,164 },
                           {  203,  0,23 },
                           {  131, 66,37 },
                           { 201,234,157 },
                           { 137, 51,128 },
                           {   255,234,0 },
                           { 167,226,226 },
                           { 255,184,184 },
                           { 218,179,214 },
                           { 209,209,209 },
                           { 207,164,142 }
};

bool checkpal(RGBPallette *pal)
{
   int i;
   bool ret = true;

//   if (pal->getNoEntries()!=13)
//      return false;

   for (i=0;i<13;i++)
     if (
	 ((*pal)[i].getRed()   != colors[i].Red  ) ||
	 ((*pal)[i].getGreen() != colors[i].Green) ||
	 ((*pal)[i].getBlue()  != colors[i].Blue )
	 )
       {
          cout << "Cmap is wrong on " << i << "th member.. ";

	  cout << "Red: " << (*pal)[i].getRed();
	  cout << " Green: " << (*pal)[i].getGreen();
	  cout << " Blue: " << (*pal)[i].getBlue() << endl;
          ret = false;
       }

   return ret;
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
      if (!checkpal(pal))
         cout << argv[i] << ": Colormap is wrong." << endl;
      delete pal;
      delete fin;
   }
}
