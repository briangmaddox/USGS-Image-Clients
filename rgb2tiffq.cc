#include "Image/RGBImageIFile.h"
#include "Image/Octree.h"
#include "Image/TIFFImageOFile.h"
#include "Image/RGBPallette.h"
#include "FrodoLib/SpinnerThingy.h"
#include <iostream.h>


void main(int argc, char **argv)
{
   SpinnerThingy  *thingy;
   RGBImageIFile  *fin;
   TIFFImageOFile *fout;
   Octree         *theTree;
   RGBPallette    *pal;
   unsigned char  *rbuf;
   unsigned char  *tbuf;
   int             i,j;
   int             desired = 256;


   if (argc == 4)
   {
      sscanf(argv[3],"%d",&desired);
      if ((desired < 8)||(desired > 256))
      {
         cerr << "Number of colors must be between 8 and 256." << endl;
         exit(42);
      }
   }
   else if (argc != 3)
   {
      cerr << "usage: " << argv[0] << " <rgbfile> <tiff> [colors]" << endl;
      exit(42);
   }

   fin  = new RGBImageIFile(argv[1]);
   if (!fin->good())
   {
      cerr << "Can't open " << argv[1] << endl;
      exit(42);
   }
   fout = new TIFFImageOFile(argv[2],fin->getWidth(),fin->getHeight());
   if (!fout->good())
   {
      cerr << "Can't open " << argv[2] << endl;
      exit(42);
   }

   thingy = new SpinnerThingy;
   cout << "Building Octree for quantization..." << endl;
   theTree = new Octree(desired);

   for (j=0;j<fin->getHeight();j++)
   {
       if ((j % 8) == 0)
          thingy->update(j);
       rbuf = (unsigned char *) fin->getRawScanline(j);
       for (i=0;i<fin->getWidth();i++)
           theTree->addColor(rbuf[3*i],rbuf[3*i+1],rbuf[3*i+2]);
       delete[] rbuf;
   }
   thingy->done("Done.");
   delete thingy;
 
   theTree->assignIndexes();
   pal = theTree->getQuantPallette();
   fout->setPallette(pal);
   thingy = new SpinnerThingy;
   cout << "Writing quantized TIFF..." << endl;
   tbuf = new unsigned char[fout->getWidth()];
   for (j=0;j<fin->getHeight();j++)
   {
       if ((j % 8) == 0)
          thingy->update(j);
       rbuf = (unsigned char *) fin->getRawScanline(j);
       for (i=0;i<fin->getWidth();i++)
       {
           tbuf[i] =theTree->getIndex(rbuf[3*i],rbuf[3*i+1],rbuf[3*i+2]);
       }
       fout->putRawScanline(tbuf,j);
       delete[] rbuf;
   }
   thingy->done("Done.");
   delete thingy;

   delete fout;
   delete theTree;
   delete[] tbuf;
   delete fin;
   delete pal;
}

 
