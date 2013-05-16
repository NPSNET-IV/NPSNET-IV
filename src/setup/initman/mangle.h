/* mangle.h */
/* Name mangling is required because of a conflict with a Yacc parser in */
/*   the Loral round world library */

/* Lex mangles */
#define yyback im_back
#define yybgin im_bgin
#define yycrank im_crank
#define yyerror im_error
#define yyestate im_estate
#define yyextra im_extra
#define yyfnd im_fnd
#define yyin im_in
#define yyinput im_input
#define yyleng im_leng
#define yyless im_less
#define yylex im_lex
#define yylineno im_lineno
#define yylook im_look
#define yylsp im_lsp
#define yylstate im_lstate
#define yylval im_lval
#define yymatch im_match
#define yymorfg im_morfg
#define yyolsp im_olsp
#define yyout im_out
#define yyoutput im_output
#define yyprevious im_previous
#define yyracc im_racc
#define yyreject im_reject
#define yysbuf im_sbuf
#define yysptr im_sptr
#define yysvec im_svec
#define yytchar im_tchar
#define yytext im_text
#define yytop im_top
#define yyunput im_unput
#define yyvstop im_vstop
/*
#define yywrap im_wrap
*/

/* YACC mangles */
#define yyact im_act
#define yychar im_char
#define yychk im_chk
#define yydebug im_debug
#define yydef im_def
#define yyerrflag im_errflag
#define yyerror im_error
#define yyexca im_exca
#define yynerrs im_nerrs
#define yypact im_pact
#define yyparse im_parse
#define yypgo im_pgo
#define yyps im_ps
#define yypv im_pv
#define yyr1 im_r1
#define yyr2 im_r2
#define yys im_s
#define yystate im_state
#define yytmp im_tmp
#define yyv im_v
#define yyval im_val

/* EOF */
