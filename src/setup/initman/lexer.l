%{ /* lexer.l -- Pattern-matching specification for identifying tokens */

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

%}

comm	#.*|\/\/.*
str	[!-~]+
nl	\n
ws	[\t ]+
int	-?[0-9]+
float	-?[0-9]*\.[0-9]+
hex	0x[0-9A-Fa-f]+

%%

{comm}	;
{ws}	;

{int}	{
	  yylval.ivalue = atoi(yytext);
	  return INT;
	}

{float}	{
	  yylval.fvalue = (float) atof(yytext);
	  return FLOAT;
	}

{hex}	{
	  sscanf(yytext, "0x%x", &(yylval.ivalue));
	  return INT;
	}

{str}	{	/* Any string which is not a number */
	  int symnum;
	  if ((symnum = symlook(yytext)) == -1) {
	     /* yylval.string = strdup(yytext); */
	     strcpy(yylval.string, yytext);
	     return STRING;
	  } else {
	     return (symnum);
	  }
	}

{nl}	{ lineno++; }

.	{ return yytext[0]; }

%%

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

