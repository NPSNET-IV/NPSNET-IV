// File: <menu.cc>

/*
 * Copyright (c) 1995,
 *    Naval Postgraduate School
 *    Computer Graphics and Video Laboratory
 *    NPSNET Research Group
 *    npsnet@cs.nps.navy.mil
 *
 *
 *    Permission to use, copy, and modify this software and its
 * documentation for any non-commercial purpose is hereby granted
 * without fee, provided that (i) the above copyright notices and the
 * following permission notices appear in ALL copies of the software
 * and related documentation, and (ii) The Naval Postgraduate School
 * Computer Graphics and Video Laboratory and the NPSNET Research Group
 * be given written credit in your software's written documentation and
 * be given graphical credit on any start-up/credit screen your 
 * software generates.
 *    This restriction helps justify our research efforts to the
 * sponsors who fund our research.
 *
 *    Do not redistribute this code without the express written
 * consent of the NPSNET Research Group. (E-mail communication and our
 * confirmation qualifies as written permission.) As stated above, this 
 * restriction helps justify our research efforts to the sponsors who
 * fund our research.
 *
 *    This software was designed and implemented at U.S. Government
 * expense and by employees of the U.S. Government.  It is illegal
 * to charge any U.S. Government agency for its partial or full use.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * E-Mail addresses:
 *    npsnet@cs.nps.navy.mil
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *    npsnet-info@cs.nps.navy.mil
 *       Contact principle investigators.
 *       Overall research project information and funding.
 *       Requests for demonstations.
 *
 * Thank you to our sponsors:  ARL, STRICOM, TRAC, ARPA and DMSO.
 */

//  Author:  Sam Kirby
//  Description:  This class is for a menu based on a cone tree structure.
//  Choices for the menu are located on the branches of the cone tree.  The
//  cone tree also contains a left arrow, a right arrow and a down arrow
//  which rotate and collapse the tree.  I use function pointers to make
//  "callbacks" for the menu.

//  Bugs:  Performer 1.2 has a bug in which you cannot pick through a pfSwitch.
//  This class needs to do just that.  So I created my own switch.  It is a
//  pfGroup which simulates a switch.  I have left the pfSwitch code which
//  should be usable in a future version of performer.

#ifndef NOC2MEASURES

#include <iostream.h>
#include "globals.h"
#include "menu.h"
#include "menu_funcs.h"

  
static pfVec4 BRANCH_COLOR = {1.0, 1.0, 1.0, 1.0};
static pfVec4 BRANCH_HL = {1.0, 0.0, 1.0, 1.0};


//  Constructor.  Builds with a callback.
MENU_LEVEL ::MENU_LEVEL(pfGroup *node, void(*function)(MENU_LEVEL*) ) {

   numChildren = 0;
   previousLevel = 0;
   cumRotation = 0.0;                        //  Rotation of tree level.

   nodeGroup = node;                         //  Root of this level.

   pfNodeName(nodeGroup,"menu_pick");

   assignCB(nodeGroup, function, this);      //  Callback of root.
 
   //  Make the control arrows and assign their callbacks.
   downGroup = makeDown();
   assignCB(downGroup,downCB, this);
   pfNodeName(downGroup,"menu_pick");
  
   rightGroup = makeRight();
   assignCB(rightGroup, rightCB, this);
   pfNodeName(rightGroup,"menu_pick");

   leftGroup = makeLeft();
   assignCB(leftGroup, leftCB, this);
   pfNodeName(leftGroup,"menu_pick");

   //  Node containing edges for the next level.
   edgesGroup = pfNewGroup();
   rotateDCS = pfNewDCS();
   //nextLevelSwitch = pfNewSwitch();     //Perf 1.2 can't pick through switch.
   nextLevelGroup = pfNewGroup();
   pfNodeName(nextLevelGroup, "simulated_switch");

   pfAddChild(rotateDCS, edgesGroup);
   
   //Known bug!!! Performer 1.2 cant't PICK through a switch!!!!
   //pfAddChild(nextLevelSwitch, nodeGroup);
   //pfAddChild(nextLevelSwitch, downGroup);
   //pfAddChild(nextLevelSwitch, rightGroup);
   //pfAddChild(nextLevelSwitch, leftGroup);
   //pfAddChild(nextLevelSwitch, rotateDCS);

   //  Build tree as performer structure.
   pfAddChild(nextLevelGroup, nodeGroup);
   pfAddChild(nextLevelGroup, downGroup);
   pfAddChild(nextLevelGroup, rightGroup);
   pfAddChild(nextLevelGroup, leftGroup);
   pfAddChild(nextLevelGroup, rotateDCS);


   //pfSwitchVal(nextLevelSwitch, PFSWITCH_OFF);   //Can't pick through switch.
   simSwitch(0);                                   //Use simulated switch for
                                                   //same functionality.

   root = pfNewDCS();
   pfNodeName(root,"menu_level_root");

   //pfAddChild(root, nextLevelSwitch);            //Can't pick through switch.
   pfAddChild(root, nextLevelGroup);

}

//  This constructor enables the user to include user data in addition to the 
//  callback.
MENU_LEVEL ::MENU_LEVEL(pfGroup *node, void(*function)(MENU_LEVEL*),   
   void* userData ) {

   numChildren = 0;
   previousLevel = 0;
   cumRotation = 0.0;

   nodeGroup = node;

   pfNodeName(nodeGroup,"menu_pick");

   assignCB(nodeGroup, function, this, userData);   //Puts user data into node.
 
   downGroup = makeDown();
   assignCB(downGroup,downCB, this);
   pfNodeName(downGroup,"menu_pick");
  
   rightGroup = makeRight();
   assignCB(rightGroup, rightCB, this);
   pfNodeName(rightGroup,"menu_pick");

   leftGroup = makeLeft();
   assignCB(leftGroup, leftCB, this);
   pfNodeName(leftGroup,"menu_pick");

   edgesGroup = pfNewGroup();
   rotateDCS = pfNewDCS();
   //nextLevelSwitch = pfNewSwitch();     //Perf 1.2 can't pick through switch.
   nextLevelGroup = pfNewGroup();
   pfNodeName(nextLevelGroup, "simulated_switch");

   pfAddChild(rotateDCS, edgesGroup);
   
   //Known bug!!! Cant't PICK through a switch!!!!
   //pfAddChild(nextLevelSwitch, nodeGroup);
   //pfAddChild(nextLevelSwitch, downGroup);
   //pfAddChild(nextLevelSwitch, rightGroup);
   //pfAddChild(nextLevelSwitch, leftGroup);
   //pfAddChild(nextLevelSwitch, rotateDCS);

   pfAddChild(nextLevelGroup, nodeGroup);
   pfAddChild(nextLevelGroup, downGroup);
   pfAddChild(nextLevelGroup, rightGroup);
   pfAddChild(nextLevelGroup, leftGroup);
   pfAddChild(nextLevelGroup, rotateDCS);


   //pfSwitchVal(nextLevelSwitch, PFSWITCH_OFF);   //Can't pick through switch.
   simSwitch(0);

   root = pfNewDCS();
   pfNodeName(root,"menu_level_root");

   //pfAddChild(root, nextLevelSwitch);            //Can't pick through switch.
   pfAddChild(root, nextLevelGroup);
}


MENU_LEVEL :: ~MENU_LEVEL(){
   int numParents = pfGetNumParents(root);
   if (pfGetParent(root, 0) != NULL){
      for (int i=numParents-1; i>=0; i--){
         pfRemoveChild(pfGetParent(root, i), root);
         pfDelete(root);
      }
   }
}

//  This member function adds the child node to the node.  It also builds
//  the resulting tree structure in performer.
void MENU_LEVEL::addMenuChild(MENU_LEVEL *child){

   float increment;
   void* arena;
    
   pfNode *edgeChild;
   pfGeode *geode, *geode1;
   pfSwitch *HLswitch;
   pfGeoSet *gset,*gset1;
   pfGeoState *gst,*gst1;
   pfVec3 *tempPoints;                                  
   static pfVec3 origin = {0.0, 0.0, 11.0};

   //  Delete the old.
   for (int i=0; i < numChildren; i++){
      edgeChild = pfGetChild(edgesGroup, 0);
      pfRemoveChild(edgesGroup, edgeChild);
      pfDelete(edgeChild);

      pfFree(points);
   }
 
   arena = pfGetSharedArena();

   numChildren++;

   if (numChildren > MAX_CHILDREN){
      cerr<<"Exceeding max number of children in addMenuChild"<<endl;
      exit(0);
   }

   pfAddChild(rotateDCS, child->root);

   //child->previousLevel = this;                     //  Backwards link to parent.

   nextLevel[(numChildren - 1)] = child;

   //child->childNumber = numChildren - 1;            //  Indentifies what child
                                                      //  number I am.

   increment = 2.0 * M_PI / numChildren;            //  Angular increment between
                                                    //  choices.


   if(NULL==(points=(pfVec3 *)pfMalloc(numChildren*sizeof(pfVec3),arena))){
         cerr<<"error in allocation: addMenuChild"<<endl;
   }

   //  Build the points which will be the tree branches.
   for (i=0; i < numChildren ; i++){

      if(NULL==(tempPoints=(pfVec3 *)pfMalloc(2*sizeof(pfVec3),arena))){
         cerr<<"error in allocation: addMenuChild"<<endl;
      } 

      if (numChildren == 1){
         points[i][X] = 0.0;
         points[i][Y] = 0.0;
         points[i][Z] = 15;
      }

      else {  
         points[i][Z] = BRANCH_Z + 11.0;
         points[i][X] = BRANCH_SHADOW * cos(i * increment);
         points[i][Y] = BRANCH_SHADOW * sin(i * increment);
      }

      //pfDCSTrans(nextLevel[i]->root, points[1][X], points[1][Y],
      //   points[1][Z]);

      pfCopyVec3(tempPoints[0], origin);
      pfCopyVec3(tempPoints[1], points[i]);

      //  Build a unhighlighted and highlighted branch.
      geode = pfNewGeode();
      geode1 = pfNewGeode();
      HLswitch = pfNewSwitch();
      gset = pfNewGSet(arena);
      gst = pfNewGState(arena);
      gset1 = pfNewGSet(arena);
      gst1 = pfNewGState(arena);

      pfGStateMode(gst, PFSTATE_ENTEXTURE, 0);
      pfGStateMode(gst, PFSTATE_TRANSPARENCY, 1);

      pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, tempPoints, NULL);
      pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
      pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
      pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, BRANCH_COLOR, NULL);

      pfGSetPrimType(gset, PFGS_LINES);
      pfGSetNumPrims(gset, 1);

      pfGStateMode(gst1, PFSTATE_ENTEXTURE, 0);
      pfGStateMode(gst1, PFSTATE_TRANSPARENCY, 1);

      pfGSetAttr(gset1, PFGS_COORD3, PFGS_PER_VERTEX, tempPoints, NULL);
      pfGSetAttr(gset1, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
      pfGSetAttr(gset1, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
      pfGSetAttr(gset1, PFGS_COLOR4, PFGS_OVERALL, BRANCH_HL, NULL);

      pfGSetPrimType(gset1, PFGS_LINES);
      pfGSetNumPrims(gset1, 1);

      pfGSetLineWidth(gset, BRANCH_THICK);
      pfGSetLineWidth(gset1, BRANCH_THICK);

      pfGSetGState(gset, gst);
      pfGSetGState(gset1, gst1);

      pfAddGSet (geode, gset);
      pfAddGSet (geode1, gset1);

      pfAddChild(HLswitch, geode);
      pfAddChild(HLswitch, geode1);

      pfSwitchVal(HLswitch, 0);

      pfAddChild(edgesGroup, HLswitch);
   }  //  End of the for loop.

}  //  End of addMenuChild.  
 
   
//  Performer 1.2 cannot use the picking mechanism through a switch.  This
//  member function simulates a switch by using a pfGroup.
void MENU_LEVEL :: simSwitch(int option){
   int numSwitchChildren;

   numSwitchChildren = pfGetNumChildren(nextLevelGroup);

   //  Remove all the children.  It does not delete them.
   for (int i=0; i < numSwitchChildren; i++){
      pfRemoveChild(nextLevelGroup, pfGetChild(nextLevelGroup,0));
   }

   //  Add back the children we want.
   switch (option) {
      case SWITCH_OFF:
          break;                              //They're all off. Keep them off.
      case SWITCH_ON:
          pfAddChild(nextLevelGroup, nodeGroup);
          pfAddChild(nextLevelGroup, downGroup);
          pfAddChild(nextLevelGroup, rightGroup);
          pfAddChild(nextLevelGroup, leftGroup);
          pfAddChild(nextLevelGroup, rotateDCS);
          break;
      case 0:
         pfAddChild(nextLevelGroup, nodeGroup);
         break;
      case 1:
         pfAddChild(nextLevelGroup, downGroup);
         break; 
      case 2: 
         pfAddChild(nextLevelGroup, rightGroup);
         break; 
      case 3:
         pfAddChild(nextLevelGroup, leftGroup);
         break;
      case 4:
         pfAddChild(nextLevelGroup, rotateDCS);
         break;
      default:
         pfAddChild(nextLevelGroup, nodeGroup);
         break;
         

   }   // Switch; 

}  //End simSwitch.

#endif
