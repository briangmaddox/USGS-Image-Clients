#include <string.h>
#include <iostream.h>
#include "Image/RGBPallette.h"
#include "Image/DGPImageIFile.h"



void usage(void)
{
    cerr << "usage:  dgpinfo [-c] <filename>" << endl;
    exit(69);
}

void main(int argc, char** argv)
{

    DGPImageIFile* fin;
    RGBPallette* pal;
    char* fn;
    char* foo;
    int color_map=0;
    long i;

    if (argc < 2)
       usage();

    if (argc == 2)
    {
       if (!strcmp(argv[1],"-c"))
       {
          cerr << "You must specify a filename." << endl;
          usage();
       }
       else if (*(argv[1]) == '-')
       {
          cerr << "Unknown option: " << argv[1] << endl;
          usage();
       } 
       else
       {
          fn = new char[strlen(argv[1])+1];
          strcpy(fn,argv[1]);
       }
    } 

    if (argc == 3)
    {
       if (!strcmp(argv[1],"-c"))
          color_map = 1;
       else
       {
          cerr << "Unknown option: " << argv[1] << endl;
          usage();
       }
       fn = new char[strlen(argv[2])+1];
       strcpy(fn,argv[2]);
    }

    fin = new DGPImageIFile(fn);
    if (!fin->good())
    {
       cerr << "Cannot open " << fn << endl;
       exit(69);
    }
    pal = (RGBPallette*)fin->getPallette();

    cout << "Name:       " << (foo=fin->getName()) << endl;
    delete[] foo;
    cout << "Version:    " << (foo=fin->getVersion()) << endl;
    delete[] foo;
    cout << "XUpperLeft: " << fin->getX() << endl;
    cout << "YUpperLeft: " << fin->getY() << endl;
    cout << "Pixel Size: " << fin->getPixelSize() << endl;
    
    cout << "Width:      " << fin->getWidth() << endl;
    cout << "Height:     " << fin->getHeight() << endl;
    cout << "Colors:     " << pal->getNoEntries() << endl;

    if (color_map)
    {
       cout << endl << "Color Map:" << endl;

       for (i=0;i<pal->getNoEntries();i++)
       {
           cout << i << " ";
           cout << (int) (*pal)[i].getRed() << " ";
           cout << (int) (*pal)[i].getGreen() << " ";
           cout << (int) (*pal)[i].getBlue() << endl;
       }
    }
   
    delete[] fn;
    delete fin;
    delete pal;
}
