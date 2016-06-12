#include "Image/TIFFImageIFile.h"
#include "Image/GeoTIFFImageIFile.h"
#include <iomanip.h>
#include <iostream.h>
#include <String.h>

int main(int argc, char **argv)
{

   TIFFImageIFile    *fin;

   unsigned short scount=0;
   char *desc;
   double *t;
   int i;
   bool geotiff;
   bool pid;

   cout << setw(20) << " ";
   cout << setw(10) << "Geotiff";
   cout << setw(10) << "PID";
   cout << endl;
   cout << setw(20) << "Filename";
   cout << setw(10) << "Missing";
   cout << setw(10) << "Missing";
   cout << endl;
   cout << setw(20) << "--------";
   cout << setw(10) << "--------";
   cout << setw(10) << "--------";
   cout << endl;
   for (i=1;i<argc;i++)
   {
      fin = new TIFFImageIFile(argv[i]);
      if (fin->good())
      {
         if (TIFFGetField(fin->getTif(),33550,&scount,&t)==0)
            geotiff = false;
         else
            geotiff = true;
         if (fin->getTag(TIFFTAG_IMAGEDESCRIPTION,&desc)==0)
            pid = false;
         else
         {
           String tmpstr(desc);
           if (tmpstr.contains("Product:"))
              pid = true;
           else
              pid = false;
         }
      }
      if (!(geotiff&&pid))
      {
         cout << setw(20) << argv[i];
         if (!geotiff)
            cout << setw(10) << "X";
         else
            cout << setw(10) << " ";
         if (!pid)
            cout << setw(10) << "X";
         else
            cout << setw(10) << " ";
             
         cout << endl;
      }
      delete fin;
   }
  
   return 0;
}
