#ifndef lint
static char rcsid [] = "$RCSfile: rdr_symbol.c,v $ $Revision: 1.6 $ $State: Exp $";
#endif
/****************************************************************************
*   File: symbol.c                                                          *
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
*       Contents: Simple symbol table                                       *
*       Created: 1/25/90                                                    *
*       Author: jesmith                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include <strings.h>
#include "librdr_local.h"
#include <stdalloc.h> /*common/include/global*/

#ifndef NO_SYMBOLS

typedef struct bucket_entry {
    char *symbol;
    struct bucket_entry *next;
} BUCKET_ENTRY;

static BUCKET_ENTRY *symbol_table[SYMBOL_TABLE_SIZE];

static uint32 Do_Hash();

void reader_init(alert_on_override)
    int32 alert_on_override;
{
    static int32 symbol_table_initted = 0;

    reader_alert = alert_on_override;

    if (symbol_table_initted)
      return;

    symbol_table_initted = 1;

    bzero((char *)symbol_table, sizeof(symbol_table));
}

char *reader_get_symbol(s)
    char *s;
{
    uint32 hashval;
    BUCKET_ENTRY *bucket;

    hashval = Do_Hash(s, (int32)strlen(s)) % SYMBOL_TABLE_SIZE;
    if (bucket = symbol_table[hashval])
    {
	do
	{
	    if (! strcmp(bucket->symbol, s)) /*Match*/
	      return bucket->symbol;
	    bucket = bucket->next;
	} while(bucket);
    }
    /* If we hit here, this symbol is new */
    bucket = (BUCKET_ENTRY *)STDALLOC(sizeof(BUCKET_ENTRY));
    bucket->symbol = (char *)STDALLOC(strlen(s)+1+sizeof(READER_UNION *));
    bzero((char *)bucket->symbol, sizeof(READER_UNION *));
    bucket->symbol += sizeof(READER_UNION *);
    strcpy(bucket->symbol,s);
    bucket->next = symbol_table[hashval];
    symbol_table[hashval] = bucket;
    return bucket->symbol;
}

/* Interface to get_symbol which strips off quotes */

char *reader_get_symbol_with_quotes(s)
    char *s;
{
    char s_no_quotes[256];

    strcpy(s_no_quotes,s);
    s_no_quotes[strlen(s)-1] = 0;
    return reader_get_symbol(s_no_quotes+1);
}

float64 reader_describe_symbol_table(printp)
    int32 printp;
{
    int32 i;
    int32 bucket_count = 0;
    int32 sym_count = 0;
    BUCKET_ENTRY **s, *b;

    for (i=0,s=symbol_table;i<SYMBOL_TABLE_SIZE;i++,s++)
    {
	if (*s)
	{
	    bucket_count++;
	    for (b = *s;b;b=b->next)
	      sym_count++;
	}
    }
    printf("Hash table summary:\n");
    printf("  %8d symbols\n  %8d buckets used (%2.2f%%)\n  %2.2f entries/bucket\n",
	   sym_count, bucket_count, 100.0*bucket_count/SYMBOL_TABLE_SIZE,
	   (float64)sym_count/bucket_count);
    if (printp)
    {
	printf("\nSymbol table follows:\n");
	for (i=0,s=symbol_table;i<SYMBOL_TABLE_SIZE;i++,s++)
	{
	    if (*s)
	    {
		printf("[%4d] ",i);
		for (b = *s;b;b=b->next)
		  printf(" %s ",b->symbol);
		printf("\n");
	    }
	}
    }
    return (float64)sym_count/bucket_count;

}

/* Hashing algorithm for interning from MIT's scheme interpreter
 * (slightly optimized).  The original code wouldn't look at more than
 * five characters, but that works really, really bad when dealing with
 * protocol constants.
 */

#define LENGTH_MULTIPLIER       5
#define SHIFT_AMOUNT            1

static uint32 Do_Hash (String_Ptr, String_Length)
    char *String_Ptr;
    int32 String_Length;
{
    uint32 i, Value;
    char * s;

    Value = (LENGTH_MULTIPLIER * String_Length);

    for (i = 0, s = String_Ptr; i < String_Length; i++, s++)
      Value = ((Value << SHIFT_AMOUNT) + *s);
    return (Value);
}

READER_UNION *reader_get_symbol_value(s)
    char *s;
{
    return *((READER_UNION **)(s-(sizeof(READER_UNION *))));
}

void reader_set_symbol_value(s, v)
    char       *s;
    READER_UNION *v;
{
    *((READER_UNION **)(s-(sizeof(READER_UNION *)))) = v;
}

#else

char *reader_get_symbol(s)
    char * s;
{
    char * copy = (char *)STDALLOC(strlen(s)+1);
    strcpy(copy,s);
    return copy;
}

char *reader_get_symbol_with_quotes(s)
    char * s;
{
    int32 slen = strlen(s);
    char * copy = (char *)STDALLOC(slen);
    strcpy(copy,s+1);
    copy[slen-2] = 0;
    return copy;
}

READER_UNION *reader_get_symbol_value(s)
    char * s;
{
    return NULL;
}

void reader_set_symbol_value(s,v)
    char *s;
    READER_UNION *v;
{
    fprintf(stderr, "Macro substitution only available when libreader compiled\n with symbols.\n");
}
#endif
