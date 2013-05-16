/* mangle.h must be included at the top of the lex-generated C code; */
/*   it renames symbols to avoid conflict with other lex/yacc-based */
/*   parsers in the system. */
#include "mangle.h"
# include "stdio.h"
# define U(x) ((unsigned char)(x))
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 2048
# define output(c) (void)putc(c,yyout)
#if defined(__cplusplus) || defined(__STDC__)

#ifdef __cplusplus
extern "C" {
#endif
	int yylex(void);
	int yyback(int *, int);
	int yyinput(void);
	int yylook(void);
	void yyoutput(int);
	int yyracc(int);
	int yyreject(void);
	void yyunput(int);
#ifndef yyless
	void yyless(long int);
#endif
#ifndef yywrap
	int yywrap(void);
#endif
#ifdef __cplusplus
}
#endif

#endif

# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO (void)fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;

# line 2 "lexer.l"

#include "im.tab.h"
#include "imstructs.h"
#include "imsymbols.h"
#include "imdefaults.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>
extern int errno;

extern int lineno;
extern int current_file_type;

#ifdef __cplusplus
extern "C" {
#endif
extern int yylook();
extern int yywrap();
extern yyback();
extern int strcasecmp();
extern int strncasecmp();
#ifdef __cplusplus
}
#endif

# define YYNEWLINE 10
yylex(void){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:

# line 40 "lexer.l"
;
break;
case 2:

# line 41 "lexer.l"
;
break;
case 3:

# line 43 "lexer.l"
{
	  yylval.ivalue = atoi(yytext);
	  return INT;
	}
break;
case 4:

# line 48 "lexer.l"
{
	  yylval.fvalue = (float) atof(yytext);
	  return FLOAT;
	}
break;
case 5:

# line 53 "lexer.l"
{
	  sscanf(yytext, "0x%x", &(yylval.ivalue));
	  return INT;
	}
break;
case 6:

# line 58 "lexer.l"
{	/* Any string which is not a number */
	  int symnum;
	  if ((symnum = symlook(yytext)) == -1) {
	     /* yylval.string = strdup(yytext); */
	     strcpy(yylval.string, yytext);
	     return STRING;
	  } else {
	     return (symnum);
	  }
	}
break;
case 7:

# line 69 "lexer.l"
{ lineno++; }
break;
case 8:

# line 71 "lexer.l"
{ return yytext[0]; }
break;
case -1:
break;
default:
(void)fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

# line 74 "lexer.l"

/* Check whether a string is a symbol */
int symlook(symstring)
   char *symstring;
{
   struct symbol *sp;

   /* Select the symbol table for the file type */
   switch (current_file_type) {
      case (ANIMATION_FILETYPE):
         sp = AnimationSymtab;
         break;
      case (BLINKING_FILETYPE):
         sp = BlinkingSymtab;
         break;
      case (FILTER_FILETYPE):
         sp = FilterSymtab;
         break;
      case (LANDMARK_FILETYPE):
         sp = LandmarkSymtab;
         break;
      case (LOCATION_FILETYPE):
         sp = LocationSymtab;
         break;
      case (MASTER_CONFIG_FILETYPE):
         sp = MasterConfSymtab;
         break;
      case (MODEL_FILETYPE):
         sp = ModelSymtab;
         break;
      case (NETWORK_FILETYPE):
         sp = NetworkSymtab;
         break;
      case (RADAR_ICON_FILETYPE):
         sp = RadarIconsSymtab;
         break;
      case (ROUND_WORLD_FILETYPE):
         sp = RoundWorldSymtab;
         break;
      case (SOUNDS_FILETYPE):
         sp = SoundsSymtab;
         break;
      case (TERRAIN_FILETYPE):
         sp = TerrainSymtab;
         break;
      case (TRANSPORT_FILETYPE):
         sp = TransportSymtab;
         break;
      default:
         printf("Error: Invalid file type.\n");
   }

   /* Search appropriate symbol table */
   while (1) {
      if (sp->name == NULL)
         return (-1);		/* table exhausted, not a symbol */
      if (sp->numchars == 0) {
         if (strcasecmp(symstring, sp->name) == 0) {
#ifdef DEBUG2
            printf("symlook(1): got symbol %s\n", sp->name);
#endif
            return (sp->value);	/* a symbol */
         }
      } else {
         if (strncasecmp(symstring, sp->name, sp->numchars) == 0) {
#ifdef DEBUG2
            printf("symlook(2): got symbol %s\n", sp->name);
#endif
            return (sp->value);	/* a symbol */
         }
      }
      sp++;			/* next symbol */
   }
}

int yyvstop[] = {
0,

8,
0,

2,
8,
0,

7,
0,

6,
8,
0,

1,
6,
8,
0,

6,
8,
0,

6,
8,
0,

6,
8,
0,

3,
6,
8,
0,

3,
6,
8,
0,

2,
0,

6,
0,

1,
0,

1,
6,
0,

6,
0,

3,
6,
0,

4,
6,
0,

1,
6,
0,

6,
0,

1,
0,

5,
6,
0,
0};
# define YYTYPE unsigned char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
4,13,	16,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,6,	4,13,	
1,7,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,8,	1,9,	
1,10,	1,11,	1,12,	1,12,	
1,12,	1,12,	1,12,	1,12,	
1,12,	1,12,	1,12,	2,7,	
10,20,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,6,	0,0,	
0,0,	2,8,	2,9,	2,10,	
0,0,	2,12,	2,12,	2,12,	
2,12,	2,12,	2,12,	2,12,	
2,12,	2,12,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
6,14,	6,14,	6,14,	6,14,	
7,15,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
7,15,	7,0,	8,17,	0,0,	
8,18,	8,18,	8,18,	8,18,	
8,18,	8,18,	8,18,	8,18,	
8,18,	8,18,	9,19,	9,19,	
9,19,	9,19,	9,19,	9,19,	
9,19,	9,19,	9,19,	9,19,	
7,16,	11,17,	0,0,	11,18,	
11,18,	11,18,	11,18,	11,18,	
11,18,	11,18,	11,18,	11,18,	
11,18,	15,0,	12,17,	7,16,	
12,18,	12,18,	12,18,	12,18,	
12,18,	12,18,	12,18,	12,18,	
12,18,	12,18,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
7,16,	0,0,	0,0,	0,0,	
15,15,	17,19,	17,19,	17,19,	
17,19,	17,19,	17,19,	17,19,	
17,19,	17,19,	17,19,	0,0,	
0,0,	0,0,	18,17,	15,15,	
18,18,	18,18,	18,18,	18,18,	
18,18,	18,18,	18,18,	18,18,	
18,18,	18,18,	20,22,	0,0,	
0,0,	0,0,	0,0,	22,0,	
15,15,	0,0,	20,22,	20,0,	
0,0,	0,0,	0,0,	11,21,	
19,19,	19,19,	19,19,	19,19,	
19,19,	19,19,	19,19,	19,19,	
19,19,	19,19,	0,0,	0,0,	
0,0,	0,0,	22,22,	0,0,	
0,0,	0,0,	20,20,	21,23,	
21,23,	21,23,	21,23,	21,23,	
21,23,	21,23,	21,23,	21,23,	
21,23,	22,22,	0,0,	0,0,	
0,0,	20,20,	0,0,	0,0,	
21,23,	21,23,	21,23,	21,23,	
21,23,	21,23,	0,0,	0,0,	
0,0,	0,0,	22,22,	0,0,	
0,0,	0,0,	20,20,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	23,23,	23,23,	23,23,	
23,23,	23,23,	23,23,	23,23,	
23,23,	23,23,	23,23,	0,0,	
21,23,	21,23,	21,23,	21,23,	
21,23,	21,23,	23,23,	23,23,	
23,23,	23,23,	23,23,	23,23,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	23,23,	23,23,	
23,23,	23,23,	23,23,	23,23,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-24,	yysvec+1,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+3,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+6,
yycrank+49,	0,		yyvstop+8,
yycrank+-175,	0,		yyvstop+11,
yycrank+140,	yysvec+6,	yyvstop+15,
yycrank+150,	yysvec+6,	yyvstop+18,
yycrank+13,	yysvec+6,	yyvstop+21,
yycrank+163,	yysvec+6,	yyvstop+24,
yycrank+176,	yysvec+6,	yyvstop+28,
yycrank+0,	yysvec+4,	yyvstop+32,
yycrank+0,	yysvec+6,	yyvstop+34,
yycrank+-211,	yysvec+7,	yyvstop+36,
yycrank+-3,	yysvec+7,	yyvstop+38,
yycrank+197,	yysvec+6,	yyvstop+41,
yycrank+212,	yysvec+6,	yyvstop+43,
yycrank+236,	yysvec+6,	yyvstop+46,
yycrank+-269,	0,		yyvstop+49,
yycrank+255,	yysvec+6,	yyvstop+52,
yycrank+-265,	yysvec+20,	yyvstop+54,
yycrank+293,	yysvec+6,	yyvstop+56,
0,	0,	0};
struct yywork *yytop = yycrank+395;
struct yysvf *yybgin = yysvec+1;
unsigned char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,
'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,
'!' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'!' ,
'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,
'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,
'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,
'!' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'!' ,
'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,
'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,
'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,'!' ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
#ident	"$Revision: 1.9 $"

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
#if defined(__cplusplus) || defined(__STDC__)
int yylook(void)
#else
yylook()
#endif
{
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
#ifndef LONGLINES
			if(yylastch > &yytext[YYLMAX]) {
				fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
				exit(1);
			}
#endif
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (void *)yyt > (void *)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
#ifndef LONGLINES
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
#endif
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((void *)yyt < (void *)yycrank) {	/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
#ifndef LONGLINES
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
#endif
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
#ifndef LONGLINES
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
#endif
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = (int)(yylastch-yytext+1);
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
#if defined(__cplusplus) || defined(__STDC__)
int yyback(int *p, int m)
#else
yyback(p, m)
	int *p;
#endif
{
	if (p==0) return(0);
	while (*p) {
		if (*p++ == m)
			return(1);
	}
	return(0);
}
	/* the following are only used in the lex library */
#if defined(__cplusplus) || defined(__STDC__)
int yyinput(void)
#else
yyinput()
#endif
{
	return(input());
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyoutput(int c)
#else
yyoutput(c)
  int c; 
#endif
{
	output(c);
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyunput(int c)
#else
yyunput(c)
   int c; 
#endif
{
	unput(c);
	}
