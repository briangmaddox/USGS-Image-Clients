// Brian Maddox
// United States Geological Survey - Mid Continent Mapping Center SES
// 11 March 1997
// This program opens the geotiff and displays some values so we can
// check it.  

#include "geotiffdump.h"
#include "Image/ImageCommon.h"
#include "Image/TIFFImageIFile.h"
#include "Image/GeoTIFFImageIFile.h"
#include <iostream.h>
#include <iomanip.h>
#include <string>

void main (int argc, char *argv[])
{
  string* argument;
  unsigned short geotagcheck = 34735; // used to make sure there is a geotiff
                                      // directory in the file
  TIFFImageIFile* t_tif;   // used to open the file as a standard tiff first
  unsigned short dontcare; // dummy var to hold return value for geotiff check

  if (argc == 2)
  {
    argument = new string(argv[1]);
    if (*argument == "-V")
    {
      cout << argv[0] << ": $Revision: 1.1.1.1 $" << endl;
      delete argument;
      exit(0);
    }
    else
      delete argument;
  }
  else if (argc < 2) // No command line arguments given
  {
    cout << "Usage: " << argv[0] << " filelist" << endl;
    exit(0);
  }

  

  GeoTIFFImageIFile* infile;
  int count;
  unsigned short foo;                  // just for when we need it
  unsigned long width, height;         // dimensions of the geotiff
  int nadtype;                         // to store the determined NAD type in
  double utmx, utmy, utmxres, utmyres; // For get_utm
  unsigned short utmzone;
  unsigned short foo1;                 // temp var
  double* pixsize;                     // Array to hold the scale values

  char* line;        // hold data string from the geoTIFF
  char cellname[32]; // Hold the cell name

  long int pid;      // Product ID
  long mapscale;     // Scale the map was made in
  int start, end, i; // for the parsing routine


  for (count = 1; count < argc; count++)
  {
    pid = 0;
    mapscale = 0;
    
    // check to make sure it's a geotiff.  This is a temp fix until I modify
    // the Image library not to just seg fault if the geotiff lib is unhappy
    t_tif = new TIFFImageIFile(argv[count]);
    if (!t_tif->getTag(geotagcheck, &dontcare)) // false means no geotiff dir
    {
      cout << argv[count] << "does not contain GeoTIFF data" << endl;
      delete t_tif;
      continue; // skip to the next argument
    }
    delete t_tif;

    infile = new GeoTIFFImageIFile(argv[count]);

    if (!infile->good())
    {
      cout << "Could not open file " << argv[count] << endl;
      delete infile;
      continue;
    }

    cout << "Now displaying information for file: " << argv[count] << endl;


    // Borrowed this from geocheck.  Parse out the product ID and stuff
    infile->getTag(TIFFTAG_IMAGEDESCRIPTION, &line);
    start = 0;
    end = 0;
    strcpy(cellname,"                  ");
    for(i=31; i < strlen(line); i++)
    {
      if( (line[i] == ' ') && (start==0) )
      {
	start = i + 1;
      }
      if( (line[i] == '.') && (end==0) )
      {
	end = i - 1;
      }
    }
    for(i=start; i <= end; i++)
    {
      cellname[i-start] = line[i];
    }
    cellname[end - start + 1] = '\0';
    sscanf(line, "USGS GeoTIFF DRG 1:%d", &mapscale);
    sscanf(line + end + 3, "Product:%d", &pid);
    cout << "Cell Name: " << cellname << endl;
    cout << "Product ID: " << pid << endl;
    cout << "Map Scale: 1:" << mapscale << endl;


    infile->getUSGS_UTM(&utmx, &utmy, &utmxres, &utmyres, &utmzone);
    cout.setf(ios::fixed);
    cout << "UTM UL x coordinate: " << setprecision(4) << utmx << endl;
    cout << "UTM UL y coordinate: " << setprecision(4) << utmy << endl;
    cout << "UTM x resolution:    " << utmxres << endl;
    cout << "UTM y resolution:    " << utmyres << endl;
    cout << "UTM zone is " << utmzone << endl;

    infile->getGeoKey(ProjectedCSTypeGeoKey, &foo);
    cout << "Projected CS Key value is: " << foo << endl;
    foo1 = foo - utmzone;
    if (foo1 == 26700) 
      nadtype = 1927;
    else if (foo1 == 26900)
      nadtype = 1983;
    else
      nadtype = 0000;
    cout << "NAD type: " << nadtype << endl;

    infile->getTag(TIFFTAG_IMAGEWIDTH, &width);
    infile->getTag(TIFFTAG_IMAGELENGTH, &height);
    cout << "Image width:  " << width << endl;
    cout << "Image height: " << height << endl;
    
    infile->getPixelScale(&pixsize);
    cout << "X scale: " << pixsize[0] << endl;
    cout << "Y scale: " << pixsize[1] << endl;
    cout << "Z scale: " << pixsize[2] << endl;
    cout << endl << endl;

    delete infile;
    delete [] pixsize;
    delete [] line;
  }

  return(0);
}
