#include <iostream.h>
#include "Image/TIFFImageIFile.h"
#include "Image/RGBImageOFile.h"
#include "Image/RGBPixel.h"
#include <string.h>
#include <stdlib.h>



void main(int argc, char** argv)
{
    TIFFImageIFile* fin;
    RGBImageOFile* fout;

    RGBPixel* pixbuff;
    char* finname;
    char* foutname;
    char* foutnamehdr;
    long i;


    if (argc < 3)
    {
       cerr << "usage: tiff2rgb <tifffile> <rgbfile>" << endl;
       exit(69);
    }

    finname = new char[strlen(argv[1])+1];
    foutname = new char[strlen(argv[2])+1];
    foutnamehdr = new char[strlen(argv[2])+5];
    strcpy(finname,argv[1]);
    strcpy(foutname,argv[2]);
    strcpy(foutnamehdr,argv[2]);
    strcat(foutnamehdr,".hdr");
    fin = new TIFFImageIFile(finname);
    if (!fin->good())
    {
       cerr << "Cannot open " << finname << endl;
       exit(69);
    }
    fout = new RGBImageOFile(foutname,foutnamehdr,fin->getWidth(),
                             fin->getHeight());
    if (!fout->good())
    {
       cerr << "Cannot open " << foutname << endl;
       exit(69);
    }

    for (i=0;i<fin->getHeight();i++)
    {
        pixbuff = (RGBPixel*)fin->getScanline(i);
        fout->putScanline(pixbuff,i);
        delete[] pixbuff;
    }

    delete fin;
    delete fout;
}
