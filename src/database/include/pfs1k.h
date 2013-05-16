/*
 * COPYRIGHT 1994 TEXAS INSTRUMENTS INCORPORATED, ALL RIGHTS RESERVED.
 * THIS MATERIAL MAY BE REPRODUCED BY OR FOR THE U.S. GOVERNMENT PURSUANT
 * TO THE COPYRIGHT LICENSE UNDER CLAUSE AT DFARS 252.227-7013 (OCT 1988).
 *
 * TEXAS INSTRUMENTS INCORPORATED, Defense Systems and Electronics Group,
 * ATTN: Randy Boys, P.O. Box 869305, M/S 8518, Plano, Texas 75086
 *
 * FILE
 *
 *   pfs1k.h - S1KE Loader interface definition
 *
 *
 * DESCRIPTION
 *
 *   This file interface to the "S1KE Loader" - a Performer loader for
 *   S1000 databases.
 *
 *   Control File
 *
 *   Since the S1000 database is accessed through an API, this loader accepts
 *   a control file (rather than a data file) that contains keyword value pairs
 *   that describe which database to load as well as parameters for narrowing and
 *   otherwise controlling the load.  See the s1kextract/docs directory for
 *   a description of the control file.
 *
 *
 * ENVIRONMENT VARIABLES
 *
 *   The following environment variables must be set if not specified in the
 *   control file.  Values set in the control file have precedence over
 *   environment variables.
 *
 *	S1KPROJ - S1000 project directory (top level - eg. /usr/s1000/proj)
 *
 *   The following environment variables may be used to control the load.
 *
 *      S1KE_SHOWBACKFACE - don't cull backfaces
 *      S1KE_VERBOSE - show extract loading messages
 *      S1KE_DEBUG - show debugging information
 *
 *
 * NOTES
 *
 *   This software uses the pfdGeoBuilder utilities for sorting/creating pfGeoSets.
 *
 *   Limitations
 *
 *     This software will only run under Performer 1.2.
 *
 *   Coordinate Frames
 *
 *     The default output reference frame is assembly coordinates (ASSY).
 *
 *     If the -o option is specified then the S1000 coordinates will be translated
 *     from ASSY to this origin (known as the user reference frame (USR)).  When
 *     the -o option is in effect, the output will always be in USR.
 *
 *     If -UTM is specified and -o is not, then the output is in UTM.  That is,
 *     the default origin is (0,0).
 *
 *     The default reference frame for the search window and origin inputs is ASSY.
 *     The command line option -UTM indicates that these inputs should be interpreted
 *     in UTM coordinates.
 *
 *   Performer 1.2 Bugs
 *
 *     - Cannot add multiple Geosets to the same Billboard.  Dies in the first CULL.
 *     - Cannot flatten a hierarcy with instanced model geometry.
 *     - Cannot compute the bounding box of a Geode with no attached Geosets
 *
 * ORIGINAL AUTHOR
 *
 *   Todd Pravata, VSL @ Texas Instruments, 6/94
 *
 * $Source: /hosts/crusty/perf/trees/perf2.0/perf/lib/libpfdb/libpfs1k/include/RCS/pfs1k.h,v $
 * $Author: hatch $
 * $Date: 1995/11/28 06:51:51 $
 * $Revision: 1.1 $
 * $State: Exp $
 * $Locker:  $
 *
 * $Log: pfs1k.h,v $
 * Revision 1.1  1995/11/28  06:51:51  hatch
 * Initial revision
 *
 * Revision 1.2  1995/07/20  19:36:09  tpravata
 * added functions for paging loader.
 *
 * Revision 1.1  1995/02/08  23:06:16  tpravata
 * Initial revision
 *
 * 
 */

#ifndef _PFS1K_H_
#define _PFS1K_H_

#include <Performer/pf.h>

/*
 * CONSTANTS
 *
 *   OTW_TRAV_MASK THERMAL_TRAV_MASK
 *
 * DESCRIPTION
 *
 *   Traversal masks for obtaining simultaneous OTW and THERMAL views
 *   The out-the-window (OTW) specific nodes in the graph will have their
 *   traversal masks set to OTW_TRAV_MASK and similarly for THERMAL specific
 *   nodes.
 *
 * USAGE (eg.)
 *
 *   pfChanTravMask((pfChannel *)chan, (long)PFTRAV_DRAW, (ulong)OTW_TRAV_MASK);
 *
 * NOTES
 *
 *   By default all nodes have their traversal masks set to 0xFFFFFFFF.
 *
 * ORIGINAL AUTHOR
 *   Todd Pravata, Texas Instruments, 11/94
 */

#define OTW_TRAV_MASK     0x00000001
#define THERMAL_TRAV_MASK 0x00000010

/*
 * CONSTANTS
 *
 *   TERRAIN_IMASK TREE_IMASK NETWORK_IMASK MODEL_IMASK
 * 
 * DESCRIPTION:
 *
 *   Intersection masks for eliminating nodes from the intersection
 *   tests.  Scene graph nodes of the specified type of geometry
 *   have their intersection masks set appropriately (PFTRAV_DESCEND).
 *
 * USAGE (eg.)
 *
 *   segset.activeMask = 1;
 *   segset.isectMask = TERRAIN_IMASK|NETWORK_IMASK;
 *   segset.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM|PFTRAV_IS_CULL_BACK;
 *    ...
 *   isect = pfSegsIsectNode(root, &segset, hits);
 *
 * NOTES
 *
 * ORIGINAL AUTHOR
 *   Todd Pravata, Texas Instruments, 1/95
 */

#define TERRAIN_IMASK 0x10000000
#define TREE_IMASK    0x20000000
#define NETWORK_IMASK 0x40000000
#define MODEL_IMASK   0x80000000

/*
 * CONSTANT
 *
 *   DB_VIEW
 * 
 * DESCRIPTION:
 *
 *   Specify the type of view to be loaded into the scene graph. *
 *
 * NOTES
 *
 *   The default view is OTW.
 *
 * ORIGINAL AUTHOR
 *   Todd Pravata, Texas Instruments, 11/94
 */

typedef enum 
{
  DB_VIEW_OTW = 0,    /* Out-the-window view - full color */
  DB_VIEW_THERMAL,    /* Thermal (FLIR) view - monochrome */
  DB_VIEW_BOTH,       /* Scene graph contains both views - use traversal mask (above) */
  DB_VIEW_DEFAULT,    /* View type obtained from the loader control file/default */
  NUM_DB_VIEWS
} DB_VIEW; 

#ifndef _PERF_2_0_
/*
 * FUNCTION
 *
 *   LoadS1K(char *filename, pfGeostate *geostate)
 *
 * DESCRIPTION
 *
 *   This is the public function used to load S1000 databases.
 *   The input file should be an S1K Loader control file (see
 *   above).
 *
 * ORIGINAL AUTHOR
 *   Todd Pravata, Texas Instruments, 6/94
 */

pfNode *LoadS1K(char *filename, pfGeoState *geostate);

#else /* _PERF_2_0_ */

/*
 * FUNCTION
 *
 *   pfdLoad_s1k(char *filename)
 *
 * DESCRIPTION
 *
 *   This is the public function used to load S1000 databases.
 *   The input file should be an S1K Loader control file (see
 *   above).
 *
 * ORIGINAL AUTHOR
 *   Todd Pravata, Texas Instruments, 2/95
 */

extern pfNode *pfdLoadFile_s1k(char *control_filename);

/* FUNCTION
 * 
 *   pfdUpdateTiles_s1k (API)
 * 
 * DESCRIPTION
 * 
 *   Process the S1K assembly quadtree loading marking the tiles
 *   that are outside of the viewing range to be deleted and
 *   tiles that have come inside the viewing range to be added.
 * 
 *   The xyz,hpr specify the postion and direction of the eyepoint
 *   and range is the desired viewing range for the database.
 * 
 *   See the man page for pfBuffer for details.
 *
 * ORIGINAL AUTHOR
 *
 *   Todd Pravata, Texas Instruments, 4/95
 */

void pfdUpdateTileStatus_s1k(pfVec3 xyz, float range);

/* FUNCTION
 * 
 *   pfdDBaseFunc_s1k (API)
 * 
 * DESCRIPTION
 * 
 *   Delete tiles marked for deletion.  Load tiles marked for loading.
 *   Note that the data parameter is not used.
 * 
 *   See the man page for pfDBaseFunc for details.
 *
 * ORIGINAL AUTHOR
 *
 *   Todd Pravata, Texas Instruments, 4/95
 */

void pfdDBaseFunc_s1k(void *data);

#endif /* _PERF_2_0_ */

#endif
