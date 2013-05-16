/* Copyright (c) 1996, Carnegie Mellon University
 *
 *    Digital Mapping Laboratory (MAPSLab)
 *    Computer Science Department
 *    Carnegie Mellon University
 *    5000 Forbes Avenue
 *    Pittsburgh, PA 15213-3890
 *    maps@cs.cmu.edu
 *    http://www.cs.cmu.edu/~MAPSLab
 *
 *
 * Permission to use, copy, and modify this software and its
 * documentation for any NON-COMMERCIAL purpose is hereby granted
 * without fee, provided that (i) the above copyright notices and the
 * following permission notices appear in ALL copies of the software
 * and related documentation, and (ii) The Digital Mapping Laboratory 
 * (MAPSLab) Computer Science Department, Carnegie Mellon University
 * be given written credit in your software's written documentation and
 * be given graphical credit on any start-up/credit screen your
 * software generates.
 *
 * We request that users of this software to return to maps@cs.cmu.edu
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 *
 * Redistribution of this code is forbidden without the express written 
 * consent of the MAPSLab Research Group. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 */
/* Read TSG (Tiled Scene Graph) header
 *****************************************************************
 * HISTORY
 *  19-Mar-96  Stephen Gifford (sjg)  Digital Mapping Laboratory
 *      School of Computer Science at Carnegie-Mellon University
 *      Created.
 */

#include <stdio.h>
#include <time.h>
#include <gl/glconsts.h>
#include <tsgpf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* Abort and free everything macro */
#define ABORT() { \
    tsgReadClose(context); \
    if (buf.data)  pfFree(buf.data); \
    return NULL; \
  }

static pfGeoState *parse_geostate(tsgContext *,struct tsgAppendBuffer *);

/* Reopen the FILE * in the dbase process */
tsgContext *tsgReadReopen(tsgContext *context)
{
  if ((context->file = open(context->path,O_RDONLY)) < 0)
    return NULL;
}

#define TREAD(data,size1,size2,file)  (read(file,data,size1*size2)!=size1*size2)

tsgContext *tsgReadOpen(char *filename)
{
  tsgContext *context;
  struct tsgAppendBuffer buf;
  int32 size;
  int i,j;
  pfGeoState *gst;
  void *arena = pfGetSharedArena();

  buf.data = NULL;
  buf.allocated = 0;
  buf.used = 0;

  /* Create the tsgContext */
  if (!(context = pfMalloc(sizeof(tsgContext),arena)))
    return NULL;

  /* Fill the context structure to a default state */
  strcpy(context->path,filename);
  context->indexed = 1;
  context->source = NULL;
  context->comment = NULL;
  context->tiles_x = context->tiles_y = 0;
  context->projinfo = NULL;
  context->datum = NULL;
  context->ntables = 0;
  context->tablenames = NULL;
  context->nmaterials = 0;
  context->names = NULL;
  context->materials = NULL;
  context->texList = NULL;
  context->file = NULL;
  context->tiles = NULL;
  context->texList = NULL;
  context->globalmat = NULL;
  context->mtl_share = NULL;  context->ctable_share = NULL;  context->fog_share = NULL;
  context->lmodel_share = NULL;  context->light_share = NULL;  context->texenv_share = NULL;
  context->texgen_share = NULL;  context->tex_share = NULL;
  if (!(context->mtl_share = pfdNewShare()))  { tsgReadClose(context);  return NULL; }
  if (!(context->ctable_share = pfdNewShare()))  { tsgReadClose(context);  return NULL; }
  if (!(context->fog_share = pfdNewShare()))  { tsgReadClose(context);  return NULL; }
  if (!(context->lmodel_share = pfdNewShare()))  { tsgReadClose(context);  return NULL; }
  if (!(context->light_share = pfdNewShare()))  { tsgReadClose(context);  return NULL; }
  if (!(context->texenv_share = pfdNewShare()))  { tsgReadClose(context);  return NULL; }
  if (!(context->texgen_share = pfdNewShare()))  { tsgReadClose(context);  return NULL; }
  if (!(context->tex_share = pfdNewShare()))  { tsgReadClose(context);  return NULL; }

  /* Open the file */
  if ((context->file = open(filename,O_RDONLY)) < 0) {
    pfNotify(PFNFY_FATAL,PFNFY_SYSERR,"Can't open GST database %s\n",
	     filename);
    tsgReadClose(context);
    return NULL;
  }

  /* read pre header */
  {
    int32 magic;
    int16 major, minor;

    /* Magic #, major, minor version */
    if (TREAD(&magic,sizeof(magic),1,context->file))  {tsgReadClose(context); return NULL;}
    if (TREAD(&major,sizeof(major),1,context->file))  {tsgReadClose(context); return NULL;}
    if (TREAD(&minor,sizeof(minor),1,context->file))  {tsgReadClose(context); return NULL;}

    /* Check versions */
    if (magic != TSG_MAGIC)  {tsgReadClose(context); return NULL;}
    if (major != TSG_MAJOR_VERSION)  {tsgReadClose(context); return NULL;}
    /* Minor version shouldn't matter */
  }

  /* Size for the rest of the header */
  if (TREAD(&buf.allocated,sizeof(int32),1,context->file))  {tsgReadClose(context);  return NULL;}

  /* Allocate the buffer and read in the header */
  if (!(buf.data = pfMalloc(buf.allocated,arena))) {
    tsgReadClose(context);
    return NULL;
  }
  if (TREAD(buf.data,buf.allocated,1,context->file))  {tsgReadClose(context);  return NULL;}

  /* Parse out the rest of the header fields */
  if (!(context->source = tsgBufReadStr(&buf))) { ABORT(); }
  if (!(context->comment = tsgBufReadStr(&buf))) { ABORT(); }
  if (tsgBufRead(&context->ll_x, sizeof(ieee64), &buf) < 0)  { ABORT(); }
  if (tsgBufRead(&context->ll_y, sizeof(ieee64), &buf) < 0)  { ABORT(); }
  if (tsgBufRead(&context->tile_size, sizeof(ieee32), &buf) < 0)  { ABORT(); }
  if (tsgBufRead(&context->tiles_x, sizeof(int32), &buf) < 0)  { ABORT(); }
  if (tsgBufRead(&context->tiles_y, sizeof(int32), &buf) < 0)  { ABORT(); }
  if (tsgBufRead(&context->zone, sizeof(int16), &buf) < 0)  { ABORT(); }
  if (!(context->projinfo = tsgBufReadStr(&buf)))  { ABORT(); }
  if (!(context->datum = tsgBufReadStr(&buf)))  { ABORT(); }

  /* Read in the material table header info */
  if (TREAD(&context->ntables,sizeof(int32),1,context->file))  { ABORT(); }
  if (TREAD(&context->nmaterials,sizeof(int32),1,context->file))  { ABORT(); }

  /* Read in the entire set of material tables */
  if (context->nmaterials || context->ntables) {
    if (TREAD(&size,sizeof(int32),1,context->file))  { ABORT(); }
    if (!(buf.data = pfRealloc(buf.data,size))) { ABORT(); }
    buf.allocated = size;
    buf.used = 0;
    if (TREAD(buf.data,buf.allocated,1,context->file))  { ABORT(); }

    /* Parse the names of the material tables (i.e. appearances) */
    if (context->ntables) {
      if (!(context->tablenames = pfMalloc(sizeof(char *)*context->ntables,arena)))  { ABORT(); }
      for (i=0;i<context->ntables;i++)  context->tablenames[i] = NULL;
      for (i=0;i<context->ntables;i++) {
	if (!(context->tablenames[i] = tsgBufReadStr(&buf)))  { ABORT(); }
      }
    }

    if (context->nmaterials) {
      /* Parse the material table names */
      if (!(context->names = pfMalloc(sizeof(char *)*context->nmaterials,arena)))  { ABORT(); }
      for (i=0;i!=context->nmaterials;i++)  context->names[i] = NULL;
      for (i=0;i!=context->nmaterials;i++)
	if (!(context->names[i] = tsgBufReadStr(&buf))) { ABORT(); }
    }
  
    /* Parse the material tables */
    if (context->ntables) {
      if (!(context->materials = pfMalloc(sizeof(pfList *)*context->ntables,arena))) { ABORT(); }
      for (i=0;i!=context->ntables;i++)
	context->materials[i] = NULL;
      for (i=0;i!=context->ntables;i++)
	if (!(context->materials[i] = pfNewList(sizeof(pfGeoState *),context->nmaterials,arena))) { ABORT(); }
    }

    /* Read out the default geostates for each table */
    if (context->ntables) {
      if (!(context->globalmat = pfMalloc(sizeof(pfGeoState *)*context->ntables,arena)))  { ABORT(); }
      for (i=0;i!=context->ntables;i++) {
	if (!(gst = parse_geostate(context,&buf)))  { ABORT(); }
	pfRef(gst);
	context->globalmat[i] = gst;
      }
    }

    /* Read out the regular geosets */
    for (i=0;i!=context->ntables;i++) {
      for (j=0;j!=context->nmaterials;j++) {
	if (!(gst = parse_geostate(context,&buf))) { ABORT(); }
	pfRef(gst);
	pfAdd(context->materials[i],gst);
      }
    }
  }

  /* Read in tile index */
  if (!(context->tiles = pfMalloc(sizeof(struct tsgTileLoc)*context->tiles_x*context->tiles_y,arena))) { ABORT(); }
  if (TREAD(context->tiles,sizeof(struct tsgTileLoc),context->tiles_x*context->tiles_y,context->file)) { ABORT(); }

  /* Create the unique texture list */
  context->texList = pfdGetSharedList(context->tex_share, pfGetTexClassType());

  /* We're done.  Free local storage and return. */
  pfFree(buf.data);
  return(context);
}

/* share_obj()
   Share a basic object type.
   */
static pfObject *share_obj(pfObject *obj, pfdShare *share)
{
  pfObject *match;

  /* Look for it */
  if (match = pfdFindSharedObject(share, obj)) {
    pfDelete(obj);
    return(match);
  }

  /* Not there, add it */
  pfdAddSharedObject(share, obj);
  return(obj);
}

/* parse_mtl()
   Parse and return a pfMaterial.
   */
int Mtl_Side_Mode[TSGMCL_NUMSIDE+2] = {TSGMCL_NUMSIDE, PFMTL_BOTH, PFMTL_FRONT, PFMTL_BACK, PFMTL_BOTH};

int Mtl_FrontBack_Mode[TSGMCL_NUMMODE+2] = {TSGMCL_NUMMODE, PFMTL_CMODE_OFF, PFMTL_CMODE_AMBIENT_AND_DIFFUSE, PFMTL_CMODE_AMBIENT, PFMTL_CMODE_DIFFUSE, PFMTL_CMODE_EMISSION, PFMTL_CMODE_SPECULAR, PFMTL_CMODE_OFF, PFMTL_CMODE_COLOR};

static pfMaterial *parse_mtl(tsgContext *context,struct tsgAppendBuffer *buf)
{
  pfMaterial *mtl;
  int8 bval;
  ieee32 fval;
  ieee32 color[3];
  int mode_val,side,colormode_side;

  mtl = pfNewMtl(pfGetSharedArena());
  if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { pfDelete(mtl); return NULL; }
  side = tsgGetMode(Mtl_Side_Mode,bval);
  pfMtlSide(mtl, side);
  if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { pfDelete(mtl); return NULL; }
  colormode_side = tsgGetMode(Mtl_Side_Mode,bval);

  if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { pfDelete(mtl); return NULL; }
  mode_val = tsgGetMode(Mtl_FrontBack_Mode,bval);
  pfMtlColorMode(mtl,colormode_side,mode_val);

  if (tsgBufRead(&fval, sizeof(ieee32), buf) < 0)  { pfDelete(mtl); return NULL; }
  pfMtlAlpha(mtl,fval);
  if (tsgBufRead(&fval, sizeof(ieee32), buf) < 0)  { pfDelete(mtl); return NULL; }
  pfMtlShininess(mtl,fval);

  if (tsgBufRead(color, sizeof(color), buf) < 0)  { pfDelete(mtl); return NULL; }
  pfMtlColor(mtl,PFMTL_AMBIENT,color[0],color[1],color[2]);
  if (tsgBufRead(color, sizeof(color), buf) < 0)  { pfDelete(mtl); return NULL; }
  pfMtlColor(mtl,PFMTL_DIFFUSE,color[0],color[1],color[2]);
  if (tsgBufRead(color, sizeof(color), buf) < 0)  { pfDelete(mtl); return NULL; }
  pfMtlColor(mtl,PFMTL_EMISSION,color[0],color[1],color[2]);
  if (tsgBufRead(color, sizeof(color), buf) < 0)  { pfDelete(mtl); return NULL; }
  pfMtlColor(mtl,PFMTL_SPECULAR,color[0],color[1],color[2]);

  /* Add it to the share list */
  return((pfMaterial *)share_obj((pfObject *)mtl, context->mtl_share));
}

/* parse_ctable()
   Parse and return a pfColorTable.
   */
static pfColortable *parse_ctable(tsgContext *context,struct tsgAppendBuffer *buf)
{
  pfColortable *table;
  int32 numcolors;
  ieee32 red,green,blue,alpha;
  pfVec4 color;
  int i;

  if (tsgBufRead(&numcolors, sizeof(int32), buf) < 0)  return NULL;
  table = pfNewCtab(numcolors, pfGetSharedArena());
  for (i=0;i<numcolors;i++) {
    if (tsgBufRead(&red, sizeof(ieee32), buf) < 0)  { pfDelete(table);  return NULL; }
    if (tsgBufRead(&green, sizeof(ieee32), buf) < 0)  { pfDelete(table);  return NULL; }
    if (tsgBufRead(&blue, sizeof(ieee32), buf) < 0)  { pfDelete(table);  return NULL; }
    if (tsgBufRead(&alpha, sizeof(ieee32), buf) < 0)  { pfDelete(table);  return NULL; }
    color[0] = red;  color[1] = green;  color[2] = blue;  color[3] = alpha;
    pfCtabColor(table, i, color);
  }

  return((pfColortable *)share_obj((pfObject *)table, context->ctable_share));
}

/* parse_fog()
   Parse and return a pfFog.
   */
int Fog_Type_Mode[TSGMFG_NUMTYPE+2] = {TSGMFG_NUMTYPE ,PFFOG_VTX_LIN ,PFFOG_VTX_LIN, PFFOG_VTX_EXP, PFFOG_VTX_EXP2, PFFOG_PIX_LIN, PFFOG_PIX_EXP, PFFOG_PIX_EXP2, PFFOG_PIX_SPLINE};
static pfFog *parse_fog(tsgContext *context,struct tsgAppendBuffer *buf)
{
  pfFog *fog;
  int8 bval;
  ieee32 red,green,blue;
  ieee32 near,far;
  ieee32 bias;
  ieee32 *range,*density;
  int32 numpoints;
  int mode_val;
  void *arena = pfGetSharedArena();
  int i;

  fog = pfNewFog(arena);
  if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { pfDelete(fog);  return NULL; }
  mode_val = tsgGetMode(Fog_Type_Mode,bval);
  pfFogType(fog, mode_val);

  if (tsgBufRead(&red, sizeof(ieee32), buf) < 0)  { pfDelete(fog);  return NULL; }
  if (tsgBufRead(&green, sizeof(ieee32), buf) < 0)  { pfDelete(fog);  return NULL; }
  if (tsgBufRead(&blue, sizeof(ieee32), buf) < 0)  { pfDelete(fog);  return NULL; }
  pfFogColor(fog, red, green, blue);

  if (tsgBufRead(&near, sizeof(ieee32), buf) < 0)  { pfDelete(fog);  return NULL; }
  if (tsgBufRead(&far, sizeof(ieee32), buf) < 0)  { pfDelete(fog);  return NULL; }
  pfFogRange(fog, near, far);

  if (tsgBufRead(&near, sizeof(ieee32), buf) < 0)  { pfDelete(fog);  return NULL; }
  if (tsgBufRead(&far, sizeof(ieee32), buf) < 0)  { pfDelete(fog);  return NULL; }
  pfFogOffsets(fog, near, far);

  if (tsgBufRead(&numpoints, sizeof(int32), buf) < 0)  { pfDelete(fog);  return NULL; }
  if (tsgBufRead(&bias, sizeof(ieee32), buf) < 0)  { pfDelete(fog);  return NULL; }

  if ((pfGetFogType(fog) == PFFOG_PIX_SPLINE) && numpoints) {
    if (!(range = pfMalloc(sizeof(range)*numpoints,arena)))  { pfDelete(fog);  return NULL; }
    if (!(density = pfMalloc(sizeof(density)*numpoints,arena)))  { pfDelete(fog);  pfFree(range);  return NULL; }
    for (i=0;i<numpoints;i++) {
      if (tsgBufRead(&range[i], sizeof(ieee32), buf) < 0)  {
	pfDelete(fog); pfFree(range); pfFree(density); return NULL;
      }
      if (tsgBufRead(&density[i], sizeof(ieee32), buf) < 0)  {
	pfDelete(fog); pfFree(range); pfFree(density); return NULL;
      }
    }
    pfFogRamp(fog,numpoints, range, density, bias);
    pfFree(range);
    pfFree(density);
  }

  return((pfFog *)share_obj((pfObject *)fog, context->fog_share));
}

/* parse_lmodel()
   Parse and return a pfLightModel.
   */
static pfLightModel *parse_lmodel(tsgContext *context,struct tsgAppendBuffer *buf)
{
  pfLightModel *lmodel;
  int8 bval;
  ieee32 atten0,atten1,atten2;
  int mode;
  ieee32 red,green,blue;

  lmodel = pfNewLModel(pfGetSharedArena());

  if (tsgBufRead(&atten0, sizeof(ieee32), buf) < 0)  { pfDelete(lmodel);  return NULL; }
  if (tsgBufRead(&atten1, sizeof(ieee32), buf) < 0)  { pfDelete(lmodel);  return NULL; }
  if (tsgBufRead(&atten2, sizeof(ieee32), buf) < 0)  { pfDelete(lmodel);  return NULL; }
  pfLModelAtten(lmodel, atten0, atten1, atten2);

  if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { pfDelete(lmodel);  return NULL; }
  if (bval & TSGMLM_LOCAL)  pfLModelLocal(lmodel, PF_ON);
  if (bval & TSGMLM_2SIDED)  pfLModelTwoSide(lmodel, PF_ON);

  if (tsgBufRead(&red, sizeof(ieee32), buf) < 0)  { pfDelete(lmodel);  return NULL; }
  if (tsgBufRead(&green, sizeof(ieee32), buf) < 0)  { pfDelete(lmodel);  return NULL; }
  if (tsgBufRead(&blue, sizeof(ieee32), buf) < 0)  { pfDelete(lmodel);  return NULL; }
  pfLModelAmbient(lmodel, red, green, blue);

  return((pfLightModel *)share_obj((pfObject *)lmodel, context->lmodel_share));
}

/* parse_light()
   Parse and return a pfLight.
   */
static pfLight *parse_light(tsgContext *context,struct tsgAppendBuffer *buf)
{
  pfLight *light;
  ieee32 red,green,blue;
  ieee32 atten0,atten1,atten2;
  ieee32 x,y,z,w;
  ieee32 dirx,diry,dirz;
  ieee32 coneangle,coneexp;
  int8 bval;

  light = pfNewLight(pfGetSharedArena());

  if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { pfDelete(light);  return NULL; }
  if (bval) pfLightOn(light);  else  pfLightOff(light);

  if (tsgBufRead(&red, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&green, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&blue, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  pfLightColor(light, PFMTL_AMBIENT, red, green, blue);

  if (tsgBufRead(&red, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&green, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&blue, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  pfLightColor(light, PFMTL_DIFFUSE, red, green, blue);

  if (tsgBufRead(&red, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&green, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&blue, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  pfLightColor(light, PFMTL_SPECULAR, red, green, blue);

  if (tsgBufRead(&atten0, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&atten1, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&atten2, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  pfLightAtten(light, atten0, atten1, atten2);

  if (tsgBufRead(&x, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&y, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&z, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&w, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  pfLightPos(light, x, y, z, w);

  if (tsgBufRead(&dirx, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&diry, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&dirz, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  pfSpotLightDir(light, dirx, diry, dirz);

  if (tsgBufRead(&coneangle, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  if (tsgBufRead(&coneexp, sizeof(ieee32), buf) < 0)  { pfDelete(light);  return NULL; }
  pfSpotLightCone(light, coneexp, coneangle);

  return((pfLight *)share_obj((pfObject *)light, context->light_share));
}

/* parse_texenv()
   Parse and return a pfTexEnv.
   */
int TexEnv_Mode[TSGMTE_NUMTYPE+2] = {TSGMTE_NUMTYPE, PFTE_MODULATE, PFTE_ALPHA, PFTE_BLEND, PFTE_DECAL, PFTE_MODULATE};
int TexEnv_Comp[TSGMTE_NUMCOMP+2] = {TSGMTE_NUMCOMP, PFTE_COMP_OFF, PFTE_COMP_OFF, PFTE_COMP_I_GETS_R, PFTE_COMP_I_GETS_G, PFTE_COMP_I_GETS_B, PFTE_COMP_I_GETS_A, PFTE_COMP_IA_GETS_RG, PFTE_COMP_IA_GETS_BA, PFTE_COMP_I_GETS_I};
static pfTexEnv *parse_texenv(tsgContext *context,struct tsgAppendBuffer *buf)
{
  pfTexEnv *tenv;
  ieee32 red,green,blue,alpha;
  int8 bval;
  int mode_val;

  tenv = pfNewTEnv(pfGetSharedArena());

  if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { pfDelete(tenv);  return NULL; }
  mode_val = tsgGetMode(TexEnv_Mode,bval);
  pfTEnvMode(tenv, mode_val);

  if (tsgBufRead(&red, sizeof(ieee32), buf) < 0)  { pfDelete(tenv);  return NULL; }
  if (tsgBufRead(&green, sizeof(ieee32), buf) < 0)  { pfDelete(tenv);  return NULL; }
  if (tsgBufRead(&blue, sizeof(ieee32), buf) < 0)  { pfDelete(tenv);  return NULL; }
  if (tsgBufRead(&alpha, sizeof(ieee32), buf) < 0)  { pfDelete(tenv);  return NULL; }
  pfTEnvBlendColor(tenv, red, green, blue, alpha);

  if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  {pfDelete(tenv);  return NULL; }
  mode_val = tsgGetMode(TexEnv_Comp,bval);
  pfTEnvComponent(tenv, mode_val);

  return((pfTexEnv *)share_obj((pfObject *)tenv, context->texenv_share));
}

/* parse_texgen()
   Parse and return a pfTexGen.
   */
int TexGen_mode_names[4] = {PF_Q,PF_R,PF_S,PF_T};
int TexGen_Mode[TSGMTG_NUMMODE+2] = {TSGMTG_NUMMODE, PFTG_OFF, PFTG_OFF, PFTG_EYE_LINEAR, PFTG_EYE_LINEAR_IDENT, PFTG_OBJECT_LINEAR, PFTG_SPHERE_MAP};
static pfTexGen *parse_texgen(tsgContext *context,struct tsgAppendBuffer *buf)
{
  pfTexGen *tgen;
  int8 bval;
  ieee32 x,y,z,d; /* texture plane equation */
  int mode_val,type;
  int i;

  tgen = pfNewTGen(pfGetSharedArena());

  for (i=0;i!=4;i++) {
    type = TexGen_mode_names[i];

    if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { pfDelete(tgen);  return NULL; }
    mode_val = tsgGetMode(TexGen_Mode,bval);
    pfTGenMode(tgen, type, mode_val);

    if (tsgBufRead(&x, sizeof(ieee32), buf) < 0)  { pfDelete(tgen);  return NULL; }
    if (tsgBufRead(&y, sizeof(ieee32), buf) < 0)  { pfDelete(tgen);  return NULL; }
    if (tsgBufRead(&z, sizeof(ieee32), buf) < 0)  { pfDelete(tgen);  return NULL; }
    if (tsgBufRead(&d, sizeof(ieee32), buf) < 0)  { pfDelete(tgen);  return NULL; }
    pfTGenPlane(tgen, type, x, y, z, d);
  }

  return((pfTexGen *)share_obj((pfObject *)tgen, context->texgen_share));
}

/* parse_texture()
   Parse and return a pfTexture.
   */
int Tex_mode_names[3] = {PFTEX_MINFILTER,PFTEX_MAGFILTER_ALPHA,PFTEX_MAGFILTER_COLOR};
static pfTexture *parse_texture(tsgContext *context,struct tsgAppendBuffer *buf)
{
  pfTexture *tex;
  char *name;
  int8 clamp;
  int i;
  int pfflags;
  int32 flags;

  tex = pfNewTex(pfGetSharedArena());

  if (!(name = tsgBufReadStr(buf)))  { pfDelete(tex);  return NULL; }
  pfLoadTexFile(tex, name);
  pfTexName(tex,name);
  pfFree(name);
  if (tsgBufRead(&clamp, sizeof(int8), buf) < 0)  { pfDelete(tex);  return NULL; }
  if (clamp & TSGMTX_CLAMPR)    pfTexRepeat(tex,PFTEX_WRAP_R, PFTEX_CLAMP);
  if (clamp & TSGMTX_CLAMPS)    pfTexRepeat(tex,PFTEX_WRAP_S, PFTEX_CLAMP);
  if (clamp & TSGMTX_CLAMPT)    pfTexRepeat(tex,PFTEX_WRAP_T, PFTEX_CLAMP);

  for (i=0;i!=3;i++) {
    if (tsgBufRead(&flags, sizeof(int32), buf) < 0)  { pfDelete(tex);  return NULL; }

    pfflags = 0;
    if (flags & TSGMTX_ADD)  pfflags |= PFTEX_ADD;
    if (flags & TSGMTX_ALPHA) pfflags |= PFTEX_ALPHA;
    if (flags & TSGMTX_BICUBIC) pfflags |= PFTEX_BICUBIC;
    if (flags & TSGMTX_BILINEAR) pfflags |= PFTEX_BILINEAR;
    if (flags & TSGMTX_COLOR) pfflags |= PFTEX_COLOR;
    if (flags & TSGMTX_DETAIL) pfflags |= PFTEX_DETAIL;
    if (flags & TSGMTX_FAST) pfflags |= PFTEX_FAST;
    if (flags & TSGMTX_GEQUAL) pfflags |= PFTEX_GEQUAL;
    if (flags & TSGMTX_LEQUAL) pfflags |= PFTEX_LEQUAL;
    if (flags & TSGMTX_LINEAR) pfflags |= PFTEX_LINEAR;
    if (flags & TSGMTX_MIPMAP) pfflags |= PFTEX_MIPMAP;
    if (flags & TSGMTX_MODULATE) pfflags |= PFTEX_MODULATE;
    if (flags & TSGMTX_POINT) pfflags |= PFTEX_POINT;
    if (flags & TSGMTX_QUADLINEAR) pfflags |= PFTEX_QUADLINEAR;
    if (flags & TSGMTX_SHARPEN) pfflags |= PFTEX_SHARPEN;
    if (flags & TSGMTX_TRILINEAR) pfflags |= PFTEX_TRILINEAR;
    pfTexFilter(tex,Tex_mode_names[i],pfflags);
  }

  return((pfTexture *)share_obj((pfObject *)tex, context->tex_share));
}

/* Mode translation arrays used in parse_geostate() */
int AlphaFunc_Mode[TSGMAF_NUMVAL+2] = {TSGMAF_NUMVAL, PFAF_ALWAYS, PFAF_ALWAYS, PFAF_EQUAL, PFAF_GEQUAL, PFAF_GREATER, PFAF_LEQUAL, PFAF_LESS, PFAF_NEVER, PFAF_NOTEQUAL, PFAF_OFF};

int CullFace_Mode[TSGMCF_NUMSIDE+2] = {TSGMCF_NUMSIDE, PFCF_OFF, PFCF_OFF, PFCF_FRONT, PFCF_BACK, PFCF_BOTH};

int Decal_Mode[TSGMDC_LAYER_NUMVAL+2] = {TSGMDC_LAYER_NUMVAL, PFDECAL_OFF, PFDECAL_OFF, PFDECAL_BASE, PFDECAL_BASE_FAST, PFDECAL_BASE_HIGH_QUALITY, PFDECAL_BASE_STENCIL, PFDECAL_BASE_DISPLACE, PFDECAL_LAYER, PFDECAL_LAYER_FAST, PFDECAL_LAYER_HIGH_QUALITY, PFDECAL_LAYER_STENCIL, PFDECAL_LAYER_DISPLACE, PFDECAL_LAYER_DISPLACE_AWAY};

int Trans_Mode[TSGMTR_NUMVAL+2] = {TSGMTR_NUMVAL, PFTR_OFF, PFTR_OFF, PFTR_ON, PFTR_BLEND_ALPHA, PFTR_FAST, PFTR_HIGH_QUALITY, PFTR_MS_ALPHA, PFTR_MS_ALPHA_MASK};

/* parse_geostate()
   Parse a geostate, create it, and return.
   */
#define GABORT() {  pfDelete(gst); return NULL; }
#define GST_UNSUPPORTED() { \
  pfNotify(PFNFY_FATAL,PFNFY_SYSERR,"Unsupported opcode %d.\n",fieldid); \
  GABORT(); \
}
static pfGeoState *parse_geostate(tsgContext *context,struct tsgAppendBuffer *buf)
{
  pfGeoState *gst;
  void *arena = pfGetSharedArena();
  int8 bval;
  int32 ival;
  ieee32 fval;
  int mode_val;
  pfMaterial *mtl;
  int i,j,k;
  int8 fieldid;

  /* Create a geostate */
  if (!(gst = pfNewGState(arena)))  { GABORT(); }

  /* Each field results in some change to the geostate */
  do {
    /* Get the field ID and read the specific data for that field */
    if (tsgBufRead(&fieldid, sizeof(int8), buf) < 0) { GABORT(); }
    switch (fieldid) {
    case TSGM_ALPHAFUNC:
      if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { GABORT(); }
      mode_val = tsgGetMode(AlphaFunc_Mode,bval);
      pfGStateMode(gst,PFSTATE_ALPHAFUNC,mode_val);
      /* Also read Alpha ref */
      if (tsgBufRead(&fval, sizeof(ieee32), buf) < 0)  { GABORT(); }
      pfGStateVal(gst,PFSTATE_ALPHAREF,fval);
      break;
    case TSGM_ANTIALIAS:
      if (tsgBufRead(&bval, sizeof(int8), buf) < 0)   { GABORT(); }
      if (bval)  pfGStateMode(gst,PFSTATE_ANTIALIAS,PFAA_ON);
      break;
    case TSGM_BACKCOLOR:
      { 
	pfMaterial *mtl;
	if (!(mtl = parse_mtl(context,buf)))   { GABORT(); }
	pfGStateAttr(gst,PFSTATE_BACKMTL,mtl);
      }
      break;
    case TSGM_COLORTABLE:
      {
	pfColortable *table;
	if (!(table = parse_ctable(context, buf)))  { GABORT(); }
	pfGStateAttr(gst, PFSTATE_COLORTABLE, table);
      }
      break;
    case TSGM_CULLFACE:
      if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { GABORT(); }
      mode_val = tsgGetMode(CullFace_Mode,bval);
      pfGStateMode(gst,PFSTATE_CULLFACE, mode_val);
      break;
    case TSGM_DECAL:
      if (tsgBufRead(&ival, sizeof(int32), buf) < 0)  { GABORT(); }
      mode_val = tsgGetMode(Decal_Mode,(int8)(ival&0xFF));
      /* Deal with TSGMDC_LAYER* */
      switch (ival & (0xF00)) {
      case TSGMDC_LAYER1:
	mode_val |= PFDECAL_LAYER_1;
	break;
      case TSGMDC_LAYER2:
	mode_val |= PFDECAL_LAYER_2;
	break;
      case TSGMDC_LAYER3:
	mode_val |= PFDECAL_LAYER_3;
	break;
      case TSGMDC_LAYER4:
	mode_val |= PFDECAL_LAYER_4;
	break;
      case TSGMDC_LAYER5:
	mode_val |= PFDECAL_LAYER_5;
	break;
      case TSGMDC_LAYER6:
	mode_val |= PFDECAL_LAYER_6;
	break;
      case TSGMDC_LAYER7:
	mode_val |= PFDECAL_LAYER_7;
	break;
      }
      if(ival&TSGMDC_LAYER_OFFSET)
	mode_val |= PFDECAL_LAYER_OFFSET;
      pfGStateMode(gst,PFSTATE_DECAL,mode_val);
      break;
    case TSGM_ENCOLORTABLE:
      if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { GABORT(); }
      if (bval)  
	pfGStateMode(gst,PFSTATE_ENCOLORTABLE,PF_ON);
      else
	pfGStateMode(gst,PFSTATE_ENCOLORTABLE,PF_OFF);
      break;
    case TSGM_ENFOG:
      if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { GABORT(); }
      if (bval)  
	pfGStateMode(gst,PFSTATE_ENFOG,PF_ON);
      else
	pfGStateMode(gst,PFSTATE_ENFOG,PF_OFF);
      break;
    case TSGM_ENHIGHLIGHTING:
      if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { GABORT(); }
      if (bval)  
	pfGStateMode(gst,PFSTATE_ENHIGHLIGHTING,PF_ON);
      else
	pfGStateMode(gst,PFSTATE_ENHIGHLIGHTING,PF_OFF);
      break;
    case TSGM_ENLIGHTING:
      if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { GABORT(); }
      if (bval)
	pfGStateMode(gst,PFSTATE_ENLIGHTING,PF_ON);
      else
	pfGStateMode(gst,PFSTATE_ENLIGHTING,PF_OFF);
      break;
    case TSGM_ENLPOINTSTATE:
      if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { GABORT(); }
      if (bval) 
	pfGStateMode(gst,PFSTATE_ENLPOINTSTATE,PF_ON);
      else
	pfGStateMode(gst,PFSTATE_ENLPOINTSTATE,PF_OFF);
      break;
    case TSGM_ENTEXGEN:
      if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { GABORT(); }
      if (bval)
	pfGStateMode(gst,PFSTATE_ENTEXGEN,PF_ON);
      else
	pfGStateMode(gst,PFSTATE_ENTEXGEN,PF_OFF);
      break;
    case TSGM_ENTEXTURE:
      if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { GABORT(); }
      if (bval)
	pfGStateMode(gst,PFSTATE_ENTEXTURE,PF_ON);
      else
	pfGStateMode(gst,PFSTATE_ENTEXTURE,PF_OFF);
      break;
    case TSGM_ENWIREFRAME:
      if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { GABORT(); }
      if (bval)  
	pfGStateMode(gst,PFSTATE_ENWIREFRAME,PF_ON);
      else
	pfGStateMode(gst,PFSTATE_ENWIREFRAME,PF_OFF);
      break;
    case TSGM_FOG:
      {
	pfFog *fog;
	if (!(fog = parse_fog(context,buf)))  { GABORT(); }
	pfGStateAttr(gst,PFSTATE_FOG,fog);
      }
      break;
    case TSGM_FRONTCOLOR:
      {
	pfMaterial *mtl;
	if (!(mtl = parse_mtl(context,buf)))  { GABORT(); }
	pfGStateAttr(gst,PFSTATE_FRONTMTL,mtl);
      }
      break;
    case TSGM_HIGHLIGHT:
      GST_UNSUPPORTED();
      break;
    case TSGM_LIGHTMODEL:
      {
	pfLightModel *lmodel;
	if (!(lmodel = parse_lmodel(context,buf)))  { GABORT(); }
	pfGStateAttr(gst,PFSTATE_LIGHTMODEL,lmodel);
      }
      break;
    case TSGM_LIGHTS:
      {
	int8 numlights;
	pfLight **lights;
	if (tsgBufRead(&numlights, sizeof(int8), buf) < 0)  { GABORT(); }
	if (!(lights = pfMalloc(sizeof(pfLight *)*numlights,arena)))  { GABORT(); }
	for (j=0;j<numlights;j++) {
	  if (!(lights[j] = parse_light(context,buf))) {
	    for (k=0;k<j;k++)
	      pfDelete(lights[k]);
	    pfFree(lights);
	    GABORT();
	  }
	}
	pfGStateAttr(gst,PFSTATE_LIGHTS,lights);
      }
      break;
    case TSGM_LPOINTSTATE:
      GST_UNSUPPORTED();
      break;
    case TSGM_TEXENV:
      {
	pfTexEnv *tenv;
	if (!(tenv = parse_texenv(context,buf)))  { GABORT(); }
	pfGStateAttr(gst,PFSTATE_TEXENV,tenv);
      }
      break;
    case TSGM_TEXGEN:
      {
	pfTexGen *tgen;
	if (!(tgen = parse_texgen(context,buf)))  { GABORT(); }
	pfGStateAttr(gst,PFSTATE_TEXGEN,tgen);
      }
      break;
    case TSGM_TEXTURE:
      /* ***** Not fully implemented **** */
      {
	pfTexture *tex;
	if (!(tex = parse_texture(context,buf)))  { GABORT(); }
	pfGStateAttr(gst,PFSTATE_TEXTURE,tex);
      }
      break;
    case TSGM_TRANSPARENCY:
      if (tsgBufRead(&bval, sizeof(int8), buf) < 0)  { GABORT(); }
      mode_val = tsgGetMode(Trans_Mode,bval&~TSGMTR_NO_OCCLUDE);
      if (bval & TSGMTR_NO_OCCLUDE)  mode_val |= PFTR_NO_OCCLUDE;
      pfGStateMode(gst,PFSTATE_TRANSPARENCY,mode_val);
      break;
    case TSGM_END_OF_FIELDS:
      break;
    default:
      pfNotify(PFNFY_FATAL,PFNFY_SYSERR,"Unknown geostate token %d\n",fieldid);
      GABORT();
    }
  } while (fieldid != TSGM_END_OF_FIELDS);

  return(gst);
}
   
