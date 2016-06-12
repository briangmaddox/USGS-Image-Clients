#include <iostream.h>
#include "Image/TIFFImageIFile.h"
#include "Image/TIFFImageOFile.h"
#include "Image/RGBPallette.h"
#include "FrodoLib/SpinnerThingy.h"
#include <string.h>
#include <stdlib.h>



void main(int argc, char** argv)
{
  SpinnerThingy thingy;
  TIFFImageIFile* fin;
  TIFFImageOFile* fout;
  RGBPallette* pal;
  

  float inputdpi;

  unsigned char *buf;
  unsigned char *buf2;
  char* finname;
  char* foutname;
  long i,j;
  
  
  if (argc < 3)
  {
    cerr << "usage: tiffresample <tifffile> <tifffile>" << endl;
    exit(69);
  }
  
  finname = new char[strlen(argv[1])+1];
  foutname = new char[strlen(argv[2])+1];
  strcpy(finname,argv[1]);
  strcpy(foutname,argv[2]);
  fin = new TIFFImageIFile(finname);
  if (!fin->good())
  {
    cerr << "Cannot open " << finname << endl;
    exit(69);
  }
  fout = new TIFFImageOFile(foutname,fin->getWidth()/2,
			    fin->getHeight()/2);
  if (!fout->good())
  {
    cerr << "Cannot open " << foutname << endl;
    exit(69);
  }
 
  fin->getTag(TIFFTAG_XRESOLUTION, &inputdpi);
  if ( (inputdpi != 250.0) && (inputdpi != 72.0) ) // my hack for now
  {
    cout << "New DPI is " << (inputdpi / 2.0) << endl;
    fout->setTag(TIFFTAG_XRESOLUTION, (inputdpi / 2));
    fout->setTag(TIFFTAG_YRESOLUTION, (inputdpi / 2));
  }
  pal = (RGBPallette *) fin->getPallette();
  fout->setPallette(pal);
  delete pal;
  
  buf2 = new unsigned char[fout->getWidth()];
  
  cout << "Downsampling TIFF to " << fout->getWidth() << " samples by "
       << fout->getHeight() << " lines... " << endl;

  for (j = 0; j < fout->getHeight(); j++)
  {
    if ((j % 128) == 0)
    {
      thingy.update(j);
    }

    buf = (unsigned char*)fin->getRawScanline(2*j);
    for (i=0;i<fout->getWidth();i++)
      buf2[i] = buf[2*i]; 
    fout->putRawScanline(buf2,j);
    delete[] buf;
  }

  thingy.done("Done.");

  delete[] buf2;
  delete fin;
  delete fout;
}
