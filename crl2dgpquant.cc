#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "Image/DGPImageOFile.h"
#include "Image/CRLImageIFile.h"
#include "Image/RGBPallette.h"
#include "Image/RGBPixel.h"

// default command line arguments
#define X 0
#define Y 0
#define PIXEL_SIZE 1.218

//------------------------

unsigned char quantize(RGBPallette* pal,RGBPixel* pix)
{
   double h1,s1,v1;
   double h2,s2,v2;
   unsigned char closest_entry;
   double closest_distance=9999999999.0;
   double d; 
   int i;

   h1 = pix->getHue();
   s1 = pix->getSaturation();
   v1 = pix->getValue();

   h1 = h1*3.14159265/180.0;

   for (i=0;i<pal->getNoEntries();i++)
   {
      h2 = (*pal)[i].getHue();
      h2 = h2*3.14159265/180.0;
      s2 = (*pal)[i].getSaturation();
      v2 = (*pal)[i].getValue();
      if (s1 == 0.0)
      {
         d = s2*s2 + (v2-v1)*(v2-v1);
      }
      else if (s2 == 0.0)
      {
         d = s1*s1 + (v2-v1)*(v2-v1);
      }
      else
      {
         d = s1*s1+s2*s2+(v2-v1)*(v2-v1)-2*s1*s2*cos(h1-h2);
      }
      if (d < closest_distance)
      {
         closest_distance = d;
         closest_entry = i;
      }
   }
   return closest_entry;
}



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
   RGBPallette* pal2;
   RGBPallette* bigpal;
   RGBPixel* temp;
   RGBPixel* pix;
   char* ptr;
   char* ptr2;
   int i,l;
   long rectcount;
   long rectsize;
   double x,y,pix_size;
   char* dgpfile;
   char* crlfile;
   long rectx, recty, x1, y1, x2, y2;
   int found_crl = 0;
   int found_dgp = 0;;
   int state;   // 0 - start
                // 1 - looking for a value
   unsigned char mapping[256];
   unsigned char ent;

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

    fin = new CRLImageIFile(crlfile);
    bigpal = (RGBPallette*)fin->getPallette();
    pal = new RGBPallette(11);
    pal2= new RGBPallette(11);
    fout = new DGPImageOFile(dgpfile,fin->getWidth(),fin->getHeight(),
                             pal->getNoEntries(), x,y,pix_size);

    temp = new RGBPixel(0,0,0);  /* black */
    pal->setEntry(0,temp);
    delete temp;
    temp = new RGBPixel(255,255,255);    /* white */
    pal->setEntry(1,temp);
    delete temp;
    temp = new RGBPixel(60,80,240);   /* blue */
    pal->setEntry(2,temp);
    delete temp;
    temp = new RGBPixel(240,20,20);  /* red */
    pal->setEntry(3,temp);
    delete temp;
    temp = new RGBPixel(144,66,45);  /* brown */
    pal->setEntry(4,temp);
    delete temp;
    temp = new RGBPixel(210,240,170); /* light green */
    pal->setEntry(5,temp);
    delete temp;
    temp = new RGBPixel(150,115,56);  /* light brown */
    pal->setEntry(6,temp);
    delete temp;
    temp = new RGBPixel(200,20,160);  /* purple */
    pal->setEntry(7,temp);
    delete temp;
    temp = new RGBPixel(190, 225, 255);  /* light blue */
    pal->setEntry(8,temp);
    delete temp;
    temp = new RGBPixel(255, 160, 255);  /* light purple */
    pal->setEntry(9,temp);
    delete temp;
    temp = new RGBPixel(255, 190, 190);  /* light red */
    pal->setEntry(10,temp);
    delete temp;

    temp = new RGBPixel(60,60,60);  /* black */
    pal2->setEntry(0,temp);
    delete temp;
    temp = new RGBPixel(255,255,255);    /* white */
    pal2->setEntry(1,temp);
    delete temp;
    temp = new RGBPixel(60,80,240);   /* blue */
    pal2->setEntry(2,temp);
    delete temp;
    temp = new RGBPixel(225,80,40);  /* red */
    pal2->setEntry(3,temp);
    delete temp;
    temp = new RGBPixel(110,65,45);  /* brown */
    pal2->setEntry(4,temp);
    delete temp;
    temp = new RGBPixel(230,240,210); /* light green */
    pal2->setEntry(5,temp);
    delete temp;
    temp = new RGBPixel(170,125,105);  /* light brown */
    pal2->setEntry(6,temp);
    delete temp;
    temp = new RGBPixel(225,40,90);  /* purple */
    pal2->setEntry(7,temp);
    delete temp;
    temp = new RGBPixel(135, 150, 215);  /* light blue */
    pal2->setEntry(8,temp);
    delete temp;
    temp = new RGBPixel(250, 195, 230);  /* light purple */
    pal2->setEntry(9,temp);
    delete temp;
    temp = new RGBPixel(250, 200, 190);  /* light red */
    pal2->setEntry(10,temp);
    delete temp;

    for (i=0;i<bigpal->getNoEntries();i++)
    {
        pix = bigpal->getEntry(i);
        ent = quantize(pal2,pix);
        mapping[i] = ent;
    }

    fout->setPallette(pal);

    
    rectcount = 1;

    for (recty=0;recty<fin->getTilesHigh();recty++)
        for (rectx=0;rectx<fin->getTilesWide();rectx++)
        {
            if ((rectcount % 25) == 0)
               cout << "." << flush;
            if ((rectcount % 100) ==0)
               cout << rectcount << flush;
            y1 = recty*fin->getMaxTileHeight();
            y2 = recty*fin->getMaxTileHeight()+
                 fin->getTileHeight(recty*fin->getTilesWide()+rectx+1)-1;
            x1 = rectx*fin->getMaxTileWidth();
            x2 = rectx*fin->getMaxTileHeight()+
                 fin->getTileWidth(recty*fin->getTilesWide()+rectx+1)-1; 
            pixbuf = (unsigned char*)fin->getRawRectangle(x1,y1,x2,y2);
            rectsize = (x2-x1+1)*(y2-y1+1);
            for (i=0;i<rectsize;i++)
                pixbuf[i] = mapping[pixbuf[i]];
            fout->putRawRectangle(pixbuf,x1,y1,x2,y2);
            delete[] pixbuf;
            rectcount++;
        }
              
    cout << endl;
    delete pal;
    delete pal2;
    delete bigpal;
    delete fin;
    delete fout;
    delete dgpfile;
    delete crlfile;
       
}
