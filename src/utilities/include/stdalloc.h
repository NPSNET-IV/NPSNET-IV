/*
 * $RCSfile: stdalloc.h,v $ $Revision: 1.7 $ $State: Exp $
 */
/****************************************************************************
*   File: stdalloc.h                                                        *
*                                                                           *
*       Copyright 1990 by BBN Systems and Technology Corporation            *
*                                                                           *
*               BBN Systems and Technology Corporation                      *
*               10 Moulton Street                                           *
*               Cambridge, MA 02238                                         *
*               617-873-1850                                                *
*                                                                           *
*       This software was developed under U.S. Government                   *
*       contract MDA903-86-C-0309.  Use, duplication and                    *
*       distribution of this software is subject to the                     *
*       provisions of DFARS 52.227-7013                                     *
*                                                                           *
*       Contents: Standard dynamic memory allocation header                 *
*       Created: Fri May 29 1992                                            *
*       Author: jesmith                                                     *
*       Remarks: Use #ifdefs to customize for new hardware.                 *
*                                                                           *
****************************************************************************/

#ifndef _STDALLOC_H_INCLUDED
#define _STDALLOC_H_INCLUDED
#endif

#if defined(lint)

/* Truly bizarre/clever definitions which suppress lint warnings
 * regarding malloc.  Thanks to cjross for these...
 */

#define STDALLOC(n)     (n, 0)
#define STDCALLOC(n,s)  (n, s, 0)
#define STDREALLOC(p,n)	(p, n, 0)
#define STDDEALLOC(p)   (p, 0)

#else /* not lint */

#ifdef USE_LIBHEAP

#include <libheap.h>
#define STDALLOC   heap_allocate
#define STDCALLOC  heap_calloc
#define STDREALLOC heap_realloc
#define STDDEALLOC heap_deallocate

#else /* not lint, not libheap */

#define heap_allocate     Explicit Reference to libheap not allowed!
#define heap_calloc       Explicit Reference to libheap not allowed!
#define heap_realloc      Explicit Reference to libheap not allowed!
#define heap_deallocate   Explicit Reference to libheap not allowed!

#ifdef USE_LIBDMALLOC

#include <libdmalloc.h>

#else /* not lint, not libheap, not libdmalloc */

#if defined(sun) || defined(sgi) || defined(hpux) || defined(aix)

/* Add machines which have the SYSV malloc.h to this list.
 * Note that although the mips has /usr/include/malloc.h, it does
 * not have /bsd43/usr/include/malloc.h, so it can't go here.
 */

#include <malloc.h>

#else /* not lint, not libheap, not libdmalloc, not sun/sgi/hpux/aix */

extern char *malloc();
extern char *calloc();
extern char *realloc();
extern void  free();

#endif /* not lint, not libheap, not libdmalloc */

#endif /* not lint, not libheap */

#define STDALLOC   malloc
#define STDCALLOC  calloc
#define STDREALLOC realloc
#define STDDEALLOC free

#endif /* not lint */
#endif /* last one */
