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
/* Print info on Tiled Scene Graph
 *****************************************************************
 * HISTORY
 *  25-Mar-96  Stephen Gifford (sjg)  Digital Mapping Laboratory
 *      School of Computer Science at Carnegie-Mellon University
 *      Created.
 */

#include <stdio.h>
#include <time.h>
/* #include <gl/glconsts.h> */
#include <tsgpf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* tsgPrint()
   Print out stats on a loaded database.
   */
void tsgPrint(tsgContext *context)
{
  int no;
  int i;

  /* Print some basic info */
  printf("-- %s --\n",context->path);
  printf(" Extents: (%g,%g) -> (%g,%g)\n",context->ll_x,context->ll_y,context->ll_x+context->tile_size*context->tiles_x,
	 context->ll_y+context->tile_size*context->tiles_y);
  printf(" Zone = %d; Projection params = %s;  Datum = %s.\n",context->zone,context->projinfo,context->datum);
  printf(" Tile size: %g\n",context->tile_size);
  printf(" Tiles: (%d,%d)\n",context->tiles_x,context->tiles_y);
  printf(" Geostate: %d geostates per %d table(s)\n",context->nmaterials,context->ntables);
  printf(" Appearances:");
  for (i=0;i!=context->ntables;i++)    printf(" %s",context->tablenames[i]);  printf("\n");
  if (no = pfdCountShare(context->mtl_share))  printf("\t%d shared materials\n",no);
  if (no = pfdCountShare(context->ctable_share))  printf("\t%d shared color tables\n",no);
  if (no = pfdCountShare(context->fog_share))  printf("\t%d share fog states\n",no);
  if (no = pfdCountShare(context->lmodel_share))  printf("\t%d shared lighting models\n",no);
  if (no = pfdCountShare(context->light_share))  printf("\t%d shared lights\n",no);
  if (no = pfdCountShare(context->texenv_share))  printf("\t%d shared texture environments\n",no);
  if (no = pfdCountShare(context->texgen_share))  printf("\t%d shared texture coordinate generators\n",no);
  if (no = pfdCountShare(context->tex_share))  printf("\t%d shared textures\n",no);
}
