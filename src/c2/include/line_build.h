//  file:  line_build.h

#ifndef __LINE_BUILD_H
#define __LINE_BUILD_H

#include "c2_const.h"
#include "po_meas.h"

class LINE_BUILD_CLASS{

protected:
   const char* fileName;

   PO_LINE_CLASS* linepart;
   PO_POINT_CLASS* pointPart[C2_SEGMENTS];

   pfVec3  points[C2_SEGMENTS];
   int numpoint;

   int movingPoint;

   ushort Dashed,
      Style, 
      Thickness,
      Closed, 
      Splined; 
   float Width; 
   ushort BeginArrowHead, 
      EndArrowHead;

   PO_COLOR Color; 

public:
 
   pfGroup* polys;
   pfVec3 difference;
   pfVec3 startpoint;
   
   PO_LINE_CLASS* tempLine;

   int firstTime;

   LINE_BUILD_CLASS(ushort, ushort, PO_COLOR, ushort, ushort, ushort, 
      float, ushort, ushort, pfVec3);
   ~LINE_BUILD_CLASS();

   void addPoint(pfVec3);
   void movePO(float, float);
   void dragUpdatePO(float, float,pfVec3[],int);
   void movePointPart(float, float,pfGroup*);
   void dragUpdatePoint(float, float,pfGroup*, pfVec3[], int);

};  //  End of LINE_BUILD_CLASS.

#endif  
