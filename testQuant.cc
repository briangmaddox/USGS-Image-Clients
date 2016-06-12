#include "DGPImageIFile.h"
#include "DGPImageOFile.h"
#include "RGBPixel.h"
#include "RGBPallette.h"
#include <math.h>
#include <iostream.h>

//RGBPixel* quantize(RGBPallette* pal,RGBPixel* pix)
unsigned char quantize(RGBPallette* pal,RGBPixel* pix)
{
   double h1,s1,v1;
   double h2,s2,v2;
   unsigned char closest_entry;
   double closest_distance=9999999999.0;
   double d; 
   unsigned char i;

   h1 = pix->getHue();
   s1 = pix->getSaturation();
   v1 = pix->getValue();

   h1 = h1*3.14159265/180.0;

   for (i=0;i<pal->getNoEntries();i++)
   {
      h2 = (*pal)[i].getHue();
      h2 = h2*3.14159265/180.0;
      s2 = (*pal)[i].getSaturation();
      v2 = (*pal)[i].getValue();
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
      if (d < closest_distance)
      {
         closest_distance = d;
         closest_entry = i;
      }
   }
/*
   temp = new RGBPixel((*pal)[closest_entry].getRed(),
                       (*pal)[closest_entry].getGreen(),
                       (*pal)[closest_entry].getBlue());
   return temp;
*/
   return closest_entry;
}


void main(int argc, char** argv)
{
    DGPImageIFile* fin;
    DGPImageOFile* fout;
    RGBPallette* pal;
    RGBPallette* pal2;
    RGBPallette* bigpal;
    long i,j;
    unsigned char* inl;
    unsigned char* outl;
    RGBPixel* pix;
    RGBPixel* temp;
    unsigned char ent;
    unsigned char mapping[256];
    

    if (argc < 3)
    {
       cerr << "usage: testQuant <infile> <outfile>" << endl;
       exit(69);
    }

    fin = new DGPImageIFile(argv[1]);
    bigpal = (RGBPallette*) fin->getPallette();
    pal = new RGBPallette(12);
    pal2= new RGBPallette(12);
    fout = new DGPImageOFile(argv[2],fin->getWidth(),fin->getHeight(),
                             pal->getNoEntries(),fin->getX(),fin->getY(),
                             fin->getPixelSize());

    temp = new RGBPixel(0,0,0);  /* black */
    pal->setEntry(0,temp);
    delete temp;
    temp = new RGBPixel(255,255,255);    /* white */
    pal->setEntry(1,temp);
    delete temp;
    temp = new RGBPixel(60,80,240);   /* blue */
    pal->setEntry(2,temp);
    delete temp;
    temp = new RGBPixel(240,20,20);  /* red */
    pal->setEntry(3,temp);
    delete temp;
    temp = new RGBPixel(144,66,45);  /* brown */
    pal->setEntry(4,temp);
    delete temp;
    temp = new RGBPixel(210,240,170); /* light green */
    pal->setEntry(5,temp);
    delete temp;
    temp = new RGBPixel(150,115,56);  /* light brown */
    pal->setEntry(6,temp);
    delete temp;
    temp = new RGBPixel(200,20,160);  /* purple */
    pal->setEntry(7,temp);
    delete temp;
    temp = new RGBPixel(190, 225, 255);  /* light blue */ 
    pal->setEntry(8,temp);
    delete temp;
    temp = new RGBPixel(255, 160, 255);  /* light purple */
    pal->setEntry(9,temp);
    delete temp;
    temp = new RGBPixel(255, 190, 190);  /* light red */
    pal->setEntry(10,temp);
    delete temp;
    temp = new RGBPixel(200, 200, 200);  /* light grey */
    pal->setEntry(11,temp);
    delete temp;

    temp = new RGBPixel(60,60,60);  /* black */
    pal2->setEntry(0,temp);
    delete temp;
    temp = new RGBPixel(255,255,255);    /* white */
    pal2->setEntry(1,temp);
    delete temp;
    temp = new RGBPixel(60,84,240);   /* blue */
    pal2->setEntry(2,temp);
    delete temp;
    temp = new RGBPixel(225,80,40);  /* red */
    pal2->setEntry(3,temp);
    delete temp;
    temp = new RGBPixel(110,65,45);  /* brown */
    pal2->setEntry(4,temp);
    delete temp;
    temp = new RGBPixel(215,240,200); /* light green */
    pal2->setEntry(5,temp);
    delete temp;
    temp = new RGBPixel(170,125,105);  /* light brown */
    pal2->setEntry(6,temp);
    delete temp;
    temp = new RGBPixel(225,40,90);  /* purple */
    pal2->setEntry(7,temp);
    delete temp;
    temp = new RGBPixel(185, 185, 225);  /* light blue */ 
    pal2->setEntry(8,temp);
    delete temp;
    temp = new RGBPixel(250, 210, 220);  /* light purple */
    pal2->setEntry(9,temp);
    delete temp;
    temp = new RGBPixel(250, 200, 190);  /* light red */
    pal2->setEntry(10,temp);
    delete temp;
    temp = new RGBPixel(170, 170, 170);  /* light grey */
    pal2->setEntry(11,temp);
    delete temp;

    for (i=0;i<bigpal->getNoEntries();i++)
    {
        pix = bigpal->getEntry(i);
        ent = quantize(pal2,pix);
        mapping[i] = ent; 
    }

    
    fout->setPallette(pal);
    for (i=0;i<fin->getHeight();i++)
    {
        inl = (unsigned char*)fin->getRawScanline(i);
        outl = new unsigned char[fin->getWidth()];
        for (j=0;j<fin->getWidth();j++)
        {
            outl[j] = mapping[inl[j]];
/*
            outl[j].setRed(temp->getRed());
            outl[j].setGreen(temp->getGreen());
            outl[j].setBlue(temp->getBlue());
            outl[j].setRed((*pal)[ent].getRed());
            outl[j].setGreen((*pal)[ent].getGreen());
            outl[j].setBlue((*pal)[ent].getBlue());
*/
/*
            cout << "Quantisizing ";
            cout << (int) pix->getRed() << " ";
            cout << (int) pix->getGreen() << " ";
            cout << (int) pix->getBlue() << " ";
            cout << "to ";
            cout << (int) outl[j].getRed() << " ";
            cout << (int) outl[j].getGreen() << " ";
            cout << (int) outl[j].getBlue() << endl;
*/
        }
        fout->putRawScanline(outl,i);
        delete[] inl;
        delete[] outl;
    }

    delete pal; 
    delete pal2;
    delete bigpal;
    delete fin;
    delete fout;
}
    
   


      
