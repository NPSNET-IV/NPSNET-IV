
typedef union 
{
    TYPED_DATA typed;
    READER_STACK * stackptr;
} YYSTYPE;
extern YYSTYPE yylval;
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
