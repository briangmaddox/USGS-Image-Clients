#include "Image/TIFFImageIFile.h"
#include "Image/RGBPallette.h"
#include "Image/RGBPixel.h"
#include <iostream.h>
#include <math.h>
#include <string.h>

char clusters[256][256];

unsigned long HSVdist(RGBPixel* p1, RGBPixel* p2)
{
   double h1, s1, v1, h2, s2, v2;
   double d;
   unsigned long dist;

   h1 = p1->getHue();
   s1 = p1->getSaturation();
   v1 = p1->getValue();
   h1 = h1*3.14159265/180.0;
   h2 = p2->getHue();
   s2 = p2->getSaturation();
   v2 = p2->getValue();
   h2 = h2*3.14159265/180.0;

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
   dist = (unsigned long) d;
   return dist;
}

void main(int argc, char **argv)
{
   RGBPixel* p1;
   RGBPixel* p2;
   unsigned short r,g,b;

   unsigned long distance;
   if (argc!=7)
   { cerr << "usage: hsvdist r1 g1 b1 r2 g2 b2" << endl;
     exit(69);
   }

   r = atoi(argv[1]); 
   g = atoi(argv[2]); 
   b = atoi(argv[3]); 
   p1 = new RGBPixel(r,g,b);
   r = atoi(argv[4]); 
   g = atoi(argv[5]); 
   b = atoi(argv[6]); 
   p2 = new RGBPixel(r,g,b);

   distance = HSVdist(p1,p2);
   delete p1;
   delete p2;
   cout << "Distance: " << distance << endl;
}
   

