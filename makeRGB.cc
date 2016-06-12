#include "Image/RGBImageOFile.h"
#include "Image/ImageDebug.h"

void main(void)
{
   RGBImageOFile *fout;
   unsigned char* buf;

   int r;
   int g;
   int b;
   int i;
   int x,y,w,h;

   x = 0; y = 0; w = 5; h = 5;
   fout = new RGBImageOFile("test2.rgb",320,320);

   buf = new unsigned char[75];
   for (r=0;r<256;r+=16)
   {
      for (g=0;g<256;g+=16)
      {
         for (b=0;b<256;b+=16)
         {
            for (i=0;i<75;i+=3)
            {
                buf[i]   = r;
                buf[i+1] = g;
                buf[i+2] = b;
            }
            fout->putRawRectangle(buf,x,y,x+w-1,y+h-1);
            x += h;
            if (x==320)
            {
               cout << "New Row (y=" << y << ")" <<  endl;
               x = 0;
               y += h;
            }
         }
      }
   }
   delete[] buf;
   delete fout;
}
               
