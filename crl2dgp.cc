#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "Image/DGPImageOFile.h"
#include "Image/CRLImageIFile.h"
#include "Image/RGBPallette.h"
#include "Image/RGBPixel.h"

// default command line arguments
#define X 0
#define Y 0
#define PIXEL_SIZE 1.218

void usage(char* name)
{
   cerr << "\n\nUSAGE: " << name;
   cerr << " <crlfile> <dgpfile>";
   cerr << " {-x num} {-y num}  {-p num}" << endl;
   cerr << "Input will come from input file and be directed to output file." << endl;
   cerr << "Both input and output files are mandatory. Other arguments\nare optional." << endl;
   cerr << "{-x num} - x position" << endl;
   cerr << "{-y num} - y position" << endl;
   cerr << "{-p num} - pixel size" << endl;

   cerr << "\nDefault options are :\n" << endl;
   cerr << "x: " << X << endl;
   cerr << "y: " << Y << endl;
   cerr << "Pixel size: " << PIXEL_SIZE << endl;
   exit(69);
}

void main(int argc, char** argv)
{

   DGPImageOFile* fout;
   CRLImageIFile* fin;
   unsigned char* pixbuf;
   RGBPallette* pal;
   char* ptr;
   char* ptr2;
   int i,l;
   long rectcount;
   double x,y,pix_size;
   char* dgpfile;
   char* crlfile;
   long rectx, recty, x1, y1, x2, y2;
   int found_crl = 0;
   int found_dgp = 0;;
   int state;   // 0 - start
                // 1 - looking for a value

   char updt[4] = { '|','/','-','\\' };
   int foo;

   x = X;
   y = Y;
   pix_size = PIXEL_SIZE;

   state = 0;
   if (argc < 3)
   {
      usage(argv[0]);
   }

   i = 1;
   while (i<argc) 
   {
       ptr=argv[i]; 
       if ((*ptr) == '-')
       {
           ptr++;
           if ((*ptr) == 'x')
           {
              errno = 0;
              x = strtod(argv[++i],&ptr2);    
              if ((x == 0.0) && (ptr2 == argv[i]))
              {
                 cerr << "Argument to -x must be numeric." << endl;
                 exit(69);
              }
           }
           else if ((*ptr) == 'y')
           {
              errno = 0;
              y = strtod(argv[++i],&ptr2);    
              if ((y == 0.0) && (ptr2 == argv[i]))
              {
                 cerr << "Argument to -y must be numeric." << endl;
                 exit(69);
              }
           }
           else if ((*ptr) == 'p')
           {
              errno = 0;
              pix_size = strtod(argv[++i],&ptr2);
              if ((pix_size == 0) && (ptr2 == argv[i]))
              {
                 cerr << "Argument to -p must be numeric." << endl;
                 exit(69);
              }
           }
           else
           {
               cerr << "Unknown argument " << argv[i] << "." << endl;
               exit(69);
           }
               
       }
       else   // filename 
       {
           l = strlen(argv[i]);
           l -= 3;
           ptr = &((argv[i])[l]); 
           if (!strcmp(ptr,"dgp"))
           {
              if (!found_dgp)
              {
                 dgpfile = new char[strlen(argv[i])+1];
                 strcpy(dgpfile,argv[i]);
                 found_dgp = 1;
              }
              else
              {
                 cerr << "You may specify only one DGP file." << endl;
                 exit(69);
              }
           }
           else if (!strcmp(ptr,"crl"))
           {
              if (!found_crl)
              {
                 crlfile = new char[strlen(argv[i]+1)];
                 strcpy(crlfile,argv[i]);
                 found_crl = 1;
              }
              else
              {
                 cerr << "You may specify only one CRL file." << endl;
                 exit(69);
              }
           }
           else 
           {
              cerr << "Unknown file type.  File must end in" << endl;
              cerr << "either .dgp or .crl ." << endl;
              exit(69);
           } 
              
       }
       i++;
    }

    if (!(found_crl && found_dgp))
    {
       usage(argv[0]);
    }

    cout << "Setting up...be patient, this could take a minute or two." << flush;

    fin = new CRLImageIFile(crlfile);
    if (!fin->good())
    {
       cerr << "Cannot open " << crlfile << endl;
       exit(69);
    }
    pal = (RGBPallette*)fin->getPallette();
    fout = new DGPImageOFile(dgpfile,fin->getWidth(),fin->getHeight(),
                             pal->getNoEntries(), x,y,pix_size);
    if (!fout->good())
    {
       cerr << "Cannot open " << dgpfile << endl;
       exit(69);
    }
    fout->setPallette(pal);

    
    rectcount = 1;
    foo = 0;

    cout << "\rConverting " << fin->getTilesHigh()*fin->getTilesWide() << " tiles.                                              " << endl;
    for (recty=0;recty<fin->getTilesHigh();recty++)
        for (rectx=0;rectx<fin->getTilesWide();rectx++)
        {
            if ((rectcount % 16) ==0)
            {
               cout << updt[foo] << " " << rectcount << "\r" << flush;
               foo++;
               foo = foo % 4;
            }
            else if ((rectcount % 4) == 0)
            {
               cout << updt[foo] << "\r" << flush;
               foo++;
               foo = foo % 4;
            }
            y1 = recty*fin->getMaxTileHeight();
            y2 = recty*fin->getMaxTileHeight()+
                 fin->getTileHeight(recty*fin->getTilesWide()+rectx+1)-1;
            x1 = rectx*fin->getMaxTileWidth();
            x2 = rectx*fin->getMaxTileHeight()+
                 fin->getTileWidth(recty*fin->getTilesWide()+rectx+1)-1; 
            pixbuf = (unsigned char*)fin->getRawRectangle(x1,y1,x2,y2);
            fout->putRawRectangle(pixbuf,x1,y1,x2,y2);
            delete pixbuf;
            rectcount++;
        }
              
    cout << endl;
    delete pal;
    delete fin;
    delete fout;
    delete dgpfile;
    delete crlfile;
       
}
