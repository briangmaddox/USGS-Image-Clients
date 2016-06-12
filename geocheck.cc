// geocheck.cc				Last Edited:	11/26/96 BGM
//					David Edwards	SES/RTA/MCMC/USGS
//
// This program is used to check USGS DRGs. As far as I know, all
// copyrights/etc to the code listed below are held by the US Geological
// Survey. It is built on top of the USGS Image library, which is built
// on top of a standard geotiff library, which is built on top of the
// standard TIFF library. 
//
// It is in usage as "checkdrg", even though there is a different program
// (written by Greg Martin) also by that name, that is used for exactly the 
// same purpose. This is the current version of "checkdrg" as of 9/17/96. 
// Usage of this program instead of the original version of "checkdrg" is 
// probably a good idea, because this version does much more complete 
// checking of values than the original.

#include "geotiffdump.h"
// #include "TInfo.h"
#include "Image/ImageCommon.h"
#include "Image/TIFFImageIFile.h"
#include "Image/GeoTIFFImageIFile.h"
#include "Image/RGBPallette.h"
#include <iostream.h>
#include <fstream.h>
#include <string>

// Obviously, this function loads the geotiff information.
// Specifically, it opens the file, reads in info, closes
// the file, and returns a data structure.
bool	loadgeotiffinfo(char *name, TInfo* info)
{
  GeoTIFFImageIFile*	fin;
  unsigned long		x,y;
  double		utmx,utmy,utmxres,utmyres;
  unsigned short 	utmzone;
  char* 		line;
  long			row;
  double*		test;
  bool			retval;
  unsigned short	foo;
  
  fin = new GeoTIFFImageIFile(name);
  if(fin->good())
  {
    retval = fin->good();
    
    fin->getGeoKey(ProjectedCSTypeGeoKey, &foo);
    info->CSProj = foo;
    
    //	    All of this is simple stuff. It attempts to get values, and
    //	    if it succeeds, it sets variables with the returned values.
    //	    If it fails, the variables are set to zero.
    if(fin->getTag(TIFFTAG_IMAGEWIDTH,&x)&&fin->getTag(TIFFTAG_IMAGELENGTH,&y))
    {
      info->width  = x;
      info->height = y;
    }
    else
    {
      info->width  = 0;
      info->height = 0;
    }
    
    if(fin->getUSGS_UTM(&utmx,&utmy,&utmxres,&utmyres,&utmzone))
    {
      info->utmx = utmx;
      info->utmy = utmy;
      info->utmxres = utmxres;
      info->utmyres = utmyres;
      info->utmzone = utmzone;
    }
    else
    {
      info->utmx = 0.0;
      info->utmy = 0.0;
      info->utmxres = 0.0;
      info->utmyres = 0.0;
      info->utmzone = 0;
    }
    if(fin->getPixelScale(&test))
    {	
      info->pixsizex = test[0];
      info->pixsizey = test[1];
      info->pixsizez = test[2]; 
    }
    else
    {
      info->pixsizex = 0.0;
      info->pixsizey = 0.0;
      info->pixsizez = 0.0;
    }

    //	    This bunch of strange-looking code breaks open a image
    //	    description tag, and parses out the product ID number,
    //	    cell name, and scale information. If the image description
    //	    tag changes, this is probably the first place that will
    //	    catch it.
    if(fin->getTag(TIFFTAG_IMAGEDESCRIPTION,&line))
    {
      int start,end,scale,pid,i;
      char* foo;
      start = 0;
      end = 0;
      strcpy(info->cellname,"                  ");
      for(i=31;i<strlen(line);i++)
      {
	if((line[i]==' ')&&(start==0))
	{
	  start = i + 1;
	}	
	if((line[i]=='.')&&(end==0))
	{
	  end = i - 1;
	}
      }
      for(i=start;i <= end; i++)
      {
	info->cellname[i-start] = line[i];
      }
      info->cellname[end - start + 1] = '\0';
      sscanf(line,"USGS GeoTIFF DRG 1:%d", &scale);
      sscanf(line+end+3, "Product:%d", &pid);
      info->scale = scale;
      info->pid = pid;
    }
    delete fin;
    return retval;
  }
  return false;
}


// This checks the data structure, looking for problems in the data.
// It's not a particularly deep sort of checking, but it should pick up
// the basic errors most of the time.
int errorcheck(TInfo* info, char* name)
{
  int	  invalid = 0;
  int	  intfoo = 0, intfoo2 = 0;
  double  foo, fooconst;
  char	  longconver[4] = "";
  double   tempfoob = 0;   // Brian's temp float var
  // All PID's should be between 1 and 999999... (Actually, all the PID's
  // I've seen have been greater than 100000, and less than 500000, but
  // that doesn't seem to be an official thing.)
  if((info->pid < 1)||(info->pid > 999999)) 
  {	
    invalid++;
    printf("%s: Bad PID! \n",name);
  }
  
  
  // The UTM zone is calculated by subtracting longitude from 180, dividing
  // the result by 6, and then rounding up. The longitude is encoded into
  // the name of the file as the 3rd through 5th digit of the number of the
  // file.
  strncpy(longconver, name + 3, 3);
  intfoo = atoi(longconver);
  //  cout << "string " << longconver << "intfoo " << intfoo << endl;
  intfoo2 = (180 - intfoo) / 6;

  if((info->utmzone != intfoo2) &&
     (info->utmzone != intfoo2 + 1))
  {
    invalid++;
    printf("%s: UTM zone not correct. %d should be %d\n", name, info->utmzone, intfoo2);
  }

  // This check is to prevent the wrong type of CS Projection from being
  // used. For new software, this shouldn't be necessary, but the image
  // library had a bug for a while that resulted in the wrong type of
  // CS Projection being used (326xx, rather than 267xx). All the types
  // of CS Projections are listed in the GeoTIFF standard, section 6.3.3
  //   I changed this as it didn't take any other NAD type into account
  //   ie, NAD83, ect.  BGM 21 Nov 1996

  int projvalue = info->CSProj - info->utmzone;
  if ( (projvalue != 26700) && (projvalue != 26900) )
  {
    invalid++;
    printf("%s: Projection type incorrect (%d).\n", name,
	   info->CSProj); 
  }
  
  // UTM X coordinates need to be greater than 0. UTM Y coordinates need
  // to be greater than 0. Typically, UTM X is a 6-digit number and
  // UTM Y is a 7-digit number, but that's not a "hard and fast" rule.
  if((info->utmx < 0.01)||(info->utmy < 0.01))
  {
    invalid++;
    printf("%s: Bad UTM coordinates! \n",name);
  }

  // Does a rough check on the scale. It gets checked again. This 
  // just checks to make sure that it's greater than 1:1.
  if(info->scale < 1)
  {
    invalid++;
    printf("%s: Bad Scale! \n", name);
  }
  
  // Does a rough check on the TIFF information. It should check
  // more specifically for heights and widths in the thousands,
  // but for the moment, it just makes sure it's not equal to zero.
  if((info->height < 1)||(info->width < 1))
  {
    invalid++;
    printf("%s: TIFF image size invalid! \n", name);
  }
  
  if(invalid == 0)
  { 
    // Set some variables specific to each map scale. The first letter of
    // the file name indicates the scale of the map contained in the file...
    // oXXXXXXXX.tif is a 24K map, fXXXXXXX.tif is a 100K map, and
    // cXXXXXXXX.tif is a 250K map. More will be added later, as need
    // warrents.
    switch(name[0])
    {
    case 'o': // A 24K map. The pixel should be 2.4384M on a
              // side.
      if(info->scale != 24000)
	invalid++;
      fooconst = 2.438;
      break;
    case 'c': // A 250K map. The pixel should be 25.4M on a 
      // side.
      if(info->scale != 250000)
	invalid++;
      fooconst = 25.39;
      break;
    case 'f': // A 100K map. The pixel should be 10.16M on a
      // side.
      if(info->scale != 100000)
	invalid++;
      fooconst = 10.15;
      break;
    case 'i': // A 63K map. The pixel should be 6.4M on a
              // side.
              // changed.  i's are actually all 63360 BGM 30 July 1997
      if(info->scale != 63360)
	invalid++;
      fooconst = 6.39;
      break;
    case 'r': // A 20K map. The pixel should be 2.032M on a
              // side.
      if(info->scale != 20000)
	invalid++;
      fooconst = 2.02;
      break;
    case 'l': // A 25K map. The pixel should be 2.54M on a
              // side.
      if(info->scale != 25000)
	invalid++;
      fooconst = 2.52;
      break;
    case 'j': // A 30K map. The pixel should be 3.048M on a
              // side.
      if(info->scale != 30000)
	invalid++;
      fooconst = 3.03;
      break;
    case 'k': // A 25K map. The pixel should be 2.54M on a side.
      if(info->scale != 25000)
	invalid++;
      fooconst = 2.52;
      break;
    default: // Presumably, a 25K or some other wierd size.
      fooconst = info->pixsizex;
      break;
    }
    //	  Use the values set above to check to make sure that pixel sizes
    //	  are correct. There is a .1 meter tolerance on all sizes, to 
    //	  make sure that floating point variations aren't a problem.
    //	  Values probably should be a whole bunch more exact... but they're
    //	  not. Deal with it. (Besides... the inprecision we're talking
    //	  here is 1/10th of a meter. If it's that close, we really don't
    //	  have a major problem.
    foo = info->pixsizex - fooconst;
    if((foo > 0.1)||(foo < -0.1))
    {
      cout << name << ": Incorrect X pixel resolution" << endl;
      invalid++;
    }
    foo = info->pixsizey - fooconst;
    if((foo > 0.1)||(foo < -0.1))
    {
      cout << name << ": Incorrect Y pixel resultion" << endl;
      invalid++;
    }
    foo = info->utmxres - fooconst;
    if((foo > 0.1)||(foo < -0.1))
    {
      cout << name << ": Incorrect UTM X resolution" << endl;
      invalid++;
    }
    foo = info->utmyres - fooconst;
    if((foo > 0.1)||(foo < -0.1))
    {
      cout << name << ": Incorrect UTM Y resolution" << endl;
      invalid++;
    }
  }	
  return invalid;

}

int main(int argc, char *argv[])
{
  // Added these 20 Nov 1996 to fix the seg fault problem. BGM
  unsigned short int checkvalue = 34735;
  bool foundit = false;
  ifstream istr;
  TIFFImageIFile* t_tif;
  unsigned short int dontcare;
  string* argument;

  TInfo*   info;
  int	   count, count2;
  int	   invalid;
  int	   anal, header, good;

  if (argc < 2)
  {
    cerr << "Usage: " << argv[0] << " filelist" << endl;
    exit(-1);
  }

  info = new TInfo;
  
  anal = strcmp(argv[0],"checkdrg.verbose");
  if(anal != 0) anal = 1;
  
  header = 0;
  good  = 0;

  // Okie, I'm making an assumption here.  If they do a checkdrg *, then
  // I'm going to assume that the files will all be there since the shell
  // will find out when it does the * replacement.  Otherwise, if they run
  // it with just 1 filename, then we'll be nice and check and make sure that
  // the file is truly there instead of just sending it to TIFF which dumps
  // core if the file isn't there.  This will actually check for file
  // existence once everyone gets moved over to Solaris so it won't kill 
  // Zeus's IO system BGM
  if (argc == 2)
  {

    argument = new string(argv[1]);
    if (*argument == "-V")
    {
      cout << argv[0] << ": $Revision: 1.1.1.1 $" << endl;
      delete argument;
      delete info;
      exit(0);
    }
    else
      delete argument;

    istr.open(argv[1]);
    if (!istr)
    {
      cerr << argv[1] << " does not exist!  Exiting " << argv[0] << endl;
      delete info;
      exit(-1);
    }
  }

  for(count = 1; count < argc; count++)
  {
    good = 0; // Reinitialize for the loop BGM 13 Feb 1997

    // changed this here to keep the library from seg faulting.  I put it in
    // here as it was the quickest way to fix this but eventaully will move
    // this error checking into the Image library itself. BGM 20 Nov 1996
    t_tif = new TIFFImageIFile(argv[count]);
    foundit = t_tif->getTag(checkvalue, &dontcare); 
    delete t_tif;

    if (foundit) // geoTIFF stuff present so go ahead and process
    {
      
      
      //  Ugh. Lots of lines of "pretty printing" stuff. This may get
      //  changed later on to only show entries that have problems.
      if(loadgeotiffinfo(argv[count],info))
      {	
	invalid = errorcheck(info,argv[count]);
	if (invalid > 0)
	  good++; // Set here to correct a minor logic bug
	if ( (invalid > 0) && (anal==0) )
	{
	  //  yuck. This prints out a wide banner with header information.
	  //  This will probably eventually get trimmed.
	  if (header == 0)
	  {
            printf("Error   File            PID     GeoRef  UTM     ");
	    printf("Pixel  Pixel   Pixel   UpperL          UpperL  ");
	    printf("      Map      Cell\n");
	    printf("Flag    Name            Number  Info    Zone    ");
	    printf("X Size Y Size  Z Size  X coord         Y coord ");
	    printf("      Scale    Name\n");
	    printf("------------------------------------------------");
	    printf("------------------------------------------------");
	    printf("---------------------\n");
	    header++;
	  }
	  
	  if(invalid > 0)
	    printf("******  ");
	  else
	    printf("        ");
	  
	  printf("%s      ",argv[count]);
	  printf("%d    ",info->pid);
	  if((info->utmzone != 0)&&(info->scale != 0))
            printf("X       ");
	  else
	    printf("        ");
          printf("%d      ",info->utmzone);
	  printf("%6.4f   %6.4f   %6.4f   ",info->pixsizex,
		 info->pixsizey, info->pixsizez);
	  
          printf("%9.4f   %9.4f   ", info->utmx, info->utmy);
	  
	  if(info->scale > 99999)
	    printf("1:%d",info->scale);
	  else
	    printf("1:%d    ",info->scale);
          printf(" %s     ",info->cellname);
	  printf("\n");
	}
      }
	else
	  //	  else - you got serious problems with your so-called DRG.
	  printf("%s is a bad geotiff. Rerun makemet/tif2usgsdrg.\n",
		 argv[count]);
    }
    else
    {
      cout << argv[count] << " does not contain geoTIFF information. ";
      cout << "Please run makemet / tif2usgsdrg." << endl;
      good++;
    }

	
    if(good == 0)
      //      printf("The DRGs are good.\n");
      cout << "The DRG " << argv[count] << " is good." << endl;
  }
  delete info;
}




