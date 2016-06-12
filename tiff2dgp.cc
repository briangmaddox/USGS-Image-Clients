#include <iostream.h>
#include "Image/TIFFImageIFile.h"
#include "Image/DGPImageOFile.h"
#include "Image/RGBPixel.h"
#include <string.h>
#include <stdlib.h>



void main(int argc, char** argv)
{
    TIFFImageIFile* fin;
    DGPImageOFile* fout;

    unsigned char* pixbuff;
    RGBPallette* pal;
    char* finname;
    char* foutname;
    long i;
    unsigned short bps;
    char updt[4] = {'|','/','-','\\'};
    int bar = 0;


    if (argc < 3)
    {
       cerr << "usage: tiff2dgp <tifffile> <dgpfile>" << endl;
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

    fin->getTag(TIFFTAG_BITSPERSAMPLE,&bps);
    if (bps != 8)
    {
       cerr << finname << " is not an 8 bit TIFF.  Bailing out..." << endl;
       exit(69);
    }
    pal = (RGBPallette *) fin->getPallette();
    fout = new DGPImageOFile(foutname,fin->getWidth(), fin->getHeight(),
                             pal->getNoEntries(),0,0,1.218);
    if (!fout->good())
    {
       cerr << "Cannot open " << foutname << endl;
       exit(69);
    }
    fout->setPallette(pal);

    cout << "Converting " << fin->getHeight() << " scanlines..." << endl;
    for (i=0;i<fin->getHeight();i++)
    {
        if ((i%20)==0)
        {
           cout << updt[bar] << " " << i << "\r" << flush;
           bar++;
           bar = bar % 4;
        }
        pixbuff = (unsigned char*)fin->getRawScanline(i);
        fout->putRawScanline(pixbuff,i);
        delete[] pixbuff;
    }
    cout << "Done.                            " << endl;

    delete fin;
    delete fout;
    delete pal;
}
