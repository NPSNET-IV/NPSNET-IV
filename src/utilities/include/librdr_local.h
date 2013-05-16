/*
 * $RCSfile: librdr_local.h,v $ $Revision: 1.6 $ $State: Exp $
 */
/****************************************************************************
*   File: librdr_local.h                                                    *
*                                                                           *
*       Copyright 1993 by Loral Advanced Distributed Simulation, Inc.       *
*                                                                           *
*               Loral Advanced Distributed Simulation, Inc.                 *
*               10 Moulton Street                                           *
*               Cambridge, MA 02238                                         *
*               617-873-1850                                                *
*                                                                           *
*       This software was developed by Loral under U. S. Government contracts *
*       and may be reproduced by or for the U. S. Government pursuant to    *
*       the copyright license under the clause at DFARS 252.227-7013        *
*       (OCT 1988).                                                         *
*                                                                           *
*       Contents: Private header file for libreader                         *
*       Created: Thu Jul  2 1992                                            *
*       Author: jesmith                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libreader.h"

/* This has something to do with the Macintoy... */
#ifdef THINK_C
#define yywrap() 1
#define malloc NewPtr
#include "liblist.h"
#endif

/*my hack*/
#define yywrap() 1

#ifndef NO_SYMBOLS

#ifndef SYMBOL_TABLE_SIZE
#define SYMBOL_TABLE_SIZE 3001 /* Should be a prime number */
#endif

#endif

/* Use to be public, now private */
#ifdef NO_BC
#ifndef NO_SYMBOLS
#define symbols_match(x,y) ((x)==(y))
#define symbols_compare(x,y) ((int)(((unsigned)(x)==(unsigned)(y)) ? 0 : \
			      (((unsigned)(x)< (unsigned)(y)) ? -1 : 1)))
#else NO_SYMBOLS
#define symbols_match(x,y) (strcmp((x),(y))==0)
#define symbols_compare(x,y) (strcmp((x),(y)))
#endif
#endif

/* These are used only in parsing */
typedef struct
{
    READER_UNION ru;
    int        type;
} TYPED_DATA;

typedef struct stack
{
    TYPED_DATA    typed;
    struct stack *next;
} READER_STACK;

/* Private prototypes */
extern char *reader_get_symbol_with_quotes();
extern int32 reader_lex_line_count;
extern int32 reader_dot_warning_given;
extern int32 reader_alert;
