#include "Image/GeoTIFFImageOFile.h"
#include "Image/TIFFImageIFile.h"
#include "Image/RGBPallette.h"
#include <iostream.h>

void main(int argc, char **argv)
{
   TIFFImageIFile* fin;
   GeoTIFFImageOFile* fout;
   RGBPallette* pal;
   unsigned char* line;
   int h,i;
   double tiepoints[6] = {0,0,0,32.0,130.0,0.0};
   double pixscale[3] = {2.384,2.384,0};

   if (argc != 2)
   {
      cerr << "oops" << endl;
      exit(42);
   }

   fin = new TIFFImageIFile(argv[1]);
   if (!fin->good())
   {
      cerr << "can't open " << argv[1] << endl;
      exit(42);
   }
   pal = (RGBPallette*) fin->getPallette();
   fout = new GeoTIFFImageOFile("foogeo.tif",fin->getWidth(),fin->getHeight());
   fout->setPallette(pal);
   delete pal;

   fout->setTiePoints(tiepoints,6);
   fout->setPixelScale(pixscale);
   fout->setGeoKey(GTModelTypeGeoKey,(unsigned short)ModelGeographic);
   fout->setGeoKey(GTRasterTypeGeoKey,(unsigned short)RasterPixelIsArea);
   fout->setGeoKey(GTCitationGeoKey,"This is a citation key");
   fout->setGeoKey(GeographicTypeGeoKey,(unsigned short)KvUserDefined);
   fout->setGeoKey(GeogCitationGeoKey,"Universal TransMercator");
   fout->setGeoKey(GeogAngularUnitsGeoKey,(unsigned short)Angular_Degree);
   fout->setGeoKey(GeogLinearUnitsGeoKey,(unsigned short)Linear_Meter);
   fout->setGeoKey(GeogGeodeticDatumGeoKey,(unsigned short)KvUserDefined);
   fout->setGeoKey(GeogEllipsoidGeoKey,(unsigned short)Ellipse_Everest1830_1967Definition);
   fout->setGeoKey(GeogSemiMajorAxisGeoKey,(double)123456.0458);
   fout->setGeoKey(GeogInvFlatteningGeoKey,(double)478.38838);

   h = fin->getHeight();
   for (i=0;i<h;i++)
   {
       line = (unsigned char *) fin->getRawScanline(i);
       fout->putRawScanline(line,i);
       delete[] line;
   }


   delete fin;
   delete fout;
}
