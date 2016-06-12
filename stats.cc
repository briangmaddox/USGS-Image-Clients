#include "Image/TIFFImageIFile.h"
#include "Image/RGBPallette.h"
#include "Image/RGBPixel.h"
#include <iostream.h>
#include <math.h>
#include <string.h>


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

   
void usage(void)
{
   cerr << "usage: stats <keyword> <tiff> <distance|frequency>" << endl;
   cerr << "<keyword> is one of: clusters map hues" << endl;
   cerr << "<distance> and <frequency> are used for clusters and map" << endl;
   exit(69);
}

void main(int argc, char **argv)
{

   unsigned short hues[256];
   unsigned short counts[256];
   unsigned long pixcounts[256];
   unsigned long D[256][256];
   unsigned char E[256][256];
   unsigned long red_avg[256];
   unsigned long green_avg[256];
   unsigned long blue_avg[256];
   unsigned long cluster_pix_counts[256];
   int no[256];
   unsigned char checked[256];
   TIFFImageIFile *fin;
   RGBPallette    *pal;
   unsigned char  *pix;
   RGBPixel* p1;
   RGBPixel* p2;
   int i,j,k;
   int done,max;
   int num = 0;
   int found;
   unsigned short hue;
   unsigned short tmph;
   unsigned short tmpc;
   unsigned long distance;
   unsigned char flags;
   int frequency;

   if (argc<3)
      usage();
   flags = 0;
   if (strcmp(argv[1],"clusters")==0)
      flags = 1;
   if (strcmp(argv[1],"map")==0)
      flags = 2;
   if (strcmp(argv[1],"hues")==0)
      flags = 4;
   if (flags==0)
      usage();    
   
   if (flags==1)
   {
      if (argc>3)
         distance = atoi(argv[3]);
      else
         usage();
   }
   if (flags==2)
   {
      if (argc>3)
         frequency = atoi(argv[3]);
      else
         usage();
   }
       
   for (i=0;i<256;i++)
       pixcounts[i] = 0;

   fin = new TIFFImageIFile(argv[2]);
   if (!fin->good())
   {
      cerr << "Cannot open " << argv[2] << endl;
      exit(69);
   }
   for (i=0;i<fin->getHeight();i++)
   {
       pix = (unsigned char*) fin->getRawScanline(i);
       for (j=0;j<fin->getWidth();j++)
       {
           pixcounts[pix[j]] = pixcounts[pix[j]] + 1;
       }
       delete[] pix;
   }
   pal = (RGBPallette*) fin->getPallette();

   delete fin; 
   if ((flags&1)!=0)
   {
      for (i=0;i<256;i++)
      {
          for (j=0;j<256;j++)
          {
              p1 = pal->getEntry(i);
              p2 = pal->getEntry(j);
              D[i][j] = HSVdist(p1,p2);
              if (D[i][j]<= distance)
                 E[i][j]=1;
              else
                 E[i][j]=0;
          }  
      }
      for (i=0;i<256;i++)
      {
          no[i] = 0;
          checked[i] = 0;
          for (j=0;j<256;j++)
              no[i] += E[i][j];
      }
      done = 0;
      while (!done)
      {
         max = -1;
         i = 0;
         for (j=0;j<256;j++)
             if ((no[j]>max)&&(checked[j]==0))
             {
                max = no[j];
                i = j;
             }
         if (max!=0)
         {
            checked[i] = 1;
            for (j=0;j<256;j++)
            {
                if ((E[i][j] == 1)&&(i!=j))
                {
                   if (checked[j]==1)
                      E[i][j] = 0;
                   else
                   {
                      for (k=0;k<256;k++)
                          E[j][k] = 0;
                      no[j] = 0;
                      checked[j] = 1;
                   }
                }
            }
            done = 1;
            for (j=0;j<256;j++)
                if (checked[j] == 0)
                done = 0;
         }
         else
           done = 1;
      }
      for (i=0;i<256;i++)
      {
          if (no[i]>0)
          {
             cout << "Cluster: ";
             red_avg[i] = 0;
             green_avg[i] = 0;
             blue_avg[i] = 0;
             cluster_pix_counts[i]=0;
             for (j=0;j<256;j++)
             {
                 if (E[i][j] == 1)
                 {
                    cout << j << " ";
                    red_avg[i]   += ((*pal)[i].getRed())*(pixcounts[i]);
                    green_avg[i] += ((*pal)[i].getGreen())*(pixcounts[i]);
                    blue_avg[i]  += ((*pal)[i].getBlue())*(pixcounts[i]);
                    cluster_pix_counts[i] += pixcounts[i];
                 }
             }
             cout << endl;
             if (cluster_pix_counts[i]!=0)
             {
                red_avg[i] = red_avg[i]/cluster_pix_counts[i];
                green_avg[i] = green_avg[i]/cluster_pix_counts[i];
                blue_avg[i] = blue_avg[i]/cluster_pix_counts[i];
                cout << "Cluster pix counts for " << i << ": ";
                cout << cluster_pix_counts[i] << endl;
             }
          }
      }
      j=0;
      for (i=0;i<256;i++)
      {
          if (no[i]>0)
          {
             if (cluster_pix_counts[i]!=0)
             {
                cout << "Average color for cluster " << j;
                cout << ": " << red_avg[i] << " ";
                cout << green_avg[i] << " " << blue_avg[i] << " ";
                cout << "Count: " << cluster_pix_counts[i] << endl;
             }
             else
             {
                cout << "No pixels found in cluster " << j << endl;
             }
             j++;
          }
      }
  
   }

   if ((flags&2)!=0)
   {
      for (i=0;i<pal->getNoEntries();i++)
      {
          if (pixcounts[i] >= frequency)
          {
             cout.width(6);
             cout << i << " ";
             cout.width(6);
             cout << (*pal)[i].getRed() << " ";
             cout.width(6);
             cout << (*pal)[i].getGreen() << " ";
             cout.width(6);
             cout << (*pal)[i].getBlue() << " ";
             cout.width(16);
             cout << pixcounts[i] << endl;
/*
             cout << "Index: " << i << " Count: " << pixcounts[i];
             cout << "   HSV:   ";
             cout << (*pal)[i].getHue() << "  ";
             cout << (*pal)[i].getSaturation() << "  ";
             cout << (*pal)[i].getValue() << "  ";
             cout << "  RGB:   ";
             cout << (*pal)[i].getRed() << "  ";
             cout << (*pal)[i].getGreen() << "  ";
             cout << (*pal)[i].getBlue() << endl;
*/
          }
      }
   }
        
   if ((flags&4)!=0)
   {
      for (i=0;i<pal->getNoEntries();i++)
      {
          hue = (*pal)[i].getHue();
          found = 0;
          for (j=0;j<num;j++)
          {
              if (hue==hues[j])    
              {  
                 counts[j] = counts[j] + 1;
                 found = 1;
              }
          }
          if (!found)
          {
             hues[num]=hue;
             counts[num] = 1;
             num++;
          }
      }
  

      for (i=0;i<num;i++)
          for (j=0;j<(num-i-1);j++)
          {
              if (hues[j] > hues[j+1])
              {
                 tmph = hues[j];
                 hues[j] = hues[j+1];
                 hues[j+1] = tmph;
                 tmpc = counts[j];
                 counts[j] = counts[j+1];
                 counts[j+1] = tmpc;
              }
          }
   
      cout << "Found " << num << " different hues." << endl;
      cout << endl;
    
      for (i=0;i<num;i++)
      {
         cout << "Hue: " << hues[i] << " Count: " << counts[i] << endl; 
      }
   }
   delete pal;
}
