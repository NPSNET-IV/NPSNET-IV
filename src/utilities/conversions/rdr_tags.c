#ifndef lint
static char rcsid [] = "$RCSfile: rdr_tags.c,v $ $Revision: 1.7 $ $State: Exp $";
#endif
/****************************************************************************
*   File: tags.c                                                            *
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
*       Contents: Tagged array lookup routines                              *
*       Created: 1/29/90                                                    *
*       Author: jesmith                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "librdr_local.h"
#include <stdio.h>
#include <varargs.h>

#ifdef sgi
#include <stdlib.h>
#endif

static int32 is_probably_a_string(s)
    char * s;
{
    int32 i;
    if (!(s && *s))
      return 0;
    /* I've arbitrarily decided that strings over 80 characters long
       do not provide useful context. */
    for (i=0;(i<80)&&(*s);i++,s++)
      if ((*s<' ') || (*s>'~'))
	return 0;
    return (*s == 0);
}
    
static tag_error(tag,table,tagged,errlevel)
    char * tag;
    READER_UNION * table;
    int32 tagged;
    int32 errlevel;
{
    int32 i;
    int32 len = table[0].integer;
    READER_UNION * entry;

    if (errlevel != READER_NO_ERRORS)
      fprintf(stderr,"Tag not found: %s\n",tag);
    if ((errlevel == READER_ERRORS_WITH_CONTEXT) ||
	(errlevel == READER_ERRORS_WITH_DESCRIPTION))
    {
	if (!len)
	  fprintf(stderr,"  Table is empty.");
	else
	{
	    fprintf(stderr,"  compared with: ");
	    for(i=(tagged ? 2 : 1);i<len;i++)
	    {
		fprintf(stderr,"`");
		if (!(entry = table[i].array))
		  fprintf(stderr,"<NULL PTR>");
		else if (is_probably_a_string((char *)entry))
		  fprintf(stderr,"<<<%s>>>",entry);
		else if (!entry[0].integer)
		  fprintf(stderr,"()");
		else if (is_probably_a_string(entry[1].charptr))
		  fprintf(stderr,"%s",entry[1].charptr);
		fprintf(stderr,"' ");
		if (i>5)
		{
		    fprintf(stderr,"...");
		    break;
		}
	    }
	}
	fprintf(stderr,"\n");
    }
}

/*VARARGS4*/
READER_UNION *reader_find_tag(tag, table, tagged, errlevel, va_alist)
    char *         tag;
    READER_UNION * table;
    int32          tagged;   /* Does this table start with a tag itself? */
    int32          errlevel;
    va_dcl
{
    va_list        args;
    int32          i;
    int32          len = table[0].integer;
    READER_UNION * entry;

    for(i=(tagged ? 2 : 1);i<len;i++)
    {
	if (!(entry = table[i].array))
	  break;
	if (entry[0].integer == 0)
	  break;
	if (symbols_match(entry[1].charptr,tag))
	  return entry;
    }
    /* If we got here, the tag isn't the first member of any of the
       arrays which make up the table. */
    if (errlevel == READER_ERRORS_WITH_DESCRIPTION)
    {
	char     *format;
	
	va_start(args);
	format = va_arg(args, char *);
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\n");
    }
    tag_error(tag,table,tagged,errlevel);
    return 0;
}

static int32 cmp_tags(ru0, ru1)
    READER_UNION * ru0;
    READER_UNION * ru1;
{
    if ((!ru0->integer) || (ru0->array[0].integer < 2))
      return -1;
    if ((!ru1->integer) || (ru1->array[0].integer < 2))
      return 1;
    return symbols_compare(ru0->array[1].charptr,
			   ru1->array[1].charptr);
}

/* The GT libc does not currently support qsort.  As soon as pjm
 * gets around to putting qsort in libc, this #ifndef can be removed,
 * and this function can be used on the GT.
 */
#ifndef _GT_
void reader_sort_tag_table(table, tagged)
    READER_UNION * table;
    int32 tagged;   /* Does this table start with a tag itself? */
{
    int32 skip = (tagged ? 2 : 1);

    qsort(&(table[skip]),
	  (int) (table[0].integer - skip),
	  sizeof(READER_UNION),
	  (int (*)())cmp_tags);
}
#endif

static READER_UNION *binarysearch_tag(key, first, last)
    char       *key;
    READER_UNION *first;
    READER_UNION *last;
{
    READER_UNION *split;
    int32 cmp_first, cmp_last;

    /* If the last entry has no tag, the key cannot be before it */
    if ((!last->integer) || (last->array[0].integer < 2))
      return NULL;

    /* If the first entry has no tag, the key must be later in the table */
    if ((!first->integer) || (first->array[0].integer < 2))
      cmp_first = 1;
    else if (!(cmp_first = symbols_compare(key,first->array[1].charptr)))
      return first;

    if (!(cmp_last = symbols_compare(key,last->array[1].charptr)))
      return last;

    if (cmp_first < 0 || cmp_last > 0 || first > last)
      return NULL;

    split = &(first[(last-first)/2]);

    /* If the split has no tag, the key must be after it */
    if ((!split->integer) || (split->array[0].integer < 2) ||
	(symbols_compare(key, split->array[1].charptr) >= 0))
      return binarysearch_tag(key, split, last-1);
    else
      return binarysearch_tag(key, first+1, split-1);
}

/*VARARGS4*/
READER_UNION *reader_find_tag_sorted(tag, table, tagged, errlevel, va_alist)
    char * tag;
    READER_UNION * table;
    int32 tagged;   /* Does this table start with a tag itself? */
    int32 errlevel;
    va_dcl
{
    va_list       args;
    int32         skip = (tagged ? 2 : 1);
    READER_UNION *result;

    
    result = binarysearch_tag(tag,
			      &(table[skip]),
			      &(table[table[0].integer - 1]));

    if (result)
      return result->array;

    /* If we got here, the tag isn't the first member of any of the
       arrays which make up the table. */
    if (errlevel == READER_ERRORS_WITH_DESCRIPTION)
    {
	char     *format;
	
	va_start(args);
	format = va_arg(args, char *);
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\n");
    }
    tag_error(tag,table,tagged,errlevel);
    return 0;
}

int32 reader_member_tag(tag, table)
    char * tag;
    READER_UNION * table;
{    
    int32 i;
    int32 table_length = table[0].integer;

    for (i=1; i<table_length; ++i)
      if(symbols_match(tag, table[i].charptr))
	return(1);

    return(0);
}

int32 reader_ft_int(symbol,table)
    char       *symbol;
    READER_UNION *table;
{
    READER_UNION * result;

    if (!(result =
	  reader_find_tag(symbol,table,
			  READER_TAGGED,READER_ERRORS_WITH_CONTEXT)) ||
	(result[0].integer != 3))
    {
	return 0;
    }
    return result[2].integer;
}

float32 reader_ft_float(symbol,table)
    char       *symbol;
    READER_UNION *table;
{
    READER_UNION * result;

    if (!(result =
	  reader_find_tag(symbol,table,
			  READER_TAGGED,READER_ERRORS_WITH_CONTEXT)) ||
	(result[0].integer != 3))
    {
	return 0.0;
    }
    return result[2].real;
}

char *reader_ft_symbol(symbol,table)
    char       *symbol;
    READER_UNION *table;
{
    READER_UNION * result;

    if (!(result =
	  reader_find_tag(symbol,table,
			  READER_TAGGED,READER_ERRORS_WITH_CONTEXT)) ||
	(result[0].integer != 3))
    {
	return 0;
    }
    return result[2].charptr;
}
