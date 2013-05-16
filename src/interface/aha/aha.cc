/* 
        "U.S. Government Proprietary.  This software is protected by U.S.
Patent application owned by the U.S. Government and exclusively licensed to
private enterprise for commercial purposes.  This license reserves the right
to the U.S. Government to practice the licensed invention by or on behalf of
the U.S. Government."

   By Jiang Zhu, Nov. 15, 1994

   The following comments are from the original AHA algorithm in SceneMan.
   The useful information are kept here in case that later versions of
   NPSNET enables the calls to the Performer routines and we want to use them.

   Requirements, restrictions for using libaha.a:
   * Call pfuInitUtil(), pfuInitInput(), ahaConfig() and ahaInit()
     before starting the main simulation loop.
   * Call pfuCollectInput() once each time through the main loop.
   * Call ahaExit() before program exits.

   By me,

   * In multiprocessing mode, the calls to pfuInitUtil() and ahaConfig() must 
     be made before the call to pfConfig() so that the shared memory are
     accesible to all processes. Specifically, for the AHA algorithm here,
     the local-global variable ahaShared must be accessible in all processes.
     pfuInitUtil() may initialize some variables shared by all processes.
     what are they? I am not sure, just maybe. This may also apply to 
     pfuInitInput().


   The followings are the new interfaces of the public functions.

     Public functions:

	void    ahaConfig();
	void    ahaInit( pfChannel * );
	void    ahaExit();
	void    ahaUpdate( long, long );
	void    ahaAddSelectableObject( pfNode *, int, long );
	void    ahaRemoveSelectableObject( pfNode * );
	void    ahaSelectObject();
	void    ahaDeselectObject();
	void    ahaSetMode( AhaMode );
	void    ahaSetClass( long );
	pfNode* ahaGetSelectedObject();
	int     ahaGetSelectedObjIndex();
        int     ahaGetPreSelectedObjIndex();

*/


#include <iostream.h>
#include "aha.h"

#define FLT_MAX_DIST	500000.0
#define MAX_SELECTABLE_OBJECTS 100


typedef struct {
  float	nmouseX, nmouseY;	// normalized mouse position
  int	inWin;
} ahaMouse;

typedef struct {
  pfChannel	*MasterChan;
  ahaMouse	Mouse;

  pfNode	*SelectedObject;
  int		SelectedObjIndex;
  pfNode	*PreselectedObject;
  int		PreselectedObjIndex;
  pfNode	*NextPreselectedObject;
  int		NextPreselectedObjIndex;

  pfNode	*SelectableObjects[MAX_SELECTABLE_OBJECTS];
  int		SelectableObjIndex[MAX_SELECTABLE_OBJECTS];
  long		SelectableObjClass[MAX_SELECTABLE_OBJECTS];
  pfVec3        SelectableObjCenter[MAX_SELECTABLE_OBJECTS];

  long		Class;
  long		NumObjects;
  AhaMode	Mode;

  pfHighlight	*PreselectHL;
  pfHighlight	*SelectHL;
  float		NextPreselectedDist;
//  New flags to ensure that highlight gets turned off
  int           SelectHLFlag;
  int           PreselectHLFlag;
} AhaShared;

volatile AhaShared *ahaShared;

// private functions

static void	ahaGetLdObjWCNear( pfVec3, pfVec3 );
static void	ahaGetMouseWCNear( pfVec3 );
static void	ahaPickAHAPreselect( void );
static int	ahaObjectInCurrentClass( pfNode * );
static int 	ahaPreDraw( pfTraverser *, void * );
static int 	ahaPostDraw( pfTraverser *, void * );

void
ahaConfig()
{
  /* called once before beginning the main simulation loop,
   * in multiprocessing mode, must be called before pfConfig()
   */
  ahaShared = (AhaShared *)pfMalloc( sizeof(AhaShared), pfGetSharedArena() );
  ahaShared->PreselectHL = pfNewHlight( pfGetSharedArena() );
  ahaShared->SelectHL    = pfNewHlight( pfGetSharedArena() );
}

void 
ahaInit( pfChannel *chan )
{
  // called once before beginning the main simulation loop

  ahaShared->NumObjects = 0;
  ahaShared->MasterChan = chan;
  ahaShared->Mode = AHA_OFF;
  ahaShared->Class = ~(0l);

  ahaShared->SelectedObject = NULL;
  ahaShared->SelectedObjIndex = -1;
  ahaShared->PreselectedObject = NULL;
  ahaShared->PreselectedObjIndex = -1;
  ahaShared->NextPreselectedObject = NULL;
  ahaShared->NextPreselectedObjIndex = -1;

  ahaShared->Mouse.inWin = 0;

  ahaShared->SelectHLFlag = FALSE;
  ahaShared->PreselectHLFlag = FALSE;

  pfHlightMode ( ahaShared->PreselectHL, PFHL_LINESPAT );
  pfHlightColor( ahaShared->PreselectHL, PFHL_FGCOLOR, 1.0f, 0.0f, 0.0f );
  pfHlightColor( ahaShared->PreselectHL, PFHL_BGCOLOR, 0.0f, 1.0f, 1.0f );

  pfHlightMode ( ahaShared->SelectHL, PFHL_LINESPAT );
  pfHlightColor( ahaShared->SelectHL, PFHL_FGCOLOR, 1.0f, 1.0f, 0.0f );
  pfHlightColor( ahaShared->SelectHL, PFHL_BGCOLOR, 0.0f, 0.0f, 1.0f );

}

void 
ahaAddSelectableObject( pfNode *object, int obj_index, long aha_class )
{

#ifdef __AHA_DEBUG__
  cerr << "add AHA object" << endl;
#endif

  if (aha_class == 0l) return;

  if (ahaShared->NumObjects < MAX_SELECTABLE_OBJECTS) 
  {
     ahaShared->SelectableObjects[ahaShared->NumObjects] = object;
     ahaShared->SelectableObjIndex[ahaShared->NumObjects] = obj_index;
     pfSetVec3 ((float *)ahaShared->SelectableObjCenter[ahaShared->NumObjects],
                0.0f, 0.0f, 0.0f);
     ahaShared->SelectableObjClass[ahaShared->NumObjects++] = aha_class;

     pfNodeTravFuncs(object, PFTRAV_DRAW, ahaPreDraw, ahaPostDraw);
     // not used for now
     // pfNodeTravData (object, PFTRAV_DRAW, something);

     /* the comment from the original code
      * clg 4aug94 set mask for picking on all selectable objects 
      */
//     pfNodeTravMask(object, PFTRAV_ISECT, PFIS_SET_PICK, PFTRAV_SELF | PFTRAV_DESCEND, PF_OR);

  }
  else 
  {
    cerr << "AHA error: maximum number of selectable objects "
	 << ahaShared->NumObjects
	 << endl;
  }
}

void ahaRemoveSelectableObject( pfNode *object )
{

#ifdef __AHA_DEBUG__
  cerr << "remove AHA object" << endl;
#endif

  for (int i = 0; i < ahaShared->NumObjects; i++) {
    if (object == ahaShared->SelectableObjects[i]) {

      ahaShared->SelectableObjects[i] = 
	ahaShared->SelectableObjects[--(ahaShared->NumObjects)];
      ahaShared->SelectableObjIndex[i] = 
	ahaShared->SelectableObjIndex[ahaShared->NumObjects];
      ahaShared->SelectableObjClass[i] = 
	ahaShared->SelectableObjClass[ahaShared->NumObjects];
      pfCopyVec3 ((float *)ahaShared->SelectableObjCenter[i],
        (float *)ahaShared->SelectableObjCenter[ahaShared->NumObjects]);

      /* the comment from the original code
       * clg 4aug94 unset mask for picking on all selectable objects 
       * Setting to 0x0 because default was ffffffff and not sure what else to do
       */
//      pfNodeTravMask(object, PFTRAV_ISECT, 0x0, PFTRAV_SELF, PF_SET);

      if (object == ahaShared->SelectedObject) 
      {
	ahaShared->SelectedObject = NULL;
	ahaShared->SelectedObjIndex = -1;
      }

      /* the comment from the original code
       * Do we have to undo the pfNodeTravFuncs? 
       */

      return;
    }
  }

  cerr << "AHA error: not found in selectable list: not removed." << endl;
}

void ahaExit()
{
  pfFree((void *)ahaShared);
}

void 
ahaUpdate( long mouse_x, long mouse_y )
{

  pfSphere bsphere;

  // called before each draw trversal

#ifdef __AHA_DEBUG__
  cerr << "update AHA" << endl;
#endif

  for (int i = 0; i < ahaShared->NumObjects; i++) {
      pfGetNodeBSphere( ahaShared->SelectableObjects[i], &bsphere );
      pfCopyVec3((float *) ahaShared->SelectableObjCenter[i], bsphere.center );
  }

/*
  The Performer utility routines can't be called as in the original code
  because in NPSNET, the device queqe is directly handled.

  pfuCollectInput();
  pfuGetMouse(&(ahaShared->Mouse));
  pfuMapMouseToChan(&(ahaShared->Mouse), ahaShared->MasterChan);
*/

  /* a note on the channel origin:
   * Currently, NPSNET does not update its channel origin when the window
   * is moved. This should be fixed later.
   * Currently, we can fool the AHA algorithm by offsetting the mouse
   * coordinate pased in.
   */
  int vp_x_org, vp_y_org, vp_x_size, vp_y_size;
  pfGetChanOrigin(ahaShared->MasterChan, &vp_x_org, &vp_y_org);
  pfGetChanSize(ahaShared->MasterChan, &vp_x_size, &vp_y_size);


  if ((mouse_x >= vp_x_org) && (mouse_x <= (vp_x_org + vp_x_size)) &&
      (mouse_y >= vp_y_org) && (mouse_y <= (vp_y_org + vp_y_size)))
  {
    ahaShared->Mouse.inWin = 1;

    ahaShared->Mouse.nmouseX = (float)(mouse_x - vp_x_org)/(float)vp_x_size; 
    ahaShared->Mouse.nmouseY = (float)(mouse_y - vp_y_org)/(float)vp_y_size; 
  } 
  else
  { 
    ahaShared->Mouse.inWin = 0;
  }

  ahaShared->PreselectedObject = ahaShared->NextPreselectedObject;
  ahaShared->PreselectedObjIndex = ahaShared->NextPreselectedObjIndex;
  ahaShared->NextPreselectedObject = NULL;
  ahaShared->NextPreselectedObjIndex = -1;
  ahaShared->NextPreselectedDist = FLT_MAX_DIST;

  if (ahaShared->Mode == AHA_OFF) {
    ahaShared->SelectedObject = NULL;
    ahaShared->SelectedObjIndex = -1;
    ahaShared->PreselectedObject = NULL;
    ahaShared->PreselectedObjIndex = -1;
    ahaShared->NextPreselectedObject = NULL;
    ahaShared->NextPreselectedObjIndex = -1;
  }
  /* As I pointed out in the README file, AHA_PICK and AHA_MIXED modes are not
   * working in NPSNET due to bugs in the Performer.
   */
  else if (((ahaShared->Mode == AHA_PICK) || (ahaShared->Mode == AHA_MIXED)) &&
	   (ahaShared->Mouse.inWin))
    ahaPickAHAPreselect(); 

#ifdef __AHA_DEBUG__
  cerr << "finish AHA update" << endl;
#endif
}

void
ahaSelectObject()
{
  if (ahaShared->Mode != AHA_OFF)
  {
    ahaShared->SelectedObject = ahaShared->PreselectedObject;
    ahaShared->SelectedObjIndex = ahaShared->PreselectedObjIndex;
  }
}

void
ahaDeselectObject()
{
  ahaShared->SelectedObject = NULL;
  ahaShared->SelectedObjIndex = -1;
}

void
ahaSetMode( AhaMode mode )
{
//  static char *modename[] = {"off","2D","3D","pick","mixed"};

  ahaShared->Mode = mode;
//  cerr << "current AHA mode is" << modename[mode] << endl;
}

void
ahaSetClass( long aha_class )
{
  ahaShared->Class = aha_class;
}

/* the comments from the original code 
 *
 * In the draw mode this function will be called for every selectable node.  
 * It will determine which object is currently closest to the mouse. It will
 * call this the NextPreselectedNode. In addition it highlights the last 
 * pre-selected and selected objects.
 *
 * Another way would have been to let predraw pick the closest node.  Then the
 * draw process callback could add the highlighting before it calls pfDraw().
 */
 
int ahaPreDraw( pfTraverser *trav, void * /* data */ )
{
  /* Only called if the object is in the viewing frustum.
   * Putting AHA test here ensures that only visible objects are tested .
   */

#ifdef __AHA_DEBUG__
  cerr << "AHA predraw" << endl;
#endif
  if (ahaShared->Mode == AHA_OFF) return PFTRAV_CONT;

  pfNode *object = pfGetTravNode(trav);

  if (!ahaObjectInCurrentClass(object)) return PFTRAV_CONT;

  int theIndex;
  pfVec3 theCenter;

  for (int i = 0; i < ahaShared->NumObjects; i++) {
     if (ahaShared->SelectableObjects[i] == object) {
        theIndex = ahaShared->SelectableObjIndex[i];
        pfCopyVec3 (theCenter, (float *)ahaShared->SelectableObjCenter[i]);
        break;
     }
  }

  if ((ahaShared->Mode == AHA_TWO_D) ||
      (ahaShared->Mode == AHA_THREE_D) ||
      (ahaShared->Mode == AHA_MIXED && ahaShared->NextPreselectedDist > 0.0)) {

//    pfSphere bsphere;
    pfVec3 mouseNearWC, objNearWC, objCenterWC;
    float mouseObjectDist;
    
    if (ahaShared->Mouse.inWin) {
//      pfGetNodeBSphere( object, &bsphere );
      ahaGetMouseWCNear( mouseNearWC );
      
      switch (ahaShared->Mode) {
      case AHA_TWO_D:
      case AHA_MIXED:
	ahaGetLdObjWCNear( theCenter, objNearWC );
	mouseObjectDist = pfDistancePt3( mouseNearWC, objNearWC );
	break;
      case AHA_THREE_D:
	pfCopyVec3( objCenterWC, theCenter);
	mouseObjectDist = pfDistancePt3( mouseNearWC, objCenterWC );
	break;
      }
      
      if (((ahaShared->NextPreselectedObject == NULL) || 
	   (ahaShared->NextPreselectedDist > mouseObjectDist)) &&
	  ((ahaShared->Mode != AHA_THREE_D) ||
	   ((ahaShared->Mode == AHA_THREE_D) && 
	    (object != ahaShared->SelectedObject))))
      {
	ahaShared->NextPreselectedObject = object;
        for (int i = 0; i < ahaShared->NumObjects; i++) 
	{
	   ahaShared->NextPreselectedObjIndex = theIndex;
	   break;
        }

	ahaShared->NextPreselectedDist = mouseObjectDist;
      }
    }
  }
    
  // Add highlights if node is selected or preselected 
  if (object == ahaShared->SelectedObject) {
    pfPushState();
    pfEnable( PFEN_HIGHLIGHTING );
    pfApplyHlight( ahaShared->SelectHL );
// This node was selected make cerrtain Post Call back turn off the high ligh
    ahaShared->SelectHLFlag = TRUE;
  }
  else if (object == ahaShared->PreselectedObject) {
    pfPushState();
    pfEnable( PFEN_HIGHLIGHTING );
    pfApplyHlight( ahaShared->PreselectHL );
// This node was preselected make cerrtain Post Call back turn off the high ligh
    ahaShared->PreselectHLFlag = TRUE;
  }

#ifdef __AHA_DEBUG__
  cerr << "finish AHA predraw" << endl;
#endif

  return PFTRAV_CONT;
}    

 
int ahaPostDraw(pfTraverser *trav, void * /* data */ )
{

#ifdef __AHA_DEBUG__
  cerr << "AHA postdraw" << endl;
#endif

  pfNode *object = pfGetTravNode(trav);

  if (ahaShared->Mode == AHA_OFF) return PFTRAV_CONT;

//  if (object == ahaShared->SelectedObject) {
// Make certain that high light is turned off.
  if (ahaShared->SelectHLFlag == TRUE) {
    ahaShared->SelectHLFlag = FALSE;
    pfDisable(PFEN_HIGHLIGHTING);
    pfPopState();
  }
//  else if (object == ahaShared->PreselectedObject) {
// Make ceratin that preselect hilgh light is turned off.
  else if (ahaShared->PreselectHLFlag == TRUE) {
    ahaShared->PreselectHLFlag = FALSE;
    pfDisable(PFEN_HIGHLIGHTING);
    pfPopState();
  }

  return PFTRAV_CONT;
}

pfNode*
ahaGetSelectedObject()
{
  return ahaShared->SelectedObject;
}

int
ahaGetSelectedObjIndex()
{
  return ahaShared->SelectedObjIndex;
}

/* Added by NPS, Paul Barham */
int
ahaGetPreSelectedObjIndex()
{
  return ahaShared->PreselectedObjIndex;
}


/* If in nonaha mode then pre-select close object under the mouse.
 *
 * As I commented in the README file, this is not working in NPSNET
 * because of the existence of the SWITCH nodes in the NPSNET scene
 * greph. This is caused by bugs in the Performer function pfChanPick().
 */
static void
ahaPickAHAPreselect()
{
    float 	nx,ny;
    long 	mode;
//    long 	result;
//    pfHit 	**hitlist[10];

/* 
 * AHA pick mode is not working in NPSNET due to bugs in pfChanPick().
 * Don't try this in NPSNET. The program crushes in multiprocessing mode.
 *
 */

    
    /* Not in NONAHA mode so return */
    if (ahaShared->Mode != AHA_PICK &&
	ahaShared->Mode != AHA_MIXED) /* clg 3aug94 add hybrid mode */
	return;	
    
    /* In AHA_PICK or AHA_MIXED mode so see if there is an object under the mouse
     * and take the closest one.
     */
    
/* 
    original code when pfuGetMouse is called in ahaUpdate

    compute normalized coordinates: 0 to 1
    nx = (1.0f + ahaShared->Mouse.xchan)/2.0f;
    ny = (1.0f + ahaShared->Mouse.ychan)/2.0f;
*/
    nx = ahaShared->Mouse.nmouseX;
    ny = ahaShared->Mouse.nmouseY;
    
    /* pick on geometry and only take the nearest hit */
    mode = PFTRAV_IS_PRIM | PFTRAV_IS_PATH | PFPK_M_NEAREST;

/*
    if ((ahaShared->MasterChan != NULL) && (pfGetChanScene(ahaShared->MasterChan) != NULL)) {
	result = pfChanPick(ahaShared->MasterChan, mode, nx, ny, 0.0f, hitlist);

	if (result > 0) {
	    pfPath *path;
	    pfQueryHit(*hitlist[0], PFQHIT_PATH, (float *) &path);

	    if (path) {

		** Find selectable node farthest along in path --
		   just in case one selectable node is the child of another **

		long maxppos = 0;
		for (int i = 0; i < ahaShared->NumObjects; i++) {

		    ** pfSearch returns -1 if not in path **

		    long ppos = pfSearch(path, ahaShared->SelectableObjects[i]);
		    if (ppos > maxppos) {
			ahaShared->NextPreselectedObject = ahaShared->SelectableObjects[i];
			ahaShared->NextPreselectedObjIndex = ahaShared->SelectableObjIndex[i];
			ahaShared->NextPreselectedDist = 0.0;
			maxppos = ppos;
		    }
		}
	    }
	} 
    }
*/
}

static int
ahaObjectInCurrentClass( pfNode * object )
{
  if (ahaShared->Class == ~(0l)) 
    return TRUE;
  else 
  {
    for (int i = 0; i < ahaShared->NumObjects; i++) 
    {
      if (ahaShared->SelectableObjects[i] == object) 
      {
	int inClass = (ahaShared->SelectableObjClass[i] && ahaShared->Class) != 0;

	return inClass;
      }
    }

    return FALSE;
  }
}

static void 
ahaGetMouseWCNear( pfVec3 mouseNearWC )
{

/*
  original code when pfuGetMouse() is called in ahaUpdate

  float scalex = (1.0f + vsMouse.xchan) / 2.0f;
  float scaley = (1.0f + vsMouse.ychan) / 2.0f;
*/

  float scalex = ahaShared->Mouse.nmouseX;
  float scaley = ahaShared->Mouse.nmouseY;

  pfVec3  lowerLeftWC, lowerRightWC, upperLeftWC, upperRightWC;
  pfVec3  windowXWC, windowYWC, windowMouseWC;

static pfFrustum *theFrustum = pfNewFrust(pfGetSharedArena());
pfGetChanBaseFrust (ahaShared->MasterChan, theFrustum);
pfGetFrustNear( theFrustum,
                lowerLeftWC,lowerRightWC, upperLeftWC, upperRightWC );

//  pfGetFrustNear( (pfFrustum *)ahaShared->MasterChan,
//			lowerLeftWC,lowerRightWC, upperLeftWC, upperRightWC );

  pfSubVec3( windowXWC, lowerRightWC, lowerLeftWC);
  pfSubVec3( windowYWC, upperLeftWC,  lowerLeftWC);
    
  pfCombineVec3( windowMouseWC, scalex, windowXWC, scaley, windowYWC );
  pfAddVec3( mouseNearWC, lowerLeftWC, windowMouseWC );
}

static void 
ahaGetLdObjWCNear(pfVec3 objWC, pfVec3 objNearWC)
{
  pfVec3 eyeWC;
  pfVec3 lowerLeftWC, lowerRightWC, upperLeftWC, upperRightWC;
  pfSeg eyeToObjSeg;
  pfPlane nearPlane;
  float isectDist;
  pfVec3 objWCMinusEyeWC, qVec;

  pfGetFrustNear( (pfFrustum *)ahaShared->MasterChan, lowerLeftWC, lowerRightWC, 
		 upperLeftWC, upperRightWC );
  pfGetFrustEye( (pfFrustum *)ahaShared->MasterChan, eyeWC );

  pfMakePtsSeg( &eyeToObjSeg, eyeWC, objWC );
  pfMakePtsPlane( &nearPlane, lowerLeftWC, lowerRightWC, upperLeftWC );

  pfPlaneIsectSeg( &nearPlane, &eyeToObjSeg, &isectDist );
  pfSubVec3( objWCMinusEyeWC, objWC, eyeWC );
  pfScaleVec3( qVec, isectDist/pfLengthVec3(objWCMinusEyeWC), objWCMinusEyeWC );
  pfAddVec3( objNearWC, eyeWC, qVec );
}

