#include <iostream.h>
#include "Image/CRLImageIFile.h"
#include "Image/RGBPallette.h"
#include <string.h>



void usage(void)
{
    cerr << "usage: crlinfo [-c] <filename>" << endl;
    exit(69);
}


void main(int argc, char** argv)
{
   CRLImageIFile* fin;
   RGBPallette* pal;
   char* fn;
   int color_map=0;
   long i;

    if (argc < 2)
       usage();

    if (argc == 2)
    {
       if (!strcmp(argv[1],"-c"))
       {
          cerr << "You must specify a filename." << endl;
          usage();
       }
       else if (*(argv[1]) == '-')
       {
          cerr << "Unknown option: " << argv[1] << endl;
          usage();
       }
       else
       {
          fn = new char[strlen(argv[1])+1];
          strcpy(fn,argv[1]);
       }
    }

    if (argc == 3)
    {
       if (!strcmp(argv[1],"-c"))
          color_map = 1;
       else
       {
          cerr << "Unknown option: " << argv[1] << endl;
          usage();
       }
       fn = new char[strlen(argv[2])+1];
       strcpy(fn,argv[2]);
    }
   

   fin = new CRLImageIFile(fn);
   if (!fin->good())
   {
      cerr << "Cannot open " << fn << endl;
      exit(69);
   }
   pal = (RGBPallette*)fin->getPallette();

   cout << "Info on CRL file:" << endl << endl;
   cout << "Width: " << fin->getWidth();
   cout << "   Height: " << fin->getHeight() << endl;
   cout << "Width in tiles: " << fin->getTilesWide() << endl;
   cout << "Height in tiles: " << fin->getTilesHigh() << endl;
   cout << "Maximum tile width in pixels: ";
   cout << fin->getMaxTileWidth() << endl;
   cout << "Maximum tile height in pixels: ";
   cout << fin->getMaxTileHeight() << endl;
   cout << "Width of lower right tile: ";
   cout << fin->getTileWidth(fin->getNumTiles()) << endl;
   cout << "Height of lower right tile: ";
   cout << fin->getTileHeight(fin->getNumTiles()) << endl;
   cout << "Number of colors: " << pal->getNoEntries() << endl;
   cout << endl;

   if (color_map)
   {
      cout << endl << "Color Map:" << endl;

      for (i=0;i<pal->getNoEntries();i++)
      {
          cout << i << " ";
          cout << (int) (*pal)[i].getRed() << " ";
          cout << (int) (*pal)[i].getGreen() << " ";
          cout << (int) (*pal)[i].getBlue() << endl;
      }
   }


   delete fin;
   delete pal;
}
