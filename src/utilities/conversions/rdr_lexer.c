# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 2048
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {NULL}, *yyout = {NULL};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
extern int yylook();
#ifdef __cplusplus
extern "C" {
#endif
extern int yywrap();
extern int yylex();
extern int yyreject();
extern int yyracc(int);
extern int yyless(int);
#ifdef __cplusplus
}
#endif
#include "librdr_local.h"
#include "y.tab.h"

#ifndef lint
static char rcsid[]="$RCSfile: rdr_lexer.l,v $ $Revision: 1.3 $ $State: Exp $";
#endif

extern float64 atof();
extern void yyerror();
int32 reader_lex_line_count = 1;
int32 reader_dot_warning_given = 0;
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
{ }
break;
case 2:
 {
    yylval.typed.ru.charptr = reader_get_symbol_with_quotes(yytext);
    return SYMBOL;
}
break;
case 3:
   {
    yylval.typed.ru.integer = 0;
    return OPEN_PAREN;
}
break;
case 4:
 {
    yylval.typed.ru.integer = 0;
    return CLOSE_PAREN;
}
break;
case 5:
 {
    yylval.typed.ru.integer = 0;
    return OPEN_SQUARE;
}
break;
case 6:
{
    yylval.typed.ru.integer = 0;
    return CLOSE_SQUARE;
}
break;
case 7:
   {
    yylval.typed.ru.integer = 0;
    return OPEN_BRACE;
}
break;
case 8:
 {
    yylval.typed.ru.integer = 0;
    return CLOSE_BRACE;
}
break;
case 9:
{
    
    yylval.typed.ru.real = atof(yytext);
    return FLOAT;
}
break;
case 10:
 {
    sscanf(yytext+2,"%x",&(yylval.typed.ru.integer));
    return INTEGER;
}
break;
case 11:
 {
    sscanf(yytext+2,"%o",&(yylval.typed.ru.integer));
    return INTEGER;
}
break;
case 12:
 {
    sscanf(yytext,"%d",&(yylval.typed.ru.integer));
    return INTEGER;
}
break;
case 13:
  {
    yylval.typed.ru.integer = yytext[0];
    return MULOP;
}
break;
case 14:
  {
    yylval.typed.ru.integer = yytext[0];
    return ADDOP;
}
break;
case 15:
{
    yylval.typed.ru.integer = yytext[0];
    return SHIFTOP;
}
break;
case 16:
  {
    yylval.typed.ru.integer = (yytext[0] == '=');
    return EQOP;
}
break;
case 17:
  {
    yylval.typed.ru.integer = (yytext[1] == '=');
    return LTOP;
}
break;
case 18:
  {
    yylval.typed.ru.integer = (yytext[1] == '=');
    return GTOP;
}
break;
case 19:
 {
    yylval.typed.ru.integer = yytext[0];
    return BITAOP;
}
break;
case 20:
 {
    yylval.typed.ru.integer = yytext[0];
    return BITXOP;
}
break;
case 21:
 {
    yylval.typed.ru.integer = yytext[0];
    return BITIOP;
}
break;
case 22:
  {
    yylval.typed.ru.integer = yytext[0];
    return ANDOP;
}
break;
case 23:
   {
    yylval.typed.ru.integer = yytext[0];
    return OROP;
}
break;
case 24:
   {
    yylval.typed.ru.integer = yytext[0];
    return NOTOP;
}
break;
case 25:
{
    yylval.typed.ru.integer = yytext[0];
    return QUESTOP;
}
break;
case 26:
{
    yylval.typed.ru.integer = yytext[0];
    return COLONOP;
}
break;
case 27:
        {
    yylval.typed.ru.integer = 1;
    return INTEGER;
}
break;
case 28:
      {
    yylval.typed.ru.integer = 0;
    return INTEGER;
}
break;
case 29:
     {
    yylval.typed.ru.charptr = reader_get_symbol(yytext);
    if (reader_get_symbol_value(yylval.typed.ru.charptr))
      return MACRO_SYMBOL;
    else
      return SYMBOL;
}
break;
case 30:
  ;
break;
case 31:
{
    reader_lex_line_count++;
}
break;
case 32:
    {
    if (!reader_dot_warning_given)
    {
	yyerror("Warning: . encountered (ignored)");
	reader_dot_warning_given = 1;
    }
}
break;
case 33:
        {
    printf("Lex error on line %d.  ??? %s ???\n",
	   reader_lex_line_count, yytext);
    exit(1);
}
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */
int yyvstop[] = {
0,

33,
0,

30,
33,
0,

31,
0,

24,
33,
0,

33,
0,

13,
33,
0,

19,
33,
0,

3,
33,
0,

4,
33,
0,

14,
33,
0,

32,
33,
0,

12,
29,
33,
0,

12,
29,
33,
0,

26,
33,
0,

33,
-1,
0,

17,
33,
0,

33,
0,

18,
33,
0,

25,
33,
0,

29,
33,
0,

5,
33,
0,

6,
33,
0,

20,
33,
0,

29,
33,
0,

27,
29,
33,
0,

7,
33,
0,

21,
33,
0,

8,
33,
0,

24,
33,
0,

16,
0,

2,
0,

22,
0,

9,
0,

29,
0,

12,
29,
0,

29,
0,

29,
0,

-1,
0,

1,
0,

15,
0,

17,
0,

18,
0,

29,
0,

23,
0,

11,
29,
0,

10,
29,
0,

28,
29,
0,

9,
0,
0};
# define YYTYPE char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,6,	1,7,	
0,0,	0,0,	1,8,	1,9,	
9,35,	1,10,	1,11,	0,0,	
1,12,	0,0,	1,12,	1,13,	
0,0,	1,14,	1,15,	1,15,	
1,15,	1,15,	1,15,	1,15,	
1,15,	1,15,	0,0,	1,16,	
1,17,	1,18,	1,19,	1,20,	
1,21,	6,32,	1,22,	18,45,	
18,46,	0,0,	1,22,	0,0,	
1,22,	20,47,	20,45,	0,0,	
0,0,	0,0,	38,50,	0,0,	
1,22,	41,51,	41,51,	41,51,	
41,51,	41,51,	41,51,	41,51,	
41,51,	1,22,	0,0,	0,0,	
1,23,	0,0,	1,24,	1,25,	
1,3,	12,36,	0,0,	12,37,	
12,37,	12,37,	12,37,	12,37,	
12,37,	12,37,	12,37,	12,37,	
12,37,	26,48,	38,50,	1,26,	
48,53,	0,0,	0,0,	0,0,	
0,0,	1,27,	0,0,	0,0,	
2,9,	0,0,	2,10,	2,11,	
1,28,	1,29,	1,30,	1,31,	
2,13,	29,49,	0,0,	2,15,	
2,15,	2,15,	2,15,	2,15,	
2,15,	2,15,	0,0,	15,39,	
2,16,	2,17,	2,18,	2,19,	
2,20,	2,21,	7,33,	0,0,	
15,39,	0,0,	0,0,	0,0,	
0,0,	0,0,	7,33,	7,33,	
13,38,	13,38,	13,38,	13,38,	
13,38,	13,38,	13,38,	13,38,	
13,38,	13,38,	0,0,	0,0,	
40,39,	0,0,	0,0,	15,39,	
0,0,	2,23,	0,0,	2,24,	
2,25,	40,39,	7,33,	7,34,	
15,39,	0,0,	7,33,	0,0,	
0,0,	0,0,	0,0,	0,0,	
7,33,	0,0,	7,33,	0,0,	
2,26,	7,33,	0,0,	0,0,	
0,0,	0,0,	2,27,	0,0,	
40,39,	7,33,	0,0,	0,0,	
0,0,	2,28,	2,29,	2,30,	
2,31,	40,39,	7,33,	0,0,	
0,0,	0,0,	7,33,	0,0,	
7,33,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
7,33,	51,51,	51,51,	51,51,	
51,51,	51,51,	51,51,	51,51,	
51,51,	7,33,	0,0,	0,0,	
0,0,	0,0,	14,39,	14,36,	
7,33,	14,40,	14,40,	14,40,	
14,40,	14,40,	14,40,	14,40,	
14,40,	14,40,	14,40,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	14,39,	14,39,	
14,39,	14,39,	14,39,	14,39,	
14,39,	14,39,	14,39,	14,39,	
14,39,	14,39,	14,39,	14,39,	
14,41,	14,39,	14,39,	14,39,	
14,39,	14,39,	14,39,	14,39,	
14,39,	14,42,	14,39,	14,39,	
0,0,	0,0,	0,0,	0,0,	
14,39,	0,0,	14,39,	14,39,	
14,39,	14,39,	14,39,	14,39,	
14,39,	14,39,	14,39,	14,39,	
14,39,	14,39,	14,39,	14,39,	
14,41,	14,39,	14,39,	14,39,	
14,39,	14,39,	14,39,	14,39,	
14,39,	14,42,	14,39,	14,39,	
17,43,	0,0,	0,0,	0,0,	
0,0,	50,54,	0,0,	50,54,	
17,43,	17,44,	50,55,	50,55,	
50,55,	50,55,	50,55,	50,55,	
50,55,	50,55,	50,55,	50,55,	
54,55,	54,55,	54,55,	54,55,	
54,55,	54,55,	54,55,	54,55,	
54,55,	54,55,	0,0,	0,0,	
17,43,	17,43,	0,0,	0,0,	
17,43,	0,0,	0,0,	0,0,	
0,0,	0,0,	17,43,	0,0,	
17,43,	0,0,	0,0,	17,43,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	17,43,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
17,43,	0,0,	0,0,	0,0,	
17,43,	0,0,	17,43,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	17,43,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	17,43,	
0,0,	0,0,	0,0,	0,0,	
22,39,	0,0,	17,43,	22,39,	
22,39,	22,39,	22,39,	22,39,	
22,39,	22,39,	22,39,	22,39,	
22,39,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
22,39,	22,39,	22,39,	22,39,	
22,39,	22,39,	22,39,	22,39,	
22,39,	22,39,	22,39,	22,39,	
22,39,	22,39,	22,39,	22,39,	
22,39,	22,39,	22,39,	22,39,	
22,39,	22,39,	22,39,	22,39,	
22,39,	22,39,	0,0,	0,0,	
0,0,	0,0,	22,39,	0,0,	
22,39,	22,39,	22,39,	22,39,	
22,39,	22,39,	22,39,	22,39,	
22,39,	22,39,	22,39,	22,39,	
22,39,	22,39,	22,39,	22,39,	
22,39,	22,39,	22,39,	22,39,	
22,39,	22,39,	22,39,	22,39,	
22,39,	22,39,	42,52,	42,52,	
42,52,	42,52,	42,52,	42,52,	
42,52,	42,52,	42,52,	42,52,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	42,52,	
42,52,	42,52,	42,52,	42,52,	
42,52,	52,52,	52,52,	52,52,	
52,52,	52,52,	52,52,	52,52,	
52,52,	52,52,	52,52,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	52,52,	52,52,	
52,52,	52,52,	52,52,	52,52,	
0,0,	0,0,	0,0,	42,52,	
42,52,	42,52,	42,52,	42,52,	
42,52,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	52,52,	52,52,	
52,52,	52,52,	52,52,	52,52,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-82,	yysvec+1,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+0,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+6,
yycrank+4,	0,		yyvstop+8,
yycrank+-145,	0,		yyvstop+11,
yycrank+0,	0,		yyvstop+13,
yycrank+2,	0,		yyvstop+16,
yycrank+0,	0,		yyvstop+19,
yycrank+0,	0,		yyvstop+22,
yycrank+51,	0,		yyvstop+25,
yycrank+108,	0,		yyvstop+28,
yycrank+193,	0,		yyvstop+31,
yycrank+60,	yysvec+14,	yyvstop+35,
yycrank+0,	0,		yyvstop+39,
yycrank+-315,	0,		yyvstop+42,
yycrank+7,	0,		yyvstop+45,
yycrank+0,	yysvec+6,	yyvstop+48,
yycrank+12,	0,		yyvstop+50,
yycrank+0,	0,		yyvstop+53,
yycrank+363,	0,		yyvstop+56,
yycrank+0,	0,		yyvstop+59,
yycrank+0,	0,		yyvstop+62,
yycrank+0,	0,		yyvstop+65,
yycrank+4,	yysvec+22,	yyvstop+68,
yycrank+0,	yysvec+22,	yyvstop+71,
yycrank+0,	0,		yyvstop+75,
yycrank+5,	0,		yyvstop+78,
yycrank+0,	0,		yyvstop+81,
yycrank+0,	0,		yyvstop+84,
yycrank+0,	0,		yyvstop+87,
yycrank+0,	yysvec+7,	0,	
yycrank+0,	0,		yyvstop+89,
yycrank+0,	0,		yyvstop+91,
yycrank+0,	yysvec+13,	0,	
yycrank+0,	yysvec+12,	0,	
yycrank+9,	yysvec+13,	yyvstop+93,
yycrank+0,	yysvec+22,	yyvstop+95,
yycrank+89,	yysvec+14,	yyvstop+97,
yycrank+33,	yysvec+22,	yyvstop+100,
yycrank+438,	yysvec+22,	yyvstop+102,
yycrank+0,	yysvec+17,	yyvstop+104,
yycrank+0,	0,		yyvstop+106,
yycrank+0,	0,		yyvstop+108,
yycrank+0,	0,		yyvstop+110,
yycrank+0,	0,		yyvstop+112,
yycrank+4,	yysvec+22,	yyvstop+114,
yycrank+0,	0,		yyvstop+116,
yycrank+278,	0,		0,	
yycrank+177,	yysvec+22,	yyvstop+118,
yycrank+461,	yysvec+22,	yyvstop+121,
yycrank+0,	yysvec+22,	yyvstop+124,
yycrank+288,	0,		0,	
yycrank+0,	yysvec+54,	yyvstop+127,
0,	0,	0};
struct yywork *yytop = yycrank+563;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,'!' ,'"' ,01  ,01  ,'%' ,01  ,01  ,
01  ,01  ,'%' ,'+' ,01  ,'-' ,01  ,'%' ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'8' ,'8' ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'A' ,'A' ,'A' ,'E' ,'A' ,'G' ,
'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'O' ,
'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,
'X' ,'G' ,'G' ,01  ,01  ,01  ,01  ,'_' ,
01  ,'A' ,'A' ,'A' ,'A' ,'E' ,'A' ,'G' ,
'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'O' ,
'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,'G' ,
'X' ,'G' ,'G' ,01  ,01  ,01  ,'!' ,01  ,
0};
char yyextra[] = {
0,1,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/* #ident	"@(#)libl:lib/ncform	1.3" */
#ident	"$Header: /src_trees/lonestar/att/usr/src/lib/libl/RCS/ncform,v 1.5 1991/08/06 11:04:55 jfw Exp $"
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
int yyback(int * p, int m) {
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
int yyinput(){
	return(input());
	}
void yyoutput(int c) {
	output(c);
	}
void yyunput(int c) {
	unput(c);
	}

int yylook(){
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
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
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
				yyleng = yylastch-yytext+1;
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


