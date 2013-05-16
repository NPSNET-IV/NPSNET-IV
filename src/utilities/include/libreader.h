/*
 * $RCSfile: libreader.h,v $ $Revision: 2.25 $ $State: Exp $
 */
/****************************************************************************
*   File: libreader.h                                                       *
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
*       Contents: Public header file for libreader                          *
*       Created: Thu Jul  2 1992                                            *
*       Author: jesmith                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LIBREADER_H_INCLUDED
#define _LIBREADER_H_INCLUDED

#include <stdtypes.h> /*common/include/global*/

/* READER_UNION:
 *
 * The basic type used by libreader.
 */
typedef union reader_union
{
    char               *charptr;
    int32               integer;
    float32             real;
    union reader_union *array;
} READER_UNION;

/* READER_UTYPE:
 *
 * Yields the type of an array element.
 */
#define READER_CHARPTR  1
#define READER_INTEGER  2
#define READER_REAL     3
#define READER_ARRAY    4
#define READER_ARRAYMAC 5

#define READER_UTYPE(arr,i) (((char*)(arr))[-(i)-2])

/* READER_ISMAC:
 *
 * Determines if the data union is actually a macro reference.
 */
#define READER_ISMAC(arr) (((char*)(arr))[-1])

/* reader_init:
 *
 * Initializes libreader.  One of the things this does is zero out
 * the string symbol table, so make sure you do this early, and only
 * once.  alert_on_override indicates that a bell should be sounded
 * and a message printed when files are read from override paths.
 */
#ifdef __cplusplus
void reader_init ( int32 );
#else
extern void reader_init(/* int32 alert_on_override */);
#endif

/* reader_get_symbol:
 *
 * Looks up a string in the symbol table and returns the equivalent
 * symbol.  Note that this will add the string to the symbol table
 * if it isn't already there.
 */
extern char *reader_get_symbol(/* char *string */);

/* reader_get_symbol_value:
 *
 * Gets the macro value (if any) associated with the passed symbol.
 */
extern READER_UNION *reader_get_symbol_value(/* char *symbol */);

/* reader_set_symbol_value:
 *
 * Sets the macro value of the passed symbol to the passed value.
 */
extern void reader_set_symbol_value(/* char *symbol,
				       READER_UNION *value
				       */);


/* reader_describe_symbol_table:
 *
 * Determines the fullness (0..1) of the libreader symbol table,
 * and if printp is non-zero, prints the symbols.
 */
extern float64 reader_describe_symbol_table(/* int32 printp */);

/* reader_find_tag:
 *
 * Given an array of the form ((tag ...) (tag ...) (tag ...))  this
 * function finds the member of the array which starts with the passed
 * tag.  The tag must be a symbol (as in reader_get_symbol).  The
 * tagged flag (either READER_UNTAGGED or READER_TAGGED) indicates
 * whether to skip the first entry.  The error level indicates the
 * extent of error messages if the tag is not found (READER_NO_ERRORS,
 * READER_REPORT_ERRORS, READER_ERRORS_WITH_CONTEXT or 
 * READER_ERRORS_WITH_DESCRIPTION).
 */
#define READER_UNTAGGED 0
#define READER_TAGGED   1

#define READER_NO_ERRORS               0
#define READER_REPORT_ERRORS           1
#define READER_ERRORS_WITH_CONTEXT     2
#define READER_ERRORS_WITH_DESCRIPTION 3

extern READER_UNION * reader_find_tag(/* char *tag,
				       READER_UNION *table,
				       int32 tagged,
				       int32 error_level,
				       [ char *description,
				         printf_arg1,
				         printf_arg2,
					 ...]
				       */);

/* reader_sort_tag_table:
 *
 * Sorts a tag table (an array of the form passed to reader_find_tag)
 * by the value of the key.  This can be used to speed up searches
 * of the table in the future using reader_find_tag_sorted.
 */
extern void reader_sort_tag_table(/* READER_UNION *table,
				     int32 tagged
				     */);

/* reader_find_tag_sorted:
 *
 * Works just like reader_find_tag, except it assumes the table
 * has been sorted with reader_sort_tag_table.
 */
extern READER_UNION * reader_find_tag_sorted(/* char *tag,
					      READER_UNION *table,
					      int32 tagged,
					      int32 error_level
					      */);

/* reader_member_tag:
 *
 * Determines if the passed tag value is a member of the array of
 * tags.
 */
extern int32 reader_member_tag( /* char *tag;
				 READER_UNION *table;
				 */);


/* reader_read:
 *
 * Reads a libreader syntax file.  flags is an inclusive OR of
 * the following: 
 *  READER_OVERRIDE  check override directories before passed directory
 *  READER_DEFAULTS  check default directories after passed directory
 *  READER_TYPING    store types of array members
 * The return value is one of READER_READ_OK==0, READER_READ_ERROR, or
 * READER_FILE_NOT_FOUND.
 */
#define READER_OVERRIDE       0x00000001
#define READER_DEFAULTS       0x00000002
#define READER_TYPING         0x00000004
#define READER_READ_OK        0
#define READER_READ_ERROR     -1
#define READER_FILE_NOT_FOUND -2

extern int32 reader_read(/* char *fname,
			    char *directory,
			    READER_UNION *ru,
			    uint32 flags
			    */);

/* reader_set_search_paths:
 *
 * Sets the override and default paths used when reader_read is
 * searching for a file.  The default override path is "." and the
 * default default path is "".  Paths are colon-separated lists of
 * directory names.
 */
extern void reader_set_search_paths(/* char *override,
				       char *default
				       */);

/* reader_pretty_print:
 *
 * Prints the passed data union on the passed stream (such as stdout
 * or an open file).  This only works on files read with typing enabled.
 */
extern void reader_pretty_print(/* READER_UNION *ru,
				   int32 stream
				   */);

/* reader_free_typed_ru:
 *
 * Frees all the memory associated with the passed data union.  This
 * only works on files read with typing enabled.
 */
extern void reader_free_typed_ru(/* READER_UNION *ru */);

/* reader_ft_int:
 *
 * Finds single integer values stored in a tagged tag table like,
 * (tag (tag number) (tag ...) ...)
 */
extern int32 reader_ft_int(/* char *symbol,
			    READER_UNION * table
			    */);

/* reader_ft_float:
 *
 * Finds single float32 values stored in a tagged tag table like,
 * (tag (tag number) (tag ...) ...)
 */
extern float32 reader_ft_float(/* char *symbol,
				READER_UNION * table
				*/);

/* reader_ft_symbol:
 *
 * Finds single symbol values stored in a tagged tag table like,
 * (tag (tag symbol) (tag ...) ...)
 */
extern char *reader_ft_symbol(/* char *symbol,
				 READER_UNION *table
				 */);

/*
 * Backward compatibility macros...
 */
#ifndef NO_BC
#define init_symbol_table reader_init
#define get_symbol       reader_get_symbol
#define get_symbol_value reader_get_symbol_value
#define set_symbol_value reader_set_symbol_value
#define find_tag         reader_find_tag
#define find_tag_sorted  reader_find_tag_sorted
#define sort_tag_table   reader_sort_tag_table
#define member_tag       reader_member_tag
#define reader_ft_table(s,t) \
  reader_find_tag((s),(t),READER_TAGGED,READER_ERRORS_WITH_CONTEXT)
#define reader_ft_untagged_table(s,t) \
  reader_find_tag((s),(t),READER_UNTAGGED,READER_ERRORS_WITH_CONTEXT)
#define describe_symbol_table reader_describe_symbol_table
#define reader_read_file(f,r)            reader_read(f,0,r,0)
#define reader_find_file(f,d,r)          reader_read(f,d,r,READER_OVERRIDE)
#define reader_set_alt_config_files(d)   reader_set_search_paths(".", d)
#define reader_alt_path_find_file(f,d,r) reader_read(f,d,r,READER_OVERRIDE|\
						     READER_DEFAULTS)
#define reader_read_typed_file(f,r)      reader_read(f,0,r,READER_TYPING)
#define reader_find_typed_file(f,r)      reader_read(f,0,r,READER_OVERRIDE|\
						     READER_TYPING)
#define TABLE_NOT_TAGGED         READER_UNTAGGED
#define TABLE_TAGGED             READER_TAGGED
#define TAGS_NO_ERRORS           READER_NO_ERRORS
#define TAGS_REPORT_ERRORS       READER_REPORT_ERRORS
#define TAGS_ERRORS_WITH_CONTEXT READER_ERRORS_WITH_CONTEXT
#define DATA_UNION               READER_UNION
#define DATA_UTYPE               READER_UTYPE
#define DATA_UCHARPTR            READER_CHARPTR
#define DATA_UINTEGER            READER_INTEGER
#define DATA_UREAL               READER_REAL
#define DATA_UARRAY              READER_ARRAY
#define DATA_UARRAYMAC           READER_ARRAYMAC
#define DATA_IS_MACRO            1
#define DATA_IS_NOT_MACRO        0
#define DATA_ISMAC               READER_ISMAC
/* Use to be public, now private */
#ifndef NO_SYMBOLS
#define symbols_match(x,y) ((x)==(y))
#define symbols_compare(x,y) ((int)(((unsigned)(x)==(unsigned)(y)) ? 0 : \
			      (((unsigned)(x)< (unsigned)(y)) ? -1 : 1)))
#else /*NO_SYMBOLS*/
#define symbols_match(x,y) (strcmp((x),(y))==0)
#define symbols_compare(x,y) (strcmp((x),(y)))
#endif /*NO_SYMBOLS*/
#endif /*NO_BC*/

#endif /*_LIBREADER_H_INCLUDED*/

#ifdef __cplusplus
}
#endif

