#include <stdio.h>
#include <iostream.h>
#include "Image/DGPImageIFile.h"
#include "Image/TIFFImageOFile.h"
#include "Image/RGBPixel.h"
#include "Image/RGBPallette.h"



void main(int argc, char** argv)
{
    DGPImageIFile* fin;
    TIFFImageOFile* fout;
    unsigned char* line;
    RGBPallette* pal;
    long i;
    int foo;
    char updt[4] = { '|','/','-','\\' };

    if (argc != 3)
    {
       cerr << "usage: dgp2tiff <dgp> <tiff>" << endl;
       exit(69);
    }


    fin = new DGPImageIFile(argv[1]);
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
    pal = (RGBPallette *) fin->getPallette();
    fout->setPallette(pal);

    cout << "Using PackBits compression..." << endl;
    cout << "Converting " << fin->getHeight() << " scanlines." << endl;

    foo = 0;

    for (i=0;i<fin->getHeight();i++)
    {
        if ((i % 128) == 0)
        {
           cout << updt[foo] << " " << i << "\r" << flush;
           foo++;
           foo = foo % 4;
        }
        else if ((i % 32) == 0)
        {
           cout << updt[foo] << "\r" << flush;
           foo++;
           foo = foo % 4;
        }
 
        line = (unsigned char*) fin->getRawScanline(i);
        fout->putRawScanline(line,i);
        delete[] line;
    }
    cout << "Done.             " << endl;
    delete pal;
    delete fin;
    delete fout;
}
    
