/* Copyright (c) 1996, Carnegie Mellon University
 *
 *    Digital Mapping Laboratory (MAPSLab)
 *    Computer Science Department
 *    Carnegie Mellon University
 *    5000 Forbes Avenue
 *    Pittsburgh, PA 15213-3890
 *    maps@cs.cmu.edu
 *    http://www.cs.cmu.edu/~MAPSLab
 *
 *
 * Permission to use, copy, and modify this software and its
 * documentation for any NON-COMMERCIAL purpose is hereby granted
 * without fee, provided that (i) the above copyright notices and the
 * following permission notices appear in ALL copies of the software
 * and related documentation, and (ii) The Digital Mapping Laboratory 
 * (MAPSLab) Computer Science Department, Carnegie Mellon University
 * be given written credit in your software's written documentation and
 * be given graphical credit on any start-up/credit screen your
 * software generates.
 *
 * We request that users of this software to return to maps@cs.cmu.edu
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 *
 * Redistribution of this code is forbidden without the express written 
 * consent of the MAPSLab Research Group. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 */
/* Read/Write TSG (Tiled Scene Graph) files for Performer.
 *****************************************************************
 * HISTORY
 *  26-Mar-96  Stephen Gifford (sjg)  Digital Mapping Laboratory
 *      School of Computer Science at Carnegie-Mellon University
 *      Created.
 */

#ifndef _tsgpf_h_
#define _tsgpf_h_

#include <tsg.h>
#include <tsgBuf.h>
#include <Performer/pf.h>
#include <Performer/pfdu.h>

#ifdef __cplusplus
extern "C" {
#endif

/* This was left out of the C header files,
 * but is in the C++ pr/pfFog.h file
 */
#ifndef PFFOG_MAXPOINTS
#define PFFOG_MAXPOINTS               12
#endif /* PFFOG_MAXPOINTS */

/* context for reading or writing a TSG file */
typedef struct _tsgContext {
  /* Not stored in file */
  int indexed;          /* Set if the dbase should use indexed geostates */

  /* Stored in file */
  char path[1024];      /* Path to file */
  char *source;		/* source of file */
  char *comment;	/* free text description of file */
  double ll_x;		/* x offset of lower left corner */
  double ll_y;		/* y offset of lower left corner */
 short zone;     /* UTM zone (1-60) or projection code */
 char *projinfo;       /* projection parameters (Zone latter for UTM) */
 char *datum;          /* horizontal map datum */
  /* --- also store datum, ellipsoid, projection, projection parameters */
  float tile_size;	/* each tile is tile_size*tile_size (x*y) */
  int tiles_x;		/* number of tiles in x direction */
  int tiles_y;		/* number of tiles in y direction */
  /* --- also store global state (from pfScene?) */
  int ntables;		/* number of alternate pfGeoState tables */
 char **tablenames;    /* name for each table */
  int nmaterials;	/* number of materials */
  char **names;		/* array of material names (nmaterials entries) */
  pfGeoState **globalmat; /* Global GeoState for each table (pfChanGState) */
  pfList **materials;	/* material tables for use with pfChanGStateTable */
  
  /* Fields above this line should be filled in in the structure passed
   * to tsgWriteOpen, those below this line are filled in by tsgWriteOpen.
   * tsgReadOpen, of course, creates the entire structure.
   */
  pfList *texList;	/* only used when reading file in Performer */
  int file;		/* open file from which tiles are read */
  long tileoffset;	/* location in file where tile table starts */
  struct tsgTileLoc {   /* offsets and lengths of tiles */
    long offset;	/* location in file (for fseek()) */
    long length;	/* length of tile in bytes */
  } *tiles;

  /* Fields below this line are used only by tsgReadOpen() */
  pfdShare *mtl_share;
  pfdShare *ctable_share;
  pfdShare *fog_share;
  pfdShare *lmodel_share;
  pfdShare *light_share;
  pfdShare *texenv_share;
  pfdShare *texgen_share;
  pfdShare *tex_share;
} tsgContext;

/* Create function */
extern tsgContext *tsgCreateContext(double,double,float,int,int,void *);

/* Read functions */
extern tsgContext *tsgReadOpen(char *filename);
extern tsgContext *tsgReadReopen(tsgContext *context);
extern void tsgReadClose(tsgContext *context);
extern pfNode *tsgReadTile(tsgContext *context, int x, int y);
extern void tsgFreeTile(tsgContext *context, int x, int y, pfNode *);

/* Write functions */
extern int tsgWriteOpen(char *filename, tsgContext *context);
extern int tsgWriteTile(tsgContext *context, int x, int y,
			pfNode *node, void *arena);
extern void tsgWriteClose(tsgContext *context);

/* Print function */
extern void tsgPrint(tsgContext *);

/* TSG constant -> Performer constant util function */
extern int tsgGetMode(int *,int8);

/* Constant arrays shared between files */
extern int Decal_Mode[];

#ifdef __cplusplus
}
#endif
#endif /* _tsgpf_h_ */
