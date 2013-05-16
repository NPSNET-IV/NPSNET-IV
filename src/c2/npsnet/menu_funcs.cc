//menu_funcs.cc

#ifndef NOC2MEASURES

#include <iostream.h>
#include "globals.h"
#include "menu_funcs.h"
#include "po_meas.h"
#include "po_ball_funcs.h"
#include "po_ball.h"
#include "po_build.h"

// Globals used by this library
extern pfGroup      *G_measures;

static pfGroup *L_menu = NULL;
static pfGeoSet *arrowsGeo[NUM_ARROWS];


void assignCB(pfGroup * node, void(*function)(MENU_LEVEL*),MENU_LEVEL* level ){
   callBack * groupCB;

if(NULL == (groupCB = (callBack *) pfMalloc(sizeof(callBack),
     pfGetSharedArena() ))){ 
   cerr<<"allocation error makeArrowGset "<<endl;
}

   groupCB->function = function;

   groupCB->nodeLevel = level;

   groupCB->userData = NULL;

   pfUserData(node, groupCB);
}

void assignCB(pfGroup * node, void(*function)(MENU_LEVEL*),MENU_LEVEL* level,
   void* userData ){
   callBack * groupCB;

if(NULL == (groupCB = (callBack *) pfMalloc(sizeof(callBack),
     pfGetSharedArena() ))){ 
   cerr<<"allocation error makeArrowGset "<<endl;
}

   groupCB->function = function;

   groupCB->nodeLevel = level;

   groupCB->userData = userData;

   pfUserData(node, groupCB);
}

void initArrowGsets(){

   arrowTypes arrow, arrowHL, quad;

   arrow = DOWN_ARR;
   arrowHL = DOWN_HL;
   quad = QUAD_DN;

   makeArrowGset(&(arrowsGeo[arrow]), &(arrowsGeo[arrowHL]),
      &(arrowsGeo[quad]), "down.lst", "downqd.lst"); 

   pfDelete(arrowsGeo[quad]);    //  I make it twice so I'll get rid of this one.

   arrow = UP_ARR;
   arrowHL = UP_HL;
   quad = QUAD_DN;

   makeArrowGset(&(arrowsGeo[arrow]), &(arrowsGeo[arrowHL]),
      &(arrowsGeo[quad]), "up.lst", "downqd.lst");

   arrow = RIGHT_ARR;
   arrowHL = RIGHT_HL;
   quad = QUAD;
  
   makeArrowGset(&(arrowsGeo[arrow]), &(arrowsGeo[arrowHL]),
      &(arrowsGeo[quad]), "right.lst", "arrowqd.lst");
   

   
   pfDelete(arrowsGeo[quad]);    //  I make it twice so I'll get rid of this one.

   arrow = LEFT_ARR;
   arrowHL = LEFT_HL;
   quad = QUAD;
  
   makeArrowGset(&(arrowsGeo[arrow]), &(arrowsGeo[arrowHL]), 
      &(arrowsGeo[quad]), "left.lst", "arrowqd.lst");
}


void makeArrowGset(pfGeoSet **arrow, pfGeoSet **arrowHL,pfGeoSet **pickQuad,
    const char *infileName, const char *infileName1){

  pfGeoState  *gstate, *gstate2;

  int numPrims, numPrims2;

  int numVertices = 0, numVertices2 = 0;

  pfVec3 *pointVerts, *pointVerts2;

  FILE *infile;

  char fileName[PF_MAXSTRING];
  char fileName1[PF_MAXSTRING];

  static pfVec4 color = {0.0, 1.0, 1.0, 0.3};
  static pfVec4 HLColor = {1.0, 0.0, 1.0, 0.5};
  static pfVec4 transparent = {0.0, 0.0, 0.0, 0.0};
  

  int *length_vec, *length_vec2;

 pfFindFile ( (char *)infileName, fileName, R_OK );
 pfFindFile ( (char *)infileName1, fileName1, R_OK );


 if(NULL == (infile =  fopen( fileName, "r"))){
   cerr<<"Unable to open "<<fileName<<endl;
  }

    fscanf(infile, "%d", &numPrims);


if(NULL == (length_vec = (int *) pfMalloc(sizeof(int)*numPrims,
     pfGetSharedArena() ))){ 
   cerr<<"allocation error makeArrowGset "<<endl;
}

for (int i = 0; i < numPrims; i++){
    fscanf(infile, "%d", &(length_vec[i]) );
    numVertices += length_vec[i];

}

    pointVerts = (pfVec3*) pfMalloc(( (numVertices) * sizeof(pfVec3) ),
        pfGetSharedArena() );

     for (i = 0; i < numVertices; i++){
        fscanf(infile, "%f %f %f", &(pointVerts[i][X]), &(pointVerts[i][Y]),
        &(pointVerts[i][Z]) );
    }

  fclose(infile);

  //make a textured panel

  gstate = pfNewGState (pfGetSharedArena());
  
  pfGStateMode (gstate, PFSTATE_ENTEXTURE, 0);

  //pfGStateAttr (gstate, PFSTATE_TEXENV, L_po_tex_env);
  //pfGStateAttr (gstate, PFSTATE_TEXTURE, L_po_point_tex[style]);
  /* set alpha function to block pixels of 0 alpha for transparent textures */
  //pfGStateMode (gstate, PFSTATE_ALPHAFUNC, PFAF_NOTEQUAL);
  pfGStateMode (gstate, PFSTATE_ALPHAFUNC, PFAF_ALWAYS);
  //pfGStateMode (gstate, PFSTATE_ALPHAREF, 0);
  pfGStateMode (gstate, PFSTATE_TRANSPARENCY,
     PFTR_MS_ALPHA | PFTR_NO_OCCLUDE);
  
  *arrow = pfNewGSet(pfGetSharedArena());
  pfGSetAttr(*arrow, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts, NULL);
  pfGSetAttr(*arrow, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
  pfGSetAttr(*arrow, PFGS_COLOR4, PFGS_OVERALL, color, NULL);
  //pfGSetAttr(*arrow, PFGS_NORMAL3, PFGS_OVERALL, normal, NULL);
  pfGSetAttr(*arrow, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);

  pfGSetPrimType(*arrow, PFGS_TRISTRIPS);

  pfGSetNumPrims(*arrow, numPrims);

//  length_vec[0] = numVertices;
  
  pfGSetPrimLengths(*arrow, length_vec);       //HARDWIRED BAD!!! JUST FOR TEST!!


  pfGSetLineWidth(*arrow, 3.0);

  pfGSetGState (*arrow, gstate);


  *arrowHL = pfNewGSet(pfGetSharedArena());
  pfGSetAttr(*arrowHL, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts, NULL);
  pfGSetAttr(*arrowHL, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
  pfGSetAttr(*arrowHL, PFGS_COLOR4, PFGS_OVERALL, HLColor, NULL);
  //pfGSetAttr(*arrowHL, PFGS_NORMAL3, PFGS_OVERALL, axis, NULL);
  pfGSetAttr(*arrowHL, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
  //pfGSetAttr(*arrowHL, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);

  pfGSetPrimType(*arrowHL, PFGS_TRISTRIPS);

  pfGSetNumPrims(*arrowHL, numPrims);

//  length_vec[0] = numVertices;
  
  pfGSetPrimLengths(*arrowHL, length_vec);       //HARDWIRED BAD!!! JUST FOR TEST!!


  pfGSetLineWidth(*arrowHL, 3.0);

  pfGSetGState (*arrowHL, gstate);



if(NULL == (infile =  fopen( fileName1, "r"))){
    cerr<<"Unable to open "<<fileName1<<endl;
}

    fscanf(infile, "%d", &numPrims2);


if(NULL == (length_vec2 = (int *) pfMalloc(sizeof(int)*numPrims2,
     pfGetSharedArena() ))){
      cerr<<"bad allocation: makeArrowGset"<<endl;
}

for (i = 0; i < numPrims2; i++){
    fscanf(infile, "%d", &(length_vec2[i]) );
    numVertices2 += length_vec2[i];

}

    pointVerts2 = (pfVec3*) pfMalloc(( (numVertices2) * sizeof(pfVec3) ),
        pfGetSharedArena() );

     for (i = 0; i < numVertices2; i++){
        fscanf(infile, "%f %f %f", &(pointVerts2[i][X]), &(pointVerts2[i][Y]),
        &(pointVerts2[i][Z]) );
    }

  fclose(infile);

  gstate2 = pfNewGState (pfGetSharedArena());
  //pfGStateMode (gstate2, PFSTATE_ENTEXTURE, 1);

  //pfGStateAttr (gstate2, PFSTATE_TEXENV, L_po_tex_env);
  //pfGStateAttr (gstate2, PFSTATE_TEXTURE, L_po_point_tex[style]);
  /* set alpha function to block pixels of 0 alpha for transparent textures */
  pfGStateMode (gstate2, PFSTATE_ALPHAFUNC, PFAF_NOTEQUAL);
  pfGStateMode (gstate2, PFSTATE_ALPHAREF, 0);
   
  *pickQuad = pfNewGSet(pfGetSharedArena());

  pfGSetAttr(*pickQuad, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts2, NULL);

  pfGSetAttr(*pickQuad, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
  //pfGSetAttr(*pickQuad, PFGS_NORMAL3, PFGS_OVERALL, normal, NULL);
  pfGSetAttr(*pickQuad, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
  //pfGSetAttr(*pickQuad, PFGS_TEXCOORD2, PFGS_PER_VERTEX, L_texcoords, NULL);
  pfGSetAttr(*pickQuad, PFGS_COLOR4, PFGS_OVERALL, transparent, NULL);
  pfGSetPrimType(*pickQuad, PFGS_QUADS);
  

  pfGSetNumPrims(*pickQuad, numPrims2);
 
  pfGSetPrimLengths(*pickQuad, length_vec2);       //HARDWIRED BAD!!! JUST FOR TEST!!

  pfGStateMode (gstate2, PFSTATE_TRANSPARENCY, PFTR_MS_ALPHA | PFTR_NO_OCCLUDE);
  pfGSetGState (*pickQuad, gstate2);


  
}

pfGroup* makeArrow(pfGeoSet *arrow, pfGeoSet *arrowHL, pfGeoSet *pickQuad,
   pfVec3 point){

  pfBillboard *board,*board2;
  pfGeode *geode;
  pfDCS *DCS;
  pfSwitch *ptSwitch;

  pfGroup *the_group;

  
  //static pfVec3 axis = {0.0, 0.0, -1.0};

  //add it to a pfBillboard
  board = pfNewBboard();
  board2 = pfNewBboard();

  //pfBboardAxis(board, axis);
  pfBboardMode(board, PFBB_ROT, PFBB_AXIAL_ROT);

  pfAddGSet(board,arrow);

  //pfBboardAxis(board2, axis);
  pfBboardMode(board2, PFBB_ROT, PFBB_AXIAL_ROT);


  pfAddGSet(board2,arrowHL);

  geode = pfNewGeode();

  pfAddGSet(geode,pickQuad);
  DCS = pfNewDCS();
  pfAddChild(DCS, geode);

  pfDCSTrans(DCS, point[X], point[Y], point[Z]);

  pfBboardPos(board,0,point);
  pfBboardPos(board2,0,point);

  ptSwitch = pfNewSwitch();

  pfAddChild(ptSwitch,board);
  pfAddChild(ptSwitch,board2);
  pfSwitchVal( ptSwitch, 0);
  the_group = pfNewGroup();


  pfAddChild(the_group,ptSwitch);
  pfAddChild(the_group, DCS);


  pfNodeName(ptSwitch, "point_switch");
  pfNodeName(the_group, "menu_pick");
  return(the_group);
 
}


pfGroup* makeDown(){
   pfGroup *group;
   pfVec3 offset = {0.0, 0.0, 5.0};

   group = makeArrow(arrowsGeo[DOWN_ARR], arrowsGeo[DOWN_HL], arrowsGeo[QUAD_DN],
      offset);
   return (group); 
}

pfGroup*  makeUp(){
   pfGroup *group;
   pfVec3 offset = {0.0, 0.0, 12.0};

   group = makeArrow(arrowsGeo[UP_ARR], arrowsGeo[UP_HL], arrowsGeo[QUAD_DN],
      offset);
   return (group); 
}

pfGroup*  makeRight(){
   pfGroup *group;
   pfVec3 offset = {0.0, 0.0, 8.0};

   group = makeArrow(arrowsGeo[RIGHT_ARR], arrowsGeo[RIGHT_HL], arrowsGeo[QUAD],
      offset);
   return (group); 
}

pfGroup*  makeLeft(){ 
   pfGroup *group;
   pfVec3 offset = {0.0, 0.0, 10.0};

   group = makeArrow(arrowsGeo[LEFT_ARR], arrowsGeo[LEFT_HL], arrowsGeo[QUAD],
      offset);
   return (group);
}




MENU_LEVEL* initMenu(){

   pfGroup* rootGroup;  
   pfGroup* tempGroup; 

   MENU_LEVEL* doneBuild;

   MENU_LEVEL* red;
   MENU_LEVEL* yellow;
   MENU_LEVEL* black;
   MENU_LEVEL* blue;
   MENU_LEVEL* green;

   MENU_LEVEL* thick1;
   MENU_LEVEL* thick2;
   MENU_LEVEL* thick3;
   MENU_LEVEL* thick4;
   MENU_LEVEL* thick5;
   MENU_LEVEL* thick6;

   MENU_LEVEL* doneM198;
   MENU_LEVEL* doneM60;
   MENU_LEVEL* done81mm;

   MENU_LEVEL* howitzer;
   MENU_LEVEL* howitzerm;
   MENU_LEVEL* howitzerh;
   MENU_LEVEL* mgunm;
   MENU_LEVEL* mortarm;

   MENU_LEVEL* genPoint;
   MENU_LEVEL* coordPoint;
   MENU_LEVEL* contactPoint;
 
   MENU_LEVEL* point;
   MENU_LEVEL* line;
   MENU_LEVEL* ballistic;
   MENU_LEVEL* closed_line;
   MENU_LEVEL* unit; 
             
   pfVec3  origin = { 0.0, 0.0, 0.0};

   L_menu = pfNewGroup();

   tempGroup = makePickableLnStrip(L_color[GREEN_M],"check.lst",
      origin);
   doneBuild = new MENU_LEVEL(tempGroup, buildItCB);



   tempGroup = makePickableTriStrip(L_color[RED_M],"circfill.tst",
      origin);
   red = new MENU_LEVEL(tempGroup, redCB);
   red->addMenuChild(doneBuild);
   
   tempGroup = makePickableTriStrip(L_color[YELLOW_M],"circfill.tst",
      origin);
   yellow = new MENU_LEVEL(tempGroup, yellowCB);
   yellow->addMenuChild(doneBuild);
   
   tempGroup = makePickableTriStrip(L_color[BLACK_M],"circfill.tst",
      origin);
   black = new MENU_LEVEL(tempGroup, blackCB);
   black->addMenuChild(doneBuild);
   
   tempGroup = makePickableTriStrip(L_color[BLUE_M],"circfill.tst",
      origin);
   blue = new MENU_LEVEL(tempGroup, blueCB);
   blue->addMenuChild(doneBuild);
   
   tempGroup = makePickableTriStrip(L_color[GREEN_M],"circfill.tst",
      origin);
   green = new MENU_LEVEL(tempGroup, greenCB);
   green->addMenuChild(doneBuild);


   tempGroup = makePickableTriStrip(L_color[BLACK_M],"thick1.tst",
      origin);
   thick1 = new MENU_LEVEL(tempGroup, thick1CB);
   thick1->addMenuChild(red);
   thick1->addMenuChild(yellow);
   thick1->addMenuChild(black);
   thick1->addMenuChild(blue);
   thick1->addMenuChild(green);

   tempGroup = makePickableTriStrip(L_color[BLACK_M],"thick2.tst",
      origin);
   thick2 = new MENU_LEVEL(tempGroup, thick2CB);
   thick2->addMenuChild(red);
   thick2->addMenuChild(yellow);
   thick2->addMenuChild(black);
   thick2->addMenuChild(blue);
   thick2->addMenuChild(green);

   tempGroup = makePickableTriStrip(L_color[BLACK_M],"thick3.tst",
      origin);
   thick3 = new MENU_LEVEL(tempGroup, thick3CB);
   thick3->addMenuChild(red);
   thick3->addMenuChild(yellow);
   thick3->addMenuChild(black);
   thick3->addMenuChild(blue);
   thick3->addMenuChild(green);

   tempGroup = makePickableTriStrip(L_color[BLACK_M],"thick4.tst",
      origin);
   thick4 = new MENU_LEVEL(tempGroup, thick4CB);
   thick4->addMenuChild(red);
   thick4->addMenuChild(yellow);
   thick4->addMenuChild(black);
   thick4->addMenuChild(blue);
   thick4->addMenuChild(green);

   tempGroup = makePickableTriStrip(L_color[BLACK_M],"thick5.tst",
      origin);
   thick5 = new MENU_LEVEL(tempGroup, thick5CB);
   thick5->addMenuChild(red);
   thick5->addMenuChild(yellow);
   thick5->addMenuChild(black);
   thick5->addMenuChild(blue);
   thick5->addMenuChild(green);

   tempGroup = makePickableTriStrip(L_color[BLACK_M],"thick6.tst",
      origin);
   thick6 = new MENU_LEVEL(tempGroup, thick6CB);
   thick6->addMenuChild(red);
   thick6->addMenuChild(yellow);
   thick6->addMenuChild(black);
   thick6->addMenuChild(blue);
   thick6->addMenuChild(green);

 
   tempGroup = makePickableLnStrip(L_color[GREEN_M],"check.lst",
      origin);
   doneM198 = new MENU_LEVEL(tempGroup, doneM198CB);
   
   tempGroup = makePickableLnStrip(L_color[GREEN_M],"check.lst",
      origin);
   doneM60 = new MENU_LEVEL(tempGroup, doneM60CB);
  
   tempGroup = makePickableLnStrip(L_color[GREEN_M],"check.lst",
      origin);
   done81mm = new MENU_LEVEL(tempGroup, done81mmCB);
   

   tempGroup = makePickableLnStrip(L_color[BLACK_M],"howitzer.lst",
      origin); 
   howitzer = new MENU_LEVEL(tempGroup, howitzerCB);
   
   tempGroup = makePickableLnStrip(L_color[BLACK_M],"howitzerm.lst",
      origin); 
   howitzerm = new MENU_LEVEL(tempGroup, howitzermCB);
   howitzerm->addMenuChild(doneM198);

   tempGroup = makePickableLnStrip(L_color[BLACK_M],"howitzerh.lst",
      origin);
   howitzerh = new MENU_LEVEL(tempGroup, howitzerhCB);
   
   tempGroup = makePickableLnStrip(L_color[BLACK_M],"mortarm.lst",
      origin); 
   mortarm = new MENU_LEVEL(tempGroup, standardCB);
   mortarm->addMenuChild(done81mm);

   tempGroup = makePickableLnStrip(L_color[BLACK_M],"mgunm.lst",
      origin);
   mgunm = new MENU_LEVEL(tempGroup, standardCB);
   mgunm->addMenuChild(doneM60);


   tempGroup = makePickableLnStrip(L_color[BLACK_M],"general.lst",
      origin); 
   genPoint = new MENU_LEVEL(tempGroup, genPointCB);
   genPoint->addMenuChild(red);
   genPoint->addMenuChild(green);
   genPoint->addMenuChild(yellow);
   genPoint->addMenuChild(black);
   genPoint->addMenuChild(blue);
 
   tempGroup = makePickableLnStrip(L_color[BLACK_M],"coord.lst",
      origin); 
   coordPoint = new MENU_LEVEL(tempGroup, coordPointCB);
   coordPoint->addMenuChild(red);
   coordPoint->addMenuChild(green);
   coordPoint->addMenuChild(yellow);
   coordPoint->addMenuChild(black);
   coordPoint->addMenuChild(blue);

   tempGroup = makePickableLnStrip(L_color[BLACK_M],"contact.lst",
      origin);
   contactPoint = new MENU_LEVEL(tempGroup, contactPointCB); 
   contactPoint->addMenuChild(red);
   contactPoint->addMenuChild(green);
   contactPoint->addMenuChild(yellow);
   contactPoint->addMenuChild(black);
   contactPoint->addMenuChild(blue);


   
   tempGroup = makePickableLnStrip(L_color[BLACK_M],"line.lst",
      origin);
   line = new MENU_LEVEL(tempGroup, lineCB);
   line->addMenuChild(thick1);
   line->addMenuChild(thick2);
   line->addMenuChild(thick3);
   line->addMenuChild(thick4);
   line->addMenuChild(thick5);
   line->addMenuChild(thick6);

   tempGroup = makePickableLnStrip(L_color[BLACK_M],"clsdline.lst",
      origin);
   closed_line = new MENU_LEVEL(tempGroup, closed_lineCB);
   closed_line->addMenuChild(thick1);
   closed_line->addMenuChild(thick2);
   closed_line->addMenuChild(thick3);
   closed_line->addMenuChild(thick4);
   closed_line->addMenuChild(thick5);
   closed_line->addMenuChild(thick6);

   tempGroup = makePickableLnStrip(L_color[BLACK_M],"general.lst",
      origin);
   point = new MENU_LEVEL(tempGroup, pointCB);
   point->addMenuChild(genPoint);
   point->addMenuChild(coordPoint);
   point->addMenuChild(contactPoint);
 
   tempGroup = makePickableLnStrip(L_color[BLACK_M],"parab.lst",
      origin);
   ballistic = new MENU_LEVEL(tempGroup, ballisticCB);
   ballistic->addMenuChild(howitzer);
   ballistic->addMenuChild(howitzerm);
   ballistic->addMenuChild(howitzerh);
   ballistic->addMenuChild(mortarm);
   ballistic->addMenuChild(mgunm);



    
   tempGroup = makePickableLnStrip(L_color[BLACK_M],"unit.lst",
      origin);
   unit = new MENU_LEVEL(tempGroup, unitCB); 
 

  
   rootGroup = makePickableLnStrip(L_color[WHITE_M],"root.lst",
      origin);  
   L_menu_root = new MENU_LEVEL(rootGroup, rootCB);
   L_menu_root->addMenuChild(line);
   L_menu_root->addMenuChild(closed_line);
   L_menu_root->addMenuChild(point);
   L_menu_root->addMenuChild(unit);
   L_menu_root->addMenuChild(ballistic); 

   //  The root is special.  We have to place its children.  For higher
   //  levels this is done when an item is chosen in standardCB.
   for (int i = 0; i< L_menu_root->numChildren; i++){

      L_menu_root->nextLevel[i]->previousLevel = L_menu_root;
      L_menu_root->nextLevel[i]->childNumber = i;

      pfDCSTrans(L_menu_root->nextLevel[i]->root,
         L_menu_root->points[i][X],
         L_menu_root->points[i][Y],
         L_menu_root->points[i][Z]);
   }

   return (L_menu_root);  

}  //  End of initMenu.

// Determine whether menu is being displayed or not
int menu_Is_On() {
   if (L_menu_root) {
      if (pfGetNumChildren(L_menu_root->nextLevelGroup) > 0)
         return TRUE;
      else
         return FALSE;
   }
   else
      return FALSE;
}

void menuOn(pfVec3 position){
   static int firstTime = 1;

   if (firstTime){
      //initArrowGsets();
      //initMenu();
      firstTime = 0;
      pfAddChild(L_menu, L_menu_root->root);
      pfAddChild(G_measures,L_menu);
      
   }
//cerr<<"in mneu on"<<endl;
   if (L_menu_root != NULL){
      pfDCSTrans(L_menu_root->root, position[X], position[Y], position[Z]);
      passMenuPosition(position); 
   }

   L_menu_root->simSwitch(SWITCH_ON);

   for (int i=0; i < L_menu_root->numChildren; i++){
      if (pfGetNumChildren(L_menu_root->nextLevel[i]->nextLevelGroup) == 0){
         L_menu_root->nextLevel[i]->simSwitch(0);
      }
   }
//cerr<<"out menu on"<<endl;
}

void menuOff(MENU_LEVEL* level){
   pfSwitch *priorEdge;
   pfSwitch *HLSwitch;

   HLSwitch = (pfSwitch *)pfGetChild(level->nodeGroup, 0);
   pfSwitchVal(HLSwitch, 0);
   
   level->simSwitch(SWITCH_OFF);

   if (level->previousLevel != 0){
      
      //pfSwitchVal(level->nextLevelSwitch, 0);    //CAN'T PICK THROUGH SWITCH.

      priorEdge = (pfSwitch *)pfGetChild(level->previousLevel->edgesGroup,
         level->childNumber);
      pfSwitchVal(priorEdge, 0);                  //This is OK, we're not pickin
                                                 //through this guy.

      menuOff(level->previousLevel);
   }
   else{
      L_menu_root->simSwitch(SWITCH_OFF);
   }   
}


//  This function reads in a file and returns a geode constructed out of 
//  tristrips.
pfGeode* makePoGeoTri(pfVec4 color, const char* infileName){

   pfGeoState  *gstate;
   pfGeoSet   *gset;
   pfGeode *geode;

   int numPrims;

   int numVertices = 0;

   pfVec3 *pointVerts;

   FILE *infile;
 
   int *length_vec;

   char fileName[PF_MAXSTRING];

   pfFindFile ( (char *)infileName, fileName, R_OK );

   if(NULL == (infile =  fopen( fileName, "r"))){
      cerr<<"Unable to open "<<fileName<<endl;
      return(FALSE);
   }

   //  get the number of primitives.
   fscanf(infile, "%d", &numPrims);


   //  Allocate storage for the number of primitives.
   if(NULL == (length_vec = (int *) pfMalloc(sizeof(int)*numPrims,
      pfGetSharedArena() ))){ return (NULL);
   }

   //  Add up the number of vertices.
   for (int i = 0; i < numPrims; i++){
      fscanf(infile, "%d", &(length_vec[i]) );
      numVertices += length_vec[i];
   }

   //Assign storage for those vertices.
   pointVerts = (pfVec3*) pfMalloc(( (numVertices) * sizeof(pfVec3) ),
      pfGetSharedArena() );

   //Read in the points.
   for (i = 0; i < numVertices; i++){
      fscanf(infile, "%f %f %f", &(pointVerts[i][X]), &(pointVerts[i][Y]),
         &(pointVerts[i][Z]) );
   }

   fclose(infile);

   //Build a geode for the points read in.
   gstate = pfNewGState (pfGetSharedArena());
   pfGStateMode (gstate, PFSTATE_ENTEXTURE, 0);
   pfGStateMode(gstate, PFSTATE_TRANSPARENCY, 1);

   gset = pfNewGSet(pfGetSharedArena());
   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts, NULL);
   pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, color, NULL);
   pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
  
   pfGSetPrimType(gset, PFGS_TRISTRIPS);

   pfGSetNumPrims(gset, numPrims);
 
   pfGSetPrimLengths(gset, length_vec);  
     
   pfGSetGState (gset, gstate);

   geode = pfNewGeode();

   pfAddGSet(geode,gset);

   return(geode);
   
}  //End of makePoGeoTri.


//  This function reads in a file and returns a geode constructed from 
//  linestrips.
pfGeode* makePoGeoLine(pfVec4 color, const char* infileName){

   pfGeoState  *gstate;
   pfGeoSet   *gset;
   pfGeode *geode;

   int numPrims;

   int numVertices = 0;

   pfVec3 *pointVerts;

   FILE *infile;
 
   int *length_vec;

   char fileName[PF_MAXSTRING];

   pfFindFile ( (char *)infileName, fileName, R_OK );

   if(NULL == (infile =  fopen( fileName, "r"))){
      cerr<<"Unable to open "<<fileName<<endl;
      return(FALSE);
   }

   fscanf(infile, "%d", &numPrims);

   //  Get the number of primitives in the geode.
   if(NULL == (length_vec = (int *) pfMalloc(sizeof(int)*numPrims,
      pfGetSharedArena() ))){ return (NULL);
   }

   //  Add up the number of vertices in the geode.
   for (int i = 0; i < numPrims; i++){
      fscanf(infile, "%d", &(length_vec[i]) );
      numVertices += length_vec[i];
   }

   //  Allocate storage for the vertices.
   pointVerts = (pfVec3*) pfMalloc(( (numVertices) * sizeof(pfVec3) ),
      pfGetSharedArena() );

   //  Read in the points.
   for (i = 0; i < numVertices; i++){
      fscanf(infile, "%f %f %f", &(pointVerts[i][X]), &(pointVerts[i][Y]),
      &(pointVerts[i][Z]) );
   }

   fclose(infile);

   //  Build the geode.
   gstate = pfNewGState (pfGetSharedArena());
   pfGStateMode (gstate, PFSTATE_ENTEXTURE, 0);
   pfGStateMode(gstate, PFSTATE_TRANSPARENCY, 1);

   gset = pfNewGSet(pfGetSharedArena());
   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts, NULL);
   pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, color, NULL);
   pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
  
   pfGSetPrimType(gset, PFGS_LINESTRIPS);

   pfGSetLineWidth(gset, 3.0);

   pfGSetNumPrims(gset, numPrims);
 
   pfGSetPrimLengths(gset, length_vec);  
     
   pfGSetGState (gset, gstate);

   geode = pfNewGeode();

   pfAddGSet(geode,gset);

   return(geode);
  
}  //End of makePoGeoLine.

//  This function reads in a file and returns a billboard constructed
// out of tristrips.
pfBillboard* makeBBTri(pfVec4 color, const char* infileName){

   pfGeoState  *gstate;
   pfGeoSet   *gset;
   pfBillboard *board;

   static pfVec3 axis = {0.0, 0.0, 1.0};
   
   int numPrims;

   int numVertices = 0;

   pfVec3 *pointVerts;

   FILE *infile;
 
   int *length_vec;

   char fileName[PF_MAXSTRING];

   pfFindFile ( (char *)infileName, fileName, R_OK );

   if(NULL == (infile =  fopen( fileName, "r"))){
      cerr<<"Unable to open "<<fileName<<endl;
      return(FALSE);
   }

   //  get the number of primitives.
   fscanf(infile, "%d", &numPrims);


   //  Allocate storage for the number of primitives.
   if(NULL == (length_vec = (int *) pfMalloc(sizeof(int)*numPrims,
      pfGetSharedArena() ))){ return (NULL);
   }

   //  Add up the number of vertices.
   for (int i = 0; i < numPrims; i++){
      fscanf(infile, "%d", &(length_vec[i]) );
      numVertices += length_vec[i];
   }

   //Assign storage for those vertices.
   pointVerts = (pfVec3*) pfMalloc(( (numVertices) * sizeof(pfVec3) ),
      pfGetSharedArena() );

   //Read in the points.
   for (i = 0; i < numVertices; i++){
      fscanf(infile, "%f %f %f", &(pointVerts[i][X]), &(pointVerts[i][Y]),
         &(pointVerts[i][Z]) );
   }

   fclose(infile);

   //Build a board for the points read in.
   gstate = pfNewGState (pfGetSharedArena());
   pfGStateMode (gstate, PFSTATE_ENTEXTURE, 0);
   pfGStateMode(gstate, PFSTATE_TRANSPARENCY, 1);

   gset = pfNewGSet(pfGetSharedArena());
   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts, NULL);
   pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, color, NULL);
   pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
  
   pfGSetPrimType(gset, PFGS_TRISTRIPS);

   pfGSetNumPrims(gset, numPrims);
 
   pfGSetPrimLengths(gset, length_vec);  
     
   pfGSetGState (gset, gstate);

   board = pfNewBboard();

   pfBboardAxis(board, axis);
   pfBboardMode(board, PFBB_ROT, PFBB_AXIAL_ROT);
   pfAddGSet(board,gset);

   pfAddGSet(board,gset);

   return(board);

   
}  //End of makeBBTri.


//  This function reads in a file and returns a billboard constructed from 
//  linestrips.
pfBillboard * makeBBLine(pfVec4 color, const char* infileName){

   pfGeoState  *gstate;
   pfGeoSet   *gset;
   pfBillboard *board;

   static pfVec3 axis = {0.0, 0.0, 1.0};
  
   int numPrims;

   int numVertices = 0;

   pfVec3 *pointVerts;

   FILE *infile;
 
   int *length_vec;

   char fileName[PF_MAXSTRING];

   pfFindFile ( (char *)infileName, fileName, R_OK );

   if(NULL == (infile =  fopen( fileName, "r"))){
      cerr<<"Unable to open "<<fileName<<endl;
      return(FALSE);
   }

   fscanf(infile, "%d", &numPrims);

   //  Get the number of primitives in the geode.
   if(NULL == (length_vec = (int *) pfMalloc(sizeof(int)*numPrims,
      pfGetSharedArena() ))){ return (NULL);
   }

   //  Add up the number of vertices in the geode.
   for (int i = 0; i < numPrims; i++){
      fscanf(infile, "%d", &(length_vec[i]) );
      numVertices += length_vec[i];
   }

   //  Allocate storage for the vertices.
   pointVerts = (pfVec3*) pfMalloc(( (numVertices) * sizeof(pfVec3) ),
      pfGetSharedArena() );

   //  Read in the points.
   for (i = 0; i < numVertices; i++){
      fscanf(infile, "%f %f %f", &(pointVerts[i][X]), &(pointVerts[i][Y]),
      &(pointVerts[i][Z]) );
   }

   fclose(infile);

   //  Build the billboard.
   gstate = pfNewGState (pfGetSharedArena());
   pfGStateMode (gstate, PFSTATE_ENTEXTURE, 0);
   pfGStateMode(gstate, PFSTATE_TRANSPARENCY, 1);

   gset = pfNewGSet(pfGetSharedArena());
   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts, NULL);
   pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, color, NULL);
   pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
  
   pfGSetPrimType(gset, PFGS_LINESTRIPS);

   pfGSetLineWidth(gset, 3.0);

   pfGSetNumPrims(gset, numPrims);
 
   pfGSetPrimLengths(gset, length_vec);  
     
   pfGSetGState (gset, gstate);

   board = pfNewBboard();

   pfBboardAxis(board, axis);
   pfBboardMode(board, PFBB_ROT, PFBB_AXIAL_ROT);
   pfAddGSet(board,gset);

   pfAddGSet(board,gset);

   return(board);
  
}  //End of makeBBLine.

//  This function creates a line strip which can be selected by a pick.
//  The function puts the points on a billboard
//  which is what we desire.  But performer 1.2 has a bug which prevents
//  picking through a billboard.  So a transparent backboard is added which
//  provides the geometry to pick.  We put them together in a group the go
//  up the group to find what we highlight.  But it's not even that simple.
//  we can't use performer's highlight with a billboard, so we use two geosets
//  one in the highlight color, one not.  Then we pick with a switch.  Its
//  overly complicated, but we had the luck of working where several performer
//  1.2 bugs merged.  New version of performer this will get much prettier.
pfGroup* makePickableLnStrip(pfVec4 color, const char* infileName, pfVec3
   point){

   pfGeoState  *gstate;
   pfGeoSet   *gset;
   pfBillboard *board,*board2;
   pfGeode *geode;
   pfDCS *DCS;
   pfSwitch *ptSwitch;
   pfGroup *the_group;
   pfVec3 *pointVerts;
   char fileName[PF_MAXSTRING];
   char quadfileName[PF_MAXSTRING];

   static pfVec4 transparent = {0.0, 0.0, 0.0, 0.0};
   static pfVec4 HLColor = {1.0, 0.0, 1.0, 1.0};

   int numPrims, numVertices = 0;

   FILE *infile;
 
   int *length_vec;

   pfFindFile( (char *)infileName, fileName, R_OK);

   //  Get the points for the backboard quad.  It's just a rectangle.
   //  But the functionality allows any shape to be read in.
   pfFindFile ( "quad.lst", quadfileName, R_OK );
   if(NULL == (infile =  fopen( quadfileName, "r"))){
      printf("Unable to open the file quad.lst");
      return(FALSE);
   }
   fscanf(infile, "%d", &numPrims);

   if(NULL == (length_vec = (int *) pfMalloc(sizeof(int)*numPrims,
      pfGetSharedArena() ))){ return (NULL);
   }

   for (int i = 0; i < numPrims; i++){
      fscanf(infile, "%d", &(length_vec[i]) );
      numVertices += length_vec[i];
   }

   pointVerts = (pfVec3*) pfMalloc(( (numVertices) * sizeof(pfVec3) ),
      pfGetSharedArena() );

   for (i = 0; i < numVertices; i++){
      fscanf(infile, "%f %f %f", &(pointVerts[i][X]), &(pointVerts[i][Y]),
         &(pointVerts[i][Z]) );
   }

   fclose(infile);

   //  Build the geostate for the quad.
   gstate = pfNewGState (pfGetSharedArena());
   //pfGStateMode (gstate, PFSTATE_TRANSPARENCY, PFTR_ON);
   //pfGStateMode (gstate, PFSTATE_TRANSPARENCY, PFTR_HIGH_QUALITY | 
   //   PFTR_NO_OCCLUDE);
   //pfGStateMode (gstate, PFSTATE_ENTEXTURE, 0);
   // Alpha function to block pixels of 0 alpha for transparent textures. 
   pfGStateMode (gstate, PFSTATE_ALPHAFUNC, PFAF_NOTEQUAL);
   //pfGStateMode (gstate, PFSTATE_ALPHAFUNC, PFAF_ALWAYS);
   pfGStateMode (gstate, PFSTATE_ALPHAREF, 0);
   pfGStateMode (gstate, PFSTATE_TRANSPARENCY, PFTR_MS_ALPHA | PFTR_NO_OCCLUDE);
   
   //  Make the geoset.
   gset = pfNewGSet(pfGetSharedArena());
   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts, NULL);
   pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   //pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OVERALL, normal, NULL);
   pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, transparent, NULL);
 
   pfGSetPrimType(gset, PFGS_QUADS);
   pfGSetNumPrims(gset, numPrims);
   pfGSetPrimLengths(gset, length_vec);       //HARDWIRED BAD!!! JUST FOR TEST!! 
   pfGSetGState (gset, gstate);


   //  Make billboards, we need two, one for each color of point.
   board = makeBBLine(color, fileName);
   board2 = makeBBLine(HLColor, fileName);

   //  Make geode for the backboard.
   geode = pfNewGeode();
   pfAddGSet(geode,gset);

   //  Need DCS to place the backboard correctly.
   DCS = pfNewDCS();
   pfAddChild(DCS, geode);

   //  Put everything in its correct position.
   pfBboardPos(board,0,point);
   pfBboardPos(board2,0,point);
   pfDCSTrans(DCS, point[X], point[Y], point[Z]);

   //  We need a switch to choose will geoset (highlighted or not) we are
   //  displaying.
   ptSwitch = pfNewSwitch();
   pfAddChild(ptSwitch,board);
   pfAddChild(ptSwitch,board2);
   pfSwitchVal( ptSwitch, 0);

   //  The overall group containing all this mess.  Add the visible portion and 
   //  the backboard for picking.
   the_group = pfNewGroup();
   pfAddChild(the_group,ptSwitch);
   pfAddChild(the_group, DCS);

   //  If there is any text, make a text node out of it.
   //if(text[0] != '&'){
   //   textnode = new PO_TEXT_CLASS (PO_TEXT,1);
   //   textnode->create_text_node(points[0],color,text);

   //   THIS IS A POTENTIAL MEMORY LEAK ******************
   //   pfAddChild(the_group,textnode->make_meas());
   //}

   //  Name what we need for picking.
   pfNodeName(ptSwitch, "point_switch");
   pfNodeName(the_group, "picked_group");

   return(the_group);
  
}  //  End of makePickableLnStrip.

//  This function creates a line strip which can be selected by a pick.
//  The function puts the points on a billboard
//  which is what we desire.  But performer 1.2 has a bug which prevents
//  picking through a billboard.  So a transparent backboard is added which
//  provides the geometry to pick.  We put them together in a group the go
//  up the group to find what we highlight.  But it's not even that simple.
//  we can't use performer's highlight with a billboard, so we use two geosets
//  one in the highlight color, one not.  Then we pick with a switch.  Its
//  overly complicated, but we had the luck of working where several performer
//  1.2 bugs merged.  New version of performer this will get much prettier.
pfGroup* makePickableTriStrip(pfVec4 color, const char* infileName, pfVec3
   point){

   pfGeoState  *gstate;
   pfGeoSet   *gset;
   pfBillboard *board,*board2;
   pfGeode *geode;
   pfDCS *DCS;
   pfSwitch *ptSwitch;
   pfGroup *the_group;
   pfVec3 *pointVerts;
   char fileName[PF_MAXSTRING];
   char quadfileName[PF_MAXSTRING];


   static pfVec4 transparent = {0.0, 0.0, 0.0, 0.0};
   static pfVec4 HLColor = {1.0, 0.0, 1.0, 1.0};

   int numPrims, numVertices = 0;

   FILE *infile;
 
   int *length_vec;

   pfFindFile( (char *)infileName, fileName, R_OK);


   //  Get the points for the backboard quad.  It's just a rectangle.
   //  But the functionality allows any shape to be read in.
   pfFindFile ( "quad.lst", quadfileName, R_OK );
   if(NULL == (infile =  fopen( quadfileName, "r"))){
      printf("Unable to open the file quad.lst");
      return(FALSE);
   }
   fscanf(infile, "%d", &numPrims);

   if(NULL == (length_vec = (int *) pfMalloc(sizeof(int)*numPrims,
      pfGetSharedArena() ))){ return (NULL);
   }

   for (int i = 0; i < numPrims; i++){
      fscanf(infile, "%d", &(length_vec[i]) );
      numVertices += length_vec[i];
   }

   pointVerts = (pfVec3*) pfMalloc(( (numVertices) * sizeof(pfVec3) ),
      pfGetSharedArena() );

   for (i = 0; i < numVertices; i++){
      fscanf(infile, "%f %f %f", &(pointVerts[i][X]), &(pointVerts[i][Y]),
         &(pointVerts[i][Z]) );
   }

   fclose(infile);

   //  Build the geostate for the quad.
   gstate = pfNewGState (pfGetSharedArena());
   pfGStateMode (gstate, PFSTATE_TRANSPARENCY, PFTR_MS_ALPHA | PFTR_NO_OCCLUDE);
   pfGStateMode (gstate, PFSTATE_ENTEXTURE, 0);
   // Alpha function to block pixels of 0 alpha for transparent textures. 
   pfGStateMode (gstate, PFSTATE_ALPHAFUNC, PFAF_NOTEQUAL);
   pfGStateMode (gstate, PFSTATE_ALPHAREF, 0);
   
   //  Make the geoset.
   gset = pfNewGSet(pfGetSharedArena());
   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts, NULL);
   pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
   //pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OVERALL, normal, NULL);
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, transparent, NULL);
 
   pfGSetPrimType(gset, PFGS_QUADS);
   pfGSetNumPrims(gset, numPrims);
   pfGSetPrimLengths(gset, length_vec);       //HARDWIRED BAD!!! JUST FOR TEST!! 
   pfGSetGState (gset, gstate);


   //  Make billboards, we need two, one for each color of point.
   board = makeBBTri(color, fileName);
   board2 = makeBBTri(HLColor, fileName);

   //  Make geode for the backboard.
   geode = pfNewGeode();
   pfAddGSet(geode,gset);

   //  Need DCS to place the backboard correctly.
   DCS = pfNewDCS();
   pfAddChild(DCS, geode);

   //  Put everything in its correct position.
   pfBboardPos(board,0,point);
   pfBboardPos(board2,0,point);
   pfDCSTrans(DCS, point[X], point[Y], point[Z]);

   //  We need a switch to choose will geoset (highlighted or not) we are
   //  displaying.
   ptSwitch = pfNewSwitch();
   pfAddChild(ptSwitch,board);
   pfAddChild(ptSwitch,board2);
   pfSwitchVal( ptSwitch, 0);

   //  The overall group containing all this mess.  Add the visible portion and 
   //  the backboard for picking.
   the_group = pfNewGroup();
   pfAddChild(the_group,ptSwitch);
   pfAddChild(the_group, DCS);

   //  If there is any text, make a text node out of it.
   //if(text[0] != '&'){
   //   textnode = new PO_TEXT_CLASS (PO_TEXT,1);
   //   textnode->create_text_node(points[0],color,text);

   //   THIS IS A POTENTIAL MEMORY LEAK ******************
   //   pfAddChild(the_group,textnode->make_meas());
   //}

   //  Name what we need for picking.
   pfNodeName(ptSwitch, "point_switch");
   pfNodeName(the_group, "picked_group");

   return(the_group);
  
}  //  End of makePickableTriStrip.



//*****************************************************************************
//Callbacks



void ballisticCB(MENU_LEVEL *level){
   standardCB(level);
}

void howitzerCB(MENU_LEVEL *level){
   standardCB(level);
}

void howitzermCB(MENU_LEVEL *level){
   standardCB(level);
}

void howitzerhCB(MENU_LEVEL *level){
   standardCB(level);
}


void downCB(MENU_LEVEL* level){
   pfSwitch *priorEdge;
   pfSwitch *HLSwitch;

   HLSwitch = (pfSwitch *)pfGetChild(level->nodeGroup, 0);
   pfSwitchVal(HLSwitch, 0);
   
   level->simSwitch(0);

   if (level->previousLevel != 0){
      
      //pfSwitchVal(level->nextLevelSwitch, 0);    //CAN'T PICK THROUGH SWITCH.

      priorEdge = (pfSwitch *)pfGetChild(level->previousLevel->edgesGroup,
         level->childNumber);
      pfSwitchVal(priorEdge, 0);                  //This is OK, we're not pickin
                                                 //through this guy.
   }
   else{
      menuOff(level);
   }
 
}

void rightCB(MENU_LEVEL*level){
   level->cumRotation += ROTATION_INCREMENT;
   if (level->cumRotation > 360.0) level->cumRotation -= 360.0;

   pfDCSRot(level->rotateDCS, level->cumRotation, 0.0, 0.0);
  
}

void leftCB(MENU_LEVEL* level){
   level->cumRotation -= ROTATION_INCREMENT;
   if (level->cumRotation < 360.0) level->cumRotation += 360.0;

   pfDCSRot(level->rotateDCS, level->cumRotation, 0.0, 0.0);
 
}

void unitCB(MENU_LEVEL* level){
   standardCB(level);   
 
}

void doneM198CB(MENU_LEVEL* level){

   pfVec3 position;
   pfMatrix tempMat;
   ballType ball = M198;
   
   pfGetDCSMat(L_menu_root->root, tempMat);

   pfGetMatRowVec3(tempMat, 3, position);

//for (int i= 0; i <4; i++){
//   for (int j=0; j<4; j++){
//cerr<<tempMat[i][j]<<" ";
//   }
//cerr<<endl;
//} 

//cerr<<position[0]<<" in done CB x from DCS"<<endl;
//cerr<<position[1]<<" in done CB y from DCS"<<endl;
//cerr<<position[2]<<" in done CB z from DCS"<<endl;

   //menuOff(L_menu_root);
   menuOff(level);
   ballOn(position, ball);
}

void doneM60CB(MENU_LEVEL* level){

   pfVec3 position;
   pfMatrix tempMat;
   ballType ball = M60;
   
   pfGetDCSMat(L_menu_root->root, tempMat);

   pfGetMatRowVec3(tempMat, 3, position);

//for (int i= 0; i <4; i++){
//   for (int j=0; j<4; j++){
//cerr<<tempMat[i][j]<<" ";
//   }
//cerr<<endl;
//} 

//cerr<<position[0]<<" in done CB x from DCS"<<endl;
//cerr<<position[1]<<" in done CB y from DCS"<<endl;
//cerr<<position[2]<<" in done CB z from DCS"<<endl;

   menuOff(level);
   ballOn(position, ball);
}

void done81mmCB(MENU_LEVEL* level){

   pfVec3 position;
   pfMatrix tempMat;
   ballType ball = Mort81mm;
   
   pfGetDCSMat(L_menu_root->root, tempMat);

   pfGetMatRowVec3(tempMat, 3, position);

//for (int i= 0; i <4; i++){
//   for (int j=0; j<4; j++){
//cerr<<tempMat[i][j]<<" ";
//   }
//cerr<<endl;
//} 

//cerr<<position[0]<<" in done CB x from DCS"<<endl;
//cerr<<position[1]<<" in done CB y from DCS"<<endl;
//cerr<<position[2]<<" in done CB z from DCS"<<endl;

   menuOff(level);
   ballOn(position, ball);
}

#endif
