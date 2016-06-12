#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include "Image/RGBImageIFile.h"
#include "Image/TIFFImageOFile.h"
#include "Image/RGBPixel.h"
#include "Image/RGBPallette.h"



void main(int argc, char** argv)
{
    RGBImageIFile* fin;
    TIFFImageOFile* fout;
    RGBPixel* line;
    long i;
    char* fn1;
    char* fn2;

    if (argc != 3)
    {
       cerr << "usage: rgb2tiff <rgb> <tiff>" << endl;
       cerr << "rgbheader must look like: " << endl << endl;
       cerr << "Width: 123" << endl;
       cerr << "Height: 123" << endl;
       cerr << endl << "rgb2tiff uses PackBits compression." << endl;
       exit(69);
    }

    cout << "Using PackBits compression..." << endl;

    fn1 = new char[strlen(argv[1])+1];
    fn2 = new char[strlen(argv[2])+1];
    strcpy(fn1,argv[1]);
    strcpy(fn2,argv[2]);

    fin = new RGBImageIFile(fn1);
    if (!fin->good())
    {
       cerr << "Cannot open " << fn1 << endl;
       exit(69);
    }
    fout = new TIFFImageOFile(fn2,fin->getWidth(),fin->getHeight());
    if (!fout->good())
    {
       cerr << "Cannot open " << fn2 << endl;
       exit(69);
    }

    delete fn1;
    delete fn2;


    for (i=0;i<fin->getHeight();i++)
    {
        line = (RGBPixel*) fin->getScanline(i);
        fout->putScanline(line,i);
        delete line;
    }
    delete fin;
    delete fout;
}
    
