
# line 1 "parser.y"
 /* parser.y -- Grammar specification */

#include <stdio.h>
#include <errno.h>
extern int errno;

#include <Performer/pf.h>

#include "pdu.h"
#include "mangle.h"
#include "setup_const.h"

#include "imstructs.h"
#include "imdefaults.h"
extern struct defs *Defaults;
extern struct MC *mc;

#ifdef __cplusplus
extern "C" {
#endif
extern int yylex();
extern int yyparse();
extern void yyerror();
#ifdef __cplusplus
}
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

int boolean;
int i, j, index, len;
char tempstr[MAX_SEARCH_PATH];
extern int error_reported, error_reported2;

# line 41 "parser.y"
typedef union
#ifdef __cplusplus
	YYSTYPE
#endif
 {
   int		cmd;
   char		string[128];
   char		single;
   int		ivalue;
   float	fvalue;
} YYSTYPE;
# define INT 257
# define STRING 258
# define FLOAT 259
# define IM_ON 260
# define IM_OFF 261
# define IM_NONE 262
# define IM_EMPTY 263
# define BLINKING_LIGHTS_FILE 264
# define IM_BOUNDING_BOX 265
# define BDIHML 266
# define C2_FILE 267
# define DIGUY_SPEED_RATIO 268
# define DIGUY_POSITION_ERROR 269
# define DIGUY_TIME_CONSTANT 270
# define DIGUY_TRANS_SMOOTHNESS 271
# define TRANS_IMMEDIATE 272
# define IMMEDIATE_TRANS 273
# define TRANS_SMOOTHEST 274
# define DIGUY_POSITION_MODE 275
# define SMOOTH_MODE 276
# define PRECISE_MODE 277
# define DIGUY_POSITION_MECHANISM 278
# define DIGUY_INTERNAL 279
# define NPSNET_INTERNAL 280
# define CHANNEL 281
# define CLIP_PLANES 282
# define IM_CONTROL 283
# define CPU_LOCK 284
# define DB_OFFSET 285
# define DB_PAGES_ALLOWED 286
# define DB_TSG_FILE 287
# define USE_ROUND_WORLD 288
# define DUMP_FILE 289
# define EARTH_SKY_MODE 290
# define DEAD_SWITCH 291
# define ENVIRONMENTAL 292
# define ENTITY_FILE 293
# define DIS_EXERCISE 294
# define FCS_MP 295
# define FCS_PORT 296
# define FCS_PROMPT 297
# define DIS_ENUM 298
# define DISv203 299
# define DISv204 300
# define FILTER_FILE 301
# define FOG 302
# define FOG_NEAR 303
# define FOG_FAR 304
# define FOG_TYPE 305
# define FOG_VTX_LIN 306
# define FOG_VTX_EXP 307
# define FOG_VTX_EXP2 308
# define FOG_PIX_LIN 309
# define FOG_PIX_EXP 310
# define FOG_PIX_EXP2 311
# define FOG_PIX_SPLINE 312
# define FORCE 313
# define FOV 314
# define IM_FRAME_RATE 315
# define GROUND_HEIGHT 316
# define HMD 317
# define HEAD_MOUNTED_DISPLAY 318
# define HORIZON_ANGLE 319
# define HUD_FONT_FILE 320
# define HUD_LEVEL 321
# define INTRO_FONT_FILE 322
# define LANDMARK_FILE 323
# define LOCATION_FILE 324
# define MARKING 325
# define MASTER 326
# define MODEL_FILE 327
# define MULTISAMPLE 328
# define NETWORK_FILE 329
# define NOTIFY_LEVEL 330
# define PDUPOSITION 331
# define PHASE 332
# define PICTURES 333
# define PIPE 334
# define PROCESS_MODE 335
# define ROAD_FILE 336
# define ROUND_WORLD_FILE 337
# define SCORING 338
# define IM_SOUND 339
# define SOUND_FILE 340
# define STEALTH 341
# define STEREO 342
# define TARGET_ENTITY 343
# define IM_TERRAIN_FILE 344
# define TRAKER_FILE 345
# define TRAKER_PORT 346
# define TARGET_BOUNDING_BOX 347
# define TRANSPORT_FILE 348
# define VEHICLE_DRIVEN 349
# define VIEW 350
# define VIEWPORT 351
# define VIEW_HPR 352
# define VIEW_XYZ 353
# define WAYPOINTS 354
# define IM_WINDOW 355
# define LOD_SCALE 356
# define LOD_FADE 357
# define IM_WEAPON_VIEW 358
# define LW_RECEIVE_PORT 359
# define LW_SEND_PORT 360
# define LW_MACHINE_NAME 361
# define ANIMATIONS 362
# define DIS_HEARTBEAT 363
# define DIS_POSDELTA 364
# define DIS_ROTDELTA 365
# define DIS_TIME_OUT 366
# define DYNAMIC_OBJS 367
# define ETHER_INTERF 368
# define FCS_CHECK 369
# define FLYBOX 370
# define MULTICAST 371
# define MULTICAST_GROUP 372
# define MULTICAST_TTL 373
# define NETWORKING 374
# define IM_PORT 375
# define PANEL_GROUP 376
# define PANEL_TTL 377
# define PANEL_PORT 378
# define REMOTE_INPUT_PANEL 379
# define STATIC_OBJS 380
# define WINDOW_SIZE 381
# define SEARCH_PATH 382
# define IM_PO_EXERCISE 383
# define IM_PO_PORT 384
# define IM_PO_DATABASE 385
# define PO_CTDB_FILE 386
# define RADAR_ICON_FILE 387
# define TREADPORT 388
# define UNIPORT 389
# define TESTPORT 390
# define UPPERBODY 391
# define MODSAF_MAP 392
# define IDENTIFY_FILE 393
# define FLYBOX_PORT 394
# define NPS_PIC_DIR 395
# define HIRES 396
# define IM_NET_OFF 397
# define RECEIVE_ONLY 398
# define SEND_ONLY 399
# define SEND_AND_RECEIVE 400
# define LEATHER_FLYBOX_PORT 401
# define LEATHER_FLYBOX_CHECK 402
# define DVW_PATHS 403
# define HMD_FILE 404
# define ANIMATION_FILE 405
# define IM_JACK 406
# define AUTO_FILE 407
# define AUTO_STATE 408
# define RANGE_FILTER 409
# define IM_MAX_ALTITUDE 410
# define CURSOR_FILE 411
# define FS_CONFIG 412
# define FS_PORT 413
# define FS_SCALE 414
# define FCS 415
# define FLYBOXIN 416
# define KAFLIGHT 417
# define KEYBOARD 418
# define SPACEBALL 419
# define LEATHER_FLYBOX 420
# define DRIVE 421
# define PAD 422
# define BALL 423
# define SHIP_PANEL 424
# define SUB_PANEL 425
# define AUSA_ODT 426
# define FAST 427
# define TAG 428
# define SKY 429
# define SKY_GROUND 430
# define SKY_CLEAR 431
# define IM_BLUE 432
# define IM_RED 433
# define IM_WHITE 434
# define IM_OTHER 435
# define IM_FATAL 436
# define WARN 437
# define NOTICE 438
# define INFO 439
# define IM_DEBUG 440
# define FPDEBUG 441
# define ALG_FREE 442
# define ALG_FLOAT 443
# define ALG_LOCK 444
# define APPCULLDRAW 445
# define APP_CULLDRAW 446
# define APPCULL_DRAW 447
# define APP_CULL_DRAW 448
# define IM_DEFAULT 449
# define AIRPORT 450
# define CANYON 451
# define VILLAGE 452
# define PIER 453
# define IM_START 454
# define CLOSED 455
# define CUSTOM 456
# define SVGA 457
# define FULL 458
# define NTSC 459
# define PAL 460
# define NORMAL 461
# define VIM 462
# define THREE_TV 463
# define THREE_BUNKER 464
# define TWO_TPORT 465
# define TWO_ADJACENT 466
# define FS_MCO 467
# define FS_ST_BUFFERS 468
# define ODT_PORT 469
# define ODT_MP 470
# define ANIMATION_DEF 471
# define SEQ_CYCLE 472
# define SEQ_SWING 473
# define BB_SEQ 474
# define MODEL_SEQ 475
# define LIGHT 476
# define BLOCK_ESPDU 477
# define FORCE_APPEARANCE 478
# define LANDMARK_ICON 479
# define MODEL_DEF1 480
# define IM_POSITION 481
# define MODEL_DEF2 482
# define JACK_DEF 483
# define WEAPON_DEF 484
# define DUDE_DEF 485
# define JADE_DEF 486
# define DIGUY_DEF 487
# define IM_HOST 488
# define RADAR_ICON 489
# define UTM_MAP 490
# define SOUND_DEF 491
# define MODEL_DEF3 492
# define BRIDGE_MASK 493
# define FOLIAGE_MASK 494
# define TERRAIN_MASK 495
# define TUNNEL_MASK 496
# define WATER_MASK 497
# define DB_PAGING 498
# define FLAT_WORLD 499
# define LOCATION 500

#include <malloc.h>
#include <memory.h>
#include <unistd.h>
#include <values.h>

#ifdef __cplusplus

#ifndef yyerror
	void yyerror(const char *);
#endif
#ifndef yylex
	extern "C" int yylex(void);
#endif
	int yyparse(void);

#endif
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
YYSTYPE yylval;
YYSTYPE yyval;
typedef int yytabelem;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#if YYMAXDEPTH > 0
int yy_yys[YYMAXDEPTH], *yys = yy_yys;
YYSTYPE yy_yyv[YYMAXDEPTH], *yyv = yy_yyv;
#else	/* user does initial allocation */
int *yys;
YYSTYPE *yyv;
#endif
static int yymaxdepth = YYMAXDEPTH;
# define YYERRCODE 256

# line 1855 "parser.y"


int lineno;
extern char current_file[];
extern int current_file_type;

void parser()
{
   extern FILE	*yyin;
   char		error[128];

   if (current_file == NULL) {
      printf("Error: No file specified in parser()\n");
   } else {
#ifdef DEBUG2
      printf("Parsing: %s\n", current_file);
      fflush(stdout);
#endif
   }

   if ((yyin = fopen(current_file, "r")) == NULL) {
      printf("Parser error. ");
      fflush(stdout);
      strcpy(error, "fopen(");
      strcat(error, current_file);
      strcat(error, ")");
      perror(error);
      exit(0);
   } else {
      lineno = 1;
      yyparse();
      fclose(yyin);
   }
}

void yyerror(s)
   char *s;
{
   printf("\nParser: %s, line %d: %s\n", current_file, lineno, s);
   switch (current_file_type) {
      case (ANIMATION_FILETYPE):
         printf("\tAnimation File formats:\n");
         printf("\tBB_SEQ model_file delay\n");
         printf("\tMODEL_SEQ model_file delay\n");
         printf("\t\t(token present?)\n");
         break;
      case (BLINKING_FILETYPE):
         printf("\tBlinking Lights File format:\n");
         printf("\tLIGHT x y z r g b size rate\n");
         printf("\t\t(token 'LIGHT' present?)\n");
         printf("\t\t(EOL comment preceded with '#'?)\n");
         break;
      case (FILTER_FILETYPE):
         printf("\tFilter File formats:\n");
         printf("\tBLOCK_ESPDU site host entity\n");
         printf("\tFORCE_APPEARANCE x y x ANDmask ORmask range\n");
         printf("\t\t(token present?)\n");
         break;
      case (LANDMARK_FILETYPE):
         printf("\tLandmark File format:\n");
         printf("\tLANDMARK_ICON key x y\n");
         printf("\t\t(token 'LANDMARK_ICON' present?)\n");
         printf("\t\t(EOL comment preceded with '#'?)\n");
         printf("\t\t('#' used as key is invalid.)\n");
         break;
      case (LOCATION_FILETYPE):
         printf("\tLocation File formats:\n");
         printf("\tMODEL_DEF name dir alive dead otype\n");
         printf("\tPOSITION x y z heading pitch roll\n");
         break;
      case (MODEL_FILETYPE):
         printf("\tModel File formats:\n");
         printf("\tMODEL_DEF name dir alive dead vtype ");
         printf("\tkind dom cntry cat subcat spec\n");
         printf("\tJACK_DEF name jack_model dir alive dead vtype ");
         printf("\tkind dom cntry cat subcat spec\n");
         printf("\tWEAPON_DEF index class x y z delay\n");
         break;
      case (NETWORK_FILETYPE):
         printf("\tNetwork File format:\n");
         printf("\tHOST hostname site# host# interface\n");
         printf("\t\t(token 'HOST' present?)\n");
         break;
      case (RADAR_ICON_FILETYPE):
         printf("\tRadar Icon File format:\n");
         printf("\tRADAR_ICON min max identifier\n");
         printf("\t\t(token 'RADAR_ICON' present?)\n");
         printf("\t\t(EOL comment preceded with '#'?)\n");
         break;
      case (ROUND_WORLD_FILETYPE):
         printf("\tRound World File format:\n");
         printf("\tUTM_MAP northing easting zone# zone_let ");
         printf("\tdatum width height\n");
         printf("\t\t(token 'UTM_MAP' present?)\n");
         break;
      case (SOUNDS_FILETYPE):
         printf("\tSound File format:\n");
         printf("\tSOUND_DEF identifier filename\n");
         printf("\t\t(token 'SOUND_DEF' present?)\n");
         break;
      case (TERRAIN_FILETYPE):
         printf("\tTerrain File format:\n");
         printf("\tMODEL_DEF filename maskname\n");
         printf("\t\t(token 'MODEL_DEF' present?)\n");
         printf("\tDB_PAGING ON/OFF:\n");
         printf("\t\t(token 'DB_PAGING' present?)\n");
         break;
      case (TRANSPORT_FILETYPE):
         printf("\tTransport File format:\n");
         printf("\tLOCATION x y z heading pitch roll\n");
         printf("\t\t(token 'LOCATION' present?)\n");
         printf("\t\t(EOL comment preceded with '#'?)\n");
         break;
      default:
         break;
   }

   exit(-1);
}


/* Assign "M" line vals */
assign_stat_models(name, dir, alive, dead, otype)
   char *name, *dir;
   int  alive, dead, otype;
{
   error_reported2 = FALSE;
   i = Defaults->num_static_objs;
   if (i < MAX_STATIC_OBJ_TYPES) {
      strncpy(Defaults->SObjects[i].flightfile, (char *) name, STRING128);
      strncpy(Defaults->SObjects[i].path, (char *) dir, STRING128);
      Defaults->SObjects[i].alive_mod = alive;
      Defaults->SObjects[i].dead_mod = dead;
      Defaults->SObjects[i].otype = otype;
      Defaults->num_static_objs++;
   } else {
      if (error_reported == FALSE) {
         /* report error once */
         printf("Too many static objects in Location File (>%d).\n",
                                                        MAX_STATIC_OBJ_TYPES);
         error_reported = TRUE;
      }
   }
   return 1;
}


/* Assign "D" line vals */
assign_dyn_models(mtype, name, jname, dir, alive, dead, vtype,
                  kind, domain, country, category, subcategory, specific)
   int mtype;
   char *name, *jname, *dir;
   int  alive, dead, vtype;
   int	kind, domain, country, category, subcategory, specific;
{
   error_reported2 = FALSE;
   i = Defaults->num_dynamic_objs;
   if (i < MAX_VEHTYPES) {
      strncpy(Defaults->DObjects[i].filename, (char *) name, STRING128);
      strncpy(Defaults->DObjects[i].jack_model, (char *) jname, STRING64 );
      Defaults->DObjects[i].mtype = mtype;
      strncpy(Defaults->DObjects[i].path, (char *) dir, STRING128);
      Defaults->DObjects[i].alive_mod = alive;
      Defaults->DObjects[i].dead_mod = dead;
      Defaults->DObjects[i].vtype = vtype;
      Defaults->DObjects[i].etype.entity_kind = (EntityKind) kind;
      Defaults->DObjects[i].etype.domain = (Domain) domain;
      Defaults->DObjects[i].etype.country = (Country) country;
      Defaults->DObjects[i].etype.category = (Category) category;
      Defaults->DObjects[i].etype.subcategory = (Subcategory) subcategory;
      Defaults->DObjects[i].etype.specific = (Specific) specific;
      Defaults->DObjects[i].etype.extra = (Extra) 0;	/* not used */
      Defaults->num_dynamic_objs++;
   } else {
      if (error_reported == FALSE) {
         /* report error once */
         printf("Too many dynamic objects in Model File (>%d).\n",
                                                       MAX_VEHTYPES);
         error_reported = TRUE;
      }
   }
   return 1;
}

assign_terrain_mask(filename, mask)
   char	*filename;
   int	mask;
{
   i = Defaults->num_masks;
   if (i < MAX_MASKS) {
      strncpy(Defaults->Masks[i].filename, filename, STRING128);
      switch (mask) {
         case 1:
            Defaults->Masks[i].mask = BRIDGE_MASK;
            break;
         case 2:
            Defaults->Masks[i].mask = FOLIAGE_MASK;
            break;
         case 3:
            Defaults->Masks[i].mask = TERRAIN_MASK;
            break;
         case 4:
            Defaults->Masks[i].mask = TUNNEL_MASK;
            break;
         case 5:
            Defaults->Masks[i].mask = WATER_MASK;
            break;
      }
      Defaults->num_masks++;
   } else {
      if (error_reported == FALSE) {
         /* report error once */
         printf("Too many masks in Terrain File (>%d).\n", MAX_MASKS);
         error_reported = TRUE;
      }
   }
   return 1;
}

/* EOF */
yytabelem yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 282
# define YYLAST 698
yytabelem yyact[]={

    35,    36,    28,    39,    29,    30,    31,    32,   493,   494,
   549,    33,   232,   233,    34,   181,   182,    37,    38,    40,
    41,    43,    44,    45,   146,    53,    56,    46,    57,    58,
    47,    61,    62,    63,    52,   197,   198,    65,    68,    69,
    70,    71,   461,   462,   463,   464,   465,   195,   196,    72,
    64,    73,    77,    79,   541,    81,    82,    83,    85,    87,
    90,    96,    97,    99,   104,   106,   107,   114,   115,   116,
   117,   123,   127,   128,   129,   131,   132,   134,   135,   137,
   138,   140,   141,   136,   142,   147,   148,   149,   150,   151,
   152,   154,    92,    91,   153,    93,    94,    95,    24,    48,
    49,    50,    51,    55,    59,    60,    66,   101,   102,   103,
   105,   122,   111,   112,   113,   126,   133,   155,   130,   120,
   121,   119,   118,   124,   143,   144,   139,   145,   100,    84,
    67,   108,    78,   339,   340,   341,   540,    89,    88,    54,
    80,    25,    86,    26,    27,   125,    98,    42,    74,    75,
    76,   402,   403,   404,   405,   406,   407,   408,   409,   410,
   411,   412,   413,   414,   415,   392,   393,   394,   395,   396,
   350,   351,   352,   353,   354,   323,   324,   325,   326,   327,
   328,   270,   271,   272,   273,   238,   239,   240,   241,   242,
   279,   280,   281,   282,   263,   264,   265,   266,   267,   268,
   269,   192,   193,   194,   390,   110,   109,    15,   391,   538,
    17,    16,    18,    19,    20,    21,    22,    23,   156,   157,
   161,   158,   159,   160,   162,   163,   164,   165,   166,   215,
   531,   529,   528,   387,   167,   168,   169,   388,   385,   383,
   381,   379,   386,   384,   382,   380,   377,   375,   367,   364,
   378,   376,   368,   365,   526,   206,   207,   209,   210,   212,
   208,   213,   211,   214,   216,   217,   218,   361,   359,   355,
   344,   362,   360,   356,   345,   334,   332,   329,   317,   335,
   333,   330,   318,   314,   310,   308,   306,   315,   311,   309,
   307,   300,   298,   295,   292,   301,   299,   296,   293,   290,
   287,   284,   258,   291,   288,   285,   259,   255,   251,   247,
   245,   256,   252,   248,   246,   243,   235,   224,   220,   244,
   236,   225,   221,   204,   199,   185,   183,   205,   200,   186,
   184,   519,   515,   512,   509,   501,   500,   499,   495,   492,
   491,   487,   479,   478,   477,   475,   474,   471,   468,   466,
   459,   448,   447,   446,   444,   443,   442,   441,   438,   435,
   434,   433,   431,   426,   418,   417,    14,   399,   398,   397,
   357,   312,   303,   302,   286,   278,   277,   274,   262,   261,
   254,   231,   230,   229,   228,   203,   201,   191,   190,   189,
   179,   178,   177,   573,   173,   511,   510,   489,   483,   460,
   455,   453,   452,   451,   450,   440,   439,   428,   424,   422,
   421,   420,   419,   374,   322,   313,   276,   275,   234,   176,
   172,   171,   170,   572,   571,   570,   569,   568,   567,   566,
   565,   564,   563,   562,   561,   560,   559,   558,   557,   556,
   555,   554,   553,   552,   551,   550,   548,   547,   546,   545,
   544,   543,   542,   539,   537,   536,   535,   534,   533,   532,
   530,   527,   525,   524,   523,   522,   521,   520,   518,   517,
   516,   514,   513,   508,   507,   506,   505,   504,   503,   502,
   498,   497,   496,   490,   488,   486,   485,   484,   482,   481,
   480,   476,   473,   472,   470,   469,   467,   458,   457,   456,
   454,   449,   445,   437,   436,   432,   427,   425,   423,   416,
   373,   349,   348,   347,   346,   343,   337,   336,   319,   305,
   304,   297,   227,   223,   222,   202,   175,   174,   180,    13,
    12,    11,    10,     9,     8,     7,     6,     5,     4,     3,
     2,     1,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   187,   188,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   219,     0,     0,     0,     0,   226,     0,     0,     0,     0,
     0,     0,     0,     0,   237,     0,     0,     0,     0,   249,
   250,     0,   253,     0,     0,   257,     0,   260,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   283,     0,
     0,     0,   289,     0,     0,   294,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   316,     0,     0,   320,   321,     0,     0,     0,   331,     0,
     0,     0,     0,   338,     0,   342,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   358,     0,     0,   363,     0,
   366,     0,   369,   370,   371,   372,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   389,     0,     0,     0,     0,
     0,   400,   401,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   429,   430 };
yytabelem yypact[]={

-10000000,  -264,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,   164,   163,   162,   135,   270,
   269,   161,   134,   131,  -245,    68,    67,  -245,  -245,   130,
   129,   128,   -71,  -229,  -244,    66,   127,   268,   126,    65,
  -160,  -245,    60,   267,   266,    59,  -245,   265,   125,   124,
   123,   122,  -287,   160,    58,  -245,  -242,    57,    52,    51,
  -245,  -245,    50,  -245,   121,    49,  -245,    44,  -245,   120,
   119,  -112,  -251,   118,   159,   158,   117,   116,  -207,  -245,
    43,   115,    42,  -245,    41,    36,  -245,    35,   264,    34,
    33,   114,   113,   263,   262,    28,    27,    26,   112,   157,
    25,  -245,    20,   261,  -245,  -245,   156,  -261,    19,  -245,
    18,    17,   260,   259,  -245,  -309,  -245,   258,    12,   257,
   256,   255,   254,  -275,    11,   111,  -245,    10,     9,  -245,
    -9,  -245,   -10,  -245,  -245,  -245,  -245,   253,   155,   -11,
   -12,   -17,   -18,   -19,   -20,   -25,  -245,   -54,  -285,   110,
   109,   108,  -245,  -245,  -304,   252,   107,   154,   153,   152,
   151,   251,   150,   250,   104,   249,   149,  -245,  -245,   103,
   248,   102,   101,   100,   247,   246,    99,   148,   147,    98,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,    97,-10000000,    96,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,    95,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,   245,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,    94,    93,    92,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,
-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,   244,   146,   145,   144,
   143,   243,   142,   242,   241,   240,    91,   141,  -451,-10000000,
-10000000,    90,   239,-10000000,-10000000,    89,   238,   237,    88,   236,
   235,    87,    86,-10000000,-10000000,   234,    85,    84,    83,   233,
   232,   231,   140,   230,   229,   228,    82,   227,   139,   226,
    81,-10000000,-10000000,-10000000,-10000000,-10000000,    80,  -464,    79,-10000000,
   225,-10000000,   224,   223,    78,    77,-10000000,    76,-10000000,-10000000,
   222,   221,   220,   219,   218,   217,   216,    75,   138,-10000000,
   137,-10000000,    74,   215,   214,    73,   213,   212,   211,    72,
-10000000,-10000000,-10000000,   210,   209,   208,   207,   206,   205,    -5,
-10000000,   204,   -27,-10000000,-10000000,   -28,   203,-10000000,-10000000,   -29,
   202,   201,   200,   199,   198,   197,   -50,   196,  -123,  -205,
-10000000,-10000000,   195,   194,   193,   192,   191,   190,-10000000,   189,
-10000000,  -249,   188,   187,   186,   185,   184,   183,-10000000,-10000000,
   182,   181,   180,   179,   178,   177,   176,   175,   174,   173,
   172,   171,   170,   169,   168,   167,-10000000,   166,-10000000,-10000000,
   136,-10000000,-10000000,-10000000 };
yytabelem yypgo[]={

     0,   541,   540,   539,   538,   537,   536,   535,   534,   533,
   532,   531,   530,   529,   366,   528 };
yytabelem yyr1[]={

     0,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     2,     2,     2,     2,    12,
     9,     4,     4,    14,    11,    10,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,    13,    13,    13,    13,    13,    13,    13,    15,    15,
     8,     8,     8,     8,     8,     8,     8,     6,     6,     6,
     3,     5 };
yytabelem yyr2[]={

     0,     0,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,    13,    13,     7,     7,     9,
    11,     9,    15,    15,    17,     9,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,    11,     5,     7,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     7,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     9,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,    11,     9,     9,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
    11,     7,     7,     7,     7,     7,     5,     5,     3,     3,
    25,    25,    27,    25,    23,    25,    15,    13,    13,    15,
    19,     9 };
yytabelem yychk[]={

-10000000,    -1,    -2,    -3,    -4,    -5,    -6,    -7,    -8,    -9,
   -10,   -11,   -12,   -13,   -14,   471,   475,   474,   476,   477,
   478,   479,   480,   481,   362,   405,   407,   408,   266,   268,
   269,   270,   271,   275,   278,   264,   265,   281,   282,   267,
   283,   284,   411,   285,   286,   287,   291,   294,   363,   364,
   365,   366,   298,   289,   403,   367,   290,   292,   293,   368,
   369,   295,   296,   297,   314,   301,   370,   394,   302,   303,
   304,   305,   313,   315,   412,   413,   414,   316,   396,   317,
   404,   319,   320,   321,   393,   322,   406,   323,   402,   401,
   324,   357,   356,   359,   360,   361,   325,   326,   410,   327,
   392,   371,   372,   373,   328,   374,   329,   330,   395,   470,
   469,   376,   377,   378,   331,   332,   333,   334,   386,   385,
   383,   384,   375,   335,   387,   409,   379,   336,   337,   338,
   382,   339,   340,   380,   341,   342,   347,   343,   344,   390,
   345,   346,   348,   388,   389,   391,   288,   349,   350,   351,
   352,   353,   354,   358,   355,   381,   482,   483,   485,   486,
   487,   484,   488,   489,   490,   491,   492,   498,   499,   500,
   258,   258,   258,   259,   257,   257,   258,   258,   257,   259,
   -15,   260,   261,   258,   262,   258,   262,   -15,   -15,   259,
   259,   259,   272,   273,   274,   276,   277,   279,   280,   258,
   262,   259,   257,   259,   258,   262,   415,   416,   420,   417,
   418,   422,   419,   421,   423,   389,   424,   425,   426,   -15,
   258,   262,   257,   257,   258,   262,   -15,   257,   259,   259,
   259,   259,   299,   300,   258,   258,   262,   -15,   427,   428,
   429,   430,   431,   258,   262,   258,   262,   258,   262,   -15,
   -15,   258,   262,   -15,   259,   258,   262,   -15,   258,   262,
   -15,   259,   259,   306,   307,   308,   309,   310,   311,   312,
   432,   433,   434,   435,   259,   258,   258,   259,   259,   397,
   398,   399,   400,   -15,   258,   262,   259,   258,   262,   -15,
   258,   262,   258,   262,   -15,   258,   262,   257,   258,   262,
   258,   262,   259,   259,   257,   257,   258,   262,   258,   262,
   258,   262,   259,   258,   258,   262,   -15,   258,   262,   257,
   -15,   -15,   258,   436,   437,   438,   439,   440,   441,   258,
   262,   -15,   258,   262,   258,   262,   257,   257,   -15,   442,
   443,   444,   -15,   257,   258,   262,   257,   257,   257,   257,
   445,   446,   447,   448,   449,   258,   262,   259,   -15,   258,
   262,   258,   262,   -15,   258,   262,   -15,   258,   262,   -15,
   -15,   -15,   -15,   257,   258,   258,   262,   258,   262,   258,
   262,   258,   262,   258,   262,   258,   262,   258,   262,   -15,
   258,   262,   450,   451,   452,   453,   454,   259,   259,   259,
   -15,   -15,   455,   456,   457,   458,   459,   460,   461,   462,
   463,   464,   465,   466,   467,   468,   257,   258,   257,   258,
   258,   258,   258,   257,   258,   257,   259,   257,   258,   -15,
   -15,   259,   257,   259,   259,   259,   257,   257,   259,   258,
   258,   259,   259,   259,   259,   257,   259,   259,   259,   257,
   258,   258,   258,   258,   257,   258,   257,   257,   257,   259,
   258,   493,   494,   495,   496,   497,   259,   257,   259,   257,
   257,   259,   257,   257,   259,   259,   257,   259,   259,   259,
   257,   257,   257,   258,   257,   257,   257,   259,   257,   258,
   257,   259,   259,   472,   473,   259,   257,   257,   257,   259,
   259,   259,   257,   257,   257,   257,   257,   257,   257,   259,
   258,   258,   259,   257,   257,   259,   257,   257,   257,   259,
   257,   257,   257,   257,   257,   257,   259,   257,   259,   259,
   257,   259,   257,   257,   257,   257,   257,   257,   259,   257,
   259,   259,   257,   257,   257,   257,   257,   257,   257,   259,
   257,   257,   257,   257,   257,   257,   257,   257,   257,   257,
   257,   257,   257,   257,   257,   257,   257,   257,   257,   257,
   257,   257,   257,   257 };
yytabelem yydef[]={

     1,    -2,     2,     3,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    26,   268,   269,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
    44,     0,    46,     0,    48,    49,    50,    51,    52,    53,
    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
    64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
    74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
    84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
    94,    95,    96,    97,     0,    99,   100,   101,   102,   103,
   104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
   114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
   124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
   154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
   164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
   174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
   184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
   194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
   204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
   214,   215,   216,     0,   218,   219,   220,   221,   222,   223,
   224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
   234,   235,   236,   237,   238,   239,   240,     0,     0,     0,
   244,   245,   246,   247,   248,   249,   250,   251,   252,   253,
   254,   255,   256,   257,   258,   259,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   266,
   267,     0,     0,    17,    18,     0,     0,     0,     0,     0,
     0,     0,     0,    47,    98,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   261,   262,   263,   264,   265,     0,     0,     0,    21,
     0,   281,     0,     0,     0,     0,   217,     0,   242,   243,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    25,
     0,    19,     0,     0,     0,     0,     0,     0,     0,     0,
    45,   241,   260,     0,     0,     0,     0,     0,     0,     0,
    20,     0,     0,    15,    16,     0,     0,   277,   278,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    22,   279,     0,     0,     0,     0,     0,     0,   276,     0,
    23,     0,     0,     0,     0,     0,     0,     0,    24,   280,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   274,     0,   270,   271,
     0,   273,   275,   272 };
typedef struct
#ifdef __cplusplus
	yytoktype
#endif
{ char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"INT",	257,
	"STRING",	258,
	"FLOAT",	259,
	"IM_ON",	260,
	"IM_OFF",	261,
	"IM_NONE",	262,
	"IM_EMPTY",	263,
	"BLINKING_LIGHTS_FILE",	264,
	"IM_BOUNDING_BOX",	265,
	"BDIHML",	266,
	"C2_FILE",	267,
	"DIGUY_SPEED_RATIO",	268,
	"DIGUY_POSITION_ERROR",	269,
	"DIGUY_TIME_CONSTANT",	270,
	"DIGUY_TRANS_SMOOTHNESS",	271,
	"TRANS_IMMEDIATE",	272,
	"IMMEDIATE_TRANS",	273,
	"TRANS_SMOOTHEST",	274,
	"DIGUY_POSITION_MODE",	275,
	"SMOOTH_MODE",	276,
	"PRECISE_MODE",	277,
	"DIGUY_POSITION_MECHANISM",	278,
	"DIGUY_INTERNAL",	279,
	"NPSNET_INTERNAL",	280,
	"CHANNEL",	281,
	"CLIP_PLANES",	282,
	"IM_CONTROL",	283,
	"CPU_LOCK",	284,
	"DB_OFFSET",	285,
	"DB_PAGES_ALLOWED",	286,
	"DB_TSG_FILE",	287,
	"USE_ROUND_WORLD",	288,
	"DUMP_FILE",	289,
	"EARTH_SKY_MODE",	290,
	"DEAD_SWITCH",	291,
	"ENVIRONMENTAL",	292,
	"ENTITY_FILE",	293,
	"DIS_EXERCISE",	294,
	"FCS_MP",	295,
	"FCS_PORT",	296,
	"FCS_PROMPT",	297,
	"DIS_ENUM",	298,
	"DISv203",	299,
	"DISv204",	300,
	"FILTER_FILE",	301,
	"FOG",	302,
	"FOG_NEAR",	303,
	"FOG_FAR",	304,
	"FOG_TYPE",	305,
	"FOG_VTX_LIN",	306,
	"FOG_VTX_EXP",	307,
	"FOG_VTX_EXP2",	308,
	"FOG_PIX_LIN",	309,
	"FOG_PIX_EXP",	310,
	"FOG_PIX_EXP2",	311,
	"FOG_PIX_SPLINE",	312,
	"FORCE",	313,
	"FOV",	314,
	"IM_FRAME_RATE",	315,
	"GROUND_HEIGHT",	316,
	"HMD",	317,
	"HEAD_MOUNTED_DISPLAY",	318,
	"HORIZON_ANGLE",	319,
	"HUD_FONT_FILE",	320,
	"HUD_LEVEL",	321,
	"INTRO_FONT_FILE",	322,
	"LANDMARK_FILE",	323,
	"LOCATION_FILE",	324,
	"MARKING",	325,
	"MASTER",	326,
	"MODEL_FILE",	327,
	"MULTISAMPLE",	328,
	"NETWORK_FILE",	329,
	"NOTIFY_LEVEL",	330,
	"PDUPOSITION",	331,
	"PHASE",	332,
	"PICTURES",	333,
	"PIPE",	334,
	"PROCESS_MODE",	335,
	"ROAD_FILE",	336,
	"ROUND_WORLD_FILE",	337,
	"SCORING",	338,
	"IM_SOUND",	339,
	"SOUND_FILE",	340,
	"STEALTH",	341,
	"STEREO",	342,
	"TARGET_ENTITY",	343,
	"IM_TERRAIN_FILE",	344,
	"TRAKER_FILE",	345,
	"TRAKER_PORT",	346,
	"TARGET_BOUNDING_BOX",	347,
	"TRANSPORT_FILE",	348,
	"VEHICLE_DRIVEN",	349,
	"VIEW",	350,
	"VIEWPORT",	351,
	"VIEW_HPR",	352,
	"VIEW_XYZ",	353,
	"WAYPOINTS",	354,
	"IM_WINDOW",	355,
	"LOD_SCALE",	356,
	"LOD_FADE",	357,
	"IM_WEAPON_VIEW",	358,
	"LW_RECEIVE_PORT",	359,
	"LW_SEND_PORT",	360,
	"LW_MACHINE_NAME",	361,
	"ANIMATIONS",	362,
	"DIS_HEARTBEAT",	363,
	"DIS_POSDELTA",	364,
	"DIS_ROTDELTA",	365,
	"DIS_TIME_OUT",	366,
	"DYNAMIC_OBJS",	367,
	"ETHER_INTERF",	368,
	"FCS_CHECK",	369,
	"FLYBOX",	370,
	"MULTICAST",	371,
	"MULTICAST_GROUP",	372,
	"MULTICAST_TTL",	373,
	"NETWORKING",	374,
	"IM_PORT",	375,
	"PANEL_GROUP",	376,
	"PANEL_TTL",	377,
	"PANEL_PORT",	378,
	"REMOTE_INPUT_PANEL",	379,
	"STATIC_OBJS",	380,
	"WINDOW_SIZE",	381,
	"SEARCH_PATH",	382,
	"IM_PO_EXERCISE",	383,
	"IM_PO_PORT",	384,
	"IM_PO_DATABASE",	385,
	"PO_CTDB_FILE",	386,
	"RADAR_ICON_FILE",	387,
	"TREADPORT",	388,
	"UNIPORT",	389,
	"TESTPORT",	390,
	"UPPERBODY",	391,
	"MODSAF_MAP",	392,
	"IDENTIFY_FILE",	393,
	"FLYBOX_PORT",	394,
	"NPS_PIC_DIR",	395,
	"HIRES",	396,
	"IM_NET_OFF",	397,
	"RECEIVE_ONLY",	398,
	"SEND_ONLY",	399,
	"SEND_AND_RECEIVE",	400,
	"LEATHER_FLYBOX_PORT",	401,
	"LEATHER_FLYBOX_CHECK",	402,
	"DVW_PATHS",	403,
	"HMD_FILE",	404,
	"ANIMATION_FILE",	405,
	"IM_JACK",	406,
	"AUTO_FILE",	407,
	"AUTO_STATE",	408,
	"RANGE_FILTER",	409,
	"IM_MAX_ALTITUDE",	410,
	"CURSOR_FILE",	411,
	"FS_CONFIG",	412,
	"FS_PORT",	413,
	"FS_SCALE",	414,
	"FCS",	415,
	"FLYBOXIN",	416,
	"KAFLIGHT",	417,
	"KEYBOARD",	418,
	"SPACEBALL",	419,
	"LEATHER_FLYBOX",	420,
	"DRIVE",	421,
	"PAD",	422,
	"BALL",	423,
	"SHIP_PANEL",	424,
	"SUB_PANEL",	425,
	"AUSA_ODT",	426,
	"FAST",	427,
	"TAG",	428,
	"SKY",	429,
	"SKY_GROUND",	430,
	"SKY_CLEAR",	431,
	"IM_BLUE",	432,
	"IM_RED",	433,
	"IM_WHITE",	434,
	"IM_OTHER",	435,
	"IM_FATAL",	436,
	"WARN",	437,
	"NOTICE",	438,
	"INFO",	439,
	"IM_DEBUG",	440,
	"FPDEBUG",	441,
	"ALG_FREE",	442,
	"ALG_FLOAT",	443,
	"ALG_LOCK",	444,
	"APPCULLDRAW",	445,
	"APP_CULLDRAW",	446,
	"APPCULL_DRAW",	447,
	"APP_CULL_DRAW",	448,
	"IM_DEFAULT",	449,
	"AIRPORT",	450,
	"CANYON",	451,
	"VILLAGE",	452,
	"PIER",	453,
	"IM_START",	454,
	"CLOSED",	455,
	"CUSTOM",	456,
	"SVGA",	457,
	"FULL",	458,
	"NTSC",	459,
	"PAL",	460,
	"NORMAL",	461,
	"VIM",	462,
	"THREE_TV",	463,
	"THREE_BUNKER",	464,
	"TWO_TPORT",	465,
	"TWO_ADJACENT",	466,
	"FS_MCO",	467,
	"FS_ST_BUFFERS",	468,
	"ODT_PORT",	469,
	"ODT_MP",	470,
	"ANIMATION_DEF",	471,
	"SEQ_CYCLE",	472,
	"SEQ_SWING",	473,
	"BB_SEQ",	474,
	"MODEL_SEQ",	475,
	"LIGHT",	476,
	"BLOCK_ESPDU",	477,
	"FORCE_APPEARANCE",	478,
	"LANDMARK_ICON",	479,
	"MODEL_DEF1",	480,
	"IM_POSITION",	481,
	"MODEL_DEF2",	482,
	"JACK_DEF",	483,
	"WEAPON_DEF",	484,
	"DUDE_DEF",	485,
	"JADE_DEF",	486,
	"DIGUY_DEF",	487,
	"IM_HOST",	488,
	"RADAR_ICON",	489,
	"UTM_MAP",	490,
	"SOUND_DEF",	491,
	"MODEL_DEF3",	492,
	"BRIDGE_MASK",	493,
	"FOLIAGE_MASK",	494,
	"TERRAIN_MASK",	495,
	"TUNNEL_MASK",	496,
	"WATER_MASK",	497,
	"DB_PAGING",	498,
	"FLAT_WORLD",	499,
	"LOCATION",	500,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"commands : /* empty */",
	"commands : commands Acommand",
	"commands : commands BLcommand",
	"commands : commands Fcommand",
	"commands : commands LMcommand",
	"commands : commands Lcommand",
	"commands : commands MCcommand",
	"commands : commands Mcommand",
	"commands : commands Ncommand",
	"commands : commands RIcommand",
	"commands : commands RWcommand",
	"commands : commands Scommand",
	"commands : commands Tcommand",
	"commands : commands TFcommand",
	"Acommand : ANIMATION_DEF STRING INT INT SEQ_CYCLE INT",
	"Acommand : ANIMATION_DEF STRING INT INT SEQ_SWING INT",
	"Acommand : MODEL_SEQ STRING FLOAT",
	"Acommand : BB_SEQ STRING FLOAT",
	"Scommand : SOUND_DEF INT STRING FLOAT",
	"Ncommand : IM_HOST STRING INT INT STRING",
	"Fcommand : BLOCK_ESPDU INT INT INT",
	"Fcommand : FORCE_APPEARANCE INT INT INT INT INT INT",
	"TFcommand : LOCATION FLOAT FLOAT FLOAT FLOAT FLOAT FLOAT",
	"RWcommand : UTM_MAP FLOAT FLOAT INT STRING INT INT INT",
	"RIcommand : RADAR_ICON INT INT STRING",
	"MCcommand : ANIMATIONS on_off",
	"MCcommand : ANIMATION_FILE STRING",
	"MCcommand : ANIMATION_FILE IM_NONE",
	"MCcommand : AUTO_FILE STRING",
	"MCcommand : AUTO_FILE IM_NONE",
	"MCcommand : AUTO_STATE on_off",
	"MCcommand : BDIHML on_off",
	"MCcommand : DIGUY_SPEED_RATIO FLOAT",
	"MCcommand : DIGUY_POSITION_ERROR FLOAT",
	"MCcommand : DIGUY_TIME_CONSTANT FLOAT",
	"MCcommand : DIGUY_TRANS_SMOOTHNESS TRANS_IMMEDIATE",
	"MCcommand : DIGUY_TRANS_SMOOTHNESS IMMEDIATE_TRANS",
	"MCcommand : DIGUY_TRANS_SMOOTHNESS TRANS_SMOOTHEST",
	"MCcommand : DIGUY_POSITION_MODE SMOOTH_MODE",
	"MCcommand : DIGUY_POSITION_MODE PRECISE_MODE",
	"MCcommand : DIGUY_POSITION_MECHANISM DIGUY_INTERNAL",
	"MCcommand : DIGUY_POSITION_MECHANISM NPSNET_INTERNAL",
	"MCcommand : BLINKING_LIGHTS_FILE STRING",
	"MCcommand : BLINKING_LIGHTS_FILE IM_NONE",
	"MCcommand : IM_BOUNDING_BOX FLOAT FLOAT FLOAT FLOAT",
	"MCcommand : CHANNEL INT",
	"MCcommand : CLIP_PLANES FLOAT FLOAT",
	"MCcommand : C2_FILE STRING",
	"MCcommand : C2_FILE IM_NONE",
	"MCcommand : IM_CONTROL FCS",
	"MCcommand : IM_CONTROL FLYBOXIN",
	"MCcommand : IM_CONTROL LEATHER_FLYBOX",
	"MCcommand : IM_CONTROL KAFLIGHT",
	"MCcommand : IM_CONTROL KEYBOARD",
	"MCcommand : IM_CONTROL PAD",
	"MCcommand : IM_CONTROL SPACEBALL",
	"MCcommand : IM_CONTROL DRIVE",
	"MCcommand : IM_CONTROL BALL",
	"MCcommand : IM_CONTROL UNIPORT",
	"MCcommand : IM_CONTROL SHIP_PANEL",
	"MCcommand : IM_CONTROL SUB_PANEL",
	"MCcommand : IM_CONTROL AUSA_ODT",
	"MCcommand : CPU_LOCK on_off",
	"MCcommand : CURSOR_FILE STRING",
	"MCcommand : CURSOR_FILE IM_NONE",
	"MCcommand : DB_OFFSET INT",
	"MCcommand : DB_PAGES_ALLOWED INT",
	"MCcommand : DB_TSG_FILE STRING",
	"MCcommand : DB_TSG_FILE IM_NONE",
	"MCcommand : DEAD_SWITCH on_off",
	"MCcommand : DIS_EXERCISE INT",
	"MCcommand : DIS_HEARTBEAT FLOAT",
	"MCcommand : DIS_POSDELTA FLOAT",
	"MCcommand : DIS_ROTDELTA FLOAT",
	"MCcommand : DIS_TIME_OUT FLOAT",
	"MCcommand : DIS_ENUM DISv203",
	"MCcommand : DIS_ENUM DISv204",
	"MCcommand : DUMP_FILE STRING",
	"MCcommand : DVW_PATHS STRING",
	"MCcommand : DVW_PATHS IM_NONE",
	"MCcommand : DYNAMIC_OBJS on_off",
	"MCcommand : EARTH_SKY_MODE FAST",
	"MCcommand : EARTH_SKY_MODE TAG",
	"MCcommand : EARTH_SKY_MODE SKY",
	"MCcommand : EARTH_SKY_MODE SKY_GROUND",
	"MCcommand : EARTH_SKY_MODE SKY_CLEAR",
	"MCcommand : ENVIRONMENTAL STRING",
	"MCcommand : ENVIRONMENTAL IM_NONE",
	"MCcommand : ENTITY_FILE STRING",
	"MCcommand : ENTITY_FILE IM_NONE",
	"MCcommand : ETHER_INTERF STRING",
	"MCcommand : ETHER_INTERF IM_NONE",
	"MCcommand : FCS_CHECK on_off",
	"MCcommand : FCS_MP on_off",
	"MCcommand : FCS_PORT STRING",
	"MCcommand : FCS_PORT IM_NONE",
	"MCcommand : FCS_PROMPT on_off",
	"MCcommand : FOV FLOAT FLOAT",
	"MCcommand : FILTER_FILE STRING",
	"MCcommand : FILTER_FILE IM_NONE",
	"MCcommand : FLYBOX on_off",
	"MCcommand : FLYBOX_PORT STRING",
	"MCcommand : FLYBOX_PORT IM_NONE",
	"MCcommand : FOG on_off",
	"MCcommand : FOG_NEAR FLOAT",
	"MCcommand : FOG_FAR FLOAT",
	"MCcommand : FOG_TYPE FOG_VTX_LIN",
	"MCcommand : FOG_TYPE FOG_VTX_EXP",
	"MCcommand : FOG_TYPE FOG_VTX_EXP2",
	"MCcommand : FOG_TYPE FOG_PIX_LIN",
	"MCcommand : FOG_TYPE FOG_PIX_EXP",
	"MCcommand : FOG_TYPE FOG_PIX_EXP2",
	"MCcommand : FOG_TYPE FOG_PIX_SPLINE",
	"MCcommand : FORCE IM_BLUE",
	"MCcommand : FORCE IM_RED",
	"MCcommand : FORCE IM_WHITE",
	"MCcommand : FORCE IM_OTHER",
	"MCcommand : IM_FRAME_RATE FLOAT",
	"MCcommand : FS_CONFIG STRING",
	"MCcommand : FS_PORT STRING",
	"MCcommand : FS_SCALE FLOAT",
	"MCcommand : GROUND_HEIGHT FLOAT",
	"MCcommand : HIRES IM_NET_OFF",
	"MCcommand : HIRES RECEIVE_ONLY",
	"MCcommand : HIRES SEND_ONLY",
	"MCcommand : HIRES SEND_AND_RECEIVE",
	"MCcommand : HMD on_off",
	"MCcommand : HMD_FILE STRING",
	"MCcommand : HMD_FILE IM_NONE",
	"MCcommand : HORIZON_ANGLE FLOAT",
	"MCcommand : HUD_FONT_FILE STRING",
	"MCcommand : HUD_FONT_FILE IM_NONE",
	"MCcommand : HUD_LEVEL on_off",
	"MCcommand : IDENTIFY_FILE STRING",
	"MCcommand : IDENTIFY_FILE IM_NONE",
	"MCcommand : INTRO_FONT_FILE STRING",
	"MCcommand : INTRO_FONT_FILE IM_NONE",
	"MCcommand : IM_JACK on_off",
	"MCcommand : LANDMARK_FILE STRING",
	"MCcommand : LANDMARK_FILE IM_NONE",
	"MCcommand : LEATHER_FLYBOX_CHECK INT",
	"MCcommand : LEATHER_FLYBOX_PORT STRING",
	"MCcommand : LEATHER_FLYBOX_PORT IM_NONE",
	"MCcommand : LOCATION_FILE STRING",
	"MCcommand : LOCATION_FILE IM_NONE",
	"MCcommand : LOD_FADE FLOAT",
	"MCcommand : LOD_SCALE FLOAT",
	"MCcommand : LW_RECEIVE_PORT INT",
	"MCcommand : LW_SEND_PORT INT",
	"MCcommand : LW_MACHINE_NAME STRING",
	"MCcommand : LW_MACHINE_NAME IM_NONE",
	"MCcommand : MARKING STRING",
	"MCcommand : MARKING IM_NONE",
	"MCcommand : MASTER STRING",
	"MCcommand : MASTER IM_NONE",
	"MCcommand : IM_MAX_ALTITUDE FLOAT",
	"MCcommand : MODEL_FILE STRING",
	"MCcommand : MODSAF_MAP STRING",
	"MCcommand : MODSAF_MAP IM_NONE",
	"MCcommand : MULTICAST on_off",
	"MCcommand : MULTICAST_GROUP STRING",
	"MCcommand : MULTICAST_GROUP IM_NONE",
	"MCcommand : MULTICAST_TTL INT",
	"MCcommand : MULTISAMPLE on_off",
	"MCcommand : NETWORKING on_off",
	"MCcommand : NETWORK_FILE STRING",
	"MCcommand : NOTIFY_LEVEL IM_FATAL",
	"MCcommand : NOTIFY_LEVEL WARN",
	"MCcommand : NOTIFY_LEVEL NOTICE",
	"MCcommand : NOTIFY_LEVEL INFO",
	"MCcommand : NOTIFY_LEVEL IM_DEBUG",
	"MCcommand : NOTIFY_LEVEL FPDEBUG",
	"MCcommand : NPS_PIC_DIR STRING",
	"MCcommand : NPS_PIC_DIR IM_NONE",
	"MCcommand : ODT_MP on_off",
	"MCcommand : ODT_PORT STRING",
	"MCcommand : ODT_PORT IM_NONE",
	"MCcommand : PANEL_GROUP STRING",
	"MCcommand : PANEL_GROUP IM_NONE",
	"MCcommand : PANEL_TTL INT",
	"MCcommand : PANEL_PORT INT",
	"MCcommand : PDUPOSITION on_off",
	"MCcommand : PHASE ALG_FREE",
	"MCcommand : PHASE ALG_FLOAT",
	"MCcommand : PHASE ALG_LOCK",
	"MCcommand : PICTURES on_off",
	"MCcommand : PIPE INT",
	"MCcommand : PO_CTDB_FILE STRING",
	"MCcommand : PO_CTDB_FILE IM_NONE",
	"MCcommand : IM_PO_DATABASE INT",
	"MCcommand : IM_PO_EXERCISE INT",
	"MCcommand : IM_PO_PORT INT",
	"MCcommand : IM_PORT INT",
	"MCcommand : PROCESS_MODE APPCULLDRAW",
	"MCcommand : PROCESS_MODE APP_CULLDRAW",
	"MCcommand : PROCESS_MODE APPCULL_DRAW",
	"MCcommand : PROCESS_MODE APP_CULL_DRAW",
	"MCcommand : PROCESS_MODE IM_DEFAULT",
	"MCcommand : RADAR_ICON_FILE STRING",
	"MCcommand : RADAR_ICON_FILE IM_NONE",
	"MCcommand : RANGE_FILTER FLOAT",
	"MCcommand : REMOTE_INPUT_PANEL on_off",
	"MCcommand : ROAD_FILE STRING",
	"MCcommand : ROAD_FILE IM_NONE",
	"MCcommand : ROUND_WORLD_FILE STRING",
	"MCcommand : ROUND_WORLD_FILE IM_NONE",
	"MCcommand : SCORING on_off",
	"MCcommand : SEARCH_PATH STRING",
	"MCcommand : SEARCH_PATH IM_NONE",
	"MCcommand : IM_SOUND on_off",
	"MCcommand : SOUND_FILE STRING",
	"MCcommand : SOUND_FILE IM_NONE",
	"MCcommand : STATIC_OBJS on_off",
	"MCcommand : STEALTH on_off",
	"MCcommand : STEREO on_off",
	"MCcommand : TARGET_BOUNDING_BOX on_off",
	"MCcommand : TARGET_ENTITY INT INT INT",
	"MCcommand : IM_TERRAIN_FILE STRING",
	"MCcommand : TESTPORT STRING",
	"MCcommand : TESTPORT IM_NONE",
	"MCcommand : TRAKER_FILE STRING",
	"MCcommand : TRAKER_FILE IM_NONE",
	"MCcommand : TRAKER_PORT STRING",
	"MCcommand : TRAKER_PORT IM_NONE",
	"MCcommand : TRANSPORT_FILE STRING",
	"MCcommand : TRANSPORT_FILE IM_NONE",
	"MCcommand : TREADPORT STRING",
	"MCcommand : TREADPORT IM_NONE",
	"MCcommand : UNIPORT STRING",
	"MCcommand : UNIPORT IM_NONE",
	"MCcommand : UPPERBODY STRING",
	"MCcommand : UPPERBODY IM_NONE",
	"MCcommand : USE_ROUND_WORLD on_off",
	"MCcommand : VEHICLE_DRIVEN STRING",
	"MCcommand : VEHICLE_DRIVEN IM_NONE",
	"MCcommand : VIEW AIRPORT",
	"MCcommand : VIEW CANYON",
	"MCcommand : VIEW VILLAGE",
	"MCcommand : VIEW PIER",
	"MCcommand : VIEW IM_START",
	"MCcommand : VIEWPORT FLOAT FLOAT FLOAT FLOAT",
	"MCcommand : VIEW_HPR FLOAT FLOAT FLOAT",
	"MCcommand : VIEW_XYZ FLOAT FLOAT FLOAT",
	"MCcommand : WAYPOINTS on_off",
	"MCcommand : IM_WEAPON_VIEW on_off",
	"MCcommand : IM_WINDOW CLOSED",
	"MCcommand : IM_WINDOW CUSTOM",
	"MCcommand : IM_WINDOW SVGA",
	"MCcommand : IM_WINDOW FULL",
	"MCcommand : IM_WINDOW NTSC",
	"MCcommand : IM_WINDOW PAL",
	"MCcommand : IM_WINDOW NORMAL",
	"MCcommand : IM_WINDOW VIM",
	"MCcommand : IM_WINDOW THREE_TV",
	"MCcommand : IM_WINDOW THREE_BUNKER",
	"MCcommand : IM_WINDOW TWO_TPORT",
	"MCcommand : IM_WINDOW TWO_ADJACENT",
	"MCcommand : IM_WINDOW FS_MCO",
	"MCcommand : IM_WINDOW FS_ST_BUFFERS",
	"MCcommand : WINDOW_SIZE INT INT INT INT",
	"Tcommand : MODEL_DEF3 STRING BRIDGE_MASK",
	"Tcommand : MODEL_DEF3 STRING FOLIAGE_MASK",
	"Tcommand : MODEL_DEF3 STRING TERRAIN_MASK",
	"Tcommand : MODEL_DEF3 STRING TUNNEL_MASK",
	"Tcommand : MODEL_DEF3 STRING WATER_MASK",
	"Tcommand : DB_PAGING on_off",
	"Tcommand : FLAT_WORLD on_off",
	"on_off : IM_ON",
	"on_off : IM_OFF",
	"Mcommand : MODEL_DEF2 STRING STRING INT INT INT INT INT INT INT INT INT",
	"Mcommand : MODEL_DEF2 INT STRING INT INT INT INT INT INT INT INT INT",
	"Mcommand : JACK_DEF STRING STRING STRING INT INT INT INT INT INT INT INT INT",
	"Mcommand : DUDE_DEF STRING STRING INT INT INT INT INT INT INT INT INT",
	"Mcommand : JADE_DEF STRING INT INT INT INT INT INT INT INT INT",
	"Mcommand : DIGUY_DEF STRING STRING INT INT INT INT INT INT INT INT INT",
	"Mcommand : WEAPON_DEF INT INT FLOAT FLOAT FLOAT FLOAT",
	"Lcommand : MODEL_DEF1 STRING STRING INT INT INT",
	"Lcommand : MODEL_DEF1 INT STRING INT INT INT",
	"Lcommand : IM_POSITION FLOAT FLOAT FLOAT FLOAT FLOAT FLOAT",
	"BLcommand : LIGHT FLOAT FLOAT FLOAT FLOAT FLOAT FLOAT FLOAT FLOAT",
	"LMcommand : LANDMARK_ICON STRING FLOAT FLOAT",
};
#endif /* YYDEBUG */
/* 
 *	Copyright 1987 Silicon Graphics, Inc. - All Rights Reserved
 */

/* #ident	"@(#)yacc:yaccpar	1.10" */
#ident	"$Revision: 1.10 $"

/*
** Skeleton parser driver for yacc output
*/
#include "stddef.h"

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	return(0)
#define YYABORT		return(1)
#ifdef __cplusplus
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( gettxt("uxlibc:78", "syntax error - cannot backup") );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#else
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( gettxt("uxlibc:78", "Syntax error - cannot backup") );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#endif
#define YYRECOVERING()	(!!yyerrflag)
#define YYNEW(type)	malloc(sizeof(type) * yynewmax)
#define YYCOPY(to, from, type) \
	(type *) memcpy(to, (char *) from, yynewmax * sizeof(type))
#define YYENLARGE( from, type) \
	(type *) realloc((char *) from, yynewmax * sizeof(type))
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
#define YYFLAG		(-10000000)

/*
** global variables used by the parser
*/
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
#if defined(__STDC__) || defined(__cplusplus)
int yyparse(void)
#else
int yyparse()
#endif
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

#if YYMAXDEPTH <= 0
	if (yymaxdepth <= 0)
	{
		if ((yymaxdepth = YYEXPAND(0)) <= 0)
		{
#ifdef __cplusplus
			yyerror(gettxt("uxlibc:79", "yacc initialization error"));
#else
			yyerror(gettxt("uxlibc:79", "Yacc initialization error"));
#endif
			YYABORT;
		}
	}
#endif

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
		if ( ++yy_ps >= &yys[ yymaxdepth ] )	/* room on stack? */
		{
			int yynewmax;
			ptrdiff_t yys_off;

			/* The following pointer-differences are safe, since
			 * yypvt, yy_pv, and yypv all are a multiple of
			 * sizeof(YYSTYPE) bytes from yyv.
			 */
			ptrdiff_t yypvt_off = yypvt - yyv;
			ptrdiff_t yy_pv_off = yy_pv - yyv;
			ptrdiff_t yypv_off = yypv - yyv;

			int *yys_base = yys;
#ifdef YYEXPAND
			yynewmax = YYEXPAND(yymaxdepth);
#else
			yynewmax = 2 * yymaxdepth;	/* double table size */
			if (yymaxdepth == YYMAXDEPTH)	/* first time growth */
			{
				void *newyys = YYNEW(int);
				void *newyyv = YYNEW(YYSTYPE);
				if (newyys != 0 && newyyv != 0)
				{
					yys = YYCOPY(newyys, yys, int);
					yyv = YYCOPY(newyyv, yyv, YYSTYPE);
				}
				else
					yynewmax = 0;	/* failed */
			}
			else				/* not first time */
			{
				yys = YYENLARGE(yys, int);
				yyv = YYENLARGE(yyv, YYSTYPE);
				if (yys == 0 || yyv == 0)
					yynewmax = 0;	/* failed */
			}
#endif
			if (yynewmax <= yymaxdepth)	/* tables not expanded */
			{
#ifdef __cplusplus
				yyerror( gettxt("uxlibc:80", "yacc stack overflow") );
#else
				yyerror( gettxt("uxlibc:80", "Yacc stack overflow") );
#endif
				YYABORT;
			}
			yymaxdepth = yynewmax;

			/* reset pointers into yys */
			yys_off = yys - yys_base;
			yy_ps = yy_ps + yys_off;
			yyps = yyps + yys_off;

			/* reset pointers into yyv */
			yypvt = yyv + yypvt_off;
			yy_pv = yyv + yy_pv_off;
			yypv = yyv + yypv_off;
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
#ifdef __cplusplus
				yyerror( gettxt("uxlibc:81", "syntax error") );
#else
				yyerror( gettxt("uxlibc:81", "Syntax error") );
#endif
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
				/* FALLTHRU */
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
		
case 15:
# line 169 "parser.y"
{
	   index = yypvt[-3].cmd;
	   if (i < MAX_ANIMATIONS) {
	      Defaults->Animations[index].count = yypvt[-2].cmd;
	      strncpy(Defaults->Animations[index].identifier, yypvt[-4].string, STRING32);
	      Defaults->Animations[index].mode = PFSEQ_CYCLE;
	      Defaults->Animations[index].reps = yypvt[-0].cmd;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Animation index %d exceeds ", yypvt[-3].cmd);
	         printf("allowable in Animation File (>%d).\n",
	                                          MAX_ANIMATIONS);
	         error_reported = TRUE;
	      }
	   }
	} break;
case 16:
# line 187 "parser.y"
{
	   index = yypvt[-3].cmd;
	   if (i < MAX_ANIMATIONS) {
	      Defaults->Animations[index].count = yypvt[-2].cmd;
	      strncpy(Defaults->Animations[index].identifier, yypvt[-4].string, STRING32);
	      Defaults->Animations[index].mode = PFSEQ_SWING;
	      Defaults->Animations[index].reps = yypvt[-0].cmd;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Animation index %d exceeds ", yypvt[-3].cmd);
	         printf("allowable in Animation File (>%d).\n",
	                                          MAX_ANIMATIONS);
	         error_reported = TRUE;
	      }
	   }
	} break;
case 17:
# line 207 "parser.y"
{
	   i = Defaults->Animations[index].num_effects;
	   if (i < MAX_ANIM_COPIES) {
	      Defaults->Animations[index].Effect[i].effect_type = TYPE_MODEL;
	      strncpy(Defaults->Animations[index].Effect[i].filename,
	                                                      yypvt[-1].string, STRING128);
	      Defaults->Animations[index].Effect[i].delay = yypvt[-0].fvalue;
	      Defaults->Animations[index].num_effects++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many effects in Animation File (>%d).\n",
	                                                   MAX_ANIM_COPIES);
	         error_reported = TRUE;
	      }
	   }
	} break;
case 18:
# line 227 "parser.y"
{
	   i = Defaults->Animations[index].num_effects;
	   if (i < MAX_ANIM_COPIES) {
	      Defaults->Animations[index].Effect[i].effect_type = TYPE_BB;
	      strncpy(Defaults->Animations[index].Effect[i].filename,
	                                                      yypvt[-1].string, STRING128);
	      Defaults->Animations[index].Effect[i].delay = yypvt[-0].fvalue;
	      Defaults->Animations[index].num_effects++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many effects in Animation File (>%d).\n",
	                                                   MAX_ANIM_COPIES);
	         error_reported = TRUE;
	      }
	   }
	} break;
case 19:
# line 248 "parser.y"
{
	   i = yypvt[-2].cmd;	/* sound number */
	   if (i < MAX_SOUND_EVENTS) {
	      j = Defaults->Sounds[i].num_events;
	      if (j < MAX_SOUNDS_PER_EVENT) {
	         strncpy(Defaults->Sounds[i].Events[j].filename, yypvt[-1].string, STRING128);
	         Defaults->Sounds[i].Events[j].range = yypvt[-0].fvalue;
	         Defaults->Sounds[i].num_events++;
	      } else {
	         printf("Too many events for sound %d in Sound File (>%d).\n",
	                                   yypvt[-2].cmd, MAX_SOUNDS_PER_EVENT);
	      }
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf(
	            "Sound index %d exceeds allowable in Sound File (>%d).\n",
	                                          yypvt[-2].cmd, MAX_SOUND_EVENTS);
	         error_reported = TRUE;
	      }
	   }
	} break;
case 20:
# line 274 "parser.y"
{
	   i = Defaults->num_hosts;
	   if (i < MAX_HOSTS) {
	      strncpy(Defaults->Hosts[i].hostname, yypvt[-3].string, STRING64);
	      Defaults->Hosts[i].siteno = yypvt[-2].cmd;
	      Defaults->Hosts[i].hostno = yypvt[-1].cmd;
	      strncpy(Defaults->Hosts[i].interf, yypvt[-0].string, STRING32);
	      Defaults->num_hosts++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many hosts in Network File (>%d).\n", MAX_HOSTS);
	         error_reported = TRUE;
	      }
	   }
	} break;
case 21:
# line 294 "parser.y"
{
	   i = Defaults->num_filters;
	   if (i < MAX_PDU_FILTERS) {
	      Defaults->Filters[i].filter_type = TYPE_BLOCK;
	      Defaults->Filters[i].Type.Block.site = yypvt[-2].cmd;
	      Defaults->Filters[i].Type.Block.host = yypvt[-1].cmd;
	      Defaults->Filters[i].Type.Block.entity = yypvt[-0].cmd;
	      Defaults->num_filters++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many entities in Filter File (>%d).\n",
	                         MAX_PDU_FILTERS);
	         error_reported = TRUE;
	      }
	   }
	} break;
case 22:
# line 314 "parser.y"
{
	   i = Defaults->num_filters;
	   if (i < MAX_PDU_FILTERS) {
	      Defaults->Filters[i].filter_type = TYPE_FORCE;
	      Defaults->Filters[i].Type.Force.x = yypvt[-5].cmd;
	      Defaults->Filters[i].Type.Force.y = yypvt[-4].cmd;
	      Defaults->Filters[i].Type.Force.z = yypvt[-3].cmd;
	      Defaults->Filters[i].Type.Force.and_mask = (unsigned int) yypvt[-2].cmd;
	      Defaults->Filters[i].Type.Force.or_mask = (unsigned int) yypvt[-1].cmd;
	      Defaults->Filters[i].Type.Force.range = yypvt[-0].cmd;
	      Defaults->num_filters++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many entities in Filter File (>%d).\n",
	                         MAX_PDU_FILTERS);
	         error_reported = TRUE;
	      }
	   }
	} break;
case 23:
# line 338 "parser.y"
{
	   i = Defaults->num_transport_locs;
	   if (i < MAX_TRANSPORT) {
	      Defaults->TransportLocs[i].x = yypvt[-5].fvalue;
	      Defaults->TransportLocs[i].y = yypvt[-4].fvalue;
	      Defaults->TransportLocs[i].z = yypvt[-3].fvalue;
	      Defaults->TransportLocs[i].heading = yypvt[-2].fvalue;
	      Defaults->TransportLocs[i].pitch = yypvt[-1].fvalue;
	      Defaults->TransportLocs[i].roll = yypvt[-0].fvalue;
	      Defaults->num_transport_locs++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many locations in Transport File (>%d).\n",
	                                 MAX_TRANSPORT);
	         error_reported = TRUE;
	      }
	   }
	} break;
case 24:
# line 361 "parser.y"
{
	   Defaults->RoundWorld.northing = yypvt[-6].fvalue;
	   Defaults->RoundWorld.easting = yypvt[-5].fvalue;
	   Defaults->RoundWorld.zone_num = yypvt[-4].cmd;
	   Defaults->RoundWorld.zone_letter = yypvt[-3].string[0];
	   Defaults->RoundWorld.map_datum = yypvt[-2].cmd;
	   Defaults->RoundWorld.width = yypvt[-1].cmd;
	   Defaults->RoundWorld.height = yypvt[-0].cmd;
	} break;
case 25:
# line 374 "parser.y"
{
	   i = Defaults->num_radar_icons;
	   if (i < MAX_RADAR_DIVS) {
	      Defaults->RadarIcons[i].min = yypvt[-2].cmd;
	      Defaults->RadarIcons[i].max = yypvt[-1].cmd;
	      Defaults->RadarIcons[i].ident = yypvt[-0].string[0];
	      Defaults->num_radar_icons++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many icons in Radar Icons File (>%d).\n",
	                           MAX_RADAR_DIVS);
	         error_reported = TRUE;
	      }
	   }
	} break;
case 26:
# line 394 "parser.y"
{
	   mc->animations = boolean;
	} break;
case 27:
# line 400 "parser.y"
{
	   strncpy(mc->animation_file, yypvt[-0].string, STRING128);
	} break;
case 28:
# line 404 "parser.y"
{
	   strcpy(mc->animation_file, "NONE");
	} break;
case 29:
# line 410 "parser.y"
{
           strncpy(mc->auto_file, yypvt[-0].string, STRING128);
        } break;
case 30:
# line 414 "parser.y"
{
           strcpy(mc->auto_file, "NONE");
        } break;
case 31:
# line 420 "parser.y"
{
           mc->auto_state =  boolean;
        } break;
case 32:
# line 426 "parser.y"
{
           mc->bdihml = boolean;
        } break;
case 33:
# line 432 "parser.y"
{
           mc->diguy_info.speed_ratio = yypvt[-0].fvalue;
        } break;
case 34:
# line 438 "parser.y"
{
           mc->diguy_info.position_error = yypvt[-0].fvalue;
        } break;
case 35:
# line 444 "parser.y"
{
           mc->diguy_info.time_constant = yypvt[-0].fvalue;
        } break;
case 36:
# line 450 "parser.y"
{
           mc->diguy_info.trans_smoothness = TRANS_IMMEDIATE;
        } break;
case 37:
# line 454 "parser.y"
{
           mc->diguy_info.trans_smoothness = IMMEDIATE_TRANS;
        } break;
case 38:
# line 458 "parser.y"
{
           mc->diguy_info.trans_smoothness = TRANS_SMOOTHEST;
        } break;
case 39:
# line 464 "parser.y"
{
           mc->diguy_info.position_mode = SMOOTH_MODE;
        } break;
case 40:
# line 468 "parser.y"
{
           mc->diguy_info.position_mode = PRECISE_MODE;
        } break;
case 41:
# line 474 "parser.y"
{
           mc->diguy_info.position_mechanism = DIGUY_INTERNAL;
        } break;
case 42:
# line 478 "parser.y"
{
           mc->diguy_info.position_mechanism = NPSNET_INTERNAL;
        } break;
case 43:
# line 484 "parser.y"
{
	   strncpy(mc->blinking_lights_file, yypvt[-0].string, STRING128);
	} break;
case 44:
# line 488 "parser.y"
{
	   strcpy(mc->blinking_lights_file, "NONE");
	} break;
case 45:
# line 494 "parser.y"
{
	   mc->BoundingBox.xmin = yypvt[-3].fvalue;
	   mc->BoundingBox.xmax = yypvt[-2].fvalue;
	   mc->BoundingBox.ymin = yypvt[-1].fvalue;
	   mc->BoundingBox.ymax = yypvt[-0].fvalue;
	} break;
case 46:
# line 503 "parser.y"
{
	   mc->channel = yypvt[-0].cmd;
	} break;
case 47:
# line 509 "parser.y"
{
	   mc->ClipPlanes.cp_near = yypvt[-1].fvalue;
	   mc->ClipPlanes.cp_far = yypvt[-0].fvalue;
	} break;
case 48:
# line 516 "parser.y"
{
	   printf("C2_FILE command no longer supported.\n");
	} break;
case 49:
# line 520 "parser.y"
{
	   /* No action */
	} break;
case 50:
# line 526 "parser.y"
{
	   mc->control = FCS;
	} break;
case 51:
# line 530 "parser.y"
{
	   mc->control = FLYBOXIN;
	} break;
case 52:
# line 534 "parser.y"
{
	   mc->control = LEATHER_FLYBOX;
	} break;
case 53:
# line 538 "parser.y"
{
	   mc->control = KAFLIGHT;
	} break;
case 54:
# line 542 "parser.y"
{
	   mc->control = KEYBOARD;
	} break;
case 55:
# line 546 "parser.y"
{
	   mc->control = KEYBOARD;
	} break;
case 56:
# line 550 "parser.y"
{
	   mc->control = SPACEBALL;
	} break;
case 57:
# line 554 "parser.y"
{
	   mc->control = SPACEBALL;
	} break;
case 58:
# line 558 "parser.y"
{
	   mc->control = SPACEBALL;
	} break;
case 59:
# line 562 "parser.y"
{
	   mc->control = UNIPORT;
	} break;
case 60:
# line 566 "parser.y"
{
           mc->control = SHIP_PANEL;
        } break;
case 61:
# line 570 "parser.y"
{
           mc->control = SUB_PANEL;
        } break;
case 62:
# line 574 "parser.y"
{
           mc->control = AUSA_ODT;
        } break;
case 63:
# line 580 "parser.y"
{
	   mc->cpu_lock = boolean;
	} break;
case 64:
# line 586 "parser.y"
{
            strncpy(mc->cursor_file, yypvt[-0].string, STRING128);
        } break;
case 65:
# line 590 "parser.y"
{
           strcpy(mc->cursor_file, "NONE");
        } break;
case 66:
# line 596 "parser.y"
{
           mc->db_offset = yypvt[-0].cmd;
        } break;
case 67:
# line 602 "parser.y"
{
           mc->db_pages_allowed = yypvt[-0].cmd;
        } break;
case 68:
# line 608 "parser.y"
{
	   strncpy(mc->db_tsg_file, yypvt[-0].string, STRING128);
        } break;
case 69:
# line 612 "parser.y"
{
           strcpy(mc->db_tsg_file, "NONE");
        } break;
case 70:
# line 618 "parser.y"
{
           mc->dead_switch = boolean;
        } break;
case 71:
# line 624 "parser.y"
{
	   mc->DIS.exercise = yypvt[-0].cmd;
	} break;
case 72:
# line 630 "parser.y"
{
	   mc->DIS.heartbeat = yypvt[-0].fvalue;
	} break;
case 73:
# line 636 "parser.y"
{
	   mc->DIS.posdelta = yypvt[-0].fvalue;
	} break;
case 74:
# line 642 "parser.y"
{
	   mc->DIS.rotdelta = yypvt[-0].fvalue;
	} break;
case 75:
# line 648 "parser.y"
{
	   mc->DIS.timeout = yypvt[-0].fvalue;
	} break;
case 76:
# line 654 "parser.y"
{
           mc->dis_enum = DISv203;
        } break;
case 77:
# line 658 "parser.y"
{
           mc->dis_enum = DISv204;
        } break;
case 78:
# line 664 "parser.y"
{
	   /* Required */
	   strncpy(mc->dump_file, yypvt[-0].string, STRING128);
	} break;
case 79:
# line 671 "parser.y"
{
	   strncpy(mc->dvw_file, yypvt[-0].string, STRING128);
	} break;
case 80:
# line 675 "parser.y"
{
	   strcpy(mc->dvw_file, "NONE");
	} break;
case 81:
# line 681 "parser.y"
{
	   mc->dynamic_objs = boolean;
	} break;
case 82:
# line 687 "parser.y"
{
	   mc->es_mode = FAST;
	} break;
case 83:
# line 691 "parser.y"
{
	   mc->es_mode = TAG;
	} break;
case 84:
# line 695 "parser.y"
{
	   mc->es_mode = SKY;
	} break;
case 85:
# line 699 "parser.y"
{
	   mc->es_mode = SKY_GROUND;
	} break;
case 86:
# line 703 "parser.y"
{
	   mc->es_mode = SKY_CLEAR;
	} break;
case 87:
# line 709 "parser.y"
{
	   strncpy(mc->environmental_file, yypvt[-0].string, STRING128);
	} break;
case 88:
# line 713 "parser.y"
{
	   strcpy(mc->environmental_file, "NONE");
	} break;
case 89:
# line 719 "parser.y"
{
           strncpy(mc->entity_file, yypvt[-0].string, STRING128);
        } break;
case 90:
# line 723 "parser.y"
{
           strcpy(mc->entity_file, "NONE");
        } break;
case 91:
# line 729 "parser.y"
{
	   strncpy(mc->ether_interf, yypvt[-0].string, STRING32);
	} break;
case 92:
# line 733 "parser.y"
{
	   strcpy(mc->ether_interf, "NONE");
	} break;
case 93:
# line 739 "parser.y"
{
	   mc->fcs.check = boolean;
	} break;
case 94:
# line 745 "parser.y"
{
           mc->fcs.mp  = boolean;
        } break;
case 95:
# line 751 "parser.y"
{
	   strncpy(mc->fcs.port, yypvt[-0].string, STRING32);
	} break;
case 96:
# line 755 "parser.y"
{
	   strcpy(mc->fcs.port, "NONE");
	} break;
case 97:
# line 761 "parser.y"
{
	   mc->fcs.prompt = boolean;
	} break;
case 98:
# line 767 "parser.y"
{
	   mc->FoV.x = yypvt[-1].fvalue;
	   mc->FoV.y = yypvt[-0].fvalue;
	} break;
case 99:
# line 774 "parser.y"
{
	   strncpy(mc->filter_file, yypvt[-0].string, STRING128);
	} break;
case 100:
# line 778 "parser.y"
{
	   strcpy(mc->filter_file, "NONE");
	} break;
case 101:
# line 784 "parser.y"
{
	   mc->flybox = boolean;
	} break;
case 102:
# line 790 "parser.y"
{
	   strncpy(mc->flybox_port, yypvt[-0].string, STRING32);
	} break;
case 103:
# line 794 "parser.y"
{
	   strcpy(mc->flybox_port, "NONE");
	} break;
case 104:
# line 800 "parser.y"
{
	   mc->fog = boolean;
	} break;
case 105:
# line 806 "parser.y"
{
           mc->fog_near = yypvt[-0].fvalue;
        } break;
case 106:
# line 812 "parser.y"
{
           mc->fog_far = yypvt[-0].fvalue;
        } break;
case 107:
# line 818 "parser.y"
{
           mc->fog_type = FOG_VTX_LIN;
        } break;
case 108:
# line 822 "parser.y"
{
           mc->fog_type = FOG_VTX_EXP;
        } break;
case 109:
# line 826 "parser.y"
{
           mc->fog_type = FOG_VTX_EXP2;
        } break;
case 110:
# line 830 "parser.y"
{
           mc->fog_type = FOG_PIX_LIN;
        } break;
case 111:
# line 834 "parser.y"
{
           mc->fog_type = FOG_PIX_EXP;
        } break;
case 112:
# line 838 "parser.y"
{
           mc->fog_type = FOG_PIX_EXP2;
        } break;
case 113:
# line 842 "parser.y"
{
           mc->fog_type = FOG_PIX_SPLINE;
        } break;
case 114:
# line 848 "parser.y"
{
	   mc->force = ForceID_Blue;
	} break;
case 115:
# line 852 "parser.y"
{
	   mc->force = ForceID_Red;
	} break;
case 116:
# line 856 "parser.y"
{
	   mc->force = ForceID_White;
	} break;
case 117:
# line 860 "parser.y"
{
	   mc->force = ForceID_Other;
	} break;
case 118:
# line 866 "parser.y"
{
	   mc->frame_rate = yypvt[-0].fvalue;
	} break;
case 119:
# line 872 "parser.y"
{
	   strncpy(mc->fs_config, yypvt[-0].string, STRING128);
	} break;
case 120:
# line 878 "parser.y"
{
	   strncpy(mc->fs_port, yypvt[-0].string, STRING32);
	} break;
case 121:
# line 884 "parser.y"
{
           mc->fs_scale = yypvt[-0].fvalue;
        } break;
case 122:
# line 890 "parser.y"
{
	   mc->ground_height = yypvt[-0].fvalue;
	} break;
case 123:
# line 896 "parser.y"
{
	   mc->hires = IM_NET_OFF;
	} break;
case 124:
# line 900 "parser.y"
{
           mc->hires = RECEIVE_ONLY;
        } break;
case 125:
# line 904 "parser.y"
{
           mc->hires = SEND_ONLY;
        } break;
case 126:
# line 908 "parser.y"
{
           mc->hires = SEND_AND_RECEIVE;
        } break;
case 127:
# line 914 "parser.y"
{
	   mc->hmd = boolean;
	} break;
case 128:
# line 920 "parser.y"
{
           strncpy(mc->hmd_file, yypvt[-0].string, STRING128);
        } break;
case 129:
# line 924 "parser.y"
{
	   strcpy(mc->hmd_file, "NONE");
	} break;
case 130:
# line 930 "parser.y"
{
	   mc->horizon_angle = yypvt[-0].fvalue;
	} break;
case 131:
# line 936 "parser.y"
{
	   strncpy(mc->hud_font_file, yypvt[-0].string, STRING128);
	} break;
case 132:
# line 940 "parser.y"
{
	   strcpy(mc->hud_font_file, "NONE");
	} break;
case 133:
# line 946 "parser.y"
{
           mc->hud_level = boolean;
        } break;
case 134:
# line 952 "parser.y"
{
	   strncpy(mc->identify_file, yypvt[-0].string, STRING128);
	} break;
case 135:
# line 956 "parser.y"
{
	   strcpy(mc->identify_file, "NONE");
	} break;
case 136:
# line 962 "parser.y"
{
	   strncpy(mc->intro_font_file, yypvt[-0].string, STRING128);
	} break;
case 137:
# line 966 "parser.y"
{
	   strcpy(mc->intro_font_file, "NONE");
	} break;
case 138:
# line 972 "parser.y"
{
           mc->jack = boolean;
        } break;
case 139:
# line 978 "parser.y"
{
	   strncpy(mc->landmark_file, yypvt[-0].string, STRING128);
	} break;
case 140:
# line 982 "parser.y"
{
	   strcpy(mc->landmark_file, "NONE");
	} break;
case 141:
# line 988 "parser.y"
{
	   mc->leather_flybox_check = yypvt[-0].cmd;
	} break;
case 142:
# line 994 "parser.y"
{
	   strncpy(mc->leather_flybox_port, yypvt[-0].string, STRING32);
	} break;
case 143:
# line 998 "parser.y"
{
	   strcpy(mc->leather_flybox_port, "NONE");
	} break;
case 144:
# line 1004 "parser.y"
{
	   strncpy(mc->location_file, yypvt[-0].string, STRING128);
	} break;
case 145:
# line 1008 "parser.y"
{
	   strcpy(mc->location_file, "NONE");
	} break;
case 146:
# line 1014 "parser.y"
{
           mc->lod_fade = yypvt[-0].fvalue;
        } break;
case 147:
# line 1020 "parser.y"
{
           mc->lod_scale = yypvt[-0].fvalue;
        } break;
case 148:
# line 1026 "parser.y"
{
           mc->lw_rport = yypvt[-0].cmd;
        } break;
case 149:
# line 1032 "parser.y"
{
           mc->lw_sport = yypvt[-0].cmd;
        } break;
case 150:
# line 1038 "parser.y"
{
           strncpy(mc->lw_toip, yypvt[-0].string, STRING128);          
        } break;
case 151:
# line 1042 "parser.y"
{
           strcpy(mc->lw_toip, "NONE");
        } break;
case 152:
# line 1048 "parser.y"
{
	   strncpy(mc->marking, yypvt[-0].string, MARKINGS_LEN);
	} break;
case 153:
# line 1052 "parser.y"
{
	   strcpy(mc->marking, "NONE");
	} break;
case 154:
# line 1058 "parser.y"
{
	   strncpy(mc->master, yypvt[-0].string, STRING64);
	} break;
case 155:
# line 1062 "parser.y"
{
	   strcpy(mc->master, "NONE");
	} break;
case 156:
# line 1068 "parser.y"
{
           mc->alt.max = yypvt[-0].fvalue;
        } break;
case 157:
# line 1074 "parser.y"
{
	   /* Required */
	   strncpy(mc->model_file, yypvt[-0].string, STRING128);
	} break;
case 158:
# line 1081 "parser.y"
{
	   strncpy(mc->modsaf_map, yypvt[-0].string, STRING128);
	} break;
case 159:
# line 1085 "parser.y"
{
	   strcpy(mc->modsaf_map, "NONE");
	} break;
case 160:
# line 1091 "parser.y"
{
	   mc->multicast.enable = boolean;
	} break;
case 161:
# line 1097 "parser.y"
{
	   strncpy(mc->multicast.group, yypvt[-0].string, STRING32);
	} break;
case 162:
# line 1101 "parser.y"
{
	   strcpy(mc->multicast.group, "NONE");
	} break;
case 163:
# line 1107 "parser.y"
{
	   mc->multicast.ttl = yypvt[-0].cmd;
	} break;
case 164:
# line 1113 "parser.y"
{
	   mc->multisample = boolean;
	} break;
case 165:
# line 1119 "parser.y"
{
	   mc->networking = boolean;
	} break;
case 166:
# line 1125 "parser.y"
{
	   /* Required */
	   strncpy(mc->network_file, yypvt[-0].string, STRING128);
	} break;
case 167:
# line 1132 "parser.y"
{
	   mc->notify_level = IM_FATAL;
	} break;
case 168:
# line 1136 "parser.y"
{
	   mc->notify_level = WARN;
	} break;
case 169:
# line 1140 "parser.y"
{
	   mc->notify_level = NOTICE;
	} break;
case 170:
# line 1144 "parser.y"
{
	   mc->notify_level = INFO;
	} break;
case 171:
# line 1148 "parser.y"
{
	   mc->notify_level = IM_DEBUG;
	} break;
case 172:
# line 1152 "parser.y"
{
           mc->notify_level = FPDEBUG;
        } break;
case 173:
# line 1158 "parser.y"
{
	   strncpy(mc->nps_pic_dir, yypvt[-0].string, STRING128);
	   len = strlen(mc->nps_pic_dir);
	   if (mc->nps_pic_dir[len-1] != '\\') {
	      mc->nps_pic_dir[len] = '\\';
	      mc->nps_pic_dir[len+1] = '\0';
	   }
	} break;
case 174:
# line 1167 "parser.y"
{
	   strcpy(mc->nps_pic_dir, "NONE");
	} break;
case 175:
# line 1173 "parser.y"
{
           mc->odt_mp = boolean;
        } break;
case 176:
# line 1179 "parser.y"
{
           strncpy(mc->odt_port, yypvt[-0].string, STRING32);
        } break;
case 177:
# line 1183 "parser.y"
{
           strcpy(mc->odt_port, "NONE");
        } break;
case 178:
# line 1189 "parser.y"
{
	   strncpy(mc->panel.group, yypvt[-0].string, STRING32);
	} break;
case 179:
# line 1193 "parser.y"
{
	   strcpy(mc->panel.group, "NONE");
	} break;
case 180:
# line 1199 "parser.y"
{
	   mc->panel.ttl = yypvt[-0].cmd;
	} break;
case 181:
# line 1205 "parser.y"
{
	   mc->panel.port = yypvt[-0].cmd;
	} break;
case 182:
# line 1211 "parser.y"
{
	   mc->pdu_position = boolean;
	} break;
case 183:
# line 1217 "parser.y"
{
	   mc->phase = ALG_FREE;
	} break;
case 184:
# line 1221 "parser.y"
{
	   mc->phase = ALG_FLOAT;
	} break;
case 185:
# line 1225 "parser.y"
{
	   mc->phase = ALG_LOCK;
	} break;
case 186:
# line 1231 "parser.y"
{
	   mc->pictures = boolean;
	} break;
case 187:
# line 1237 "parser.y"
{
	   mc->pipe = yypvt[-0].cmd;
	} break;
case 188:
# line 1243 "parser.y"
{
	   strncpy(mc->PO.ctdb_file, yypvt[-0].string, STRING128);
	} break;
case 189:
# line 1247 "parser.y"
{
	   strcpy(mc->PO.ctdb_file, "NONE");
	} break;
case 190:
# line 1253 "parser.y"
{
	   mc->PO.database = yypvt[-0].cmd;
	} break;
case 191:
# line 1259 "parser.y"
{
	   mc->PO.exercise = yypvt[-0].cmd;
	} break;
case 192:
# line 1265 "parser.y"
{
	   mc->PO.port = yypvt[-0].cmd;
	} break;
case 193:
# line 1271 "parser.y"
{
	   mc->port = yypvt[-0].cmd;
	} break;
case 194:
# line 1277 "parser.y"
{
	   mc->process_mode = APPCULLDRAW;
	} break;
case 195:
# line 1281 "parser.y"
{
	   mc->process_mode = APP_CULLDRAW;
	} break;
case 196:
# line 1285 "parser.y"
{
	   mc->process_mode = APPCULL_DRAW;
	} break;
case 197:
# line 1289 "parser.y"
{
	   mc->process_mode = APP_CULL_DRAW;
	} break;
case 198:
# line 1293 "parser.y"
{
	   mc->process_mode = IM_DEFAULT;
	} break;
case 199:
# line 1299 "parser.y"
{
	   strncpy(mc->radar_icon_file, yypvt[-0].string, STRING128);
	} break;
case 200:
# line 1303 "parser.y"
{
	   strcpy(mc->radar_icon_file, "NONE");
	} break;
case 201:
# line 1309 "parser.y"
{
           mc->range_filter = yypvt[-0].fvalue;
        } break;
case 202:
# line 1316 "parser.y"
{
	   mc->remote_input_panel = boolean;
	} break;
case 203:
# line 1322 "parser.y"
{
	   strncpy(mc->road_file, yypvt[-0].string, STRING128);
	} break;
case 204:
# line 1326 "parser.y"
{
	   strcpy(mc->road_file, "NONE");
	} break;
case 205:
# line 1332 "parser.y"
{
	   strncpy(mc->round_world_file, yypvt[-0].string, STRING128);
	} break;
case 206:
# line 1336 "parser.y"
{
	   strcpy(mc->round_world_file, "NONE");
	} break;
case 207:
# line 1342 "parser.y"
{
	   mc->scoring = boolean;
	} break;
case 208:
# line 1348 "parser.y"
{
	   /* The search path is prepended to with repeated invocations */
	   /* of the command. */
	   if (strlen(mc->search_path) == 0)	/* First invocation */
	      strncpy(mc->search_path, yypvt[-0].string, MAX_SEARCH_PATH);
	   else {				/* Subsequent invocations */
	      if (strcasecmp(mc->search_path, "NONE") == 0)
	         bzero(mc->search_path, 5);
	      /* Will the new path fit? */
	      len = (strlen(mc->search_path) + 1); /* 1 for the colon */
	      if ((len + strlen(yypvt[-0].string)) < MAX_SEARCH_PATH) {
	         /* adequate space */
	         strcpy(tempstr, yypvt[-0].string);
	         strcat(tempstr, ":");
	         strcat(tempstr, mc->search_path);
	         strcpy(mc->search_path, tempstr);
	      } else {
	         printf("Inadequate storage for path \"%s\".\n", yypvt[-0].string);
	      }
	   }
	} break;
case 209:
# line 1370 "parser.y"
{
	   strcpy(mc->search_path, "NONE");
	} break;
case 210:
# line 1376 "parser.y"
{
	   mc->sound = boolean;
	} break;
case 211:
# line 1382 "parser.y"
{
	   strncpy(mc->sound_file, yypvt[-0].string, STRING128);
	} break;
case 212:
# line 1386 "parser.y"
{
	   strcpy(mc->sound_file, "NONE");
	} break;
case 213:
# line 1392 "parser.y"
{
	   mc->static_objs = boolean;
	} break;
case 214:
# line 1398 "parser.y"
{
	   mc->stealth = boolean;
	} break;
case 215:
# line 1404 "parser.y"
{
	   mc->stereo = boolean;
	} break;
case 216:
# line 1410 "parser.y"
{
           mc->target_bbox = boolean;
        } break;
case 217:
# line 1416 "parser.y"
{
          mc->target_entity.site = yypvt[-2].cmd;
          mc->target_entity.host = yypvt[-1].cmd;
          mc->target_entity.entity = yypvt[-0].cmd;
	} break;
case 218:
# line 1424 "parser.y"
{
	   /* Required */
	   strncpy(mc->terrain_file, yypvt[-0].string, STRING128);
	} break;
case 219:
# line 1431 "parser.y"
{
	   strncpy(mc->testport, yypvt[-0].string, STRING32);
	} break;
case 220:
# line 1435 "parser.y"
{
	   strcpy(mc->testport, "NONE");
	} break;
case 221:
# line 1441 "parser.y"
{
	   strncpy(mc->traker_file, yypvt[-0].string, STRING128);
	} break;
case 222:
# line 1445 "parser.y"
{
	   strcpy(mc->traker_file, "NONE");
	} break;
case 223:
# line 1451 "parser.y"
{
	   strncpy(mc->traker_port, yypvt[-0].string, STRING32);
	} break;
case 224:
# line 1455 "parser.y"
{
	   strcpy(mc->traker_port, "NONE");
	} break;
case 225:
# line 1461 "parser.y"
{
	   strncpy(mc->transport_file, yypvt[-0].string, STRING128);
	} break;
case 226:
# line 1465 "parser.y"
{
	   strcpy(mc->transport_file, "NONE");
	} break;
case 227:
# line 1471 "parser.y"
{
	   strncpy(mc->treadport, yypvt[-0].string, STRING32);
	} break;
case 228:
# line 1475 "parser.y"
{
	   strcpy(mc->treadport, "NONE");
	} break;
case 229:
# line 1481 "parser.y"
{
	   strncpy(mc->uniport, yypvt[-0].string, STRING32);
	} break;
case 230:
# line 1485 "parser.y"
{
	   strcpy(mc->uniport, "NONE");
	} break;
case 231:
# line 1491 "parser.y"
{
           strncpy(mc->upperbody, yypvt[-0].string, STRING32);
        } break;
case 232:
# line 1495 "parser.y"
{
           strcpy(mc->upperbody, "NONE");
        } break;
case 233:
# line 1501 "parser.y"
{
           mc->use_round = boolean;
        } break;
case 234:
# line 1507 "parser.y"
{
	   strncpy(mc->vehicle_driven, yypvt[-0].string, STRING32);
	} break;
case 235:
# line 1511 "parser.y"
{
	   strcpy(mc->vehicle_driven, "NONE");
	} break;
case 236:
# line 1517 "parser.y"
{
	   mc->view = AIRPORT;
	} break;
case 237:
# line 1521 "parser.y"
{
	   mc->view = CANYON;
	} break;
case 238:
# line 1525 "parser.y"
{
	   mc->view = VILLAGE;
	} break;
case 239:
# line 1529 "parser.y"
{
	   mc->view = PIER;
	} break;
case 240:
# line 1533 "parser.y"
{
	   mc->view = IM_START;
	} break;
case 241:
# line 1539 "parser.y"
{
	   mc->ViewPort.xmin = yypvt[-3].fvalue;
	   mc->ViewPort.xmax = yypvt[-2].fvalue;
	   mc->ViewPort.ymin = yypvt[-1].fvalue;
	   mc->ViewPort.ymax = yypvt[-0].fvalue;
	} break;
case 242:
# line 1548 "parser.y"
{
	   mc->ViewHPR.heading = yypvt[-2].fvalue;
	   mc->ViewHPR.pitch = yypvt[-1].fvalue;
	   mc->ViewHPR.roll = yypvt[-0].fvalue;
	} break;
case 243:
# line 1556 "parser.y"
{
	   mc->ViewXYZ.x = yypvt[-2].fvalue;
	   mc->ViewXYZ.y = yypvt[-1].fvalue;
	   mc->ViewXYZ.z = yypvt[-0].fvalue;
	} break;
case 244:
# line 1564 "parser.y"
{
	   mc->waypoints = boolean;
	} break;
case 245:
# line 1570 "parser.y"
{
           mc->weapon_view = boolean;
        } break;
case 246:
# line 1576 "parser.y"
{
	   mc->window.type = CLOSED;
	} break;
case 247:
# line 1580 "parser.y"
{
	   mc->window.type = CUSTOM;
	} break;
case 248:
# line 1584 "parser.y"
{
	   mc->window.type = SVGA;
	} break;
case 249:
# line 1588 "parser.y"
{
	   mc->window.type = FULL;
	} break;
case 250:
# line 1592 "parser.y"
{
	   mc->window.type = NTSC;
	} break;
case 251:
# line 1596 "parser.y"
{
	   mc->window.type = PAL;
	} break;
case 252:
# line 1600 "parser.y"
{
	   mc->window.type = NORMAL;
	} break;
case 253:
# line 1604 "parser.y"
{
	   mc->window.type = VIM;
	} break;
case 254:
# line 1608 "parser.y"
{
	   mc->window.type = THREE_TV;
	} break;
case 255:
# line 1612 "parser.y"
{
	   mc->window.type = THREE_BUNKER;
	} break;
case 256:
# line 1616 "parser.y"
{
	   mc->window.type = TWO_TPORT;
	} break;
case 257:
# line 1620 "parser.y"
{
           mc->window.type = TWO_ADJACENT;
        } break;
case 258:
# line 1624 "parser.y"
{
	   mc->window.type = FS_MCO;
	} break;
case 259:
# line 1628 "parser.y"
{
           mc->window.type = FS_ST_BUFFERS;
        } break;
case 260:
# line 1634 "parser.y"
{
	   mc->window.xmin = yypvt[-3].cmd;
	   mc->window.xmax = yypvt[-2].cmd;
	   mc->window.ymin = yypvt[-1].cmd;
	   mc->window.ymax = yypvt[-0].cmd;
	} break;
case 261:
# line 1644 "parser.y"
{
	   assign_terrain_mask(yypvt[-1].string, 1);
	} break;
case 262:
# line 1648 "parser.y"
{
	   assign_terrain_mask(yypvt[-1].string, 2);
	} break;
case 263:
# line 1652 "parser.y"
{
	   assign_terrain_mask(yypvt[-1].string, 3);
	} break;
case 264:
# line 1656 "parser.y"
{
	   assign_terrain_mask(yypvt[-1].string, 4);
	} break;
case 265:
# line 1660 "parser.y"
{
	   assign_terrain_mask(yypvt[-1].string, 5);
	} break;
case 266:
# line 1666 "parser.y"
{
           mc->db_paging = boolean;
        } break;
case 267:
# line 1672 "parser.y"
{
           mc->flat_world = boolean;
        } break;
case 268:
# line 1679 "parser.y"
{
           boolean = TRUE;
        } break;
case 269:
# line 1683 "parser.y"
{
           boolean = FALSE;
        } break;
case 270:
# line 1690 "parser.y"
{
	   assign_dyn_models(TYPE_FLT, yypvt[-10].string, "", yypvt[-9].string, yypvt[-8].cmd, yypvt[-7].cmd, yypvt[-6].cmd, yypvt[-5].cmd, 
                             yypvt[-4].cmd, yypvt[-3].cmd, yypvt[-2].cmd, yypvt[-1].cmd, yypvt[-0].cmd);
	} break;
case 271:
# line 1695 "parser.y"
{
	   /* Convert first "INT" back to string when model name is a num */
	   char tmp[32];
	   sprintf(tmp, "%d", yypvt[-10].cmd);
	   assign_dyn_models(TYPE_FLT, tmp, "", yypvt[-9].string, yypvt[-8].cmd, yypvt[-7].cmd, yypvt[-6].cmd, yypvt[-5].cmd,
                             yypvt[-4].cmd, yypvt[-3].cmd, yypvt[-2].cmd, yypvt[-1].cmd, yypvt[-0].cmd);
	} break;
case 272:
# line 1705 "parser.y"
{
           assign_dyn_models(TYPE_TIPS, yypvt[-11].string, yypvt[-10].string, yypvt[-9].string, yypvt[-8].cmd, yypvt[-7].cmd, yypvt[-6].cmd, yypvt[-5].cmd,
                             yypvt[-4].cmd, yypvt[-3].cmd, yypvt[-2].cmd, yypvt[-1].cmd, yypvt[-0].cmd);
        } break;
case 273:
# line 1712 "parser.y"
{
           assign_dyn_models(TYPE_JCD, yypvt[-10].string, "", yypvt[-9].string, yypvt[-8].cmd, yypvt[-7].cmd, yypvt[-6].cmd, yypvt[-5].cmd,
                             yypvt[-4].cmd, yypvt[-3].cmd, yypvt[-2].cmd, yypvt[-1].cmd, yypvt[-0].cmd);
        } break;
case 274:
# line 1719 "parser.y"
{
           assign_dyn_models(TYPE_JADE, yypvt[-9].string, "", "", yypvt[-8].cmd, yypvt[-7].cmd, yypvt[-6].cmd, yypvt[-5].cmd,
                             yypvt[-4].cmd, yypvt[-3].cmd, yypvt[-2].cmd, yypvt[-1].cmd, yypvt[-0].cmd);
        } break;
case 275:
# line 1726 "parser.y"
{
           assign_dyn_models(TYPE_DIGUY, yypvt[-10].string, yypvt[-9].string, "", yypvt[-8].cmd, yypvt[-7].cmd, yypvt[-6].cmd, yypvt[-5].cmd,
                             yypvt[-4].cmd, yypvt[-3].cmd, yypvt[-2].cmd, yypvt[-1].cmd, yypvt[-0].cmd);
        } break;
case 276:
# line 1733 "parser.y"
{
	   if (error_reported == FALSE) {
	      struct Weap *current;

	      /* Get current model */
	      i = (Defaults->num_dynamic_objs-1);
	      /* Get current munition */
	      j = Defaults->DObjects[i].num_weapons;

	      if (j < MAX_FIRE_INFO) {
	         current = &(Defaults->DObjects[i].weapons[j]);
	         current->index = yypvt[-5].cmd;
	         current->wclass = yypvt[-4].cmd;
	         current->x = yypvt[-3].fvalue;
	         current->y = yypvt[-2].fvalue;
	         current->z = yypvt[-1].fvalue;
	         current->delay = yypvt[-0].fvalue;
	         Defaults->DObjects[i].num_weapons++;
	      } else {
	         if (error_reported2 == FALSE) {
	            /* report error once */
	            printf("Too many weapons in Model File (>%d). ",
	                                                  MAX_FIRE_INFO);
	            printf("Model %s.\n", Defaults->DObjects[i].filename);
	            error_reported2 = TRUE;
	         }
	      }
	   }
	} break;
case 277:
# line 1766 "parser.y"
{
	   assign_stat_models(yypvt[-4].string, yypvt[-3].string, yypvt[-2].cmd, yypvt[-1].cmd, yypvt[-0].cmd);
	} break;
case 278:
# line 1770 "parser.y"
{
	   /* Convert first "INT" back to string when model name is a num */
	   char tmp[32];
	   sprintf(tmp, "%d", yypvt[-4].cmd);
	   assign_stat_models(tmp, yypvt[-3].string, yypvt[-2].cmd, yypvt[-1].cmd, yypvt[-0].cmd);
	} break;
case 279:
# line 1779 "parser.y"
{
	   if (error_reported == FALSE) {
	      struct Pos *current;

	      /* Get current model */
	      i = (Defaults->num_static_objs-1);
	      /* Get current point */
	      j = Defaults->SObjects[i].num_points;

	      if (j < MAX_POINTS) {
	         current = &(Defaults->SObjects[i].points[j]);
	         current->x = yypvt[-5].fvalue;
	         current->y = yypvt[-4].fvalue;
	         current->z = yypvt[-3].fvalue;
	         current->heading = yypvt[-2].fvalue;
	         current->pitch = yypvt[-1].fvalue;
	         current->roll = yypvt[-0].fvalue;
	         Defaults->SObjects[i].num_points++;
	      } else {
	         if (error_reported2 == FALSE) {
	            /* report error once */
	            printf("Too many points in Location File (>%d). ",
	                                                     MAX_POINTS);
	            printf("Model %s.\n", Defaults->SObjects[i].flightfile);
	            error_reported2 = TRUE;
	         }
	      }
	   }
	} break;
case 280:
# line 1812 "parser.y"
{
	   i = Defaults->num_blinking_lights;
	   if (i < MAX_BLINKING_LIGHTS) {
	      Defaults->BlinkingLights[i].Position.x = yypvt[-7].fvalue;
	      Defaults->BlinkingLights[i].Position.y = yypvt[-6].fvalue;
	      Defaults->BlinkingLights[i].Position.z = yypvt[-5].fvalue;
	      Defaults->BlinkingLights[i].Color.r = yypvt[-4].fvalue;
	      Defaults->BlinkingLights[i].Color.g = yypvt[-3].fvalue;
	      Defaults->BlinkingLights[i].Color.b = yypvt[-2].fvalue;
	      Defaults->BlinkingLights[i].size = yypvt[-1].fvalue;
	      Defaults->BlinkingLights[i].blink_rate = yypvt[-0].fvalue;
	      Defaults->num_blinking_lights++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many lights in Blinking Lights File (>%d).\n",
	                                     MAX_BLINKING_LIGHTS);
	         error_reported = TRUE;
	      }
	   }
	} break;
case 281:
# line 1837 "parser.y"
{
	   i = Defaults->num_landmarks;
	   if (i < MAX_LANDMARKS) {
	      Defaults->Landmarks[i].icon = yypvt[-2].string[0];
	      Defaults->Landmarks[i].x = yypvt[-1].fvalue;
	      Defaults->Landmarks[i].y = yypvt[-0].fvalue;
	      Defaults->num_landmarks++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many landmarks in Landmark File (>%d).\n",
	                                   MAX_LANDMARKS);
	         error_reported = TRUE;
	      }
	   }
	} break;
	}
	goto yystack;		/* reset registers in driver code */
}
