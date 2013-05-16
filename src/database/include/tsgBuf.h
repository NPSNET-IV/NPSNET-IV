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
/* Tiled Scene Graph buffer handling header.
 *****************************************************************
 * HISTORY
 *  26-Mar-96  Stephen Gifford (sjg)  Digital Mapping Laboratory
 *      School of Computer Science at Carnegie-Mellon University
 *      Created.
 */

#ifndef _tsgBuf_h_
#define _tsgBuf_h_

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct tsgAppendBuffer {
  char *data;
  size_t allocated;
  size_t used;
};

/* Write buffer manipulation */
extern struct tsgAppendBuffer *tsgBufCreate(void *arena);
extern int tsgBufWrite(const void *data, size_t size,
		       struct tsgAppendBuffer *buf);
extern int tsgBufWriteStr(const char *str, struct tsgAppendBuffer *buf);
extern void tsgBufFree(struct tsgAppendBuffer *buf);

/* Read buffer manipulation */
extern int tsgBufRead(void *data, size_t size, struct tsgAppendBuffer *buf);
extern char *tsgBufReadStr(struct tsgAppendBuffer *);

#ifdef __cplusplus
}
#endif
#endif /* _tsg_h_ */
