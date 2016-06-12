#include "Image/TIFFImageIFile.h"
#include "Image/RGBPallette.h"
#include "Image/ImageDebug.h"


void main(int argc, char** argv)
{
   TIFFImageIFile *fin;
   RGBPallette *pal;

   int i,j;
   int w,h;
   double foo[3];
   bool color_used[255];
   unsigned char *line;

   if (argc < 2)
   {
      cerr << "usage: tiffmap <file>" << endl;
      exit(69);
   }

   
   for (i=0;i<255;i++)
       color_used[i] = false;

   fin = new TIFFImageIFile(argv[1]);
   if (!fin->good())
   {
      cerr << "Can't open " << argv[1] << endl;
      exit(69);
   }

   
   pal = (RGBPallette *) fin->getPallette();

   h = fin->getHeight();
   w = fin->getWidth();


   for (j=0;j<h;j++)
   {
      line = (unsigned char *) fin->getRawScanline(j);
      for (i=0;i<w;i++)
          color_used[line[i]] = true;
      delete[] line;
   }
   
   cout << "13" << endl;
   cout << "0 0 0" << endl;
   cout << "255 255 255" << endl;
   cout << "0 151 164" << endl;
   cout << "203 0 23" << endl;
   cout << "131 66 37" << endl;
   cout << "201 234 157" << endl;
   cout << "137 51 128" << endl;
   cout << "255 234 0" << endl;
   cout << "167 226 226" << endl;
   cout << "255 184 184" << endl;
   cout << "218 179 214" << endl;
   cout << "209 209 209" << endl;
   cout << "207 164 142" << endl;
   for (i=0;i<255;i++)
   {
      if (color_used[i])
      {
          cout << i << endl;
      }
   }

   delete pal;
   delete fin;
}


