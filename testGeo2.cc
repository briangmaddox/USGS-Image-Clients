#include "Image/GeoTIFFImageIFile.h"
#include <iostream.h>

void main(int argc, char **argv)
{
   GeoTIFFImageIFile *fin;
   double tp[6];
   double ps[3];
   int i;

   fin = new GeoTIFFImageIFile(argv[1]);

   fin->getTiePoints(tp,6);
   fin->getPixelScale(ps);
  
   cout << "(" << tp[0] << "," << tp[1] << "," << tp[2] << ") ==> (";
   cout << tp[3] << "," << tp[4] << "," << tp[5] << ")" << endl;

   cout << "[" << ps[0] << "," << ps[1] << "," << ps[2] << "]" << endl;
   
   delete fin;
}
 
