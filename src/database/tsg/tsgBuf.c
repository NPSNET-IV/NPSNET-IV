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
/* TSG (Tiled Scene Graph) buffer append functions.
 *****************************************************************
 * HISTORY
 *  14-Mar-96  Michael Polis (mfp)  Digital Mapping Laboratory
 *      School of Computer Science at Carnegie-Mellon University
 *      Created.
 */

#include <tsgpf.h>
#include <tsgBuf.h>

#define BUF_CHUNK 4000

struct tsgAppendBuffer *tsgBufCreate(void *arena)
{
  struct tsgAppendBuffer *buf;

  if((buf=pfMalloc(sizeof(struct tsgAppendBuffer),arena))==NULL)
    return NULL;
  buf->allocated = BUF_CHUNK;
  buf->used = 0;
  if((buf->data = pfMalloc(buf->allocated,arena))==NULL)
    return NULL;

  return buf;
}


int tsgBufWrite(const void *data, size_t size, struct tsgAppendBuffer *buf)
{
  if(size<=0){
    pfNotify(PFNFY_NOTICE,PFNFY_INTERNAL,
	     "Appending %d bytes is impossible\n", size);
    return -1;
  }

  if(size+buf->used>=buf->allocated){
    buf->allocated = size+buf->used+BUF_CHUNK;
    if((buf->data = pfRealloc(buf->data,buf->allocated))==NULL)
      return -1;
  }
  memmove(buf->data+buf->used,data,size);
  buf->used+=size;

  return size;
}


int tsgBufWriteStr(const char *str, struct tsgAppendBuffer *buf)
{
  size_t size;
  char dummy[1];

  dummy[0] = 0;
  if (!str)  str = dummy;

  size = strlen(str)+1;
  
  if(size+buf->used>=buf->allocated){
    buf->allocated = size+buf->used+BUF_CHUNK;
    if((buf->data = pfRealloc(buf->data,buf->allocated))==NULL)
      return -1;
  }
  memmove(buf->data+buf->used,str,size);
  buf->used+=size;

  return size;
}


void tsgBufFree(struct tsgAppendBuffer *buf)
{
  if (buf->data) pfFree(buf->data);
  pfFree(buf);
}
