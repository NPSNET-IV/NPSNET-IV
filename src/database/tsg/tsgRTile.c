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
/* Read TSG (Tiled Scene Graph) tile
 *****************************************************************
 * HISTORY
 *  20-Mar-96  Stephen Gifford (sjg)  Digital Mapping Laboratory
 *      School of Computer Science at Carnegie-Mellon University
 *      Created.
 */

#include <stdio.h>
#include <time.h>
/* #include <gl/glconsts.h> */
#include <tsgpf.h>
#include <unistd.h>

static pfGeode *parse_geode(tsgContext *,struct tsgAppendBuffer *);
static pfNode *parse_node(tsgContext *,struct tsgAppendBuffer *,pfNode *);
static pfNode *parse_thing(tsgContext *,struct tsgAppendBuffer *);
static pfLayer *parse_layer(tsgContext *,struct tsgAppendBuffer *);
static pfBillboard *parse_billboard(tsgContext *,struct tsgAppendBuffer *);
static pfGroup *parse_group(tsgContext *,struct tsgAppendBuffer *);
static pfLOD *parse_lod(tsgContext *,struct tsgAppendBuffer *);
static pfSCS *parse_scs(tsgContext *,struct tsgAppendBuffer *);

/* tsgReadTile()
   Read and return a scene graph.
   */
#define ABORT() { \
  if (buf.data)  pfFree(buf.data); \
}

#define TREAD(data,size1,size2,file)  (read(file,data,size1*size2)!=size1*size2)

pfNode *tsgReadTile(tsgContext *context, int x, int y)
{
  pfNode *node=NULL;
  struct tsgTileLoc *tile;
  struct tsgAppendBuffer buf;
  void *arena = pfGetSharedArena();

  buf.data = NULL;
  buf.allocated = 0;
  buf.used = 0;
  
  /* Sanity check */
  if (x < 0 || y < 0 || x >= context->tiles_x || y >= context->tiles_y) {
    pfNotify(PFNFY_WARN, PFNFY_SYSERR, "Invalid tile request (%d,%d) in tsgReadTile().\n",x,y);
    return NULL;
  }
  tile = &tsgTile(context,x,y);

  /* Might be empty */
  if (!tile->length)
    return NULL;

  /* Load the tile into a buffer */
  buf.allocated = tile->length;
  if (!(buf.data = pfMalloc(buf.allocated,arena))) {
    pfNotify(PFNFY_WARN, PFNFY_SYSERR, "Not enough memory in tsgReadTile().\n");
    ABORT();
  }
  lseek(context->file, tile->offset, SEEK_SET);
  if (TREAD(buf.data,buf.allocated,1,context->file))  { ABORT(); }

  /* Parse the tile until we're out of stuff */
  while (buf.used < buf.allocated) {
    /* Parse stuff */
    if (!(node = parse_thing(context,&buf))) { ABORT(); }
  }

  /* Free the bag */
  pfFree(buf.data);
  
  /* Just return the geode for now */
  return(node);
}

/* parse_thing()
   Parse and return whatever object is encountered.
   */
#define TILE_UNSUPPORTED() { \
  pfNotify(PFNFY_FATAL,PFNFY_SYSERR,"Unknown tile opcode %d.\n",opcode); \
  if (node)  pfDelete(node); \
  return NULL; \
}
static pfNode *parse_thing(tsgContext *context,struct tsgAppendBuffer *buf)
{
  int8 opcode;
  pfNode *node;

  /* Get the opcode */
  if (tsgBufRead(&opcode,sizeof(opcode), buf) < 0)  { return NULL; }

  switch (opcode) {
  case TSG_GEODE:
    {
      pfGeode *geode;
      if (!(geode = parse_geode(context,buf))) { return NULL; }
      node = (pfNode *)geode;
    }
    break;
  case TSG_BILLBOARD:
    {
      pfBillboard *bboard;
      if (!(bboard = parse_billboard(context,buf))) { return NULL; }
      node = (pfNode *)bboard;
    }
    break;
  case TSG_TEXT:
    TILE_UNSUPPORTED();
    break;
  case TSG_GROUP:
    {
      pfGroup *group;
      if (!(group = parse_group(context,buf))) { return NULL; }
      node = (pfNode *)group;
    }
    break;
  case TSG_PARTITION:
    TILE_UNSUPPORTED();
    break;
  case TSG_LAYER:
    {
      pfLayer *layer;
      if (!(layer = parse_layer(context,buf))) { return NULL; }
      node = (pfNode *)layer;
    }
    break;
  case TSG_LOD:
    {
      pfLOD *lod;
      if (!(lod = parse_lod(context,buf))) { return NULL; }
      node = (pfNode *)lod;
    }
    break;
  case TSG_SCS:
    {
      pfSCS *scs;
      if (!(scs = parse_scs(context,buf))) { return NULL; }
      node = (pfNode *)scs;
    }
    break;
  case TSG_DCS:
    TILE_UNSUPPORTED();
    break;
  case TSG_SWITCH:
    TILE_UNSUPPORTED();
    break;
  case TSG_SEQUENCE:
    TILE_UNSUPPORTED();
    break;
  case TSG_MORPH:
    TILE_UNSUPPORTED();
    break;
  case TSG_EXTREF:
    TILE_UNSUPPORTED();
    break;
  case TSG_DEFINE:
    TILE_UNSUPPORTED();
    break;
  case TSG_USE:
    TILE_UNSUPPORTED();
    break;
  default:
    return NULL;
    break;
  }

  return node;
}

/* parse_node()
   Parse node specific fields.
   */
static pfNode *parse_node(tsgContext *context, struct tsgAppendBuffer *buf, pfNode *node)
{
  char *name;
  int32 masks[4];
  ieee32 x, y, z, rad;
  pfSphere sph;

  if (!(name = tsgBufReadStr(buf))) { return NULL; }
  if (*name)
    pfNodeName(node,name);
  pfFree(name);

  if (tsgBufRead(masks,sizeof(masks),buf) < 0) { return NULL; }
  pfNodeTravMask(node,PFTRAV_APP,masks[0], PFTRAV_SELF, PF_SET);
  pfNodeTravMask(node,PFTRAV_CULL,masks[1], PFTRAV_SELF, PF_SET);
  pfNodeTravMask(node,PFTRAV_DRAW,masks[2], PFTRAV_SELF, PF_SET);
  pfNodeTravMask(node,PFTRAV_ISECT,masks[3], PFTRAV_SELF, PF_SET);

  if (tsgBufRead(&x,sizeof(ieee32),buf) < 0)  { return NULL; }
  if (tsgBufRead(&y,sizeof(ieee32),buf) < 0)  { return NULL; }
  if (tsgBufRead(&z,sizeof(ieee32),buf) < 0)  { return NULL; }
  sph.center[0] = x;  sph.center[1] = y;  sph.center[2] = z;
  if (tsgBufRead(&rad,sizeof(ieee32),buf) < 0)  { return NULL; }
  sph.radius = rad;
  if (rad >= 0.0f)
    pfNodeBSphere(node, &sph, PFBOUND_STATIC);
  else {
    sph.radius *= -1.0f;
    pfNodeBSphere(node, &sph, PFBOUND_DYNAMIC);
  }

  return(node);
}

/* parse_group_info()
   Parse all group specific information (i.e. children) */
static pfGroup *parse_group_info(tsgContext *context,struct tsgAppendBuffer *buf,pfGroup *group)
{
  int32 numchild;
  pfNode *child;
  int i;

  /* Read # of children */
  if (tsgBufRead(&numchild,sizeof(numchild),buf) < 0)  { return NULL; }

  /* Read the children */
  for (i=0;i<numchild;i++) {
    if (!(child = parse_thing(context,buf)))  { return NULL; }
    pfAddChild(group,child);
  }

  return(group);
}

/* parse_layer()
   Parse and return a pfLayer.
   */
#define LAYER_ABORT() { \
  if (layer)  pfDelete(layer); \
  return NULL; \
}
static pfLayer *parse_layer(tsgContext *context, struct tsgAppendBuffer *buf)
{
  pfLayer *layer = NULL;
  int mode;
  int32 ival;

  if (!(layer = pfNewLayer()))  { LAYER_ABORT(); }

  /* Parse the general pfNode fields */
  if (!(parse_node(context,buf,(pfNode *)layer))) { LAYER_ABORT(); }

  /* Layer mode */
  if (tsgBufRead(&ival,sizeof(ival),buf) < 0) { LAYER_ABORT(); }
  mode = tsgGetMode(Decal_Mode,(int8)ival&0xff);
  if(ival&TSGMDC_LAYER_OFFSET)
    mode |= PFDECAL_LAYER_OFFSET;
  pfLayerMode(layer,mode);

  /* Read the group related foo */
  if (!parse_group_info(context, buf, (pfGroup *)layer))  { LAYER_ABORT(); }

  return(layer);
}

/* parse_geoset_attr()
   Parse one of the attributes (color, normals, texture, etc...) for a geoset.
   */
int Geoset_Bind_Type[TSGG_BIND_NUMVAL+2] = {TSGG_BIND_NUMVAL,-1,PFGS_OFF,PFGS_OVERALL,PFGS_PER_PRIM,PFGS_PER_VERTEX};
static int parse_geoset_attr(struct tsgAppendBuffer *buf, size_t size, int8 *binding,
			     int16 *numattr, void **attr, int16 *numind, int16 **indices)
{
  int8 bval;
  void *arena = pfGetSharedArena();

  if (*attr)  pfFree(*attr);  *attr = NULL;
  if (*indices)  pfFree(*indices);  *indices = NULL;

  /* Bind type */
  if (tsgBufRead(&bval,sizeof(bval),buf) < 0)  { return -1; }
  if ((*binding = tsgGetMode(Geoset_Bind_Type,bval)) < 0)  { return -1; }

  /* Number of attributes */
  if (tsgBufRead(numattr,sizeof(int16),buf) < 0)  { return -1; }

  /* Attribute data */
  if (*numattr) {
    if (!(*attr = pfMalloc(size*(*numattr),arena)))  { return -1; }
    if (tsgBufRead(*attr,size*(*numattr),buf) < 0)  { return -1; }
  }

  /* Indices */
  if (tsgBufRead(numind,sizeof(int16),buf) < 0)  { return -1; }
  if (*numind) {
    if (!(*indices = pfMalloc(sizeof(int16)*(*numind),arena))) { return -1; }
    if (tsgBufRead(*indices,sizeof(int16)*(*numind),buf) < 0)  { return -1; }
  }

  return 0;
}

/* parse_geoset()
   Parse and return a geoset.
   */
#define GEOSET_ABORT() { \
  if (gset)  pfDelete(gset); \
  if (primlengths)  pfFree(primlengths); \
  if (indices)  pfFree(indices); \
  if (colors)  pfFree(colors); \
  if (norms)  pfFree(norms); \
  if (tex_coords)  pfFree(tex_coords); \
  if (vec3)  pfFree(vec3); \
  return NULL; \
}
int Geoset_Prim_Type[TSGG_PRIM_NUMVAL+2] = {TSGG_PRIM_NUMVAL,-1,PFGS_POINTS,PFGS_LINES,PFGS_LINESTRIPS,PFGS_FLAT_LINESTRIPS,PFGS_TRIS,PFGS_QUADS,PFGS_TRISTRIPS,PFGS_FLAT_TRISTRIPS,PFGS_POLYS};
static pfGeoSet *parse_geoset(tsgContext *context, struct tsgAppendBuffer *buf)
{
  int8 bval;
  int primtype;
  int32 numprims,gstate;
  ieee32 linewidth,pointsize;
  int32 isect_mask;
  int8 flags;
  pfGeoSet *gset=NULL;
  ieee32 min[3],max[3];
  pfBox box;
  int32 *primlengths=NULL;
  ushort *indices=NULL;
  pfVec4 *colors=NULL;
  int16 numind;
  int8 binding;
  pfVec3 *norms=NULL;
  pfVec2 *tex_coords=NULL;
  pfVec3 *vec3=NULL;
  int16 numattr;
  void *arena = pfGetSharedArena();

  if (!(gset = pfNewGSet(arena)))  { GEOSET_ABORT(); }

  /* Geoset info that's not dependent on the type */
  if (tsgBufRead(&bval,sizeof(int8),buf) < 0)  { GEOSET_ABORT(); }
  if ((primtype = tsgGetMode(Geoset_Prim_Type,bval)) < 0)  { GEOSET_ABORT(); }

  if (tsgBufRead(&numprims,sizeof(int32),buf) < 0)  { GEOSET_ABORT(); }
  if (tsgBufRead(&gstate,sizeof(int32),buf) < 0)  { GEOSET_ABORT(); }
  if (tsgBufRead(&linewidth,sizeof(ieee32),buf) < 0)  { GEOSET_ABORT(); }
  if (tsgBufRead(&pointsize,sizeof(ieee32),buf) < 0)  { GEOSET_ABORT(); }
  if (tsgBufRead(&isect_mask,sizeof(int32),buf) < 0)  { GEOSET_ABORT(); }

  /* Set basic info */
  pfGSetPrimType(gset, primtype);
  pfGSetNumPrims(gset, numprims);
  if (gstate != -1) {
    if (context->indexed)
      pfGSetGStateIndex(gset, gstate);
    else
      pfGSetGState(gset, pfGet(context->materials[0],gstate));
  } else
    pfGSetGState(gset, context->globalmat[0]);
  pfGSetPntSize(gset, pointsize);

  /* Various flags and associated data */
  if (tsgBufRead(&flags,sizeof(int8),buf) < 0)  { GEOSET_ABORT(); }
  if (flags & TSGG_HASBOUND) {
    if (tsgBufRead(min,sizeof(min),buf) < 0)  { GEOSET_ABORT(); }
    if (tsgBufRead(max,sizeof(max),buf) < 0)  { GEOSET_ABORT(); }
    box.min[0] = min[0];    box.min[1] = min[1];    box.min[2] = min[2];
    box.max[0] = max[0];    box.max[1] = max[1];    box.max[2] = max[2];
    pfGSetBBox(gset, &box, PFBOUND_STATIC);
  }
  if (flags & TSGG_HASLENGTHS) {
    if (!(primlengths = pfMalloc(sizeof(int32)*numprims,arena))) { GEOSET_ABORT(); }
    if (tsgBufRead(primlengths,sizeof(int32)*numprims,buf) < 0)  { GEOSET_ABORT(); }
    pfGSetPrimLengths(gset,primlengths);
  }

  /* Read attributes (color, normals, texture coords, vertex indices) */
  if (parse_geoset_attr(buf, sizeof(pfVec4), &binding, &numattr, (void **) &colors,
			&numind, (int16 **) &indices) < 0) { GEOSET_ABORT(); }
  pfGSetAttr(gset, PFGS_COLOR4, binding, colors, indices);
  colors = NULL; indices = NULL;

  if (parse_geoset_attr(buf, sizeof(pfVec3), &binding, &numattr, (void **) &norms,
			&numind, (int16 **) &indices) < 0) { GEOSET_ABORT(); }
  pfGSetAttr(gset, PFGS_NORMAL3, binding, norms, indices);
  norms = NULL; indices = NULL;

  if (parse_geoset_attr(buf, sizeof(pfVec2), &binding, &numattr, (void **) &tex_coords,
			&numind, (int16 **) &indices) < 0) { GEOSET_ABORT(); }
  pfGSetAttr(gset, PFGS_TEXCOORD2, binding, tex_coords, indices);
  tex_coords = NULL; indices = NULL;

  if (parse_geoset_attr(buf, sizeof(pfVec3), &binding, &numattr, (void **) &vec3,
			&numind, (int16 **) &indices) < 0) { GEOSET_ABORT(); }
  pfGSetAttr(gset, PFGS_COORD3, binding, vec3, indices);
  vec3 = NULL; indices = NULL;

  /* Must set the isect mask later */
  pfGSetIsectMask(gset, isect_mask, PFTRAV_SELF, PF_SET);

  return gset;
}

/* parse_geode_info()
   Parse the information related to a geode.
   */
static pfGeode *parse_geode_info(tsgContext *context, struct tsgAppendBuffer *buf,pfGeode *geode)
{
  pfGeoSet *geoset;
  int32 numgsets;
  int i;

  /* Number of geosets */
  if (tsgBufRead(&numgsets,sizeof(int32),buf) < 0)  { return NULL; }

  /* Read in geosets */
  for (i=0;i<numgsets;i++) {
    if (!(geoset = parse_geoset(context, buf)))  { return NULL; }
    pfAddGSet(geode, geoset);
  }

  return(geode);
}

/* parse_geode()
   Parse, allocate, and return a geode.
   */
static pfGeode *parse_geode(tsgContext *context, struct tsgAppendBuffer *buf)
{
  pfGeode *geode = NULL;

  /* New geode */
  if (!(geode = pfNewGeode()))  { return NULL; }

  /* Parse the general pfNode fields */
  if (!(parse_node(context,buf,(pfNode *)geode))) { return NULL; }

  /* Read geode fields */
  if (!parse_geode_info(context,buf,geode))  { pfDelete(geode);  return NULL; }

  return(geode);
}

/* parse_billboard()
   Parse, allocate, and return a billboard.
   */
#define BBOARD_ABORT() { \
  if (pos)  pfFree(pos); \
  if (bboard)  pfDelete(bboard); \
  return NULL; \
}
int Bboard_Mode[TSGB_NUM+2] = {TSGB_NUM, TSGB_AXIAL, TSGB_AXIAL, TSGB_WORLD, TSGB_EYE};
static pfBillboard *parse_billboard(tsgContext *ctx, struct tsgAppendBuffer *buf)
{
  pfBillboard *bboard = NULL;
  int32 numpos;
  pfVec3 *pos=NULL,axis;
  int32 mval;
  int mode,i;

  /* New billboard */
  if (!(bboard = pfNewBboard()))  { BBOARD_ABORT(); }

  /* Parse the general pfNode fields */
  if (!(parse_node(ctx,buf,(pfNode *)bboard))) { BBOARD_ABORT(); }

  /* Billboard specific fields */
  /* Positions */
  if (tsgBufRead(&numpos,sizeof(int32),buf) < 0)  { BBOARD_ABORT(); }
  if (numpos) {
    if (!(pos = pfMalloc(sizeof(pfVec3)*numpos,pfGetSharedArena())))  { BBOARD_ABORT(); }
    if (tsgBufRead(pos,sizeof(pfVec3)*numpos,buf) < 0)  { BBOARD_ABORT(); }
  }

  /* Mode and axis */
  if (tsgBufRead(&mval,sizeof(int32),buf) < 0)  { BBOARD_ABORT(); }
  mode = tsgGetMode(Bboard_Mode,(int8)mval);
  if (tsgBufRead(&axis,sizeof(pfVec3),buf) < 0)  { BBOARD_ABORT(); }

  /* Set the billboard positions */
  for (i=0;i<numpos;i++)
    pfBboardPos(bboard, i, pos[i]);
  pfBboardMode(bboard, PFBB_ROT, mode);
  pfBboardAxis(bboard, axis);

  /* Read geode info */
  if (!parse_geode_info(ctx,buf,(pfGeode *)bboard))  { BBOARD_ABORT(); }

  if (pos)  pfFree(pos);
  return bboard;
}

/* parse_group()
   Parse, allocate, and return a group.
   */
static pfGroup *parse_group(tsgContext *ctx,struct tsgAppendBuffer *buf)
{
  pfGroup *group = NULL;

  /* New group */
  if (!(group = pfNewGroup()))  { return NULL; }

  /* Node info */
  if (!(parse_node(ctx,buf,(pfNode *)group))) { pfDelete(group);  return NULL; }

  if (!(parse_group_info(ctx,buf,group)))  { pfDelete(group);  return NULL; }

  return group;
}

/* parse_lod()
   Level of Detail node.
   */
#define LOD_ABORT() { \
  if (lod)  pfDelete(lod); \
  if (ranges)  pfFree(ranges); \
  if (trans) pfFree(trans); \
  if (lodstate) pfDelete(lodstate); \
  return NULL; \
}
static pfLOD *parse_lod(tsgContext *ctx,struct tsgAppendBuffer *buf)
{
  int32 numranges;
  ieee32 *ranges=NULL,*trans=NULL;
  pfLOD *lod = NULL;
  pfLODState *lodstate = NULL;
  int i;
  pfVec3 center;
  int8 has_state;
  ieee32 fval;

  /* New LOD node */
  if (!(lod = pfNewLOD()))  { return NULL; }

  /* Node info */
  if (!parse_node(ctx,buf,(pfNode *)lod)) { LOD_ABORT(); }

  /* LOD specific info */
  if (tsgBufRead(&numranges,sizeof(int32),buf) < 0)  { LOD_ABORT(); }
  /* Ranges and transitions */
  if (numranges) {
    if (!(ranges = pfMalloc(sizeof(ieee32)*numranges,pfGetSharedArena())))  { LOD_ABORT(); }
    if (!(trans = pfMalloc(sizeof(ieee32)*numranges,pfGetSharedArena())))  { LOD_ABORT(); }
    if (tsgBufRead(ranges,sizeof(ieee32)*numranges,buf) < 0)  { LOD_ABORT(); }
    if (tsgBufRead(trans,sizeof(ieee32)*numranges,buf) < 0)  { LOD_ABORT(); }
  }
  /* Center */
  if (tsgBufRead(center,sizeof(center),buf) < 0)  { LOD_ABORT(); }
  pfLODCenter(lod, center);

  /* LOD State info */
  if (tsgBufRead(&has_state,sizeof(int8),buf) < 0)  { LOD_ABORT(); }
  if (has_state) {
    if (!(lodstate = pfNewLODState())) { LOD_ABORT(); }
  
    if (tsgBufRead(&fval,sizeof(ieee32),buf) < 0) { LOD_ABORT(); }
    pfLODStateAttr(lodstate, PFLODSTATE_RANGE_RANGESCALE, fval);
    if (tsgBufRead(&fval,sizeof(ieee32),buf) < 0) { LOD_ABORT(); }
    pfLODStateAttr(lodstate, PFLODSTATE_RANGE_RANGEOFFSET, fval);
    if (tsgBufRead(&fval,sizeof(ieee32),buf) < 0) { LOD_ABORT(); }
    pfLODStateAttr(lodstate, PFLODSTATE_RANGE_STRESSSCALE, fval);
    if (tsgBufRead(&fval,sizeof(ieee32),buf) < 0) { LOD_ABORT(); }
    pfLODStateAttr(lodstate, PFLODSTATE_RANGE_STRESSOFFSET, fval);
    if (tsgBufRead(&fval,sizeof(ieee32),buf) < 0) { LOD_ABORT(); }
    pfLODStateAttr(lodstate, PFLODSTATE_TRANSITION_RANGESCALE,fval);
    if (tsgBufRead(&fval,sizeof(ieee32),buf) < 0) { LOD_ABORT(); }
    pfLODStateAttr(lodstate, PFLODSTATE_TRANSITION_RANGEOFFSET,fval);
    if (tsgBufRead(&fval,sizeof(ieee32),buf) < 0) { LOD_ABORT(); }
    pfLODStateAttr(lodstate, PFLODSTATE_TRANSITION_STRESSSCALE,fval);
    if (tsgBufRead(&fval,sizeof(ieee32),buf) < 0) { LOD_ABORT(); }
    pfLODStateAttr(lodstate, PFLODSTATE_TRANSITION_STRESSOFFSET,fval);

    pfLODLODState(lod, lodstate);
  }    

  /* Group info */
  if (!parse_group_info(ctx,buf,(pfGroup *)lod)) { LOD_ABORT(); }

  /* Apply the lod and transitions ranges */
  for (i=0;i<numranges;i++) {
    pfLODRange(lod, i, ranges[i]);
    pfLODTransition(lod, i, trans[i]);
  }

  pfFree(ranges);
  pfFree(trans);
  return lod;
}

static pfSCS *parse_scs(tsgContext *ctx,struct tsgAppendBuffer *buf)
{
  pfSCS *scs = NULL,*real_scs = NULL;
  pfMatrix mat,new_mat;
  int mode;
  pfSphere sph;
  int i,j;

  pfMakeIdentMat(mat);

  /* New SCS node */
  if (!(scs = pfNewSCS(mat)))  { return NULL; }

  /* Node info */
  if (!parse_node(ctx,buf,(pfNode *)scs))  { pfDelete(scs);  return NULL; }

  /* SCS specific info */
  if (tsgBufRead(&mat,sizeof(pfMatrix),buf) < 0)  { pfDelete(scs);  return NULL; }

  /* Create the real SCS */
  if (!(real_scs = pfNewSCS(mat)))  { pfDelete(scs);  return NULL; }

  /* Copy over node attributes */
  pfNodeName(real_scs,pfGetNodeName(scs));
  mode = pfGetNodeBSphere(scs,&sph);
  pfNodeBSphere(real_scs,&sph,mode);

  pfDelete(scs);

  /* Group info */
  if (!parse_group_info(ctx,buf,(pfGroup *)real_scs))  { pfDelete(scs);  return NULL; }

  return real_scs;
}
