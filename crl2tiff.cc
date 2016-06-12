#include <iostream.h>
#include <string.h>
#include "Image/TIFFImageOFile.h"
#include "Image/CRLImageIFile.h"



void main(int argc, char** argv)
{
   
    char* fn1;
    char* fn2;

    CRLImageIFile*  fin;
    TIFFImageOFile* fout;
    unsigned char* line; 
    RGBPallette* pal;
    long i,k;
    int foo;
    char updt[4] = { '|','/','-','\\' };

    if (argc < 3)
    {
        cerr << "usage: crl2tiff <crlfile> <tifffile>" << endl;
        exit(69);
    }
    
    fn1 = new char[strlen(argv[1])+1];
    fn2 = new char[strlen(argv[2])+1];
    strcpy(fn1,argv[1]);
    strcpy(fn2,argv[2]);

    cout << "Setting up...be patient, this could take a minute or two." << flush;
    fin = new CRLImageIFile(fn1);
    if (!fin->good())
    {
       cerr << "Can't open " << fn1 << endl;
       exit(69);
    }
    fout = new TIFFImageOFile(fn2,fin->getWidth(),fin->getHeight());
    if (!fout->good())
    {
       cerr << "\rCan't open " << fn2 << endl;
       exit(69);
    }

    pal = (RGBPallette*)fin->getPallette();
    fout->setPallette(pal); 

    cout << "\rConverting " << fin->getHeight() << " scanlines.                                   " << endl;

    k = 1;
    foo = 0;
    for (i=0;i<fin->getHeight();i++)
    {
        if ((k % 128) == 0) 
        {
           cout << updt[foo] << " " << k << "\r" << flush;
           foo++;
           foo = foo % 4;
        }
        else if ((k % 32) == 0)
        {
           cout << updt[foo] << "\r" << flush;
           foo++;
           foo = foo % 4;
        }
        line = (unsigned char*) fin->getRawScanline(i);
        fout->putRawScanline(line,i);
        delete[] line;
        k++;
    }
    cout << "done..." << endl;

//    if (pal) delete pal;


    delete fin;
    

    delete fout;
 
}

