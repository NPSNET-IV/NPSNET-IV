
# line 2 "rdr_parser.y"

#ifndef lint
static char rcsid[]="$RCSfile: rdr_parser.y,v $ $Revision: 1.3 $ $State: Exp $";
#endif

#include "librdr_local.h"
#include <stdio.h>
#include <stdalloc.h> /*common/include/global*/

static READER_UNION * make_array();
static READER_UNION * make_mac_array();
static READER_STACK * stack_init();
static READER_STACK * stack_push();
static int32 do_typing = 0;

# line 18 "rdr_parser.y"
typedef union 
{
    TYPED_DATA typed;
    READER_STACK * stackptr;
} YYSTYPE;
# define SYMBOL 257
# define MACRO_SYMBOL 258
# define INTEGER 259
# define OPEN_PAREN 260
# define CLOSE_PAREN 261
# define OPEN_SQUARE 262
# define CLOSE_SQUARE 263
# define OPEN_BRACE 264
# define CLOSE_BRACE 265
# define FLOAT 266
# define MULOP 267
# define ADDOP 268
# define SHIFTOP 269
# define EQOP 270
# define LTOP 271
# define GTOP 272
# define NOTOP 273
# define BITAOP 274
# define BITXOP 275
# define BITIOP 276
# define ANDOP 277
# define OROP 278
# define QUESTOP 279
# define COLONOP 280
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
typedef int yytabelem;
# define YYERRCODE 256

# line 227 "rdr_parser.y"


#ifdef USESTRINGDOTH
# include <string.h>
#else
# include <strings.h>
#endif

#if (mips & lint)
extern char *strtok (/* char *s1, *s2 */);
#endif

static READER_UNION parsed_data;
static char *file_name;
int32 reader_alert;
static char override_path[4096] = ".";
static int32 n_override_dirs = 1;
static char default_path[4096] = "";
static int32 n_default_dirs = 0;

static READER_STACK *stack_push(typed_ptr, stack_ptr)
    TYPED_DATA   *typed_ptr;
    READER_STACK *stack_ptr;
{
    READER_STACK * new_stack;

    new_stack = (READER_STACK *)STDALLOC(sizeof(READER_STACK));
    new_stack->typed = *typed_ptr;
    new_stack->next = stack_ptr;
    return new_stack;
}

static READER_STACK *stack_push_array(arr, stack_ptr)
    READER_UNION   *arr;
    READER_STACK *stack_ptr;
{
    int32 len = arr[0].integer;
    int32 i;

    /*  an array given to us has already been processed with make_array and
     *  is made of READER_UNIONs, but we need to push TYPED_DATAs.
     */

    for (i=1;i<len;i++)
    {
	TYPED_DATA typed;
	typed.ru = arr[i];
	if (do_typing)
	    typed.type = READER_UTYPE(arr,i);
	stack_ptr = stack_push(&typed, stack_ptr);
    }

    return stack_ptr;
}

static void free_stack(stack_ptr)
    READER_STACK *stack_ptr;
{
    if (stack_ptr)
    {
	free_stack(stack_ptr->next);
	STDDEALLOC(stack_ptr);
    }
}

static READER_UNION *make_array (stack_ptr, ismac)
    READER_STACK *stack_ptr;
    int32         ismac;
{
    int32 len, n_unions;
    READER_STACK * s;
    READER_UNION * result;
    char *types;

    /*  Determine array length.  We need len data unions, and optionally len
     *  type bytes at the beginning rounded up to the nearest multiple of
     *  sizeof READER_UNION.
     */

    for (len=1, s=stack_ptr; s; len++,s=s->next)
	;

    n_unions = len;

    if (do_typing)
	n_unions += len / sizeof(READER_UNION) + 1;
    result = (READER_UNION *)STDALLOC(n_unions * sizeof(READER_UNION));
    result += (n_unions - len);
    result[0].integer = len;

    if (do_typing)
    {
	types = ((char*) result) - 2;
	types[0] = READER_INTEGER;
    }

    /*  copy the data.  top of stack is last array element.
    */

    if (s = stack_ptr)
    {
	READER_UNION *dp = result + len - 1;
	READER_STACK *tmp;
	if (do_typing)
	    types -= len - 1;
	do {
	    *dp = s->typed.ru;
	    if (do_typing){
		*types++ = s->typed.type;
                if ((s->typed.type == READER_ARRAY) && ismac)
                    ((char *)(dp->array))[-1] = ismac;
	    }
            dp--;
	    tmp = s;
	    s = s->next;
	    STDDEALLOC(tmp);
	} while (s);
    }

    return result;
}

void yyerror(s)
    char *s;
{
    fprintf(stderr,"%s: %s at line %d\n",
	    file_name, s, reader_lex_line_count);
}

void reader_set_search_paths(override, defalt)
    char *override;
    char *defalt;
{
    if (override)
    {
	strncpy(override_path, override, sizeof(override_path));
	strtok(override_path, ":");
	for(n_override_dirs=1;strtok(NULL, ":");n_override_dirs++)
	  ;
    }
    else
    {
	*override_path = 0;
	n_override_dirs = 0;
    }

    if (defalt)
    {
	strncpy(default_path, defalt, sizeof(default_path));
	strtok(default_path, ":");
	for(n_default_dirs=1;strtok(NULL, ":");n_default_dirs++)
	  ;
    }
    else
    {
	*default_path = 0;
	n_default_dirs = 0;
    }
}

static int32 read_file(f, ru)
    FILE         *f;
    READER_UNION *ru;
{
    int32 errcond;
    extern int errno;
    extern FILE *yyin;

    yyin = f;
    reader_lex_line_count = 1;
    reader_dot_warning_given = 0;
    errcond = yyparse();
    fclose(f);

    if (errcond)
    {
	ru->integer = 0;
	return READER_READ_ERROR;
    }
    else
    {
	*ru = parsed_data;
	return 0;
    }
}

static int32 search_and_read(search_path, search_len, fname, ru, alert)
    char         *search_path;
    int32         search_len;
    char         *fname;
    READER_UNION *ru;
    int32         alert;
{
    char path[256];
    int32 i;
    FILE *f=NULL;

    for (i=0;i<search_len;i++,search_path+=strlen(search_path)+1)
    {
	if (*search_path)
	{
	    strcpy(path, search_path);
	    strcat(path, "/");
	}
	else
	  *path = 0;
	strcat(path, fname);
	if (f = fopen(path,"r"))
	{
	    if (alert)
	      printf("Reading file %s from %s\n", fname, search_path);
	    break;
	}
    }

    if (f)
    {
	file_name = path;
	return read_file(f, ru);
    }
    else
      return READER_FILE_NOT_FOUND;
}

int32 reader_read(fname, dir, ru, flags)
    char         *fname;
    char         *dir;
    READER_UNION *ru;
    uint32        flags;
{
    int32 ret;

    /* In case something fails */
    ru->integer = 0;

    do_typing = (flags & READER_TYPING) != 0;

    if (flags & READER_OVERRIDE)
    {
	if ((ret = search_and_read(override_path, n_override_dirs,
				   fname, ru, reader_alert)) !=
	    READER_FILE_NOT_FOUND)
	  return ret;
    }

    if ((ret = search_and_read(dir ? dir : "", 1, fname, ru, 0)) !=
	READER_FILE_NOT_FOUND)
      return ret;

    if (flags & READER_DEFAULTS)
      ret = search_and_read(default_path, n_default_dirs, fname, ru, 0);

    return ret;
}	  

static void output_string(s, stream)
    char *s;
    FILE *stream;
{
#define MAXCOL 75
    static int32 column = 0;
    static int32 indent = 0;
    static int32 prespace = 0;
    int32 i;
    int32 len;
    int32 wrap, oparen, cparen;

    oparen = (index(s, '(')) != 0;
    cparen = (index(s, ')')) != 0;

    if (oparen)
      column = MAXCOL; /* Force an indent */

    len = strlen(s);

    wrap = column+len >= MAXCOL;

    if (oparen || cparen || wrap)
      prespace = 0;

    if (prespace)
      len += 1; 

    if (wrap)
    {
	if (indent)
	  putc('\n', stream);
	for (i=0;i<indent;i++)
	  putc(' ', stream);

	column = indent;
    }

    if (prespace)
      putc(' ', stream);

    if (oparen)
      indent += 1;
    if (cparen)
      indent -= 1;

    fputs(s, stream);
    column += len;

    prespace = !oparen;

    if (!indent && cparen)
    {
	putc('\n', stream);
	prespace = column = 0;
    }
}

void reader_pretty_print(ru, stream)
    READER_UNION  *ru;
    FILE          *stream;
{
    char buf[100];
    int32 i, len = ru[0].integer;

    output_string("(", stream);

    for (i=1;i<len;i++)
    {
	switch (READER_UTYPE(ru,i))
	{
	  case READER_INTEGER:
	    sprintf(buf, "%d", ru[i].integer);
	    output_string(buf, stream);
	    break;
	  case READER_REAL:
	    /*  Must detect if %g prints without decimal point or
	     *  E-notation, or the number could be read back as an int.
	     */
	    sprintf(buf, "%g", ru[i].real);
	    if (!index(buf, '.') && !index(buf, 'e'))
	      strcat(buf, ".0");
	    output_string(buf, stream);
	    break;
	  case READER_CHARPTR:
	    sprintf(buf, "\"%s\"", ru[i].charptr);
	    output_string(buf, stream);
	    break;
	  case READER_ARRAY:
	    if (ru[i].array)
	      reader_pretty_print(ru[i].array, stream);
	    else
	      output_string("()", stream);
	    break;
	  default:
	    output_string("<unknown>", stream);
	    break;
	}
    }

    output_string(")", stream);
}

void reader_free_typed_ru(ru)
    READER_UNION *ru;
{
    int32 i, len = ru[0].integer;

    for ( i = 1; i < len; ++i ){
	switch (READER_UTYPE(ru,i)){
	    case READER_INTEGER:
	        break;
	    case READER_REAL:
	        break;
	    case READER_CHARPTR:
	        break;
	    case READER_ARRAY:
                if (!READER_ISMAC(ru[i].array))
	            reader_free_typed_ru(ru[i].array);
	        break;
	    default:
	        break;
	}
    }
    if (!READER_ISMAC(ru))
        STDDEALLOC(ru);
}
yytabelem yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 36
# define YYLAST 217
yytabelem yyact[]={

    41,    40,    39,    36,    37,    38,    30,    35,    34,    33,
    32,    31,    30,    57,    29,     1,    20,     5,    41,    40,
    39,    36,    37,    38,     9,    35,    34,    33,    32,    31,
    30,    41,    40,    39,    36,    37,    38,     8,    35,    34,
    33,    32,    31,    30,    41,    40,    39,    36,    37,    38,
    10,    35,    34,    33,    32,    41,    30,    41,    40,    39,
    36,    37,    38,     6,    35,    34,    33,    30,     0,    30,
    41,    40,    39,    36,    37,    38,     0,    35,    34,    41,
    40,    39,    30,    41,    40,    39,    36,    37,    38,     3,
    35,    30,    18,    41,    40,    30,    41,    40,    39,    36,
    37,    38,    41,    40,    39,    30,    37,    38,    30,    17,
    14,     0,     0,    12,    30,     0,    18,     0,    18,    13,
    11,    17,    14,    16,    22,    12,     0,     0,    44,    15,
     0,    13,    11,    17,    14,    16,    43,    12,     0,    21,
     0,    15,     0,    13,    11,    17,    14,    16,    27,    12,
     0,    14,     0,    15,    12,    13,    11,    17,    14,    16,
    13,    12,    42,     0,     0,    15,     0,    13,     0,     0,
    45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
    55,    56,    24,     7,     2,     7,     0,    23,     4,     0,
    19,     0,     0,     0,     0,     0,    25,    58,     0,     7,
     0,    26,     0,     7,     4,    28,     0,     0,     4,     7,
     0,     7,     0,     0,    19,     0,    19 };
yytabelem yypact[]={

  -101, -1000,  -101, -1000, -1000, -1000,  -248, -1000, -1000, -1000,
 -1000, -1000,  -149,  -108, -1000, -1000,  -113, -1000, -1000, -1000,
  -101,  -249,  -149, -1000, -1000, -1000,  -125, -1000,  -137, -1000,
  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,
  -149,  -149,  -273, -1000, -1000,  -267,  -223,  -210,  -197,  -184,
  -171,  -165,  -188,  -188,  -174,  -212,  -273,  -149,  -236 };
yytabelem yypgo[]={

     0,    63,    50,   182,   139,    37,    24,   187,    17,   184,
    89,    15 };
yytabelem yyr1[]={

     0,    11,     8,     7,     2,    10,    10,    10,    10,    10,
     6,     6,     9,     9,     9,     9,     1,     3,     3,     3,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     5 };
yytabelem yyr2[]={

     0,     3,     9,     3,     3,     3,     3,     3,     3,     3,
     7,     5,     3,     5,     3,     5,     3,     7,     5,     3,
    11,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     5,     3,     3,     3 };
yytabelem yychk[]={

 -1000,   -11,    -9,   -10,    -7,    -8,    -1,    -3,    -5,    -6,
    -2,   257,   262,   268,   259,   266,   260,   258,   -10,    -7,
   264,    -4,   273,    -7,    -3,    -3,    -9,   261,    -9,   263,
   279,   278,   277,   276,   275,   274,   270,   271,   272,   269,
   268,   267,    -4,   261,   265,    -4,    -4,    -4,    -4,    -4,
    -4,    -4,    -4,    -4,    -4,    -4,    -4,   280,    -4 };
yytabelem yydef[]={

     0,    -2,     1,    12,    14,     5,     6,     7,     8,     9,
     3,    16,     0,     0,    19,    35,     0,     4,    13,    15,
     0,     0,     0,    33,    34,    18,     0,    11,     0,    17,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    32,    10,     2,     0,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,     0,    20 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"SYMBOL",	257,
	"MACRO_SYMBOL",	258,
	"INTEGER",	259,
	"OPEN_PAREN",	260,
	"CLOSE_PAREN",	261,
	"OPEN_SQUARE",	262,
	"CLOSE_SQUARE",	263,
	"OPEN_BRACE",	264,
	"CLOSE_BRACE",	265,
	"FLOAT",	266,
	"MULOP",	267,
	"ADDOP",	268,
	"SHIFTOP",	269,
	"EQOP",	270,
	"LTOP",	271,
	"GTOP",	272,
	"NOTOP",	273,
	"BITAOP",	274,
	"BITXOP",	275,
	"BITIOP",	276,
	"ANDOP",	277,
	"OROP",	278,
	"QUESTOP",	279,
	"COLONOP",	280,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"file : data_list",
	"macro_defn : symbol_type OPEN_BRACE data_list CLOSE_BRACE",
	"macro_value : macro_symbol_type",
	"macro_symbol_type : MACRO_SYMBOL",
	"data : macro_defn",
	"data : symbol_type",
	"data : integer_type",
	"data : float_type",
	"data : array_type",
	"array_type : OPEN_PAREN data_list CLOSE_PAREN",
	"array_type : OPEN_PAREN CLOSE_PAREN",
	"data_list : data",
	"data_list : data_list data",
	"data_list : macro_value",
	"data_list : data_list macro_value",
	"symbol_type : SYMBOL",
	"integer_type : OPEN_SQUARE math_expr CLOSE_SQUARE",
	"integer_type : ADDOP integer_type",
	"integer_type : INTEGER",
	"math_expr : math_expr QUESTOP math_expr COLONOP math_expr",
	"math_expr : math_expr OROP math_expr",
	"math_expr : math_expr ANDOP math_expr",
	"math_expr : math_expr BITIOP math_expr",
	"math_expr : math_expr BITXOP math_expr",
	"math_expr : math_expr BITAOP math_expr",
	"math_expr : math_expr EQOP math_expr",
	"math_expr : math_expr LTOP math_expr",
	"math_expr : math_expr GTOP math_expr",
	"math_expr : math_expr SHIFTOP math_expr",
	"math_expr : math_expr ADDOP math_expr",
	"math_expr : math_expr MULOP math_expr",
	"math_expr : NOTOP math_expr",
	"math_expr : macro_value",
	"math_expr : integer_type",
	"float_type : FLOAT",
};
#endif /* YYDEBUG */
/* 
 *	Copyright 1987 Silicon Graphics, Inc. - All Rights Reserved
 */

/* #ident	"@(#)yacc:yaccpar	1.10" */
#ident	"$Revision: 1.5 $"

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-1000)

/*
** global variables used by the parser
*/
YYSTYPE yyv[ YYMAXDEPTH ];	/* value stack */
int yys[ YYMAXDEPTH ];		/* state stack */

YYSTYPE *yypv;			/* top of value stack */
int *yyps;			/* top of state stack */

int yystate;			/* current state */
int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */
int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */



/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
yyparse()
{
	register YYSTYPE *yypvt;	/* top of value stack for $vars */

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

	goto yystack;
	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ YYMAXDEPTH ] )	/* room on stack? */
		{
			yyerror( "yacc stack overflow" );
			YYABORT;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
				yynerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 1:
# line 62 "rdr_parser.y"
{
			      parsed_data = (yypvt[-0].stackptr)->typed.ru;
			      free_stack(yypvt[-0].stackptr);
			  } break;
case 2:
# line 68 "rdr_parser.y"
{
			      READER_UNION *arr;

			      arr = make_array(yypvt[-1].stackptr, 1);
			      reader_set_symbol_value(yypvt[-3].typed.ru.charptr, arr);
			      yyval.typed.ru.charptr = yypvt[-3].typed.ru.charptr;
			  } break;
case 3:
# line 77 "rdr_parser.y"
{
			      yyval.typed.ru.array = reader_get_symbol_value(yypvt[-0].typed.ru.charptr);
			  } break;
case 4:
# line 82 "rdr_parser.y"
{ yyval.typed.ru.charptr = yypvt[-0].typed.ru.charptr; } break;
case 5:
# line 85 "rdr_parser.y"
{
			      yyval.typed.ru.charptr = yypvt[-0].typed.ru.charptr;
			      yyval.typed.type = READER_CHARPTR;
			  } break;
case 6:
# line 90 "rdr_parser.y"
{
			      yyval.typed.ru.charptr = yypvt[-0].typed.ru.charptr;
			      yyval.typed.type = READER_CHARPTR;
			  } break;
case 7:
# line 95 "rdr_parser.y"
{
			      yyval.typed.ru.integer = yypvt[-0].typed.ru.integer;
			      yyval.typed.type = READER_INTEGER;
			  } break;
case 8:
# line 100 "rdr_parser.y"
{
			      yyval.typed.ru.real = yypvt[-0].typed.ru.real;
			      yyval.typed.type = READER_REAL;
			  } break;
case 9:
# line 105 "rdr_parser.y"
{
			      yyval.typed.ru.array = yypvt[-0].typed.ru.array;
			      yyval.typed.type = READER_ARRAY;
			  } break;
case 10:
# line 111 "rdr_parser.y"
{
			      yyval.typed.ru.array = make_array(yypvt[-1].stackptr, 0);
			  } break;
case 11:
# line 115 "rdr_parser.y"
{
			      yyval.typed.ru.array = 0;
			  } break;
case 12:
# line 120 "rdr_parser.y"
{
			      yyval.stackptr = stack_push(&(yypvt[-0].typed), (READER_STACK*) NULL);
			  } break;
case 13:
# line 124 "rdr_parser.y"
{
			      yyval.stackptr = stack_push(&(yypvt[-0].typed), yypvt[-1].stackptr);
			  } break;
case 14:
# line 128 "rdr_parser.y"
{
			      yyval.stackptr = stack_push_array(yypvt[-0].typed.ru.array, (READER_STACK*) NULL);
			  } break;
case 15:
# line 132 "rdr_parser.y"
{
			      yyval.stackptr = stack_push_array(yypvt[-0].typed.ru.array, yypvt[-1].stackptr);
			  } break;
case 16:
# line 137 "rdr_parser.y"
{ yyval.typed.ru.charptr = yypvt[-0].typed.ru.charptr; } break;
case 17:
# line 140 "rdr_parser.y"
{ yyval.typed.ru.integer = yypvt[-1].typed.ru.integer; } break;
case 18:
# line 142 "rdr_parser.y"
{
			      if (yypvt[-1].typed.ru.integer == '+')
				yyval.typed.ru.integer = yypvt[-0].typed.ru.integer;
			      else
				yyval.typed.ru.integer = - yypvt[-0].typed.ru.integer;
			  } break;
case 19:
# line 149 "rdr_parser.y"
{ yyval.typed.ru.integer = yypvt[-0].typed.ru.integer; } break;
case 20:
# line 152 "rdr_parser.y"
{ yyval.typed.ru.integer = yypvt[-4].typed.ru.integer ? yypvt[-2].typed.ru.integer : yypvt[-0].typed.ru.integer; } break;
case 21:
# line 154 "rdr_parser.y"
{ yyval.typed.ru.integer = yypvt[-2].typed.ru.integer || yypvt[-0].typed.ru.integer; } break;
case 22:
# line 156 "rdr_parser.y"
{ yyval.typed.ru.integer = yypvt[-2].typed.ru.integer && yypvt[-0].typed.ru.integer; } break;
case 23:
# line 158 "rdr_parser.y"
{ yyval.typed.ru.integer = yypvt[-2].typed.ru.integer | yypvt[-0].typed.ru.integer; } break;
case 24:
# line 160 "rdr_parser.y"
{ yyval.typed.ru.integer = yypvt[-2].typed.ru.integer ^ yypvt[-0].typed.ru.integer; } break;
case 25:
# line 162 "rdr_parser.y"
{ yyval.typed.ru.integer = yypvt[-2].typed.ru.integer & yypvt[-0].typed.ru.integer; } break;
case 26:
# line 164 "rdr_parser.y"
{
			      if (yypvt[-1].typed.ru.integer)		/* meaning == */
				yyval.typed.ru.integer = (yypvt[-2].typed.ru.integer == yypvt[-0].typed.ru.integer);
			      else		/* meaning != */
				yyval.typed.ru.integer = (yypvt[-2].typed.ru.integer != yypvt[-0].typed.ru.integer);
			  } break;
case 27:
# line 171 "rdr_parser.y"
{
			      if (yypvt[-1].typed.ru.integer)		/* meaning <= */
				yyval.typed.ru.integer = (yypvt[-2].typed.ru.integer <= yypvt[-0].typed.ru.integer);
			      else		/* meaning < */
				yyval.typed.ru.integer = (yypvt[-2].typed.ru.integer < yypvt[-0].typed.ru.integer);
			  } break;
case 28:
# line 178 "rdr_parser.y"
{
			      if (yypvt[-1].typed.ru.integer)		/* meaning >= */
				yyval.typed.ru.integer = (yypvt[-2].typed.ru.integer >= yypvt[-0].typed.ru.integer);
			      else		/* meaning > */
				yyval.typed.ru.integer = (yypvt[-2].typed.ru.integer > yypvt[-0].typed.ru.integer);
			  } break;
case 29:
# line 185 "rdr_parser.y"
{
			      if (yypvt[-1].typed.ru.integer == '<')	/* meaning << */
				yyval.typed.ru.integer = yypvt[-2].typed.ru.integer << yypvt[-0].typed.ru.integer;
			      else		/* meaning >> */
				yyval.typed.ru.integer = yypvt[-2].typed.ru.integer >> yypvt[-0].typed.ru.integer;
			  } break;
case 30:
# line 192 "rdr_parser.y"
{
			      if (yypvt[-1].typed.ru.integer == '+')
				yyval.typed.ru.integer = yypvt[-2].typed.ru.integer + yypvt[-0].typed.ru.integer;
			      else
				yyval.typed.ru.integer = yypvt[-2].typed.ru.integer - yypvt[-0].typed.ru.integer;
			  } break;
case 31:
# line 199 "rdr_parser.y"
{
			      if (yypvt[-1].typed.ru.integer == '*')
				yyval.typed.ru.integer = yypvt[-2].typed.ru.integer * yypvt[-0].typed.ru.integer;
			      else if (yypvt[-1].typed.ru.integer == '/')
				yyval.typed.ru.integer = yypvt[-2].typed.ru.integer / yypvt[-0].typed.ru.integer;
			      else
				yyval.typed.ru.integer = yypvt[-2].typed.ru.integer % yypvt[-0].typed.ru.integer;
			  } break;
case 32:
# line 208 "rdr_parser.y"
{
			      if (yypvt[-1].typed.ru.integer == '!')
				yyval.typed.ru.integer = !(yypvt[-0].typed.ru.integer);
			      else
				yyval.typed.ru.integer = ~(yypvt[-0].typed.ru.integer);
			  } break;
case 33:
# line 215 "rdr_parser.y"
{
			      if (yypvt[-0].typed.ru.array[0].integer == 2)
				yyval.typed.ru.integer = yypvt[-0].typed.ru.array[1].integer;
			      else
				yyval.typed.ru.integer = 1;
			  } break;
case 34:
# line 222 "rdr_parser.y"
{ yyval.typed.ru.integer = yypvt[-0].typed.ru.integer; } break;
case 35:
# line 225 "rdr_parser.y"
{ yyval.typed.ru.real = yypvt[-0].typed.ru.real; } break;
	}
	goto yystack;		/* reset registers in driver code */
}
