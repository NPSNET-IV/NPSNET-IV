// file: line_build.cc

#ifndef NOC2MEASURES

#include <iostream.h>
#include "globals.h"
#include "line_build.h"
#include "terrain.h"

LINE_BUILD_CLASS::LINE_BUILD_CLASS(ushort dashed, ushort style, PO_COLOR color,
   ushort thickness, ushort closed, ushort splined, float width, ushort
   beginArrowHead, ushort endArrowHead, pfVec3 firstPoint){

   Dashed = dashed;
   Style = style;
   Color = color; 
   Thickness = thickness;
   Closed = closed;
   Splined = splined;
   Width = width;
   BeginArrowHead = beginArrowHead;
   EndArrowHead = endArrowHead;

   difference[X] = 0.0;
   difference[Y] = 0.0;

   firstTime = 1;

   movingPoint = -1;

   fileName = "conept.tst";
   numpoint = 0;                    //  Note this "numpoint" is a member.

   pfCopyVec3(points[numpoint], firstPoint);

   pointPart[numpoint] = new PO_POINT_CLASS(L_color[BLACK_M], fileName,
      firstPoint);

   numpoint++;

   linepart = new PO_LINE_CLASS(dashed, style, color, thickness, closed, splined,
      width, beginArrowHead, endArrowHead, numpoint, points); 

   polys = pfNewGroup();

   if (linepart->polys != NULL){
      pfAddChild(polys, linepart->polys);
       pfNodeName(linepart->polys, "line_part");
   }

   pfNodeName(pointPart[0]->polys, "point_part");
   pfAddChild(polys, pointPart[0]->polys); 
   
   pfNodeName(polys, "line_build");              

}  //  End of constructor.


LINE_BUILD_CLASS::~LINE_BUILD_CLASS(){

}  //  End of destructor.

void LINE_BUILD_CLASS::addPoint(pfVec3 point){

   pointPart[numpoint] = new PO_POINT_CLASS(L_color[BLACK_M], fileName,
      point);
   pfNodeName(pointPart[numpoint]->polys, "point_part");
   pfAddChild(polys, pointPart[numpoint]->polys);

   pfCopyVec3(points[numpoint], point);

   numpoint++;
 
   linepart->addPoint(point);

   if (linepart->polys != NULL){
      pfAddChild(polys, linepart->polys);
       pfNodeName(linepart->polys, "line_part");
   }

}  //  End of addPoint.


void LINE_BUILD_CLASS::movePO(float xpos, float ypos){
     
   if (firstTime){
      startpoint[X] = xpos;
      startpoint[Y] = ypos;
      startpoint[Z] = gnd_level2(startpoint);
      firstTime = 0;
   }

   if(linepart){
      linepart->movePO(xpos, ypos, 0);
   }

   difference[X] = xpos - startpoint[X];
   difference[Y] = ypos - startpoint[Y];

   for (int i = 0; i < numpoint; i++){

      pointPart[i]->movePO(points[i][X]+difference[X],
         points[i][Y]+difference[Y], 0);
   }

}  //  End of movePO.


void LINE_BUILD_CLASS::dragUpdatePO(float xpos, float ypos, pfVec3 pointArray[],
   int numArrayPts){

   if(linepart){
      linepart->dragUpdatePO(xpos, ypos, 0);
      pfNodeName(linepart->polys, "line_part");
   }

   difference[X] = xpos - startpoint[X];
   difference[Y] = ypos - startpoint[Y];

   for (int i = 0; i < numpoint; i++){
      pointPart[i]->dragUpdatePO(points[i][X]+difference[X],
         points[i][Y]+difference[Y], 0);
      points[i][X] += difference[X];
      points[i][Y] += difference[Y];
   }
   firstTime = 1;
   difference[X] = 0;
   difference[Y] = 0;

    for (i = 0; i < numArrayPts; i++){
      pfCopyVec3(pointArray[i], points[i]);
   }

}  //  End of dragUpdatePO.

void LINE_BUILD_CLASS::movePointPart(float xpos, float ypos, pfGroup* group){
//cerr<<"in movePointPart"<<endl;
   
   pfGeode* tempGeode;
   pfVec3 tempPoint;
   pfVec3* tempPoints;
   int tempNumPoint;

   tempPoint[X] = xpos;
   tempPoint[Y] = ypos;
   tempPoint[Z] = gnd_level2(tempPoint);
   
   if (firstTime){
      for (int i = 0; i < numpoint; i++){
         if (group == pointPart[i]->polys){
            movingPoint = i;
         }
      }

      if (numpoint > 1){
         if ((movingPoint == 0) || (movingPoint == numpoint - 1)){
            tempNumPoint = 2;
         }
         else{
            tempNumPoint = 3;
         }

         if (Closed) tempNumPoint = 3;

         //  Now allocate the storage.
         if(NULL==(tempPoints=(pfVec3 *)pfMalloc((tempNumPoint)*sizeof(pfVec3),
            pfGetSharedArena() )))
         {
            cerr<<"error in allocation in movePointPart"<<endl;
         }

         
         if (Closed){
            if (movingPoint == 0) {
               pfCopyVec3(tempPoints[0], points[numpoint - 1]); 
            } 
            else{
               pfCopyVec3(tempPoints[0], points[(movingPoint - 1)% numpoint]);
            }

            pfCopyVec3(tempPoints[1], tempPoint);
            pfCopyVec3(tempPoints[2], points[(movingPoint + 1)% numpoint]);
         }
         else {
            if (movingPoint == 0) {
               pfCopyVec3(tempPoints[0], tempPoint);
               pfCopyVec3(tempPoints[1], points[1]);
            }
            else{
               if (movingPoint == (numpoint - 1)){
                  pfCopyVec3(tempPoints[0], points[movingPoint - 1]);
                  pfCopyVec3(tempPoints[1], tempPoint);
               }
               else{
                  pfCopyVec3(tempPoints[0], points[movingPoint - 1]);
                  pfCopyVec3(tempPoints[1], tempPoint);
                  pfCopyVec3(tempPoints[2], points[movingPoint + 1]);
               }
            }
         }  

         tempLine = new PO_LINE_CLASS(Dashed, Style, HIGHLIGHT, Thickness,
            0, Splined, Width, BeginArrowHead, EndArrowHead,
            tempNumPoint, tempPoints, 1);
      }
      firstTime = 0;
   }

   
   if (numpoint > 1){
      //  I didn't make a mistake here, this is an assignment which will return
      //  a null if it fails.  I'm looking to assign temp to the second child
      //  of polys.  If that child doesn't exist I don't want to continue with 
      //  next portion of the conditional. I want it as = NOT == 
      if (tempGeode = (pfGeode*)pfGetChild(linepart->polys, 2) ){
         pfRemoveChild(linepart->polys, tempGeode);    //  Out with the old.
         pfDelete(tempGeode);
      }
 
      if ((movingPoint != 0) || Closed){
         tempLine->setPoint(1, tempPoint);
      }
      else{
         tempLine->setPoint(0, tempPoint);
      }

      tempGeode = tempLine->makeQuickLine();
      pfAddChild(linepart->polys, tempGeode);
   }   

   pointPart[movingPoint]->movePO(xpos, ypos, 0);
}

void LINE_BUILD_CLASS::dragUpdatePoint(float xpos, float ypos, pfGroup*,
   pfVec3 pointArray[],int numArrayPts){
//cerr<<"in dragUpdatePoint"<<endl;
//cerr<<"xpos "<<xpos<<" ypos "<<ypos<<endl;
   pfGeode* tempGeode;

   pointPart[movingPoint]->dragUpdatePO(xpos, ypos, 0);

   points[movingPoint][X] = xpos;
   points[movingPoint][Y] = ypos;
   points[movingPoint][Z] = gnd_level2(points[movingPoint]);

   if (numpoint > 1){
      //  I didn't make a mistake here, this is an assignment which will return
      //  a null if it fails.  I'm looking to assign temp to the second child
      //  of polys.  If that child doesn't exist I don't want to continue with 
      //  next portion of the conditional. I want it as = NOT == 
      if (tempGeode = (pfGeode*)pfGetChild(linepart->polys, 2) ){
         pfRemoveChild(linepart->polys, tempGeode);    //  Out with the old.
         pfDelete(tempGeode);
      }
  
      pfRemoveChild(polys, linepart->polys);
      pfDelete(linepart->polys);

      pfFree(linepart);

      linepart = new PO_LINE_CLASS(Dashed, Style, Color, Thickness, Closed,
         Splined, Width, BeginArrowHead, EndArrowHead, numpoint, points);

      if (linepart->polys != NULL){
         pfAddChild(polys, linepart->polys);
         pfNodeName(linepart->polys, "line_part");
      }
   }   
   movingPoint = -1;
   firstTime = 1;

   for (int i = 0; i < numArrayPts; i++){
      pfCopyVec3(pointArray[i], points[i]);
   }
}

#endif
