// newtiffresample
// Brian Maddox - USGS MCMC - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-19 19:37:06 $
// Version: $Revision: 1.1.1.1 $

#include <iostream.h>
#include "Image/TIFFImageIFile.h"
#include "Image/TIFFImageOFile.h"
#include "Image/RGBPallette.h"
#include "FrodoLib/SpinnerThingy.h"
#include <string.h>
#include <stdlib.h>
#include <getopt.h>


void usage(char* myname)
{
  cerr << "Usage: " << myname << " [options] original-tiff new-tiff" << endl;
  cerr << "where options are: " << endl;
  cerr << "   -V   version information" << endl;
  cerr << "   -p   run Prioritized Nearest Neighbor Algorithm (default)" 
       << endl;
  cerr << "   -n   run normal Nearest Neighbor Algorithm" << endl;
  cerr << "   -?   print this information" << endl;
}


// This is the second Prioritized Nearest Neighbor Algorithm.  Am trying to
// enhance it and make it a little faster
int whichColor(unsigned char insquare[2][2], bool& PNNflag)
{
  int colorchart[13]; // used to hold number of colors count here
  int tempcount = 0; // temp counter
  int highestcount = 0; // highest count found
  int highestindex = 0; // index of the highest count
  bool equalflag = false; // set to true if we find something of equal value

  for (int mycount = 0; mycount < 13; mycount++)
    colorchart[mycount] = 0; // initialize it

  // Loop through the 4 pixels.  Increment the colorchart counter for the
  // pallette index found
  for (int colcount = 0; colcount < 2; colcount++)
    for (int rowcount = 0; rowcount < 2; rowcount++)
      colorchart[insquare[colcount][rowcount]]++;

  // Since they have the highest priority, check if there are any black or
  // brown pixels in the square first
  if ( (colorchart[0] > 0) && (PNNflag == true) )
    return 0;
  else if ( (colorchart[4] > 0) && (PNNflag == true) )
    return 4;
  else // switch to nearest neighbor routine since neither found
  {
    // Skip the 1st index since that's black, easier to just check brown
    // again than to loop around it
    for (int mycount = 0; mycount < 13; mycount++)
    {
      tempcount = colorchart[mycount];
      if (tempcount > highestcount) // more pixels here than the others
      {
	equalflag = false; // Since we have a highest
	highestcount = tempcount;
	highestindex = mycount;
      }
      else if (tempcount == highestcount)
      {
	equalflag = true; // don't have one highest anymore
      }
    }
  }

  // Now, check and see if we have a true highest color
  if (!equalflag)
    return highestindex;
  else // go through priority on the remaining indices
  {
    if (colorchart[10] > 0)
      return 10;
    else if (colorchart[6] > 0)
      return 6;
    else if (colorchart[11] > 0)
      return 11;
    else if (colorchart[0] > 0)
      return 0;
    else if (colorchart[8] > 0)
      return 8;
    else if (colorchart[2] > 0)
      return 2;
    else if (colorchart[9] > 0)
      return 9;
    else if (colorchart[3] > 0)
      return 3;
    else if (colorchart[12] > 0)
      return 12;
    else if (colorchart[4] > 0)
      return 4;
    else if (colorchart[5] > 0)
      return 5;
    else
      return 1;
  }

}

void main(int argc, char** argv)
{
  if (argc == 1) // called with no arguments
  {
    usage(argv[0]);
    exit(0);
  }

  SpinnerThingy thingy;
  TIFFImageIFile* intiff;
  TIFFImageOFile* outtiff;
  RGBPallette* pal;
  
  bool PNNflag = true; // true if use Prioritized Nearest Neighbor

  int myopt; // for getopt

  float inputdpi, outputdpi;

  unsigned char* buf;
  unsigned char* buf2;
  unsigned char* buf3; // added to hold the second scanline

  unsigned char insquare[2][2]; // for the counting routine

  long i,j;
  
  while ( (myopt = getopt(argc, argv, "Vpn?")) != -1)
    switch (myopt)
    {
    case 'V':
    {
      cout << argv[0] << " $Revision: 1.1.1.1 $" << endl;
      exit(0);
      break;
    }
    case 'p':
    {
      PNNflag = true;
      break;
    }
    case 'n':
    {
      PNNflag = false;
      break;
    }
    case '?':
    {
      usage(argv[0]);
      exit(0);
      break;
    }
    default:
    {
      usage(argv[0]);
      exit(0);
      break;
    }
    }

  if ( (optind >= argc) || ( (optind + 2) > argc) )
  {
    usage(argv[0]);
    exit(0);
  }

  if (PNNflag)
    cout << "Using Priortized Nearest Neighbor Algorithm" << endl;
  else
    cout << "Using Normal Neasest Neighbor Algorithm" << endl;

  intiff = new TIFFImageIFile(argv[optind++]);
  if (!intiff->good())
  {
    cerr << "Cannot open " << argv[optind - 1] << endl;
    exit(69);
  }
  outtiff = new TIFFImageOFile(argv[optind], intiff->getWidth()/2,
			       intiff->getHeight()/2);
  if (!outtiff->good())
  {
    cerr << "Cannot open " << argv[optind] << endl;
    delete intiff;
    delete outtiff;
    exit(69);
  }
 
  intiff->getTag(TIFFTAG_XRESOLUTION, &inputdpi);
  if ( (inputdpi != 250.0) && (inputdpi != 72.0) ) // my hack for now
  {
    //    cout << "New DPI is " << (inputdpi / 2.0) << endl;
    outtiff->setTag(TIFFTAG_XRESOLUTION, (inputdpi / 2));
    outtiff->setTag(TIFFTAG_YRESOLUTION, (inputdpi / 2));
    outputdpi = inputdpi / 2;
  }
  else
    outputdpi = inputdpi;

  pal = (RGBPallette *) intiff->getPallette();
  outtiff->setPallette(pal);
  delete pal;
  
  buf2 = new unsigned char[outtiff->getWidth()];
  cout << "Downsampling TIFF to " << outtiff->getWidth() << " columns by "
       << outtiff->getHeight() << " lines at " << outputdpi << "DPI...." 
       << endl;

  for (j = 0; j < outtiff->getHeight(); j++)
  {
    if ((j % 128) == 0)
    {
      thingy.update(j);
    }

    buf = (unsigned char*)intiff->getRawScanline(2*j);
    buf3 = (unsigned char*)intiff->getRawScanline((2 * j) + 1);
    for (i=0; i < outtiff->getWidth(); i++)  // Altered this to try to use
                                             // priority
    {    
      insquare[0][0] = buf[2 * i]; insquare[0][1] = buf[(2 * i) + 1];
      insquare[1][0] = buf3[2 * i]; insquare[1][1] = buf3[(2 * i) + 1];
      buf2[i] = whichColor(insquare, PNNflag);
    }

    outtiff->putRawScanline(buf2, j);
    delete[] buf;
    delete[] buf3;
  }
  thingy.done("Done.");

  delete[] buf2;
  delete intiff;
  delete outtiff;
}
