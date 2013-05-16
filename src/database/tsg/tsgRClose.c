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
/* Free/close function for the TSG header.
 *****************************************************************
 * HISTORY
 *  20-Mar-96  Stephen Gifford (sjg)  Digital Mapping Laboratory
 *      School of Computer Science at Carnegie-Mellon University
 *      Created.
 */

#include <stdio.h>
#include <time.h>
/* #include <gl/glconsts.h> */
#include <tsgpf.h>

void tsgReadClose(tsgContext *context)
{
  int i;

  if (context->source)  pfFree(context->source);
  if (context->comment)  pfFree(context->comment);
  if (context->projinfo)  pfFree(context->projinfo);
  if (context->datum)  pfFree(context->datum);

  if (context->tablenames) {
    for (i=0;i!=context->ntables;i++)
      if (context->tablenames[i])  pfFree(context->tablenames[i]);
    pfFree(context->tablenames);
  }
  if (context->names) {
    for (i=0;i!=context->nmaterials;i++)
      if (context->names[i])  pfFree(context->names[i]);
    pfFree(context->names);
  }
  for (i=0;i!=context->ntables;i++)  pfDelete(context->materials[i]);
  if (context->materials)  pfFree(context->materials);

  if (context->texList)  pfDelete(context->texList);
  if (context->file)  close(context->file);
  if (context->tiles)  pfFree(context->tiles);

  /* Clear out shared objects */
  pfdDelShare(context->mtl_share, PF_ON);
  pfdDelShare(context->ctable_share, PF_ON);
  pfdDelShare(context->fog_share, PF_ON);
  pfdDelShare(context->lmodel_share, PF_ON);
  pfdDelShare(context->light_share, PF_ON);
  pfdDelShare(context->texenv_share, PF_ON);
  pfdDelShare(context->texgen_share, PF_ON);
  pfdDelShare(context->tex_share, PF_ON);
}
