#include "Image/TIFFImageIFile.h"
#include "Image/TIFFImageOFile.h"
#include "Image/RGBPallette.h"
#include <unistd.h>
#include <iostream.h>
#include <fstream.h>
#include <string>

int main(int argc, char** argv)
{

  string* argument;
  if (argc == 2)
  {
    argument = new string(argv[1]);
    if (*argument == "-V")
    {
      cout << argv[0] << ": $Revision: 1.1.1.1 $" << endl;
      delete argument;
      exit(0);
    }
    else
      delete argument;
  }

   
  TIFFImageIFile *fin;
  TIFFImageOFile *fout;

  int k;
  int i,j;
  int w,h;
  unsigned char *line;
  char *tempfn = "CFtmp.temp";
  bool indexes[256];
  RGBPallette* pal;
  bool atLeastOneBad;
  ifstream istr;

  if (argc < 2)
  {
    cerr << "usage: checktiff <file> ..." << endl;
    exit(69);
  }

  atLeastOneBad = false;
   
  for (k=1;k<argc;k++)
  {
    /*    istr.open(argv[k]);
    if (!istr)
    {
      cout << "Could not open " << argv[k] << endl;
      istr.close();
      continue;
    }
    istr.close();
    */
    for (i=0;i<256;i++)
      indexes[i] = true;
    fin = new TIFFImageIFile(argv[k]);
    pal = (RGBPallette *) fin->getPallette();
    if (!fin->good())
    {
      cout << "Can't open " << argv[k] << endl;
      exit(69);
    }
   
    h = fin->getHeight();
    w = fin->getWidth();

    fout = new TIFFImageOFile(tempfn,w,h);
    fout->setPallette(pal);
    for (j=0;j<h;j++)
    {
      line = (unsigned char *) fin->getRawScanline(j);
      for (i=0;i<w;i++)
      {
	if (line[i]>12)
	{
	  if (indexes[line[i]])
	  {
	    atLeastOneBad = true;
	    cout << argv[k] << ": found bad index - ";
	    cout << (int) line[i] << endl;
	    indexes[line[i]] = false;
	  }
	  line[i] = (char) 0;
	} 
      }
      fout->putRawScanline(line,j);
      delete[] line;
    }
    delete fout;
    delete fin;
    delete pal;
    unlink(argv[k]);
    link(tempfn,argv[k]);
    unlink(tempfn);
  }
  if (atLeastOneBad)
    return 1;
  else
    return 0;
}   
