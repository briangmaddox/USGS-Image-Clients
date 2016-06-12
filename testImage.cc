#include "Image/RGBImage.h"
#include "Image/ImageDebug.h"
#include <iostream.h>

void main(int argc, char **argv)
{
  RGBImage* fin;
  
  gDebug.setDebugLevel(9);

  if (argc!=2)
  {
     cerr << "oops..." << endl;
     exit(1);
  }
  fin = new RGBImage(argv[1]);
  delete fin;
}
