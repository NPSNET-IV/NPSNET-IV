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
/* TSG (Tiled Scene Graph) format header.
 *****************************************************************
 * HISTORY
 *  14-Mar-96  Michael Polis (mfp)  Digital Mapping Laboratory
 *      School of Computer Science at Carnegie-Mellon University
 *      Created.
 */


#ifndef _tsg_h_
#define _tsg_h_

#ifdef __cplusplus
extern "C" {
#endif

/* define types of known size */
typedef char int8;
typedef short int16;
typedef int int32;
typedef float ieee32;
typedef double ieee64;

#define TSG_MAGIC 0x5453478D
#define TSG_MAJOR_VERSION 1
#define TSG_MINOR_VERSION 0

/* Materials (each corresponds to a pfGeoState) */
enum tsgMaterials {
  TSGM_ALPHAFUNC,
  TSGM_ANTIALIAS,
  TSGM_BACKCOLOR,
  TSGM_COLORTABLE,
  TSGM_CULLFACE,
  TSGM_DECAL,
  TSGM_ENCOLORTABLE,
  TSGM_ENFOG,
  TSGM_ENHIGHLIGHTING,
  TSGM_ENLIGHTING,
  TSGM_ENLPOINTSTATE,
  TSGM_ENTEXGEN,
  TSGM_ENTEXTURE,
  TSGM_ENWIREFRAME,
  TSGM_FOG,
  TSGM_FRONTCOLOR,
  TSGM_HIGHLIGHT,	/* not yet supported */
  TSGM_LIGHTMODEL,
  TSGM_LIGHTS,
  TSGM_LPOINTSTATE,	/* not yet supported */
  TSGM_TEXENV,
  TSGM_TEXGEN,
  TSGM_TEXTURE,		/* incomplete support */
  TSGM_TRANSPARENCY,
  TSGM_END_OF_FIELDS,	/* no data, indicates end of fields for material */
  TSGM_NUMMATERIALS
};

/* material data */

/*    TSGM_ALPHAFUNC */
enum tsgmAFValue {
  TSGMAF_ALWAYS,
  TSGMAF_EQUAL,
  TSGMAF_GEQUAL,
  TSGMAF_GREATER,
  TSGMAF_LEQUAL,
  TSGMAF_LESS,
  TSGMAF_NEVER,
  TSGMAF_NOTEQUAL,
  TSGMAF_OFF,
  TSGMAF_NUMVAL
};

/*    TSGM_ALPHAREF (no enumerated data) */

/*    TSGM_ANTIALIAS (no enumerated data) */

/*    TSGM_BACKCOLOR */
enum tsgmCLSide {
  TSGMCL_FRONT,
  TSGMCL_BACK,
  TSGMCL_BOTH,
  TSGMCL_NUMSIDE
};
enum tsgmCLMode {
  TSGMCL_AMB,
  TSGMCL_DIF,
  TSGMCL_AMBDIF,
  TSGMCL_EMS,
  TSGMCL_SPC,
  TSGMCL_NONE,
  TSGMCL_COLOR,
  TSGMCL_NUMMODE
};

/*    TSGM_COLORTABLE (no enumerated data) */

/*    TSGM_CULLFACE */
enum tsgmCFSide {
  TSGMCF_NEITHER,
  TSGMCF_FRONT,
  TSGMCF_BACK,
  TSGMCF_BOTH,
  TSGMCF_NUMSIDE
};

/*    TSGM_DECAL */
#define TSGMDC_OFF			0
#define TSGMDC_BASE			1
#define TSGMDC_BASE_FAST		2
#define TSGMDC_BASE_HIGH_QUALITY	3
#define TSGMDC_BASE_STENCIL		4
#define TSGMDC_BASE_DISPLACE		5
#define TSGMDC_LAYER			6
#define TSGMDC_LAYER_FAST		7
#define TSGMDC_LAYER_HIGH_QUALITY	8
#define TSGMDC_LAYER_STENCIL		9
#define TSGMDC_LAYER_DISPLACE		10
#define TSGMDC_LAYER_DISPLACE_AWAY	11
#define TSGMDC_LAYER_NUMVAL             12
/* for multiple layers using DISPLACE or DISPLACE_AWAY, OR one of these in
 * to indicate the layer (or nothing for layer 0).
 */
#define TSGMDC_LAYER1			0x100
#define TSGMDC_LAYER2			0x200
#define TSGMDC_LAYER3			0x300
#define TSGMDC_LAYER4			0x400
#define TSGMDC_LAYER5			0x500
#define TSGMDC_LAYER6			0x600
#define TSGMDC_LAYER7			0x700
/* adds a constant offset to layer position (use with PFDECAL_BASE_DISPLACE) */
#define TSGMDC_LAYER_OFFSET		0x10000

/*    TSGM_ENCOLORTABLE (no enumerated data) */

/*    TSGM_ENFOG (no enumerated data) */

/*    TSGM_ENHIGHLIGHTING (no enumerated data) */

/*    TSGM_ENLIGHTING (no enumerated data) */

/*    TSGM_ENLPOINTSTATE (no enumerated data) */

/*    TSGM_ENTEXGEN (no enumerated data) */

/*    TSGM_ENTEXTURE (no enumerated data) */

/*    TSGM_ENWIREFRAME (no enumerated data) */

/*    TSGM_FOG */
enum tsgmFGType {
  TSGMFG_VLIN,		/* linear at vertices */
  TSGMFG_VEXP,		/* exponential at vertices */
  TSGMFG_VEXP2,		/* doubly exponential at vertices */
  TSGMFG_PLIN,		/* linear at pixels */
  TSGMFG_PEXP,		/* exponential at pixels */
  TSGMFG_PEXP2,		/* doubly exponential at pixels */
  TSGMFG_SPLINE,	/* defined by spline */
  TSGMFG_NUMTYPE
};

/*    TSGM_FRONTCOLOR (shares enumerations with TSGM_BACKCOLOR */

/*    TSGM_HIGHLIGHT (not yet supported) */

/*    TSGM_LIGHTMODEL */
#define TSGMLM_LOCAL	0x1	/* model is local (not infinite) */
#define TSGMLM_2SIDED	0x2	/* lighting is 2 sided */

/*    TSGM_LIGHTS (no enumerated data) */

/*    TSGM_LPOINTSTATE (not yet supported) */

/*    TSGM_TEXENV */
enum tsgmTEType {
  TSGMTE_ALPHA,
  TSGMTE_BLEND,
  TSGMTE_DECAL,
  TSGMTE_MODULATE,
  TSGMTE_NUMTYPE
};
enum tsgmTEComp {
  TSGMTE_OFF,
  TSGMTE_I_GETS_R,
  TSGMTE_I_GETS_G,
  TSGMTE_I_GETS_B,
  TSGMTE_I_GETS_A,
  TSGMTE_I_GETS_RG,
  TSGMTE_I_GETS_BA,
  TSGMTE_I_GETS_I,
  TSGMTE_NUMCOMP
};

/*    TSGM_TEXGEN */
enum tsgmTGMode {
  TSGMTG_OFF,
  TSGMTG_EYE_LINEAR,
  TSGMTG_EYE_IDENT,
  TSGMTG_OBJECT_LINEAR,
  TSGMTG_SPHERE_MAP,
  TSGMTG_NUMMODE
};

/*    TSGM_TEXTURE (incomplete support) */
/* texture wrap flags */
#define TSGMTX_CLAMPR	0x1
#define TSGMTX_CLAMPS	0x2
#define TSGMTX_CLAMPT	0x4
/* texture filter flags */
#define TSGMTX_ADD		0x0001
#define TSGMTX_ALPHA		0x0002
#define TSGMTX_BICUBIC		0x0004
#define TSGMTX_BILINEAR		0x0008
#define TSGMTX_COLOR		0x0010
#define TSGMTX_DETAIL		0x0020
#define TSGMTX_FAST		0x0040
#define TSGMTX_GEQUAL		0x0080
#define TSGMTX_LEQUAL		0x0100
#define TSGMTX_LINEAR		0x0200
#define TSGMTX_MIPMAP		0x0400
#define TSGMTX_MODULATE		0x0800
#define TSGMTX_POINT		0x1000
#define TSGMTX_QUADLINEAR	0x2000
#define TSGMTX_SHARPEN		0x4000
#define TSGMTX_TRILINEAR	0x8000

/*    TSGM_TRANSPARENCY	*/
#define TSGMTR_OFF		0
#define TSGMTR_ON		1
#define TSGMTR_BLEND_ALPHA	2
#define TSGMTR_FAST		3
#define TSGMTR_HIGH_QUALITY	4
#define TSGMTR_MS_ALPHA		5
#define TSGMTR_MS_ALPHA_MASK	6
#define TSGMTR_NO_OCCLUDE	0x80	/* OR'ed with one of the above */
#define TSGMTR_NUMVAL           7


/* OpCodes */
enum tsgOpCodes {
  TSG_GEODE,
  TSG_BILLBOARD,
  TSG_TEXT,		/* not supported yet */
  TSG_GROUP,
  TSG_PARTITION,	/* not supported yet */
  TSG_LAYER,
  TSG_LOD,
  TSG_SCS,
  TSG_DCS,		/* not supported yet */
  TSG_SWITCH,		/* not supported yet */
  TSG_SEQUENCE,		/* not supported yet */
  TSG_MORPH,		/* not supported yet */
  TSG_EXTREF,		/* not supported yet */
  TSG_DEFINE,		/* not supported yet */
  TSG_USE,		/* not supported yet */
  TSG_NUMOPCODES
};


/* Geometry */

/* primitive types */
#define TSGG_POINTS		0
#define TSGG_LINES		1
#define TSGG_LINESTRIPS		2
#define TSGG_FLAT_LINESTRIPS	3
#define TSGG_TRIANGLES		4
#define TSGG_QUADRILATERALS	5
#define TSGG_TRISTRIPS		6
#define TSGG_FLAT_TRISTRIPS	7
#define TSGG_POLYGONS		8
#define TSGG_PRIM_NUMVAL        9

/* geoset property flags */
#define TSGG_HASBOUND	0x1	/* has static bounding box */
#define TSGG_FLATSHADE	0x2	/* Use flat shading */
#define TSGG_WIREFRAME	0x4	/* draw wireframe */
#define TSGG_COMPILE	0x8	/* compile a GL display list */
#define TSGG_HASLENGTHS	0x10	/* has a lengths array */

/* binding of attributes */
#define TSGG_OFF	0
#define TSGG_OVERALL	1
#define TSGG_PER_PRIM	2
#define TSGG_PER_VTX	3
#define TSGG_BIND_NUMVAL     4

/* types of attributes (used by tsgmGSSetAttr) */
#define TSGG_COLOR	0
#define TSGG_NORMAL	1
#define TSGG_TCOORD	2
#define TSGG_VERTEX	3

/* Billboard rotation modes */
#define TSGB_AXIAL	0	/* rotates on z axis */
#define TSGB_WORLD	1	/* rotates on z and x axes */
#define TSGB_EYE	2	/* rotates on z, x, and y axes */
#define TSGB_NUM        3

typedef struct {
  ieee32 x,y;
} tsgVec2;

typedef struct {
  ieee32 x,y,z;
} tsgVec3;

typedef struct {
  ieee32 x,y,z,q;
} tsgVec4;

/* Macro for getting a tile structure from a tsgContext */
#define tsgTile(context,x,y) ((context)->tiles[(x)*(context)->tiles_y+(y)])

#ifdef __cplusplus
}
#endif
#endif /* _tsg_h_ */
