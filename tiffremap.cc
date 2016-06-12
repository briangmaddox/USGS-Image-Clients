#include "Image/TIFFImageIFile.h"
#include "Image/TIFFImageOFile.h"
#include "Image/RGBPixel.h"
#include "Image/RGBPallette.h"
#include <fstream.h>
#include <iostream.h>


unsigned char newmap[256];
RGBPallette* newpallette;
unsigned short num_colors;



void readSetup(const char* fn)
{
   int i; 
   int n;
   unsigned short r, g, b;
   unsigned short map1, map2;
   RGBPixel* foo = new RGBPixel;
   ifstream* fin = new ifstream(fn,ios::in|ios::nocreate);

   *fin >> n;

   newpallette = new RGBPallette(n);

   for (i=0;i<n;i++)
   {
      *fin >> r >> g >> b; 
      foo->setRed(r); 
      foo->setGreen(g); 
      foo->setBlue(b); 
      newpallette->setEntry(i,foo);
   } 
   
   num_colors = 0; 
   while (!fin->eof())
   {
      *fin >> map1 >> map2;
      newmap[map1] = map2;
      num_colors++;
   }
   num_colors--;
   delete fin;
}


void main(int argc, char **argv)
{
   TIFFImageIFile* fin;
   TIFFImageOFile* fout;
   unsigned char* line;
   char *Software = new char[100];
   long height,width;
   long row,column;
   int foo;
   char updt[4] = { '|','/','-','\\' };
   float inputdpi;

   if (argc < 4)
   {
      cerr << "Usage: " << argv[0] << " <oldfile> <newfile> <mapfile>" << endl;
      exit(69);
   }

   fin = new TIFFImageIFile(argv[1]);
   if (!fin->good())
   {
      cerr << "Cannot open " << argv[1] << endl;
      exit(69);
   }
   fout = new TIFFImageOFile(argv[2],fin->getWidth(),fin->getHeight());
   if (!fout->good())
   {
      cerr << "Cannot open " << argv[2] << endl;
      exit(69);
   }
   height = fin->getHeight();
   width = fin->getWidth();
   
  fin->getTag(TIFFTAG_XRESOLUTION, &inputdpi);
  if ( (inputdpi != 250.0) && (inputdpi != 72.0) ) // my hack for now
  {
    cout << "Output DPI is: " << inputdpi << endl;
    fout->setTag(TIFFTAG_XRESOLUTION, inputdpi);
    fout->setTag(TIFFTAG_YRESOLUTION, inputdpi);
  }
    
   readSetup(argv[3]);

   fin->getTag(TIFFTAG_SOFTWARE,&Software);
   fout->setTag(TIFFTAG_SOFTWARE,Software);

   fout->setPallette(newpallette);
   cout << "Remapping " << height << " scanlines." << endl;
   foo = 0;
   for (row=0;row<height;row++)
   {
      if ((row%128) == 0)
      {
         cout << updt[foo] << " " << row << "\r" << flush;
         foo++;
         foo = foo % 4;
      }
      else if ((row%32) == 0)
      {
         cout << updt[foo] << "\r" << flush;
         foo++;
         foo = foo % 4;
      }
      line = (unsigned char *) fin->getRawScanline(row);
      for (column=0;column<width;column++)
          line[column] = newmap[line[column]];
      fout->putRawScanline(line,row);
      delete[] line;
   }
   cout << "Done.           " << endl;
   delete fin;
   delete fout;
   delete newpallette;
   delete Software;
}
