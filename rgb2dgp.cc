#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "Image/DGPImageOFile.h"
#include "Image/RGBImageIFile.h"
#include "Image/RGBPallette.h"
#include "Image/RGBPixel.h"

// default command line arguments
#define X 0
#define Y 0
#define PIXEL_SIZE 1.218

void usage(char* name)
{
   cerr << "\n\nUSAGE: " << name;
   cerr << " <rgbfile> <dgpfile>";
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
   RGBImageIFile* fin;
   RGBPixel* pixbuff;
   RGBPallette* pal;
   char* ptr;
   char* ptr2;
   int i,l;
   double x,y,pix_size;
   char* dgpfile;
   char* rgbfile;
   long rectx, recty, x1, y1, x2, y2;
   int found_rgb = 0;
   int found_dgp = 0;;
   int state;   // 0 - start
                // 1 - looking for a value

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
           else if (!strcmp(ptr,"rgb"))
           {
              if (!found_rgb)
              {
                 rgbfile = new char[strlen(argv[i]+1)];
                 strcpy(rgbfile,argv[i]);
                 found_rgb = 1;
              }
              else
              {
                 cerr << "You may specify only one RGB file." << endl;
                 exit(69);
              }
           }
           else 
           {
              cerr << "Unknown file type.  File must end in" << endl;
              cerr << "either .dgp or .rgb ." << endl;
              exit(69);
           } 
              
       }
       i++;
    }

    if (!(found_rgb && found_dgp))
    {
       usage(argv[0]);
    }

    fin = new RGBImageIFile(rgbfile);
    if (!fin->good())
    {
       cerr << "Cannot open " << rgbfile << endl;
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

    
    for (i=0;i<fin->getHeight();i++)
    {
        pixbuff = (RGBPixel *) fin->getScanline(i);
        fout->putScanline(pixbuff,i);
        delete[] pixbuff;
    } 

              
    delete pal;
    delete fin;
    delete fout;
    delete dgpfile;
    delete rgbfile;
       
}
