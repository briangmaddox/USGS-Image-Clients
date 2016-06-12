#include "Image/DGPImageIOFile.h"
#include "Image/RGBPixel.h"
#include "Image/RGBPallette.h"
#include "FrodoLib/SpinnerThingy.h"
#include <iostream.h>
#include <GetOpt.h>


// Color index definitions.  This corresponds to standard colormap
#define PINK           9
#define GREEN          5
#define BLUE           2
#define PURPLE         10
#define LTBLUE         8
#define WHITE          1
#define BROWN          4
#define LTBROWN        12
#define LTGREY         11

// These are the thresholds and granularities for denoising
#define PINKTHRESHOLD         30
#define GREENTHRESHOLD        35 
#define BLUETHRESHOLD          8
#define PURPLETHRESHOLD       30
#define LTBLUETHRESHOLD        8
#define DENOISEGRANULARITY    20 

// These are the granularities for descreening
#define PINKGRAN               3
#define GREENGRAN              3
#define BLUEGRAN               4
#define PURPLEGRAN             3
#define LTBLUEGRAN             3

// These are the number of pixels which must be present if
// descreening is to be done on a particular color.
#define LEASTPINK              100000
#define LEASTGREEN             100000
#define LEASTBLUE              100000
#define LEASTPURPLE            100000
#define LEASTLTBLUE            100000
#define LEASTBROWN             100000
#define LEASTLTBROWN           100000
#define LEASTLTGREY            100000

int pink_t;
int green_t;
int blue_t;
int purple_t;
int ltblue_t;

int do_green=1;
int do_pink=1;
int do_purple=1;
int do_ltblue=1;
int do_blue=1;

int pinkCount = 0;
int greenCount = 0;
int blueCount = 0;
int purpleCount = 0;
int ltblueCount = 0;

void den_count(int &pink, 
               int& purple,
               int& green,
               int& blue,
               int& ltblue,
               unsigned char* rect,
               long address,long width,long columns, long rows)
{
    long i,j;
    pink=0;
    purple=0;
    green=0;
    blue=0;
    ltblue=0;
    for (i=0;i<columns;i++)
        for (j=0;j<rows;j++)
        {
            switch (rect[j*width+address+i])
            {
               case PINK:   pink++; break;
               case PURPLE: purple++; break;
               case GREEN:  green++; break;
               case BLUE:   blue++; break;
               case LTBLUE: ltblue++; break;
               default:     break;
            }
        }
    pinkCount   += pink;
    purpleCount += purple;
    greenCount  += green;
    blueCount   += blue;
    ltblueCount += ltblue;
}

void change_from(unsigned char color, unsigned char* rect,
               long address, long width,long columns,long rows)
{
    long i,j;
    for (i=0;i<columns;i++)
        for (j=0;j<rows;j++)
        {
            if (rect[j*width+address+i] == color)
               rect[j*width+address+i] = WHITE;
        }
}

void den_proc(unsigned char* rect,long width,
             long rows, long granularity)
{
    long i;
    long columns;
    int pink;
    int purple;
    int green;
    int blue;
    int ltblue;

    for (i=0;i<width;i+=granularity)
    {
        if (i+granularity>=width)
        {
           columns = width - i;
           den_count(pink,
                     purple,
                     green,
                     blue,
                     ltblue,
                     rect,i,width,columns,rows);
           if ((pink<pink_t)&&do_pink)
           {
              pinkCount -= pink;
              change_from(PINK,rect,i,width,columns,rows);
           }
           if ((purple<purple_t)&&do_purple)
           {
              purpleCount -= purple;
              change_from(PURPLE,rect,i,width,columns,rows);
           }
           if ((green<green_t)&&do_green)
           {
              greenCount -= green;
              change_from(GREEN,rect,i,width,columns,rows);
           }
           if ((blue<blue_t)&&do_blue)
           {
              blueCount -= blue;
              change_from(BLUE,rect,i,width,columns,rows);
           }
           if ((ltblue<ltblue_t)&&do_ltblue)
           {
              ltblueCount -= ltblue;
              change_from(LTBLUE,rect,i,width,columns,rows);
           }
        }
        else
        {
           den_count(pink,
                     purple,
                     green,
                     blue,
                     ltblue,
                     rect,i,width,granularity,rows);
           if ((pink<pink_t)&&do_pink)
           {
              pinkCount -= pink;
              change_from(PINK,rect,i,width,granularity,rows);
           }
           if ((purple<purple_t)&&do_purple)
           {
              purpleCount -= purple;
              change_from(PURPLE,rect,i,width,granularity,rows);
           }
           if ((green<green_t)&&do_green)
           {
              greenCount -= green;
              change_from(GREEN,rect,i,width,granularity,rows);
           }
           if ((blue<blue_t)&&do_blue)
           {
              blueCount -= blue;
              change_from(BLUE,rect,i,width,granularity,rows);
           }
           if ((ltblue<ltblue_t)&&do_ltblue)
           {
              ltblueCount -= ltblue;
              change_from(LTBLUE,rect,i,width,granularity,rows);
           }
        }
    }
}

void denoise(DGPImageIOFile* foo, int granularity)
{
     long height;
     long i;
     unsigned char* slice;
     int num_rows_left;
     SpinnerThingy thingy;

     height = foo->getHeight();

     cout << "Denoising..." << endl;
     for (i=0;i<height;i+=granularity)
     {
         thingy.update(i);
         if (i+granularity>=height)
         {
            num_rows_left = height-i;

            slice=(unsigned char*)foo->getRawRectangle(0,i,
                                  foo->getWidth()-1, i+num_rows_left-1);
            den_proc(slice,foo->getWidth(),num_rows_left,granularity);
            foo->putRawRectangle(slice,0,i,
                                    foo->getWidth()-1,i+num_rows_left-1);
         }
         else
         {
            slice=(unsigned char*)foo->getRawRectangle(0,i,
                                  foo->getWidth()-1, i+granularity-1);
            den_proc(slice,foo->getWidth(),granularity,granularity);
            foo->putRawRectangle(slice,0,i,
                                    foo->getWidth()-1,i+granularity-1);
         }
         delete[] slice;
     }
     thingy.done("Done.");
}


int contains(unsigned char color, int num, unsigned char* rect, 
             long address,long width,long columns, long rows)
{
    long i,j;
    int ret=0;
    int howmany=0;
    for (i=0;i<columns;i++)
        for (j=0;j<rows;j++)
        {
            if (rect[j*width+address+i] == color)
               howmany++;
        }
    if (howmany>=num)
       ret=1;
    else
       ret=0;
    return ret;
}

void change_to(unsigned char color, unsigned char* rect,
               long address, long width,long columns, long rows)
{
    long i,j;
    for (i=0;i<columns;i++)
        for (j=0;j<rows;j++)
        {
            if (rect[j*width+address+i] == WHITE)
               rect[j*width+address+i] = color;
        }
} 

void des_proc(unsigned char color, unsigned char* rect,long width,
             long rows, long granularity,int num)
{
    long i;
    long columns;

    for (i=0;i<width;i+=rows)
    {
        if (i+granularity>=width)
        {
           columns = width-i;
           if (contains(color,num,rect,i,width,columns,rows))
              change_to(color,rect,i,width,columns,rows);
        }
        else
        {
           if (contains(color,num,rect,i,width,granularity,rows))
              change_to(color,rect,i,width,granularity,rows);
        }
    }
}

void descreen(DGPImageIOFile* foo, unsigned char color, 
              int granularity, int num)
{
     long height;
     long i;
     unsigned char* slice;
     int num_rows_left;
     SpinnerThingy thingy;
       
     height = foo->getHeight();

     cout << "Descreening..." << endl;
     for (i=0;i<height;i+=granularity)
     {
         if ((i%(granularity*10))==0)
            thingy.update(i);
         if (i+granularity>=height)
         {
            num_rows_left = height-i;
            
            slice=(unsigned char*)foo->getRawRectangle(0,i,
                                  foo->getWidth()-1, i+num_rows_left-1);
            des_proc(color,slice,foo->getWidth(),num_rows_left,granularity,num);
            foo->putRawRectangle(slice,0,i,
                                    foo->getWidth()-1,i+num_rows_left-1);
         }
         else
         {
            slice=(unsigned char*)foo->getRawRectangle(0,i,
                                  foo->getWidth()-1, i+granularity-1);
            des_proc(color,slice,foo->getWidth(),granularity,granularity,num);
            foo->putRawRectangle(slice,0,i,
                                    foo->getWidth()-1,i+granularity-1);
         }
         delete[] slice;
     }   
     thingy.done("Done.");
}

void usage(char *msg)
{
   cerr << "Error: " << msg << endl;
   cerr << "Usage: descreen [ -rpgblh ] [-N <threshold>] <file>.dgp" << endl;
   exit(42);
}

void help(void)
{
   cout << "Usage: descreen [options] <file.dgp>" << endl << endl;
   cout << "Where options may be:" << endl;
   cout << "   -r     Don't process the light red urban tint" << endl;
   cout << "   -p     Don't process the light purple revised tint" << endl;
   cout << "   -g     Don't process the green forest tint" << endl;
   cout << "   -b     Don't process the dark blue water tint" << endl;
   cout << "   -l     Don't process the light blue water tint" << endl;
   cout << "   -N n   Set noise elimination kernel size to n X n" << endl;
   cout << "   -h     Help" << endl;
   exit(0);
}

void main(int argc, char** argv)
{
     DGPImageIOFile* finout;
     long denGran=0L;
     long pinkGran;
     long purpleGran;
     long greenGran;
     long blueGran;
     long ltblueGran;;
     int option_char;
     GetOpt getopt(argc, argv,"rpgblhN:");
     

     while ((option_char = getopt())!=EOF)
     {
        switch (option_char)
        {
           case 'r': do_pink=0; break; 
           case 'p': do_purple=0; break; 
           case 'g': do_green=0; break; 
           case 'b': do_blue=0; break; 
           case 'l': do_ltblue=0; break; 
           case 'N': denGran = atoi(getopt.optarg); break;
           case 'h': help(); break;
           case '?': usage("unknown option"); break;
        }
     }

     if (getopt.optind < argc)
        finout = new DGPImageIOFile(argv[getopt.optind]);
     else
        usage("missing <file> argument.");

     if (!finout->good())
     {
        cerr << "Cannot open " << argv[getopt.optind] << endl;
        exit(69);
     }

     if (denGran==0) denGran = DENOISEGRANULARITY;
     pinkGran = PINKGRAN;
     purpleGran = PURPLEGRAN;
     greenGran = GREENGRAN;
     blueGran = BLUEGRAN;
     ltblueGran = LTBLUEGRAN;
     
     pink_t = PINKTHRESHOLD;
     purple_t = PURPLETHRESHOLD; 
     green_t = GREENTHRESHOLD; 
     blue_t = BLUETHRESHOLD; 
     ltblue_t = LTBLUETHRESHOLD; 

     denoise(finout,denGran);


     if (do_pink)
        cout << "Pink count:   " << pinkCount << endl;
     if (do_purple)
        cout << "Purple count: " << purpleCount << endl;
     if (do_green)
        cout << "Green count:  " << greenCount << endl;
     if (do_blue)
        cout << "Blue count:   " << blueCount << endl;
     if (do_ltblue)
        cout << "LtBlue count: " << ltblueCount << endl;

     if ((pinkCount >= LEASTPINK)&&(do_pink))
     {
        cout << "Doing pink..." << endl;
        descreen(finout,PINK,pinkGran,1);
     }
     if ((purpleCount >= LEASTPURPLE)&&(do_purple))
     {
        cout << "Doing purple..." << endl;
        descreen(finout,PURPLE,purpleGran,1);
     }
     if ((greenCount >= LEASTGREEN)&&(do_green))
     {
        cout << "Doing green..." << endl;
        descreen(finout,GREEN,greenGran,1);
     }
     if ((blueCount >= LEASTBLUE)&&(do_blue))
     {
        cout << "Doing blue..." << endl;
        descreen(finout,BLUE,blueGran,1);
     }
     if ((ltblueCount >= LEASTLTBLUE)&&(do_ltblue))
     {
        cout << "Doing light blue..." << endl;
        descreen(finout,LTBLUE,ltblueGran,1);
     }

     delete finout;
}

