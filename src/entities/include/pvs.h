/***************************************************************************\
Title: pvs.h
Author:  Bryan Stewart
Date:    11/29/95

Purpoose: A hybrid pvs algorithm from UNC pfPortals and the current pvs.h
\*****************************************************************************/

#ifndef __PVS_H__
#define __PVS_H__

#include <Performer/pf.h>
#include <pfutil.h>	// For pfuTraverser stuff
    
#define  MAXPVSCELLS     27

typedef struct
{
   int		list[MAXPVSCELLS];
} PVSData;

typedef struct
{
    int           lastcell;		// The last cell view within
    pfBox	  *bboxes[MAXPVSCELLS];	// The extents of each cell
    pfSwitch	  *cells[MAXPVSCELLS];  // The displayable cells
    //pfBox        *lastbbox;		// The last cell we were in
    PVSData       *cellinfo[MAXPVSCELLS];// The list of visible cells
} LocatorData;

/* Callbacks: */

//extern long EnterLocatorCull(pfTraverser *trav, void *data);
//extern long pfEnterLocatorDraw(pfTraverser *trav, void *data);
//extern long pfExitLocatorDraw(pfTraverser *trav, void *data);
//extern long EnterCellIsect(pfTraverser *trav, void *data);
//extern long EnterCellCull(pfTraverser *trav, void *data);
//extern long pfExitCellDraw(pfTraverser *trav, void *data);

extern pfGroup  *initPVS();
extern void updatePVS(pfGroup *, pfCoord);
extern void addWaterCell(pfSwitch *);
extern void removeWaterCell();

/*
 * Globals, visible outside the library:
 */
//extern pfGroup 	*Locator;		// The global Locator node
//extern pfSwitch	*PVSCells[MAXPVSCELLS];
extern pfGroup	*PVSCells[MAXPVSCELLS];

#endif
