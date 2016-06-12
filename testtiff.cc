#include "Image/TIFFImageOFile.h"
#include "Image/RGBPallette.h"
#include "Image/RGBPixel.h"
#include <iostream.h>

void main(void)
{
   TIFFImageOFile *fout;
   RGBPallette    *pal;
   RGBPixel       *pix;
   unsigned char  *tbuf;
   unsigned char   val;
   int             count;
   int             i;
   int             x,y;


   pal = new RGBPallette(19);
   
   for (i=0;i<19;i++)
   {
      pix = new RGBPixel(i*13,i*13,i*13);
      pal->setEntry(i,pix);
      delete pix;
   }

   fout = new TIFFImageOFile("foo.tif",320,320);

   fout->setPallette(pal);

   tbuf = new unsigned char[320];
   val = 0;
   count = 1;
   for (y=0;y<320;y++)
   {
       for (x=0;x<320;x++)    
       {
           tbuf[x] = val;
           if ((count%10) == 0)
           {
              val++;
              val = val%19;
           }
           count++;
       }
       fout->putRawScanline(tbuf,y);
   }

   delete[] tbuf;
   delete   pal;
   delete   fout;
}
