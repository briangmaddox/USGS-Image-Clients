#include <iostream.h>
#include "Image/TIFFImageIFile.h"
#include "Image/TIFFImageOFile.h"
#include "Image/RGBPixel.h"
#include <string.h>
#include <stdlib.h>



void main(int argc, char** argv)
{
    TIFFImageIFile* fin;
    TIFFImageOFile* fout;

    RGBPixel* pixbuff;
    char* finname;
    char* foutname;
    long i;


    if (argc < 3)
    {
       cerr << "usage: pb2lzw <file1> <file2>" << endl;
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
    fout = new TIFFImageOFile(foutname,fin->getWidth(), fin->getHeight(),2);
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
