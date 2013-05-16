//file: po_ball.cc
//author: sam kirby

#ifndef NOC2MEASURES

#include "globals.h"
#include "po_ball.h"
#include "po_ball_funcs.h"
#include "menu_funcs.h"

//  This constructor creates a ballistics box.  It consists of arrow buttons
//  as well as rays which represent the sector of fire for the chosen weapon.
BALL_BOX :: BALL_BOX(){
   void *arena;
   pfGeode* arrowGeode;

   arena = pfGetSharedArena();

   root = pfNewDCS();

   HLGroup = NULL;

   current = NO_ARROW;

   coef.ELEV_INC = 5.0;
   coef.TRAV_INC = 5.0;
 
   lowRightDCS = pfNewDCS();
   upRightDCS = pfNewDCS();
   upLeftDCS = pfNewDCS();
   lowLeftDCS = pfNewDCS();

   //  Make the arrows and assign their callbacks.
   downGroup = makeDown();
   assignBallCB(downGroup,downBallCB, this);
   pfNodeName(downGroup,"menu_pick");

   upGroup = makeUp();
   assignBallCB(upGroup,upBallCB, this);
   pfNodeName(upGroup,"menu_pick");
  
   rightGroup = makeRight();
   assignBallCB(rightGroup, rightBallCB, this);
   pfNodeName(rightGroup,"menu_pick");

   leftGroup = makeLeft();
   assignBallCB(leftGroup, leftBallCB, this);
   pfNodeName(leftGroup,"menu_pick");

   arrowGeode = pfNewGeode();
   arrowGeode = makeArrowGeo();
   pfAddChild(lowRightDCS, arrowGeode);
   assignBallCB((pfGroup*)lowRightDCS,lowRightCB, this);
   pfNodeName(lowRightDCS,"ball_pick");
   
   arrowGeode = makeArrowGeo();
   pfAddChild(upRightDCS, arrowGeode);
   assignBallCB((pfGroup*)upRightDCS,upRightCB, this);
   pfNodeName(upRightDCS,"ball_pick");
  
   arrowGeode = makeArrowGeo();
   pfAddChild(upLeftDCS, arrowGeode);
   assignBallCB((pfGroup*)upLeftDCS,upLeftCB, this);
   pfNodeName(upLeftDCS,"ball_pick");
 
   arrowGeode = makeArrowGeo();
   pfAddChild(lowLeftDCS, arrowGeode);
   assignBallCB((pfGroup*)lowLeftDCS,lowLeftCB, this);
   pfNodeName(lowLeftDCS,"ball_pick");

   pfAddChild(root, downGroup);
   pfAddChild(root, upGroup);
   pfAddChild(root, rightGroup);
   pfAddChild(root, leftGroup);
   pfAddChild(root, lowRightDCS);
   pfAddChild(root, upRightDCS);
   pfAddChild(root, upLeftDCS);
   pfAddChild(root, lowLeftDCS);

}

//  Sets the parameters for a M-198 howitzer.
M198_BOX :: M198_BOX() : BALL_BOX(){

   typeBall = M198;

   coef.MAX_ELEV = 72.0;
   coef.MAX_DEPRESS = -5.0;
   coef.MAX_TRAV = 45.0;
   coef.MUZZLE_V = 50.0;
   coef.BARREL_L = 6.096;

   coef.DRAG = -0.3;
   
   cumRotRight = 0.0;
   cumRotLeft = 45.0;
   cumRotUp = -45.0;
   cumRotLow = -5.0;

   spread = 45.0;

   coef._1G = 208.0;
   coef._2G = 236.0;
   coef._3G = 276.0;
   coef._4G = 316.0;
   coef._5G = 376.0;
   coef._3W = 297.0;
   coef._4W = 337.0;
   coef._5W = 397.0;
   coef._6W = 476.0;
   coef._7W = 572.0;
   coef._8 = 688.0;
 
}

//  Sets the parameters for a M-60 machine gun.
M60_BOX :: M60_BOX() : BALL_BOX(){

   typeBall = M60;

   coef.ELEV_INC = 1.0;

   coef.MAX_ELEV = 5.0;
   coef.MAX_DEPRESS = -5.0;
   coef.MAX_TRAV = 180.0;
   coef.MUZZLE_V = 838.0;
   coef.BARREL_L = 0.56;
   coef.DRAG = -0.8;

   
   cumRotRight = 0.0;
   cumRotLeft = 90.0;
   cumRotUp = -5.0;
   cumRotLow = 0.0;

   spread = 90.0;
}

//  Sets parameters for a 81mm mortar.
Mort81mm_BOX :: Mort81mm_BOX() : BALL_BOX(){

   typeBall = Mort81mm;

   coef.TRAV_INC = 1.0;

   coef.MAX_ELEV = 85.0;
   coef.MAX_DEPRESS = 45.0;
   coef.MAX_TRAV = 5.0;
   coef.MUZZLE_V = 250.0;
   coef.BARREL_L = 1.277;
   coef.DRAG = -0.5;
  
   cumRotRight = 0.0;
   cumRotLeft = 5.0;
   cumRotUp = -85.0;
   cumRotLow = -45.0;

   spread = 5.0;
  
}
     
#endif 
