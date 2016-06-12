#include "Image/Octree.h"
#include <iostream.h>
#include "Image/RGBPallette.h"

void visit(OctreeNode *node)
{
   int i,j;
   for (j=0;j<node->level;j++) cout << "  ";
   cout << "Level: " << (int) node->level << endl;
   for (j=0;j<node->level;j++) cout << "  ";
   cout << "<Rm Gm Bm>: <" << (int) node->redMidpoint << " ";
   cout << (int) node->greenMidpoint << " ";
   cout << (int) node->blueMidpoint << "> ";
   cout << "Index: " << node->my_index << " ";
   cout << "Colors: " << node->numColors;
   if (node->leaf) cout << " LEAF";
   cout << endl;
   for (j=0;j<node->level;j++) cout << "  ";
   cout << "Totals: <" << node->redTotal << " ";
   cout << node->greenTotal << " " << node->blueTotal << ">" << endl;
}

void main(void)
{
   Octree theTree(6);
   OctreeNode* node;
   RGBPallette* pal;
   int i;
   unsigned char r,g,b;

   theTree.addColor(200,200,200);
   theTree.addColor(0,0,0);
   theTree.addColor(201,201,201);
   theTree.addColor(201,201,201);
   theTree.addColor(201,201,201);
   theTree.addColor(201,201,201);
   theTree.addColor(201,201,201);
   theTree.addColor(200,0,0);
   theTree.addColor(200,0,0);
   theTree.addColor(200,0,0);
   theTree.addColor(200,0,0);
   theTree.addColor(200,0,0);
   theTree.addColor(200,0,0);
   theTree.addColor(202,202,202);
   theTree.addColor(202,202,202);
   theTree.addColor(202,202,202);
   theTree.addColor(0,20,200);
   theTree.addColor(0,20,200);
   theTree.addColor(255,255,255);
   theTree.addColor(255,255,255);
   theTree.addColor(255,255,255);
   theTree.addColor(0,255,255);
   theTree.addColor(0,255,255);
   theTree.addColor(0,255,255);
   theTree.addColor(202,202,202);
   theTree.addColor(128,128,128);
   theTree.addColor(128,128,128);
   
   theTree.addColor(128,0,128);
   theTree.addColor(128,0,128);
   theTree.addColor(128,0,128);
   theTree.addColor(128,0,128);
   theTree.addColor(128,0,129);
   theTree.addColor(128,0,129);


   theTree.assignIndexes();

   theTree.traverseVLR(visit);

   pal = theTree.getQuantPallette();
 
   cout << "Got the pallette..." << endl;
   cout << "Pallette has " << pal->getNoEntries() << " entries" << endl;
   for (i=0;i<pal->getNoEntries();i++)
   {
       cout << i << ":  <";
       cout << (*pal)[i].getRed() << " ";
       cout << (*pal)[i].getGreen() << " ";
       cout << (*pal)[i].getBlue() << ">" << endl;
   }
   delete pal;

   i = theTree.getIndex(200,200,200);
   theTree.getRep(200,200,200,r,g,b);
   cout << "<200,200,200> -> index: " << i << " <" << (int) r << " ";
   cout << (int) g << " " << (int) b << ">" << endl; 

   i = theTree.getIndex(0,0,0);
   theTree.getRep(0,0,0,r,g,b);
   cout << "<  0,  0,  0> -> index: " << i << " <" << (int) r << " ";
   cout << (int) g << " " << (int) b << ">" << endl; 

   i = theTree.getIndex(201,201,201);
   theTree.getRep(201,201,201,r,g,b);
   cout << "<201,201,201> -> index: " << i << " <" << (int) r << " ";
   cout << (int) g << " " << (int) b << ">" << endl; 

   i = theTree.getIndex(200,0,0);
   theTree.getRep(200,0,0,r,g,b);
   cout << "<200,  0,  0> -> index: " << i << " <" << (int) r << " ";
   cout << (int) g << " " << (int) b << ">" << endl; 

   i = theTree.getIndex(202,202,202);
   theTree.getRep(202,202,202,r,g,b);
   cout << "<202,202,202> -> index: " << i << " <" << (int) r << " ";
   cout << (int) g << " " << (int) b << ">" << endl; 

   i = theTree.getIndex(0,20,200);
   theTree.getRep(0,20,200,r,g,b);
   cout << "<  0, 20,200> -> index: " << i << " <" << (int) r << " ";
   cout << (int) g << " " << (int) b << ">" << endl; 

   i = theTree.getIndex(255,255,255);
   theTree.getRep(255,255,255,r,g,b);
   cout << "<255,255,255> -> index: " << i << " <" << (int) r << " ";
   cout << (int) g << " " << (int) b << ">" << endl; 

   i = theTree.getIndex(0,255,255);
   theTree.getRep(0,255,255,r,g,b);
   cout << "<  0,255,255> -> index: " << i << " <" << (int) r << " ";
   cout << (int) g << " " << (int) b << ">" << endl; 

   i = theTree.getIndex(128,128,128);
   theTree.getRep(128,128,128,r,g,b);
   cout << "<128,128,128> -> index: " << i << " <" << (int) r << " ";
   cout << (int) g << " " << (int) b << ">" << endl; 

   i = theTree.getIndex(128,0,128);
   theTree.getRep(128,0,128,r,g,b);
   cout << "<128,  0,128> -> index: " << i << " <" << (int) r << " ";
   cout << (int) g << " " << (int) b << ">" << endl; 

   i = theTree.getIndex(128,0,129);
   theTree.getRep(128,0,129,r,g,b);
   cout << "<128,  0,129> -> index: " << i << " <" << (int) r << " ";
   cout << (int) g << " " << (int) b << ">" << endl; 

}
   
