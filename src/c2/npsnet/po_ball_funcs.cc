//file: po_ball_funcs.cc
//author: sam kirby
//  This file contains the functions which build the visualizations of the
//  ballistics.  The functions are called from the main menu which then
//  constructs a menu with rays indicating sectors of fire.  This "ballistics
//  box" is then used to determine the visualiztion of the weaons firing.

#ifndef NOC2MEASURES

#include <iostream.h>
#include "globals.h"
#include "common_defs.h"
#include "po_ball_funcs.h"
#include "menu_funcs.h"
#include "po_funcs.h"
#include "highlight.h"
#include "ball_equa.h"
#include "po_build.h"

// Globals used by this library
extern           pfGroup          *G_measures;
extern volatile  DYNAMIC_DATA     *G_dynamic_data;


static pfGroup *L_po_ball;           // The root of the ballistics tree.
static BALL_BOX *L_ballBox;          // Current ballistics box.   

//  Initialize the ballistics box to the chosen type.
void initBallStuff(ballType ball){      
   switch (ball){
      case M198:
         L_ballBox = new M198_BOX();
         break;
      case M60:
         L_ballBox = new M60_BOX();
         break;
      case Mort81mm:
         L_ballBox = new Mort81mm_BOX();
         break;
   }

   //  Set the DCS's to the correct rotations.
   pfDCSRot(L_ballBox->lowRightDCS, L_ballBox->cumRotRight, 0.0,
      L_ballBox->cumRotLow);
   pfDCSRot(L_ballBox->lowLeftDCS, L_ballBox->cumRotLeft, 0.0,
      L_ballBox->cumRotLow);
   pfDCSRot(L_ballBox->upRightDCS, L_ballBox->cumRotRight, 0.0,
      L_ballBox->cumRotUp);
   pfDCSRot(L_ballBox->upLeftDCS, L_ballBox->cumRotLeft, 0.0,
      L_ballBox->cumRotUp);
   
   initBallMenu(L_ballBox);

}  //  End of initBallStuff.

//  Turn the ballistics box menu on.
void ballOn(pfVec3 position, ballType ball){
   static int firstTime = 1;

   initBallStuff(ball);

   if (firstTime){ 
      firstTime = 0;
      L_po_ball = pfNewGroup();
      pfAddChild(G_measures,L_po_ball);
   }
   
   //  Put the ball box in the correct place and add to tree.
   if (L_po_ball != NULL){
      pfDCSTrans(L_ballBox->root, position[X], position[Y], position[Z]);
      pfAddChild(L_po_ball, L_ballBox->root);  
   }

}


//Turn the ballistics box off.  I have a memory leak here because I don't
//delete the old box, needs fixing.
void ballOff(){
    pfRemoveChild(L_po_ball, L_ballBox->root);
//    pfDelete(L_ballBox->root);     //Problem here.  I really need to get
                                     //rid of the old menu but it crahes when
                                     //I do this.  It's just a deletion prob.
}


//  This assigns a callback to a group.  The callback can then be used
//  to manipulate the box.  Note this is very similar to the general menu
//  callback, except the function used uses a BALL_BOX instead of a MENU_LEVEL.
//  The callback function is again passed by a pointer to the function.
void assignBallCB(pfGroup * group, void(*function)(BALL_BOX*),BALL_BOX* box){
   ballCB * CB;

   if(NULL == (CB = (ballCB *) pfMalloc(sizeof(ballCB),
     pfGetSharedArena() ))){ 
   }

   CB->function = function;            //  Assign the function.
   CB->ballBox = box;                  //  Assign the box.

   pfUserData(group, CB);
}

//  This function makes the large arrows which show the rays for the sectors
//  of fire.
pfGeode* makeArrowGeo(){
   static pfVec4 color = {1.0, 1.0, 1.0, 1.0};
   return ( makePoGeoTri(color, "arrow.tst") );
}

//  This function build the graphical representation of the trajectory pathes
//  of the desired weapon.  It uses a Runge Kutta function called "simple" to
//  get the actual points and then builds and returns a geode.
pfGeode* buildTrajPaths(float worldX, float worldY, float left,
   float right, float upper, float lower, BALL_BOX* box){

   pfGeode* returnGeo;
   pfGeoState  *gstate;
   pfGeoSet   *gset;

   pfVec3 *pointVerts;
  
   static pfVec4 color[] = {{1.0, 0.0, 0.0, 0.2},        
                           {0.7, 0.0, 0.3, 0.2},
                           {0.5, 0.0, 0.5, 0.2},
                           {0.3, 0.0, 0.7, 0.2},
                           {0.0, 0.0, 0.0, 0.2},
                           {1.0, 1.0, 1.0, 1.0}};
  
   int colorIndex = 0, numColors = 6;

   int *length_vec;

   returnGeo = pfNewGeode();
 
   left = (left < right) ? left + 360.0 : left;
   
   //  Cycle through all the represented elevations and headings.
   for (float elev = lower; elev <= upper; elev += box->coef.ELEV_INC){
      for (float heading = right; heading <= left;
         heading += box->coef.TRAV_INC){

         //  Get the points describing the trajectory.
         pointVerts = simple(worldX, worldY, heading, elev, box);
    
         //  Build the graphical representation.
         gstate = pfNewGState (pfGetSharedArena());
         pfGStateMode (gstate, PFSTATE_ENTEXTURE, 0);
         pfGStateMode(gstate, PFSTATE_TRANSPARENCY, 1);

         gset = pfNewGSet(pfGetSharedArena());
         pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts, NULL);
         pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
         pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, color[colorIndex], NULL);
         pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
  
         pfGSetPrimType(gset, PFGS_FLAT_LINESTRIPS);

         pfGSetLineWidth(gset, 1.0);

         pfGSetNumPrims(gset, 1);

         if(NULL==(length_vec = (int *) pfMalloc(sizeof(int),
            pfGetSharedArena()))){
            cerr<<"error in allocation buildTrajPaths"<<endl;
         }

         length_vec[0] = NUM_TRAJ_POINTS;

         pfGSetPrimLengths(gset, length_vec);
  
         pfGSetGState (gset, gstate);

         pfAddGSet(returnGeo,gset);

      }  //  End of first for.
     //  Cycle through colors, helps to see the trajectories.
     colorIndex = (colorIndex + 1) % numColors;
   }  //  End of second for.
  return(returnGeo);
}  //  End of buildTrajPaths.


//  This function build the graphical representation of the trajectory pathes
//  of the desired weapon.  It uses a Runge Kutta function called "simple" to
//  get the actual points and then builds and returns a geode.  It is the
//  same as buildTrajPaths except rather than returning lines representing 
//  the trajectories it returns a "fan" representing the flight pathes of
//  the weapons.
pfGeode* buildTrajFan(float worldX, float worldY, float left,
   float right, float upper, float lower, BALL_BOX* box){

   pfGeode* returnGeo;
   pfGeoState  *gstate, *gstate1;
   pfGeoSet   *gset, *gset1, *gset2;
   int *length_vec;

   pfVec3 *pointVerts, *pointVerts1, *pointVerts2;
 
   static pfVec4 color[] = {{1.0, 0.0, 0.0, 0.2},
                           {0.9, 0.0, 0.1, 0.2},
                           {0.8, 0.0, 0.2, 0.2},
                           {0.7, 0.0, 0.3, 0.2},
                           {0.5, 0.0, 0.4, 0.2},
                           {0.4, 0.0, 0.5, 0.2},
                           {0.0, 0.0, 0.0, 1.0}};

   int colorIndex = 0, numColors = 6, BALL_BLACK = 6;

   returnGeo = pfNewGeode();
   
   left = (left < right) ? left + 360.0 : left;
   
   //  Cycle through all elevations and build fans.
   for (float elev = lower; elev <= upper; elev += box->coef.ELEV_INC){
 
     
      if(NULL==(pointVerts=
         (pfVec3 *)pfMalloc(2 * NUM_TRAJ_POINTS *sizeof(pfVec3), 
          pfGetSharedArena() ))){
             cerr<<"error in allocation: function simple"<<endl;
             return(NULL);       
      }
     
      pointVerts1 = simple(worldX, worldY, left, elev, box);
      pointVerts2 = simple(worldX, worldY, right, elev, box);

      for (int i = 0; i < NUM_TRAJ_POINTS; i++){
         pointVerts[2 * i][X] = pointVerts1[i][X];
         pointVerts[2 * i][Y] = pointVerts1[i][Y];
         pointVerts[2 * i][Z] = pointVerts1[i][Z];

         pointVerts[2 * i + 1][X] = pointVerts2[i][X];
         pointVerts[2 * i + 1][Y] = pointVerts2[i][Y];
         pointVerts[2 * i + 1][Z] = pointVerts2[i][Z];
      }

      //  Build the fans for the geode.
      gstate = pfNewGState (pfGetSharedArena());
      pfGStateMode (gstate, PFSTATE_ENTEXTURE, 0);
      pfGStateMode (gstate, PFSTATE_TRANSPARENCY,
         PFTR_HIGH_QUALITY | PFTR_NO_OCCLUDE);
      pfGStateMode (gstate, PFSTATE_ALPHAFUNC, PFAF_ALWAYS);
     
      gset = pfNewGSet(pfGetSharedArena());
      pfGSetGState (gset, gstate);
      pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts, NULL);
      pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
      pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, color[colorIndex], NULL);
      pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
  
      pfGSetPrimType(gset, PFGS_FLAT_TRISTRIPS);

      pfGSetLineWidth(gset, 1.0);

      pfGSetNumPrims(gset, 1);

      if(NULL==(length_vec = (int *) pfMalloc(sizeof(int),
         pfGetSharedArena()))){
         cerr<<"error in allocation buildTrajPaths"<<endl;
      }

      length_vec[0] = 2 * NUM_TRAJ_POINTS;

      pfGSetPrimLengths(gset, length_vec);
  
      //  Make black border lines on the fans.
      gstate1 = pfNewGState (pfGetSharedArena());
      pfGStateMode (gstate1, PFSTATE_ENTEXTURE, 0);
      pfGStateMode (gstate1, PFSTATE_TRANSPARENCY, PFTR_HIGH_QUALITY);
      pfGStateMode (gstate1, PFSTATE_ALPHAFUNC, PFAF_ALWAYS);
     
      gset1 = pfNewGSet(pfGetSharedArena());
      pfGSetGState (gset1, gstate1);
      pfGSetAttr(gset1, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts1, NULL);
      pfGSetAttr(gset1, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
      pfGSetAttr(gset1, PFGS_COLOR4, PFGS_OVERALL, color[BALL_BLACK], NULL);
      pfGSetAttr(gset1, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
  
      pfGSetPrimType(gset1, PFGS_FLAT_LINESTRIPS);

      pfGSetLineWidth(gset1, 1.0);

      pfGSetNumPrims(gset1, 1);

      if(NULL==(length_vec = (int *) pfMalloc(sizeof(int),
         pfGetSharedArena()))){
         cerr<<"error in allocation buildTrajPaths"<<endl;
      }

      length_vec[0] = NUM_TRAJ_POINTS;

      pfGSetPrimLengths(gset1, length_vec);

      gset2 = pfNewGSet(pfGetSharedArena());
      pfGSetGState (gset2, gstate1);
      pfGSetAttr(gset2, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts2, NULL);
      pfGSetAttr(gset2, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
      pfGSetAttr(gset2, PFGS_COLOR4, PFGS_OVERALL, color[BALL_BLACK], NULL);
      pfGSetAttr(gset2, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
  
      pfGSetPrimType(gset2, PFGS_FLAT_LINESTRIPS);

      pfGSetLineWidth(gset2, 1.0);

      pfGSetNumPrims(gset2, 1);

      if(NULL==(length_vec = (int *) pfMalloc(sizeof(int),
         pfGetSharedArena()))){
         cerr<<"error in allocation buildTrajPaths"<<endl;
      }

      length_vec[0] = NUM_TRAJ_POINTS;

      pfGSetPrimLengths(gset2, length_vec);



      pfAddGSet(returnGeo,gset);
      pfAddGSet(returnGeo,gset1);
      pfAddGSet(returnGeo,gset2);
      colorIndex = (colorIndex + 1) % numColors;
   }
   return(returnGeo);
}  //  End of buildTrajFan.


//  This function builds the menu used by the ballistics box.
void initBallMenu(BALL_BOX* box){
   pfGroup* rootGroup;
   pfGroup* tempGroup;

   MENU_LEVEL* ballMenu;
   MENU_LEVEL* fan;
   MENU_LEVEL* linefan;
   MENU_LEVEL* _1G;
   MENU_LEVEL* _3G;
   MENU_LEVEL* _5W;
   MENU_LEVEL* _8;

   pfVec3  origin = { 0.0, 0.0, 0.0};

   switch (box->typeBall){
      case M198:
         rootGroup = makePickableLnStrip(L_color[WHITE_M],
            "howitzerm.lst", origin);
         break;
      case M60:
         rootGroup = makePickableLnStrip(L_color[WHITE_M],
            "mgunm.lst", origin);
         break;
      case Mort81mm:
         rootGroup = makePickableLnStrip(L_color[WHITE_M],
            "mortarm.lst", origin);
         break;
   }

   //  Make the root of the menu.
   ballMenu = new MENU_LEVEL(rootGroup, rootCB, box);

   //  Allow the levels to be drawn.
   ballMenu->simSwitch(SWITCH_ON);

   //  I want the down arrow to act differently than a normal menu.
   //  I don't just want it to collapse, I want it to shut the 
   //  ballistics box off.  This may create a memory leak.
   assignCB(ballMenu->downGroup, ballDownCB, ballMenu);

   //  Depending on which ballistics we're using build a different menu.
   switch (box->typeBall){
      case M60: case Mort81mm:
         tempGroup = makePickableLnStrip(L_color[BLACK_M],
            "fan.lst", origin);
         fan = new MENU_LEVEL(tempGroup, fanCB, box);
         ballMenu->addMenuChild(fan);

         tempGroup = makePickableLnStrip(L_color[BLACK_M],
            "linefan.lst", origin); 
         linefan = new MENU_LEVEL(tempGroup, linefanCB, box);
         ballMenu->addMenuChild(linefan);
         break;

      case M198:
         tempGroup = makePickableLnStrip(L_color[BLACK_M],
            "fan.lst", origin); 
         fan = new MENU_LEVEL(tempGroup, fanCB, box);

         tempGroup = makePickableLnStrip(L_color[BLACK_M],
            "linefan.lst", origin);
         linefan = new MENU_LEVEL(tempGroup, linefanCB, box);

         tempGroup = makePickableLnStrip(L_color[BLACK_M],
            "_1G.lst", origin);
         _1G = new MENU_LEVEL(tempGroup, _1GCB, box);
         ballMenu->addMenuChild(_1G);
         _1G->addMenuChild(fan);
         _1G->addMenuChild(linefan);

         tempGroup = makePickableLnStrip(L_color[BLACK_M],
            "_3G.lst", origin);
         _3G = new MENU_LEVEL(tempGroup, _3GCB, box);
         ballMenu->addMenuChild(_3G);
         _3G->addMenuChild(fan);
         _3G->addMenuChild(linefan);

         tempGroup = makePickableLnStrip(L_color[BLACK_M],
            "_5W.lst", origin); 
         _5W = new MENU_LEVEL(tempGroup, _5WCB, box);
         ballMenu->addMenuChild(_5W);
         _5W->addMenuChild(fan);
         _5W->addMenuChild(linefan);

         tempGroup = makePickableLnStrip(L_color[BLACK_M],
            "_8.lst", origin);
         _8 = new MENU_LEVEL(tempGroup, _8CB, box);
         ballMenu->addMenuChild(_8);
         _8->addMenuChild(fan);
         _8->addMenuChild(linefan);
 
         break;        
   }

   
   //  The root is special.  We have to place its children.  For higher
   //  levels this is done when an item is chosen in standardCB.
   for (int i = 0; i< ballMenu->numChildren; i++){

      ballMenu->nextLevel[i]->previousLevel = ballMenu;
      ballMenu->nextLevel[i]->childNumber = i;

      pfDCSTrans(ballMenu->nextLevel[i]->root,
        ballMenu->points[i][X],
        ballMenu->points[i][Y],
        ballMenu->points[i][Z]);
   }

   pfDCSTrans(ballMenu->root, 0.0, 0.0, 15.0);
   pfAddChild(box->root, ballMenu->root);  

}  //  End of initBallMenu. 

//Callbacks.  These are the callbacks used by the ballistics menu.
//***************************************************************************

//This function moves the rays.  Clicking the up arrows moves the rays up
//within there limits.  They are constarined by the weapons actual limits
//and by the other rays.
//The pitch angles are negative of what you'd think.
void upBallCB(BALL_BOX* box){
   switch (box->current){
      case NONE:
         break;
      case LR: case LL: 
         box->cumRotLow -= INCREMENT;
         if (box->cumRotLow < -box->coef.MAX_ELEV) box->cumRotLow =
            -box->coef.MAX_ELEV;
         pfDCSRot(box->lowRightDCS, box->cumRotRight, 0.0, box->cumRotLow);       
         pfDCSRot(box->lowLeftDCS, box->cumRotLeft, 0.0, box->cumRotLow);

         if (box->cumRotLow < box->cumRotUp){
            box->cumRotUp = box->cumRotLow;
            pfDCSRot(box->upRightDCS, box->cumRotRight, 0.0, box->cumRotUp);
            pfDCSRot(box->upLeftDCS, box->cumRotLeft, 0.0, box->cumRotUp);
         }
       
         break;
      case UR: case UL:
         box->cumRotUp -= INCREMENT;
         if (box->cumRotUp < -box->coef.MAX_ELEV) box->cumRotUp =
            -box->coef.MAX_ELEV;
         pfDCSRot(box->upRightDCS, box->cumRotRight, 0.0, box->cumRotUp);
         pfDCSRot(box->upLeftDCS, box->cumRotLeft, 0.0, box->cumRotUp);
         break;
      default:
         break;
   }
  
}  //  End of upBallCB.


//  Same as upBallCB, opposite direction.
void downBallCB(BALL_BOX* box){

   switch (box->current){
      case NONE:
         break;
      case LR: case LL: 
         box->cumRotLow += INCREMENT;
         if (box->cumRotLow > -box->coef.MAX_DEPRESS) box->cumRotLow =
            -box->coef.MAX_DEPRESS;
         pfDCSRot(box->lowRightDCS, box->cumRotRight, 0.0, box->cumRotLow);
         pfDCSRot(box->lowLeftDCS, box->cumRotLeft, 0.0, box->cumRotLow);
         break;
      case UR: case UL:
         box->cumRotUp += INCREMENT;
         if (box->cumRotUp > -box->coef.MAX_DEPRESS) box->cumRotUp =
            -box->coef.MAX_DEPRESS;
         pfDCSRot(box->upRightDCS, box->cumRotRight, 0.0, box->cumRotUp);
         pfDCSRot(box->upLeftDCS, box->cumRotLeft, 0.0, box->cumRotUp);
 
         if (box->cumRotUp > box->cumRotLow){
            box->cumRotLow = box->cumRotUp;
            pfDCSRot(box->lowRightDCS, box->cumRotRight, 0.0, box->cumRotLow);
            pfDCSRot(box->lowLeftDCS, box->cumRotLeft, 0.0, box->cumRotLow);
         }
         break;
      default:
         break;

   }

}  //  End of downBallCB.


//  Same a upBallCB and downBallCB except used for traverse.
void rightBallCB(BALL_BOX* box){
   switch (box->current){
      case NONE:
         break;
      case LR: case UR:
         box->spread -= INCREMENT;
         box->cumRotRight += INCREMENT;
         if (box->cumRotRight > 360.0) box->cumRotRight -= 360.0;
         pfDCSRot(box->lowRightDCS, box->cumRotRight, 0.0, box->cumRotLow);
         pfDCSRot(box->upRightDCS, box->cumRotRight, 0.0, box->cumRotUp);
        
         if (box->spread < 0.0){
            box->spread += INCREMENT;
            box->cumRotLeft += INCREMENT;
            if (box->cumRotLeft > 360.0) box->cumRotLeft -= 360.0;
            pfDCSRot(box->lowLeftDCS, box->cumRotLeft,0.0, box->cumRotLow);
            pfDCSRot(box->upLeftDCS, box->cumRotLeft, 0.0, box->cumRotUp);
         }
         break;
      case LL: case UL:
         box->spread += INCREMENT;
         box->cumRotLeft += INCREMENT;
         if (box->cumRotLeft > 360.0) box->cumRotLeft -= 360.0;
         pfDCSRot(box->lowLeftDCS, box->cumRotLeft,0.0, box->cumRotLow);
         pfDCSRot(box->upLeftDCS, box->cumRotLeft, 0.0, box->cumRotUp);
        
         if (box->spread > box->coef.MAX_TRAV){
            box->spread -= INCREMENT;
            box->cumRotRight += INCREMENT;
            if (box->cumRotRight > 360.0) box->cumRotRight -= 360.0;
            pfDCSRot(box->lowRightDCS, box->cumRotRight,0.0, box->cumRotLow);
            pfDCSRot(box->upRightDCS, box->cumRotRight, 0.0, box->cumRotUp);
         }
         break;
      default:
         break;
   }
     
}  //  End of rightBallCB.


//  Same as rightBallCB, different direction.
void leftBallCB(BALL_BOX* box){
   switch (box->current){
      case NONE:
         break;
      case LR:case UR: 
         box->spread += INCREMENT;
         box->cumRotRight -= INCREMENT;
         if (box->cumRotRight < 0.0) box->cumRotRight += 360.0;
         pfDCSRot(box->lowRightDCS, box->cumRotRight, 0.0, box->cumRotLow);
         pfDCSRot(box->upRightDCS, box->cumRotRight, 0.0, box->cumRotUp);
         if (box->spread > box->coef.MAX_TRAV){
            box->spread -= INCREMENT;
            box->cumRotLeft -= INCREMENT;
            if (box->cumRotLeft < 0.0) box->cumRotLeft += 360.0;
            pfDCSRot(box->lowLeftDCS, box->cumRotLeft,0.0, box->cumRotLow);
            pfDCSRot(box->upLeftDCS, box->cumRotLeft, 0.0, box->cumRotUp);
         }
         break;
      case LL:case UL:
         box->spread -= INCREMENT;
         box->cumRotLeft -= INCREMENT;
         if (box->cumRotLeft < 0.0) box->cumRotLeft += 360.0;
         pfDCSRot(box->lowLeftDCS, box->cumRotLeft, 0.0, box->cumRotLow);
         pfDCSRot(box->upLeftDCS, box->cumRotLeft, 0.0, box->cumRotUp);

         if (box->spread < 0.0){
            box->spread += INCREMENT;
            box->cumRotRight -= INCREMENT;
            if (box->cumRotRight < 0.0) box->cumRotRight += 360.0;
            pfDCSRot(box->lowRightDCS, box->cumRotRight,0.0, box->cumRotLow);
            pfDCSRot(box->upRightDCS, box->cumRotRight, 0.0, box->cumRotUp);
         }
         break;
      default:
         break;
   }
  
}  //  End of leftBallCB.

// Picks the lower right ray in the ballistics box.
void lowRightCB(BALL_BOX* box){
   box->current = LR;
   HighlightNode(G_dynamic_data->HL, (pfNode*)box->lowRightDCS);
}  //  End of lowRightCB.

// Picks the upper right ray in the ballistics box.
void upRightCB(BALL_BOX* box){
   box->current = UR;
   HighlightNode(G_dynamic_data->HL,(pfNode*) box->upRightDCS);
}  //  End of upRightCB.

// Picks the upper left ray in the ballistics box.
void upLeftCB(BALL_BOX* box){
   box->current = UL;
   HighlightNode(G_dynamic_data->HL,(pfNode*) box->upLeftDCS);
}  //  End of upLeftCB.

// Picks the lower left ray in the ballistics box.
void lowLeftCB(BALL_BOX* box){
   box->current = LL;
   HighlightNode(G_dynamic_data->HL, (pfNode*)box->lowLeftDCS);
}  //  End of lowLeftCB.

//  I want this to shut off the ball box display.
void ballDownCB(MENU_LEVEL*){
   ballOff();
}

//  This is a callback from the ballistics menu.  It will generate the 
//  visulization of the weapon as a fan rather than as lines.
void fanCB(MENU_LEVEL* level){

   pfGeode* geode;
   pfVec3 world;
   pfMatrix tempMat;
   callBack *CB;
   BALL_BOX* box;
 
   CB = (callBack*)pfGetUserData(level->nodeGroup);

   box = (BALL_BOX*)(CB->userData);

   pfGetDCSMat(box->root, tempMat);

   pfGetMatRowVec3(tempMat, 3, world);

   geode = buildTrajFan(world[X], world[Y], box->cumRotLeft,
      box->cumRotRight, -(box->cumRotUp), -(box->cumRotLow), box);

   pfAddChild(L_po_ball, geode);
   ballOff();                                //  Turn the rays and menu off.

}  //  End of fanCB.


//  This is a callback from the ballistics menu.  It will generate the 
//  visulization of the weapon as lines.
void linefanCB(MENU_LEVEL* level){

   pfGeode* geode;
   pfVec3 world;
   pfMatrix tempMat;
   callBack *CB;
   BALL_BOX* box;
 
   CB = (callBack*)pfGetUserData(level->nodeGroup);

   box = (BALL_BOX*)(CB->userData);
 
   pfGetDCSMat(box->root, tempMat);

   pfGetMatRowVec3(tempMat, 3, world);

   geode = buildTrajPaths(world[X], world[Y], box->cumRotLeft,
      box->cumRotRight, -(box->cumRotUp), -(box->cumRotLow), box);

  
   pfAddChild(L_po_ball, geode);
   ballOff();

}  //  End of linefanCB.

//  This function is used exclusively with the M-198 visualizer.  Each of 
//  these callbacks is a different charge for the round.  The function sets
//  needed parameters. 
void _1GCB(MENU_LEVEL* level){
   callBack *CB;
   BALL_BOX* box;
  
   CB = (callBack*)pfGetUserData(level->nodeGroup);

   box = (M198_BOX*)(CB->userData);

   box->coef.MUZZLE_V = box->coef._1G;

   standardCB(level);
}  //  End of _1GCB.


//  This function is used exclusively with the M-198 visualizer.  Each of 
//  these callbacks is a different charge for the round.  The function sets
//  needed parameters. 
void _3GCB(MENU_LEVEL* level){
   callBack *CB;
   BALL_BOX* box;
  
   CB = (callBack*)pfGetUserData(level->nodeGroup);

   box = (M198_BOX*)(CB->userData);

   box->coef.MUZZLE_V = box->coef._3G;

   standardCB(level);
}  //  End of _3GCB.

//  This function is used exclusively with the M-198 visualizer.  Each of 
//  these callbacks is a different charge for the round.  The function sets
//  needed parameters. 
void _5WCB(MENU_LEVEL* level){
   callBack *CB;
   BALL_BOX* box;
  
   CB = (callBack*)pfGetUserData(level->nodeGroup);

   box = (M198_BOX*)(CB->userData);

   box->coef.MUZZLE_V = box->coef._5W;

   standardCB(level);
}  //  End of _5WCB.

//  This function is used exclusively with the M-198 visualizer.  Each of 
//  these callbacks is a different charge for the round.  The function sets
//  needed parameters. 
void _8CB(MENU_LEVEL* level){
   callBack *CB;
   BALL_BOX* box;
  
   CB = (callBack*)pfGetUserData(level->nodeGroup);

   box = (M198_BOX*)(CB->userData);

   box->coef.MUZZLE_V = box->coef._8;

   standardCB(level);
}  //End of _8CB.


void rootBallCB(MENU_LEVEL*){
}

#endif
