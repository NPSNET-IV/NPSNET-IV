//  po_build.cc
//  This files contains the functions which are used to create po measures
//  from the sand table interface.  The first thing you will see are a bunch
//  of globals.  I have tried to avoid these but may have painted myself into
//  a corner.  However, the use of these globals has been contained to this
//  file.  Why I had to use them.  My tree menus go upward representing choices.
//  It really resembles a state tree and could be represented using a finite
//  state machine.  Each icon which is selected up the tree also sets a variable
//  when the tree actually collapes.  I'm not saying it can't be done without
//  globals.  If you have a better idea, have at it.

#ifndef NOC2MEASURES

#include <iostream.h>
#include "globals.h"
#include "menu.h"
#include "po_meas.h"
#include "po_build.h"
#include "menu_funcs.h"
#include "line_build.h"
#include "terrain.h"


static LINE_BUILD_CLASS* builder = NULL;

static int numpoints = 0;
static int firstPoint = 1;
static ushort addingLine = 0;

static PO_TYPE buildType;
static PO_COLOR buildColor;
static pfVec3 points[C2_SEGMENTS];
// static pfVec3 origin;
static uint direction=0;
// static uint munition;
// static uint detonator;
static ushort dashed=0;
static ushort style = SP_LSplain;
static ushort thickness = 1;
static ushort closed = 0;
static ushort splined = 0;
static float width = 1.0f;
static ushort beginArrowHead = 0;
static ushort endArrowHead = 0;
// static ushort minefieldType;
// static ushort size;
// static ushort rowWidth;
// static ushort density;

static ushort defaultThick = 3;
static PO_COLOR defaultColor = BLACK_M;


void passMenuPosition(pfVec3 point){

   if (numpoints == 0) numpoints = 1;      //  Don't want to keep reseting.
                                           //  only if it's zero do I want to
                                           //  put that first point in.  If menu
                                           //  has been displayed, the number of
                                           //  points will be 1 or greater.

   if (!addingLine){
      PFCOPY_VEC3(points[0], point);       //  points[0] always contains the
                                           //  position of the menu if we're
                                           //  not adding a line.
   }
}

void addLine(float xpos, float ypos){

   pfVec3 point;

   point[X] = xpos;
   point[Y] = ypos;
   point[Z] = gnd_level2(point);
 
   if (addingLine){

      if (!firstPoint){
         PFCOPY_VEC3(points[numpoints], point);
         builder->addPoint(point);
         numpoints++;
      }   
      else{

         numpoints = 0;               //  Get rid of the menu as the first 
                                      //  line point.  points[0] used to contain
                                      //  the position of the menu.

         PFCOPY_VEC3(points[numpoints], point);                                                  
      
         builder = new LINE_BUILD_CLASS(dashed, style, buildColor, thickness,
            closed, splined, width, beginArrowHead, endArrowHead, point);
         pfAddChild(L_po_meas, builder->polys); 

         firstPoint = 0;
         numpoints++;
      } 
   }

}  //  End of addLine.

void dragPoint(float xpos, float ypos, pfGroup* group){
//cerr<<"in drag point"<<endl;
   if (builder != NULL){
      builder->movePointPart(xpos, ypos, group);
   }
   else {
      cerr<<"something wrong dragging point in line build"<<endl;
   }
   
}

void dropPoint(float xpos, float ypos, pfGroup* group){
 //cerr<<"in drop point"<<endl; 
   if (builder != NULL){
      builder->dragUpdatePoint(xpos, ypos, group, points, numpoints);
   }
   else {
      cerr<<"something wrong dragging point in line build"<<endl;
   } 
}

void dragLine(float xpos, float ypos){

   if (builder != NULL){
      builder->movePO(xpos, ypos);
   }
   else {
      cerr<<"something wrong dragging line build"<<endl;
   }
}

void dropLine(float xpos, float ypos){
//cerr<<"in dropLine"<<endl;
   if (builder != NULL){
      builder->dragUpdatePO(xpos, ypos, points, numpoints);
   }
   else {
      cerr<<"something wrong dragging line build"<<endl;
   }
}

//*****************************************************************************
//
//
//  Callbacks

void buildItCB(MENU_LEVEL* level){

PO_MEASURES_CLASS* measure;

   switch (buildType){

      case PO_POINT:
         measure = new PO_POINT_CLASS(style, buildColor, dashed, direction, 0,
            points[0]);
         numpoints = 0;
         break;
      case PO_LINE:
         if (numpoints > 1){
            measure = new PO_LINE_CLASS(dashed, style, buildColor, thickness, 
               closed, splined, width, beginArrowHead, endArrowHead, numpoints,
               points); 
         }
         else{
            measure = NULL;
         }

         numpoints = 0;
         addingLine = 0;
         firstPoint = 1;

         if (builder){
            pfRemoveChild(L_po_meas, builder->polys);
            pfDelete(builder->polys);
            pfFree(builder);
            builder = NULL;
         }
         break;

      default:
         cerr<<"error in buildItCB "<<endl;
         break; 
   }  

   if (measure){
      poArray[poCount].key.v.okey.space = 0;
      poArray[poCount].key.v.okey.obj_id.simulator.site = poSite;
      poArray[poCount].key.v.okey.obj_id.simulator.host = poHost;
      poArray[poCount].key.v.okey.obj_id.object = object;
      poArray[poCount].poPointer = measure;

      pfAddChild(L_po_meas, poArray[poCount].poPointer->polys);
      poCount++; 

      measure->updatePDU();
      measure->sendPDU();
   }

   //menuOff(L_menu_root);
   menuOff(level);

}


//  This function is the standard callback when an icon is selected on the
//  menu.  It highlights that icon and its branch and also shuts of 
//  previously selected icons.  It makes some assumptions on the way it
//  handles some of the switches.  It assumes the icon is has the same
//  performer group representation as a PO_POINT.  Note, an icon doesn't
//  have to be this type, it can be any performer group.  But to get the
//  highlighting right you'd have to change the below code.
void standardCB(MENU_LEVEL *node){
   pfSwitch *HLSwitch;
   pfSwitch *priorEdge;

   HLSwitch = (pfSwitch *)pfGetChild(node->nodeGroup, 0);
   pfSwitchVal(HLSwitch, 1);

   node->simSwitch(SWITCH_ON);
   //pfSwitchVal(node->nextLevelSwitch, PFSWITCH_ON);  //Can't pick through switch.

   pfSwitchVal(HLSwitch, 1);

   if (node->previousLevel != NULL){
      
      priorEdge = (pfSwitch *)pfGetChild(node->previousLevel->edgesGroup,
        node->childNumber);
      pfSwitchVal(priorEdge, 1);

      //  Turn off all the other siblings.
      for(int i=0; i<node->previousLevel->numChildren; i++){
         if (node->previousLevel->nextLevel[i] != node){
            HLSwitch = (pfSwitch *)pfGetChild
               (node->previousLevel->nextLevel[i]->nodeGroup, 0);
            pfSwitchVal(HLSwitch, 0);

            node->previousLevel->nextLevel[i]->simSwitch(0);

            priorEdge = (pfSwitch *)pfGetChild
            (node->previousLevel->edgesGroup, i);
            pfSwitchVal(priorEdge, 0);

         }  
      }
   }  //  End of if (node->previousLevel != NULL)

   for (int i = 0; i< node->numChildren; i++){
      node->nextLevel[i]->simSwitch(0);
      node->nextLevel[i]->previousLevel = node;
      node->nextLevel[i]->childNumber = i;

      pfDCSTrans(node->nextLevel[i]->root, node->points[i][X],
         node->points[i][Y], node->points[i][Z]);
   }
}  //  End of standardCB.

void rootCB(MENU_LEVEL *root){
   pfSwitch *HLSwitch;
   HLSwitch = (pfSwitch *)pfGetChild(root->nodeGroup, 0);

   root->simSwitch(SWITCH_ON);
   //pfSwitchVal(root->nextLevelSwitch, PFSWITCH_ON);
   pfSwitchVal(HLSwitch, 1);

}

void pointCB(MENU_LEVEL *level){
   standardCB(level);
   buildType = PO_POINT;
   style = SP_PSgeneral;
}

void genPointCB(MENU_LEVEL *level){
   standardCB(level);
   style = SP_PSgeneral;
}

void coordPointCB(MENU_LEVEL *level){
   standardCB(level);
   style = SP_PScoordinating;
}

void contactPointCB(MENU_LEVEL *level){
   standardCB(level);
   style = SP_PScontact;
}

void lineCB(MENU_LEVEL *level){
   standardCB(level);
   buildType = PO_LINE;
   closed = 0;
   addingLine = 1;
   style = SP_LSplain;
   thickness = defaultThick;
   buildColor = defaultColor;
}

void closed_lineCB(MENU_LEVEL* level){
   standardCB(level);
   buildType = PO_LINE;
   closed = 1;
   addingLine = 1;
   style = SP_LSplain;
   thickness = defaultThick;
   buildColor = defaultColor;  
}

void blackCB(MENU_LEVEL* level){
   standardCB(level);
   buildColor = BLACK_M;
}

void yellowCB(MENU_LEVEL* level){
   standardCB(level);
   buildColor = YELLOW_M;
}

void redCB(MENU_LEVEL* level){
   standardCB(level);
   buildColor = RED_M;
}

void greenCB(MENU_LEVEL* level){
   standardCB(level);
   buildColor = GREEN_M;
}

void blueCB(MENU_LEVEL* level){
   standardCB(level);
   buildColor = BLUE_M;
}

void thick1CB(MENU_LEVEL* level){
   standardCB(level);
   thickness = 1;
}

void thick2CB(MENU_LEVEL* level){
   standardCB(level);
   thickness = 2;
}

void thick3CB(MENU_LEVEL* level){
   standardCB(level);
   thickness = 3;
}

void thick4CB(MENU_LEVEL* level){
   standardCB(level);
   thickness = 4;
}

void thick5CB(MENU_LEVEL* level){
   standardCB(level);
   thickness = 5;
}

void thick6CB(MENU_LEVEL* level){
   standardCB(level);
   thickness = 6;
}

#endif
