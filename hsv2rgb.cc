#include "Image/HSVPixel.h"
#include "Image/RGBPixel.h"
#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <stdlib.h>


void main(int argc, char** argv)
{
   unsigned short r,g,b;
   unsigned short h,s,v;
   int rgb;
   RGBPixel* foo;
   HSVPixel* foo2;

   if (argc<5)
   {
      cerr << "usage: hsv2rgb -h <h> <s> <v>" << endl;
      cerr << "       hsv2rgb -r <r> <g> <b>" << endl;
      exit(69);
   }

   if (!strcmp(argv[1],"-h"))
      rgb = 0;
   else if (!strcmp(argv[1],"-r"))
      rgb = 1;
   else
   {
      cerr << "usage: hsv2rgb -h <h> <s> <v>" << endl;
      cerr << "       hsv2rgb -r <r> <g> <b>" << endl;
      exit(69);
   }

   if (rgb)
   {
      sscanf(argv[2],"%hu",&r);
      sscanf(argv[3],"%hu",&g);
      sscanf(argv[4],"%hu",&b);
      foo = new RGBPixel(r,g,b);
      cout << "Hue: " << foo->getHue() << endl;
      cout << "Saturation: " << foo->getSaturation() << endl;
      cout << "Value: " << foo->getValue() << endl;
      delete foo;
   }
   else
   { 
      sscanf(argv[2],"%hu",&h);
      sscanf(argv[3],"%hu",&s);
      sscanf(argv[4],"%hu",&v);
      foo2 = new HSVPixel(h,s,v);
      cout << "Red: " << foo2->getRed() << endl;
      cout << "Green: " << foo2->getGreen() << endl;
      cout << "Blue: " << foo2->getBlue() << endl;
      delete foo2;
   }
}
