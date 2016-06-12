#include <iostream.h>
#include "Image/DGPImageIFile.h"
#include "Image/TIFFImageOFile.h"
#include "Image/CorePalletteImageData.h"
#include "Image/RGBPallette.h"
#include <string.h>
#include <stdlib.h>



void main(int argc, char** argv)
{
    DGPImageIFile* fin;
    CorePalletteImageData* fmem;
    TIFFImageOFile* fout;
    RGBPallette* pal;

    unsigned char *buf;
    unsigned char *buf2;
    char* finname;
    char* foutname;
    long i,j;


    if (argc < 3)
    {
       cerr << "usage: dgpresample <dgpfile> <tifffile>" << endl;
       exit(69);
    }

    finname = new char[strlen(argv[1])+1];
    foutname = new char[strlen(argv[2])+1];
    strcpy(finname,argv[1]);
    strcpy(foutname,argv[2]);
    fin = new DGPImageIFile(finname);
    if (!fin->good())
    {
       cerr << "Cannot open " << finname << endl;
       exit(69);
    }
    fmem = new CorePalletteImageData(fin->getWidth(),fin->getHeight(),DRG_RGB);
    pal = (RGBPallette *) fin->getPallette();
    fmem->setPallette(pal);
    delete pal;
    for (i=0;i<fin->getHeight();i++)
    {
        buf = (unsigned char *) fin->getRawScanline(i);
        fmem->putRawScanline(buf,i);
        delete[] buf;
    }
    cout << "Got it into memory..." << endl;
    delete fin;

    fout = new TIFFImageOFile(foutname,fmem->getWidth()/2,
                             fmem->getHeight()/2);
    if (!fout->good())
    {
       cerr << "Cannot open " << foutname << endl;
       exit(69);
    }
    pal = (RGBPallette *) fmem->getPallette();
    fout->setPallette(pal);
    delete pal;

    buf2 = new unsigned char[fout->getWidth()];

    for (j=0;j<fout->getHeight();j++)
    {
        buf = (unsigned char*)fmem->getRawScanline(2*j);
        for (i=0;i<fout->getWidth();i++)
            buf2[i] = buf[2*i]; 
        fout->putRawScanline(buf2,j);
        delete[] buf;
    }

    delete[] buf2;
    delete fmem;
    delete fout;
}
