//  File: <picking.cc>
//  Modifications:  Sam Kirby Jul 1995.  
//  This file is a modification of the performer pickfly functions.  I currently
//  have 3 differnet types of picks, one for menu items one for pomeasures and
//  another to use the ballistics menus.  These should be consolidated in a
//  general function which does them all but they work now, albeit they are
//  specialized to do their unique thing.

#ifndef NOC2MEASURES

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <iostream.h>
#include <Performer/pf.h>
#include "globals.h"
#include "common_defs.h"
#include "pfutil.h"
#include "picking.h"
#include "highlight.h"
#include "menu_funcs.h"
#include "po_ball_funcs.h"
#include "po_meas.h"
#include "po_funcs.h"
#include "po_build.h"
#include "terrain.h"
#include "po_interface.h"

// Globals used by this library
extern volatile  DYNAMIC_DATA     *G_dynamic_data;
extern           pfGroup          *G_measures;
extern           pfGroup          *G_fixed;


Pick *NewPick(pfScene *S, pfChannel **C)
{
   Pick *P = (Pick *)pfMalloc(sizeof(Pick), pfGetSharedArena());
   P->chan = C;
   P->scene = S;
   P->group = NULL;
   P->pathname = (char *)pfMalloc(sizeof(char) *300, pfGetSharedArena());
   P->pathname[0] = '\0';
   P->traverse = PFTRAV_IS_PRIM | PFPK_M_NEAREST | PFTRAV_IS_PATH;
   
   return P;
}

float GetPickedX(Pick *P)
{
   return P->pickX;
}

float GetPickedY(Pick *P)
{
   return P->pickY;
}

pfNode *GetPickNode(Pick *P)
{
   return P->picked;
}

void PickTraverse(Pick *P, long token)
{
   P->traverse = token;
}


long MakePathString(char *pname, pfPath *P)
{
   int i, n;
   char *name;
   const char *tmp;
   
   n = pfGetListArrayLen(P);

   pname[0] = '\0';
 
   for(i=0;i<n;i++)
   {
      if (pfGet(P, i) == NULL){
         break;
      }

      tmp = pfGetNodeName(pfGet(P, i));

      if (tmp)
      {
         name = strdup(tmp);
	 pname[strlen(pname)+1] = '\0';
	 pname[strlen(pname)] = '"';
	 strcat(pname, name);
	 free(name);
	 pname[strlen(pname)+1] = '\0';
	 pname[strlen(pname)] = '"';
      }
      else
      {
         name = strdup(pfGetTypeName((pfObject *)pfGet(P, i)));  
         strcat(pname, name);
         free(name);

      }

      pname[strlen(pname)+1] = '\0';
      pname[strlen(pname)] = '/';

    }
    return n;
}



pfNode *DoPick(Pick *P, long mousex,long mousey)
{
   long            pick_count = 0;
   static pfHit 	**ipicked[10];
   char * nodeName;
   char dummy[] = {'\0'};
   
   if ( po_input_mode() ) {
      pfNodePickSetup(P->scene);
      pfuCalcNormalizedChanXY(&P->pickX, &P->pickY, *P->chan, mousex, mousey);
   
      if ((*P->chan != NULL) && (P->scene != NULL))
         pick_count = pfChanPick(*P->chan, P->traverse, P->pickX, P->pickY, 0.0f,
            ipicked);
   } 
   else {
      pick_count = 0;
   }

   if (pick_count > 0)
   {	
      pfQueryHit(*ipicked[0], PFQHIT_NODE, &P->picked);
      pfQueryHit(*ipicked[0], PFQHIT_POINT, &P->point);          //SAK.
      pfQueryHit(*ipicked[0], PFQHIT_PATH, &P->path);          //SAK.

      MakePathString(P->pathname, P->path);
      pfNotify(PFNFY_INFO, PFNFY_PRINT, "Picking.cc - Picked path is:\n%s",
         P->pathname);


      //  Check that we picked a L_po_meas.i.e. is L_po_meas in path.  If we
      //  find it, get the group.  Search for it by node name.
      if ( strstr(P->pathname, "L_po_meas") != NULL ){

         P->group = pfGetParent(P->picked, 0);
         nodeName = (char *)pfGetNodeName(P->group);

         //  Need to do this or it will crash will a null string in the 
         //  string functions.
         if (nodeName == NULL) nodeName = dummy;
            //Go up tree until we reach the picked node.
            while ((strcmp("picked_group", nodeName ) != 0)
               && (strcmp("point_part", nodeName ) != 0)
               && (strcmp("line_part", nodeName ) != 0) ){

               //Never found the picked group if we find L_po_meas first.
               if (strcmp("L_po_meas", nodeName ) == 0){
                  P->group = NULL;
                  break;
               }

               P->group = pfGetParent(P->group,0);
   
               nodeName = (char *)pfGetNodeName(P->group);
               if (nodeName == NULL) nodeName = dummy;

            }  //End while.

      }  // End if.

      else{
         P->group = NULL;
      }

   }
   else {
      P->picked = NULL;
      P->group = NULL;
   }

   return P->picked;

}  //  End of DoPick.



//This does much the same thing as DoPick with certain things unique to
//picking menu items.  It should be made general and coombined with DoPick.
//But for now each has its specialized pick.
pfNode *doMenuPick(Pick *P, long mousex,long mousey)
{
   long            pick_count = 0;
   static pfHit 	**ipicked[10];
   char * nodeName;
   char dummy[] = {'\0'};
  
   pfNodePickSetup(P->scene);
   pfuCalcNormalizedChanXY(&P->pickX, &P->pickY, *P->chan, mousex, mousey);

   if ((*P->chan != NULL) && (P->scene != NULL))
      pick_count = pfChanPick(*P->chan, P->traverse, P->pickX, P->pickY, 0.0f,
         ipicked);
          
   if (pick_count > 0)
   {	
      pfQueryHit(*ipicked[0], PFQHIT_NODE, &P->picked);
      pfQueryHit(*ipicked[0], PFQHIT_POINT, &P->point);          //SAK.
      pfQueryHit(*ipicked[0], PFQHIT_PATH, &P->path);          //SAK.

      MakePathString(P->pathname, P->path);
      pfNotify(PFNFY_INFO, PFNFY_PRINT, 
		"Picking.cc - Picked path is:\n%s", P->pathname);

      //  Check that we picked a menu pick.  Search by using node name.  If we
      //  find it return the group.
        
      if ( strstr(P->pathname, "menu_pick") != NULL ){

         P->group = pfGetParent(P->picked, 0);


         nodeName = (char *)pfGetNodeName(P->group);

         //  Need to do this to ensure string functions don't crash with a null
         //  NULL string.
         if (nodeName == NULL) nodeName = dummy;

         //Go up tree until we reach the picked node.
         while (strcmp("menu_pick", nodeName ) != 0){

            //Never found the picked group if we find L_po_meas first.
            if (strcmp("L_po_meas", nodeName ) == 0){
               P->group = NULL;
               break;
            }

            if (!pfGetNumParents(P->group)){
               P->group = NULL;
               break;
            }

            P->group = pfGetParent(P->group,0);
                 
            nodeName = (char *)pfGetNodeName(P->group);

            //  Prevent crash with NULL string.
            if (nodeName == NULL) nodeName = dummy;

         }  //End while.

      }  // End if, "menu_pick".

      else{

         P->group = NULL;

         if ( strstr(P->pathname, "ball_pick") != NULL ){
            P->group = pfGetParent(P->picked, 0);

            nodeName = (char *)pfGetNodeName(P->group);

            //  Prevent crash on a NULL string.
            if (nodeName == NULL) nodeName = dummy;

            //Go up tree until we reach the picked node.
            while (strcmp("ball_pick", nodeName ) != 0){

               //Never found the picked group if we find L_po_meas first.
               if (strcmp("L_po_meas", nodeName ) == 0){
                  P->group = NULL;
                  break;
               }

               if (!pfGetNumParents(P->group)){
                  P->group = NULL;
                  break;
               }

               P->group = pfGetParent(P->group,0);
                
               nodeName = (char *)pfGetNodeName(P->group);

               if (nodeName == NULL) nodeName = dummy;

            }  //End while.

         }  // End if, "ball_pick".
          
      }  //  End else.

    }  //  End if, (pick_count > 0).

    else {
       P->picked = NULL;
       P->group = NULL;
    }

    return P->picked;
}


char *GetPickedPathname(Pick *P)
{
   return P->pathname;
}


//SAK.
//  This function picks objects which are to be highlighted.  It also gets the
//  group of the picked object so that it can be moved.  This too is a
//  specialized picking function which has alot in common with menupick.  But
//  for now each has its own specialized picking functions.  It then highlights
//  the pick.  This gets a little messy, because points, lines and polygons are
//  highlighted in different ways (because of billboard highlighting problems 
//  encountered.  Lastly, I have different states I move through depending if
//  the mouse button is depressed or if it was depressed last time through the
//  function.  This could be improved and done more formally.
void HLPick(){
   static int firstTime = 1;
   static Pick *thePick;
   pfNode *pickedNode;
   pfNode *child;
   static pfSwitch *lastSwitch=NULL;

   int numChildren = 0;
   char *nodeName;
   char dummy[] = {'\0'};
   long mousex, mousey;

   static mouseWasDown = 0;
    

//   if (firstTime){
//      firstTime = 0;
//      thePick = NewPick(G_dynamic_data->pipe[0].channel[0].scene,
//         &(G_dynamic_data->pipe[0].channel[0].chan_ptr)  );
      thePick = NewPick((pfScene *)G_measures,
         (pfChannel **)&(G_dynamic_data->pipe[0].channel[0].chan_ptr)  );
//   }

   if (!pickedGroup){
      lastSwitch = NULL;
   }
    
   mousex = get_valuator(NPS_MOUSEX);
   mousey = get_valuator(NPS_MOUSEY);

   pickedNode = DoPick(thePick, mousex, mousey);

   if ( (pickedNode != NULL) && (thePick->group != NULL) &&
      !get_button(NPS_LEFTMOUSE)){     //I've got a pick and the left mouse button is
                                  //up.
//cerr<<"path: "<<thePick->pathname<<endl;
      pickedGroup = thePick->group;
      numChildren = pfGetNumChildren(thePick->group);

      for (int i= 0; i < numChildren; i++){
         child = pfGetChild(thePick->group, i);
         nodeName = (char *)pfGetNodeName(child);

         //  Avoid crash of NULL string in string functions.
         if (nodeName == NULL) nodeName = dummy;


         //  I have to find out which kind of geoset I'm highlighting since
         //  each is done differently.  I then have to reset switches so I 
         //  will be able to restore the old color once the node is no longer 
         //  highlighted.  I must also ensure I don't manipulate pointers on
         //  highlighted nodes which may have been deleted.
         if (strcmp("highlight", nodeName ) == 0){       //  Highlight a line.
            pickedNode = child;
            poHighLight(pfGetGSet(pickedNode, 0));

            // Reset flags.
            if (lastSwitch != NULL) {
               pfSwitchVal( lastSwitch, 0);
               lastSwitch = NULL;
            }

            break;
         }

         else {
            if (strcmp("point_switch", nodeName ) == 0){   //  Highlight point.
               pfSwitchVal((pfSwitch *)child, 1);

               //  Reset flags and restore.
               if (lastSwitch != (pfSwitch *)child) {
                 poHighLight(pfGetGSet(pfGetChild(child,0), 0));
                 if (lastSwitch != NULL) pfSwitchVal( lastSwitch, 0);
               }

               lastSwitch = (pfSwitch *)child;            //Save switch so I can
                                                          //restore.
               break;

            }  //  End of if, "point_switch".

         }  //  End else.  
                      
      }  //  End of for loop.

   }  //  End of if, pick with button up.

   else{  //This picks ground and anything else out there.  Turned off for now
      // or else the ground gets highlighted.  Must maintain states.
      //if (pickedNode != NULL) cerr<<"name of the picked "<<
      //   pfGetNodeName(pickedNode)<<endl;
      //   cerr<<"path: "<<thePick->pathname<<endl;   
      // HighlightNode(G_dynamic_data->HL, pickedNode);

      if (get_button(NPS_LEFTMOUSE)){

         //  Left mouse is down and I have a legitimate pick.  So drag this pick.
         thePick = NewPick((pfScene *)G_fixed,
            (pfChannel **)&(G_dynamic_data->pipe[0].channel[0].chan_ptr)  );
         pickedNode = DoPick(thePick, mousex, mousey);
         firstTime = TRUE;
         if (pickedGroup != NULL){
            mouseWasDown = 1;                          //  Set flag.
            drag(pickedGroup, thePick);
//cerr<<"returned from drag in picking"<<endl;
         }
         else{  // Left mouse is down, I'm picking ground
            if ((pickedNode != NULL) && (thePick->group == NULL))  
               addLine(thePick->point[X], thePick->point[Y]);
         }
      }

      else{   //  Mouse is not down.

         //Reset the highlighting and what was picked.
         if (lastSwitch != NULL) {
            pfSwitchVal( lastSwitch, 0);
            lastSwitch = NULL;
         }

         //else{        
            poHighLight(NULL);    
         //}
       
         //I've released an object I was draging.  Process this drag and reset
         // flag.
         if (pickedGroup != NULL && mouseWasDown){
            processDrag(pickedGroup, thePick);
            mouseWasDown = 0;
         }

         pickedGroup = NULL;        //  Mouse was not down so I have no picked
                                    //  group at this time.
      }  //  End of else, mouse is not down.
   }  //  End of else.
  
}  //End of HLPick.



//SAK.
//  This is another part of the picking, in this case the picking of menu 
//  buttons.  It too should be part of a more general but for now each picking
//  has its own specialized picking routine.
void menuPick(){
   //static int firstTime = 1;
   static Pick *thePick;
   pfNode *pickedNode;

   char *nodeName;
   char dummy[] = {'\0'};
   long mousex, mousey;
   pfVec3 worldPos;
   callBack* pointCB;
   int set_to_terrain = FALSE;
/*
   if (firstTime){
      firstTime = 0;
      thePick = NewPick(G_dynamic_data->pipe[0].channel[0].scene,
         &(G_dynamic_data->pipe[0].channel[0].chan_ptr)  );
   }
*/    
   //Perform appropriate pick based on which mouse button pressed
   if (get_button(NPS_LEFTMOUSE)) {
      thePick = NewPick((pfScene *)G_fixed,
         (pfChannel **)&(G_dynamic_data->pipe[0].channel[0].chan_ptr)  );
      set_to_terrain = TRUE;
   }
   else {
           thePick = NewPick((pfScene *)G_measures,
              (pfChannel **)&(G_dynamic_data->pipe[0].channel[0].chan_ptr)  );
        }

   mousex = get_valuator(NPS_MOUSEX);
   mousey = get_valuator(NPS_MOUSEY);

   pickedNode = doMenuPick(thePick, mousex, mousey);

   if (thePick->group) nodeName = (char *)pfGetNodeName(thePick->group);

   //  Prevent crash in string functions with a NULL string.
   if (nodeName == NULL) nodeName = dummy;

   //  Got a point on the ground.  Put the menu there.
   if( thePick->group == NULL && set_to_terrain){
      worldPos[X] = thePick->point[X];
      worldPos[Y] = thePick->point[Y];
      worldPos[Z] = gnd_level2(worldPos);
      menuOn(worldPos);     
   }

   // Got a button on an active menu.  Execute its callback.
   if ( (pickedNode != NULL) && (thePick->group != NULL) &&
      strcmp("menu_pick", nodeName) == 0) {
      pointCB = (callBack*)pfGetUserData(thePick->group);

      //Callback is stored as a pointer.  Call the function.
      (*(pointCB->function))(pointCB->nodeLevel);
   }
 
}  //  End of menuPick.

//SAK.
//  Another specialized picking function.  This one is used with the 
//  ballistics planner.
void ballPick(){
//   static int firstTime = 1;
   static Pick *thePick;
   pfNode *pickedNode;
   
   char *nodeName;
   char dummy[] = {'\0'};
   long mousex, mousey;

   ballCB* ballBoxCB;

//   if (firstTime){
//      firstTime = 0;
//      thePick = NewPick(G_dynamic_data->pipe[0].channel[0].scene,
//      &(G_dynamic_data->pipe[0].channel[0].chan_ptr)  );      
      thePick = NewPick((pfScene *)G_measures,
      (pfChannel **)&(G_dynamic_data->pipe[0].channel[0].chan_ptr)  );      
//   }
 
   mousex = get_valuator(NPS_MOUSEX);
   mousey = get_valuator(NPS_MOUSEY);

   pickedNode = doMenuPick(thePick, mousex, mousey);
       
   if (thePick->group) nodeName = (char *)pfGetNodeName(thePick->group);

   // Prevent crash with a null string.
   if (nodeName == NULL) nodeName = dummy;

   if ((pickedNode != NULL) && (thePick->group != NULL) &&
      strcmp("ball_pick", nodeName) == 0){
         //Get the callback pointer.
         ballBoxCB = (ballCB*)pfGetUserData(thePick->group);

         //  Call the callback.
         (*(ballBoxCB->function))(ballBoxCB->ballBox);

   }  
 
}  //  End of ballPick.


//  This function is currently not used.  It was used to test some of the
//  mouse buttons etc.
void mouseTest(Pick *P){

   float    	pickX;
   float    	pickY;
   float        mouseX;
   float        mouseY;
   pfVec3       *point;
   long         pick_count = 0;
   static pfHit **ipicked[10];

   pfNodePickSetup(P->scene);

   while (get_button(NPS_LEFTMOUSE)){

      mouseX = get_valuator(NPS_MOUSEX);
      mouseY = get_valuator(NPS_MOUSEY);

      pfuCalcNormalizedChanXY(&pickX, &pickY, *P->chan, mouseX, mouseY);

      if ((*P->chan != NULL) && (P->scene != NULL))
         pick_count = pfChanPick(*P->chan, P->traverse, pickX, pickY, 0.0f,
            ipicked);

      if (pick_count > 0)
      {		
        pfQueryHit(*ipicked[0], PFQHIT_POINT, &point);          //SAK.
        //cerr<<"world x "<<point[X]<<"world y "<<point[Y]<<endl;
      }
   }
}  //  End of mouseTest.


//  This function drags po measures. It uses a picked measures member function
//  to move the measure. 
void drag(pfGroup* group, Pick* thePick){

   char* groupName;
  
   //  Get the group name and if not NULL proceed.
   groupName = (char*)pfGetNodeName(group);

   //  See if what I've picked is a line builder.
   if (groupName){
      if ( !(strcmp(groupName, "point_part")) ){
         dragPoint(thePick->point[X], thePick->point[Y], group);
      }
      else{
         if ( !(strcmp(groupName, "line_part")) ){
            dragLine(thePick->point[X], thePick->point[Y]);
         }
      }
   }
        
   //  Look in the poArray and see if what I've picked is in the array.
   for (int i = 0; i < poCount; i++){

      if (poArray[i].poPointer->polys == group){
         //  If it is in the array, use its member function to move to the 
         //  appropriate location.
         poArray[i].poPointer->movePO(thePick->point[X],
            thePick->point[Y], 1);
         break;
      }
   }
//cerr<<"leaving drag"<<endl;
}  //  End of drag.


//  At the end of the drag I process the dragged object with this function
//  using a member function of the object.  The member function does any
//  required actions once an object has been dragged.
void processDrag(pfGroup* group, Pick* thePick){

   char* groupName;
  
   //  Get the group name and if not NULL proceed.
   groupName = (char*)pfGetNodeName(group);

   //  See if what I've picked is a line builder.
   if (groupName){
      if ( !(strcmp(groupName, "point_part")) ){
         dropPoint(thePick->point[X], thePick->point[Y], group);
      }
      else{
         if ( !(strcmp(groupName, "line_part")) ){
            dropLine(thePick->point[X], thePick->point[Y]);
         }
      }
   }
     
   // Check if in the array.  If so process using its member function.
   for (int i = 0; i < poCount; i++){
      if (poArray[i].poPointer->polys == group){
         poArray[i].poPointer->dragUpdatePO(thePick->point[X], 
            thePick->point[Y], 1);
         break;
      }
   }
}

#endif
