/*
 * Copyright 1993, 1994, 1995, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 *
 * UNPUBLISHED -- Rights reserved under the copyright laws of the United
 * States.   Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS LEGEND:
 * Use, duplication or disclosure by the Government is subject to restrictions
 * as set forth in FAR 52.227.19(c)(2) or subparagraph (c)(1)(ii) of the Rights
 * in Technical Data and Computer Software clause at DFARS 252.227-7013 and/or
 * in similar or successor clauses in the FAR, or the DOD or NASA FAR
 * Supplement.  Contractor/manufacturer is Silicon Graphics, Inc.,
 * 2011 N. Shoreline Blvd. Mountain View, CA 94039-7311.
 *
 * THE CONTENT OF THIS WORK CONTAINS CONFIDENTIAL AND PROPRIETARY
 * INFORMATION OF SILICON GRAPHICS, INC. ANY DUPLICATION, MODIFICATION,
 * DISTRIBUTION, OR DISCLOSURE IN ANY FORM, IN WHOLE, OR IN PART, IS STRICTLY
 * PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF SILICON
 * GRAPHICS, INC.
 */
/*
 * tex.c
 * --------------
 * $Revision: 1.84 $
 * $Date: 1996/08/24 02:48:15 $
 *
 *	This is a special tex.c with minor changes for tracking IMPACT
 *	memory usage and getting improved memory usage on IMPACT.
 *	Minor changes have been made to pfuMakeSceneTexList() and
 *	pfuMakeTexList() that will affect the order of textures in the
 *	returned list.
 *	It is highly recommended that this be used in place of tex.c on IMPACT
 *	and it should have no effect on other graphics patforms.
 */

#include <string.h>
#include <values.h>
#include <stdlib.h>
#include <ctype.h>

#include <Performer/pf.h>
#include "texXLoad.h"
/* #include <Performer/pfutil.h> */

#define DIM_1D 		0
#define DIM_PROXY_1D	1
#define DIM_2D		2
#define DIM_PROXY_2D	3
#define DIM_3D		4
#define DIM_PROXY_3D	5
#define DIM_4D		6
#define DIM_PROXY_4D	7
#define MAXLEVEL    14

/* texture level identification */
typedef struct __TexLevIDRec
	{
#ifdef IRISGL
    int width, height, depth, extent; /* sizes declared for level */
    int brd_data;		/* border data declared by app */
    int numcomp;		/* #components in internal format [1:4]=>[0:3] */
    int compdepth;		/* component depth (bits) internal fmt [4/8/12]=>[0:2]*/
    int hx2;				/* Horizontal/vertical x2 mode */
    int db_possible;	/* is texture-double-buffering possible */
    int internalformat;	/* internal format assigned by TM */
#else /* OPENGL */
    GLsizei width, height, depth, extent; /* sizes declared for level */
    GLint brd_data;		/* border data declared by app */
    GLint numcomp;		/* #components in internal format [1:4]=>[0:3] */
    GLint compdepth;		/* component depth (bits) internal fmt [4/8/12]=>[0:2]*/
    GLint hx2;				/* Horizontal/vertical x2 mode */
    GLint db_possible;	/* is texture-double-buffering possible */
    GLenum internalformat;	/* internal format assigned by TM */
#endif
    } __TexLevID;

/* texture level descriptor */
typedef struct __TexLevInfoRec
	{
    struct __TexLevInfoRec *packed;	/* common block for packed case */
    __TexLevID id; 						/* identification data */
#ifdef IRISGL
    int numpage, totpage;			/* #mipmap pages and log */
    int brd_page;					/* offset to border page(s) (-1=>none) */
    int lod;								/* LOD level number */
    int prohibited;					/* prohibited level */
#else /* OPENGL */
    GLsizei numpage, totpage;			/* #mipmap pages and log */
    GLsizei brd_page;					/* offset to border page(s) (-1=>none) */
    GLint lod;								/* LOD level number */
    GLint prohibited;					/* prohibited level */
#endif
    } __TexLevInfo;

/* texture descriptor */
typedef struct __TextureInfoRec
	{
    __TexLevInfo lev[MAXLEVEL];	/* 14 possible levels */
    __TexLevInfo *packed;			/* common block for packed case */
    __TexLevID id; 					/* identification data */
#ifdef IRISGL
    int numpage;					/* total # tram pages for texture */
    int dim;							/* texture dimension [1D:4D]=>[0:3] */
    int vx2;							/* Horizontal/vertical x2 mode */
    int mm_enab; 					/* enable mipmapping */
#else /* OPENGL */
    GLsizei numpage;					/* total # tram pages for texture */
    GLint dim;							/* texture dimension [1D:4D]=>[0:3] */
    GLint vx2;							/* Horizontal/vertical x2 mode */
    GLint mm_enab; 					/* enable mipmapping */
#endif
    } __TextureInfo;

#ifdef IRISGL
static int format_enums[4][3] = {
{TX_I_8, 		TX_I_8, 		TX_I_12},
{TX_I_12A_4,	TX_IA_8,		TX_IA_12},
{TX_RGB_5, 		TX_RGB_8,	TX_RGB_12},
{TX_RGBA_4, 	TX_RGBA_8,	TX_RGBA_12}
};
#else /* OPENGL */
static GLenum format_enums[4][3] = {
{GL_LUMINANCE4_EXT, 			GL_LUMINANCE8_EXT, 		 GL_LUMINANCE12_EXT},
{GL_LUMINANCE4_ALPHA4_EXT, GL_LUMINANCE8_ALPHA8_EXT,GL_LUMINANCE12_ALPHA12_EXT},
{GL_RGB4_EXT, 					GL_RGB8_EXT, 				 GL_RGB12_EXT},
{GL_RGBA4_EXT, 				GL_RGBA8_EXT, 				 GL_RGBA12_EXT}
};
#endif

static void CalculateNumPages( pfTexture*, int, int, int, int );
static void total_pages( __TextureInfo* );
static void pages_per_level( __TextureInfo*, int );
static void set_internalformat( __TextureInfo*, int );

static pfList		**texList = NULL;


/*
 *	processGeoSet() -- add textures referenced by geoset to list
 */

static void
processGeoSet (pfGeoSet *geoset, pfList *list)
{
    pfGeoState	*geostate;
    pfTexture	*mainTex;

    /* validate argument */
    if (geoset == NULL || (geostate = pfGetGSetGState(geoset)) == NULL)
	return;

    /* add main texture (if defined) to list */
    if (((mainTex = pfGetGStateAttr(geostate, PFSTATE_TEXTURE)) != NULL)&&
	((pfGetGStateInherit(geostate)&PFSTATE_TEXTURE) == 0))
	if (pfSearch(list, mainTex) < 0)
	    pfAdd(list, mainTex);
}

/*
 *	makeTexListAux() -- recursively locate pfTextures in scene graph
 */

static void
makeTexListAux(pfNode *node, pfList *list)
{
    int		numChildren;
    int		numGeoSets;
    int		child;

    /* validate arguments */
    if (node == NULL)
	return;

    /* perform class-dependent node processing */
	
    if (pfIsOfType(node, pfGetGeodeClassType()))
    {
	/* process geode-class nodes by inspection */
	numGeoSets = pfGetNumGSets((pfGeode *)node);
	for (child = 0; child < numGeoSets; child++)
	    processGeoSet(pfGetGSet((pfGeode *)node, child), list);
    }
    else if (pfIsOfType(node, pfGetGroupClassType()))
    {
	/* process group-class nodes by recursion */
	numChildren = pfGetNumChildren((pfGroup *)node);
	for (child = 0; child < numChildren; child++)
	    makeTexListAux(pfGetChild((pfGroup *)node, child), list);
    }
    /* NOTE: lightpoint and lightsource nodes are skipped */
}

void
sortTextureList(
	pfList *list,
	pfList *new_list,
	int size,
	int this_num_components )
{
	int i;
	unsigned int *image;     /* pointer to texture image */
	int num_components; /* 1=I, 2=IA, 3=RGB, 4=RGBA */
	int ns = 0;     /* texture size in 's' dimension */
	int nt = 0;     /* texture size in 't' dimension */
	int nr = 0;     /* texture size in 'r' dimension */

	for ( i=0; i<size; i++ )
	  {
		pfTexture* tex = pfGet( list, i );

/* get details of texture's in-memory representation */
		pfGetTexImage(tex, &image, &num_components, &ns, &nt, &nr);

		if ( num_components == this_num_components )
			pfAdd( new_list, tex );
	  }
}

/*
 *	showTex() -- display a texture in the center of the screen
 *
 *	  NOTE: You'd better be calling this from a draw-process!
 */

static void
showTex(pfWindow *w, pfTexture *texture, int ns, int nt)
{
    int	 xs;		/*  window size in  x  dimension */
    int	 ys;		/*  window size in  y  dimension */
    float	 xs2,  ys2, ns2, nt2;
    pfVec2	 v;
    static pfVec4 clr = {0.0f, 0.0f, 0.0f, 0.0f};

    static	const pfVec2 t[] =
	{{0.0, 0.0,},
	 {1.0, 0.0,},
	 {1.0, 1.0,},
	 {0.0, 1.0,}};
    static 	const pfMatrix identity =
	{{1.0, 0.0, 0.0, 0.0},
	 {0.0, 1.0, 0.0, 0.0},
	 {0.0, 0.0, 1.0, 0.0},
	 {0.0, 0.0, 0.0, 1.0}};

    pfPushState();
    pfBasicState();
    pfEnable(PFEN_TEXTURE);

    pfGetWinSize(w, &xs, &ys);

    /* clamp texture display size to window size */
    if (ns > xs)
	ns = 0.9*xs;
    if (nt > ys)
	nt = 0.9*ys;

    xs2 = xs/2.0f;
    ys2 = ys/2.0f;
    ns2 = ns/2.0f;
    nt2 = nt/2.0f;
    
    pfPushIdentMatrix();
    pfApplyTex(texture);

    /* background will be black */
#ifdef IRISGL    
    
    ortho2(-0.5, xs - 0.5, -0.5, ys - 0.5);
    viewport(0, xs-1, 0, ys-1);
    zbuffer(0);
    pfClear(PFCL_COLOR, clr);
    /* texture will be full-intensity */
    cpack(0xffffffff);
    /* presume that texture is smaller than window size */
    bgnpolygon();
    t2f(t[0]);
    PFSET_VEC2(v, xs2 - ns2, ys2 - nt2);
    v2f(v);
    t2f(t[1]);
    PFSET_VEC2(v, xs2 + ns2, ys2 - nt2);
    v2f(v);
    t2f(t[2]);
    PFSET_VEC2(v, xs2 + ns2, ys2 + nt2);
    v2f(v);
    t2f(t[3]);
    PFSET_VEC2(v, xs2 - ns2, ys2 + nt2);
    v2f(v);
    endpolygon();
    zbuffer(1);
#else /* OPENGL */ /* XXX showTex - need display to swap buffers */
    {
    GLint mm;
    glGetIntegerv(GL_MATRIX_MODE, &mm);
    if (mm != GL_PROJECTION)
	glMatrixMode(GL_PROJECTION);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glLoadIdentity(); 
    glOrtho(0.0f, xs, 0.0f, ys, -1.0f, 1.0f);
    glViewport(0, 0, xs, ys);
    pfClear(PFCL_COLOR, clr);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    glTexCoord2fv(t[0]);
    PFSET_VEC2(v, xs2 - ns2, ys2 - nt2);
    glVertex2fv(v);
    glTexCoord2fv(t[1]);
    PFSET_VEC2(v, xs2 + ns2, ys2 - nt2);
    glVertex2fv(v);
    glTexCoord2fv(t[2]);
    PFSET_VEC2(v, xs2 + ns2, ys2 + nt2);
    glVertex2fv(v);
    glTexCoord2fv(t[3]);
    PFSET_VEC2(v, xs2 - ns2, ys2 + nt2);
    glVertex2fv(v);
    glEnd();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    if (mm != GL_PROJECTION)
            glMatrixMode(mm);
    }
#endif /* GL type */        

    pfSwapWinBuffers(w);

    pfPopMatrix();
    pfPopState();
}

/*
 *	powerOfTwo -- return least power of 2 >= argument
 */

static int
powerOfTwo (int arg)
{
    int	pot = 1;

    while (pot < arg)
	pot = pot << 1;

    return pot;
}

/*
 *	pfxGetTexSize() -- return memory requirements of a texture (in bytes)
 */

int
pfxGetTexSize (pfTexture *texture)
{
    unsigned int 	*image;		/* pointer to texture image */
    int	components;	/* 1=I, 2=IA, 3=RGB, 4=RGBA */
    int	ns = 0;		/* texture size in 's' dimension */
    int	nt = 0;		/* texture size in 't' dimension */
    int	nr = 0;		/* texture size in 'r' dimension */
    int	format;
    int	bytesPerTexel;
    int	filter;
    int	size;

    /* validate argument */
    if (texture == NULL)
	return 0;

    /* get details of texture's in-memory representation */
    pfGetTexImage(texture, &image, &components, &ns, &nt, &nr);
    if (nr < 1) nr = 1;

    /* decode storage "depth" from internal format */
    format = pfGetTexFormat(texture, PFTEX_INTERNAL_FORMAT);
    switch (format) {
#if 0
    case PFTEX_I_8:
#endif
    case PFTEX_I_12A_4:
    case PFTEX_IA_8:
    case PFTEX_RGB_5:
#ifdef OPENGL
    case PFTEX_RGB5_A1:
#endif
    case PFTEX_RGBA_4:
    case PFTEX_I_16:
        bytesPerTexel = 2;
        break;
    case PFTEX_RGB_12:
	bytesPerTexel = 4.5;
	break;
    case PFTEX_RGBA_12:
	bytesPerTexel = 6;
	break;
    case PFTEX_IA_12:
    case PFTEX_RGBA_8:
    default:
        bytesPerTexel = 4;
        break;
    }

    if (ns == 0 && nt == 0)
	size = 0;
    else
    {
	/* adjust dimensions to reflect power-of-two requirement */
	ns = powerOfTwo(ns);
	nt = powerOfTwo(nt);

	/* MIP-map textures have an extra storage requirement */
	filter = pfGetTexFilter(texture, PFTEX_MINFILTER);
	if (filter == PFTEX_MIPMAP          || 
	    filter == PFTEX_MIPMAP_LINEAR   ||
	    filter == PFTEX_MIPMAP_BILINEAR || 
	    filter == PFTEX_MIPMAP_TRILINEAR )
	    size = (ns*nt*nr) * bytesPerTexel * 4.0f/3.0f + 4;
	else
	    size = (ns*nt*nr) * bytesPerTexel;
    }

    return size;
}

/*
 *	pfuDownloadTexList() -- download each texture in list
 *
 *	  NOTE: You'd better be calling this from a draw-process!
 */

void
pfxDownloadTexList (pfList *list, int style)
{
    pfWindow	*w;
    unsigned int 	*image;		/* pointer to texture image */
    int		components;	/* 1=I, 2=IA, 3=RGB, 4=RGBA */
    int		ns = 0;		/* texture size in 's' dimension */
    int		nt = 0;		/* texture size in 't' dimension */
    int		nr = 0;		/* texture size in 'r' dimension (ignored) */
    int		i;
    int		n;
    double	listStartTime;
    double	listElapsedTime;
    int		textureSize = -1;
#ifdef IRISGL
	 char machine[12];
#else /* OPENGL */
	 const GLubyte *machine;
#endif
	 int impact=0;
	 int numtram=0;

    /* validate argument */
    if (list == NULL)
	return;

    pfQuerySys(PFQSYS_TEXTURE_MEMORY_BYTES, &textureSize);

#ifdef IRISGL
	gversion( machine );
	impact = !strncmp( machine, "GL4DIMP", 7 );

	if ( impact )
		numtram = textureSize/(1024*1024);

#else /* OPENGL */
	machine = glGetString( GL_RENDERER );
	impact = !strncmp( machine, "IMPACT", 6 );

	if ( impact )
		numtram = atoi( &machine[11] );
#endif
    
    pfNotify(PFNFY_INFO, PFNFY_PRINT, "pfuDownloadTexList texture processing");

    /* process each texture in list */
    listStartTime = pfGetTime();
    for (i = 0, n = pfGetNum(list); i < n; i++)
    {
	pfTexture	*texture	= pfGet(list, i);
	int		 bytes;
	int		 ss;
	int		 st;
	double		 startTime;
	double		 elapsedTime;
	static int	 total		= 0;

	/* get details of texture's in-memory representation */
	pfGetTexImage(texture, &image, &components, &ns, &nt, &nr);
	if (nr < 1) nr = 1;

	/* print statistics */
	pfNotify(PFNFY_INFO, PFNFY_MORE, 
	    "  Loading texture:");
	pfNotify(PFNFY_INFO, PFNFY_MORE, 
	    "    File name           = %s", pfGetTexName(texture));
	pfNotify(PFNFY_INFO, PFNFY_MORE, 
	    "    Image size (SxTxR)  = %dx%dx%d", ns, nt, nr);
	ss = powerOfTwo(ns);
	st = powerOfTwo(nt);
	if (ns != ss || nt != st)
	    pfNotify(PFNFY_INFO, PFNFY_MORE, 
		"    Scaled size (SxTxR) = %dx%dx%d", ss, st, nr);

	/* accumulate total texture size defined during execution */
	bytes	= pfxGetTexSize(texture);
	total += bytes;

	if ( impact )
		CalculateNumPages( texture, numtram, components, ns, nt );
	 else
	    pfNotify(PFNFY_INFO, PFNFY_MORE, 
		"    Download size       = %8d bytes", bytes);

	/* download texture to graphics hardware */
	startTime = pfGetTime();
	switch (style)
	{
	case PFXTEX_DEFINE:
		 pfFormatTex(texture);
		 break;

	case PFXTEX_SHOW:
		 w = pfGetCurWin();
		 showTex(w, texture, ns, nt);
		 break;

	/*case PFUTEX_APPLY:*/
	default:
		 pfApplyTex(texture);
		 break;
	}
	elapsedTime = pfGetTime() - startTime;

	if (elapsedTime > 0.0f)
	{
	    pfNotify(PFNFY_INFO, PFNFY_MORE, 
		"    Download time       =%8.3f ms", 1000.0f*elapsedTime);
	    pfNotify(PFNFY_INFO, PFNFY_MORE, 
		"    Download rate       =%8.3f MB/sec", bytes/(1024*1024*elapsedTime));
	}

	pfNotify(PFNFY_INFO, PFNFY_MORE, 
		 "    Total texture use   =%8.3f MB",
		 total * 1.0f/(1024.0f*1024.0f));
	if (textureSize > 0)
	{
	    pfNotify(PFNFY_INFO, PFNFY_MORE, 
		     "    This texture uses   =%8.3f%% of %dMB memory",
		     bytes*100.0f/textureSize,
		     textureSize/(1024*1024));
	    pfNotify(PFNFY_INFO, PFNFY_MORE, 
		     "    Total texture use   =%8.3f%% of %dMB memory", 
		     total*100.0f/textureSize,
		     textureSize/(1024*1024));
	}

    }
    listElapsedTime = pfGetTime() - listStartTime;

    pfNotify(PFNFY_INFO, PFNFY_MORE, "  Download totals");
    pfNotify(PFNFY_INFO, PFNFY_MORE, "    Textures: %d", n);
    pfNotify(PFNFY_INFO, PFNFY_MORE, "    Elapsed time: %.3f sec", listElapsedTime);
    pfNotify(PFNFY_INFO, PFNFY_MORE, NULL);
}

static void CalculateNumPages(
	pfTexture *texture,
	int numtram,
	int components,
	int ns,
	int nt )
{
	static int tot_pages = 0;
	static int free_alpha = 0;
	static int free_blue = 0;
	static int free_green = 0;
	static pfList *detailL=NULL;
	static int download_detail=0;
	__TextureInfo tex;
	pfTexture *detail;
	int numbits;
	int format;

/* calculate the # of TRAM pages for IMPACT */
	memset( &tex, 0, sizeof(__TextureInfo) );
	tex.id.numcomp = components;
	tex.id.width = ns;
	if ( download_detail )
		tex.id.width *= 2;
	tex.id.height = nt;
	if ( !download_detail )
		tex.mm_enab = 1;

	format = pfGetTexFormat( texture, PFTEX_INTERNAL_FORMAT );
	switch (format)
	  {
		case PFTEX_I_16:
			numbits = 16;
			break;
		case PFTEX_RGBA_12:
		case PFTEX_RGB_12:
			if ( numtram == 1 )
				numbits = 4;
			 else
				numbits = 12;
			break;
		case PFTEX_IA_12:
		case PFTEX_I_12A_4:
			if ( numtram == 1 )
				numbits = 8;
			 else
				numbits = 12;
			break;
		case PFTEX_RGBA_8:
		case PFTEX_RGB_5:
			if ( numtram == 1 )
				numbits = 4;
			 else
				numbits = 8;
			break;
		case PFTEX_IA_8:
#ifndef IRISGL
		case GL_LUMINANCE8_EXT:
#endif
			numbits = 8;
			break;
		case PFTEX_RGBA_4:
#ifndef IRISGL
		case PFTEX_RGB_4:
#endif
			numbits = 4;
			break;
		default:
			pfNotify(PFNFY_INFO, PFNFY_MORE, 
				 "    Unsupported internal download texture format" );
			break;
	  }

	tex.id.compdepth = numbits / 4 - 1;
	tex.id.internalformat = format_enums[tex.id.numcomp-1][tex.id.compdepth];
	set_internalformat( &tex, numtram );
	tex.vx2 = tex.id.db_possible;
	total_pages( &tex );

	if ( numtram > 1 )
		switch( components )
		  {
			case 1:
				if ( tot_pages+tex.numpage < 256 )
				  {
					free_green += tex.numpage;
					free_blue += tex.numpage;
					free_alpha += tex.numpage;
					tot_pages += tex.numpage;
				  }
				 else if ( free_alpha >= tex.numpage )
					free_alpha -= tex.numpage;
				 else if ( free_blue >= tex.numpage )
					free_blue -= tex.numpage;
				 else if ( free_green >= tex.numpage )
					free_green -= tex.numpage;
				 else
				  {
					free_green += tex.numpage;
					free_blue += tex.numpage;
					free_alpha += tex.numpage;
					tot_pages += tex.numpage;
				  }
				break;
			case 2:
				if ( free_alpha >= tex.numpage && free_blue >= tex.numpage )
				  {
					free_alpha -= tex.numpage;
					free_blue -= tex.numpage;
				  }
				 else
				  {
					tot_pages += tex.numpage;
					free_alpha += tex.numpage;
					free_blue += tex.numpage;
				  }
				break;
			case 3:
				tot_pages += tex.numpage;
				free_alpha += tex.numpage;
				break;
			case 4:
				tot_pages += tex.numpage;
				break;
		  }
		 else if ( numtram == 1 )
			tot_pages += tex.numpage;
	
	pfNotify(PFNFY_INFO, PFNFY_MORE, 
		"    Download size       = %4d pages", tex.numpage);
	pfNotify(PFNFY_INFO, PFNFY_MORE, 
		 "    Total hardware texture use   =%4d Pages", tot_pages );

	if ( (detail=pfGetTexDetailTex( texture )) )
	  {
		if ( !detailL )
		  {
			detailL = pfNewList(sizeof(pfTexture*), 4, pfGetSharedArena());
			pfAdd( detailL, detail );
			download_detail = 1;
		  }
		 else if ( pfSearch( detailL, detail ) == -1 )
		  {
			pfAdd( detailL, detail );
			download_detail = 1;
		  }
	  }
	 else
		download_detail = 0;

	if ( download_detail )
	  {
		unsigned int *im;
		int c;
		int s, t, r;
		int ss, st;

/* get details of texture's in-memory representation */
		pfGetTexImage(detail, &im, &c, &s, &t, &r);
		if (r < 1) r = 1;

		/* print statistics */
		pfNotify(PFNFY_INFO, PFNFY_MORE, 
		"  Loading detail texture:");
		pfNotify(PFNFY_INFO, PFNFY_MORE, 
		"    File name           = %s", pfGetTexName(detail));
		pfNotify(PFNFY_INFO, PFNFY_MORE, 
		"    Image size (SxTxR)  = %dx%dx%d", s, t, r);
		ss = powerOfTwo(s);
		st = powerOfTwo(t);
		if (s != ss || t != st)
		pfNotify(PFNFY_INFO, PFNFY_MORE, 
		"    Scaled size (SxTxR) = %dx%dx%d", ss, st, r);

		CalculateNumPages( detail, numtram, c, s, t );
	  }
}

static void
total_pages(
	__TextureInfo *tex )
{
	long lod = 0;
	__TexLevInfo *lev;

   if ( tex->id.extent != 0 )
	  {
		tex->dim = 6;

		if ( tex->id.width==0 || tex->id.height==0 || tex->id.depth==0 )
			 return;

		if ( tex->id.brd_data )
			 return;

		if ( tex->mm_enab )
			 return;

		memcpy( &tex->lev[lod].id, &tex->id, sizeof(__TexLevID) );
		pages_per_level( tex, lod );
	  }
    else if ( tex->id.depth != 0 )
	  {
		tex->dim = 4;

		if ( tex->id.width==0 || tex->id.height==0 )
			return;

		if( tex->id.brd_data )
	   	return;

		if ( tex->mm_enab )
	   	return;

		memcpy( &tex->lev[lod].id, &tex->id, sizeof(__TexLevID) );
    	pages_per_level( tex, lod );
	  }
    else if ( tex->id.height != 0 )
	  {
		tex->dim = 2;

		if ( tex->id.width==0 )
			return;

		lev = &tex->lev[lod];
		memcpy( &lev->id, &tex->id, sizeof(__TexLevID) );
    	pages_per_level( tex, lod );

    	if ( tex->mm_enab )
		  {
			lev = &tex->lev[lod];
			while( !( lev->id.width==1 && lev->id.height==1 ) )
			  {
				memcpy( &tex->lev[++lod].id, &lev->id, sizeof(__TexLevID) );

				lev = &tex->lev[lod];
				if( (lev->id.width >>= 1) == 0 )
					lev->id.width = 1;
				if( (lev->id.height >>= 1) == 0 )
					lev->id.height = 1;
				pages_per_level(tex, lod );
			  }
		  }
	  }
    else if ( tex->id.width != 0 )
	  {
		tex->dim = 0;
		lev = &tex->lev[lod];
		memcpy(&lev->id, &tex->id, sizeof(__TexLevID));
    	pages_per_level(tex, lod );

    	if ( tex->mm_enab )
		  {
			lev = &tex->lev[lod];
			while( lev->id.width !=1 )
			  {
				memcpy( &tex->lev[++lod].id, &lev->id, sizeof(__TexLevID) );
				lev = &tex->lev[lod];

				if((lev->id.width >>= 1) == 0)
					lev->id.width = 1;

				pages_per_level(tex, lod );
			  }
		  }
	  }
}

static void
pages_per_level(
	__TextureInfo *tex,
	int lod)
{
	long tm_ssize, tm_tsize, border_tb, border_lr, corner, overlap; 
	long numpage, numspg, numtpg, brd_numpage, thickness;
	__TexLevInfo *lev = &tex->lev[lod], *levtmp;

/* tm_ssize is the original width/2 for 1D and 2D, width for 3D and 4D;
   divided by 2 if we have horizontal x2 */
	tm_ssize = (tex->dim<DIM_3D ? lev->id.width>>1 : lev->id.width)>>tex->id.hx2;
	tm_tsize = lev->id.height >> tex->vx2;

/* default is no border pages */
	lev->brd_page = -1;
	brd_numpage = 0;

/* packed case */
	if ( tm_tsize <= 16 && tm_ssize <= 8 && tex->dim < DIM_3D )
	  {
		lev->numpage = brd_numpage = overlap = 0;
		if ( tex->id.brd_data || tex->mm_enab )
			lev->brd_page = 0;
		if ( tex->packed )
		  {
	   	if ( lod > 0 )
			  {
				levtmp = &tex->lev[lod-1];
				if ( (levtmp->id.width>>1>>tex->id.hx2)+(levtmp->id.width>>tex->vx2)
						< 1)
				  {
					lev->prohibited = 1;
					return;
				  }
			  }
			lev->packed = tex->packed;
		  }
		 else
		  {
			lev->numpage = lev->totpage = 1;
			tex->numpage++;
			tex->packed = lev;
		  }
	  }
    else
/* not the packed case */
	  {
    	if( (numspg = tm_ssize >> 5) == 0 )
			numspg = 1;
    	if( (numtpg = tm_tsize >> 5) == 0 )
			numtpg = 1;
		lev->numpage = 0;

/* for 1D, image data is in the border pages */
    	if ( tex->dim > DIM_PROXY_1D )
	   	lev->numpage = numspg * numtpg;

/* for 3D and 4D, calc total pages */
		if ( tex->dim > DIM_PROXY_2D )
		  {
			if ( (thickness = lev->id.depth>>1) == 0 )
				thickness = 1;
			lev->numpage *= thickness * (lev->id.extent == 0 ? 1 : lev->id.extent);
			if ( lev->numpage == 0 )
				lev->numpage = 1;
		  }

/* Default setting */
    	border_tb = border_lr = 0;

/* if border data supplied or 1D */
    	if ( tex->id.brd_data || tex->dim < DIM_2D )
		  {

/* 8 or 16 rows @ 32 or 64 texels per row */
			border_tb = tm_ssize >> 8 >> tex->vx2;

/* partial page gets 1 page */
			if ( border_tb == 0 )
				border_tb = 1;
		  }

		overlap = 0;
    	if ( tex->dim > DIM_PROXY_1D && (tex->id.brd_data || tex->mm_enab) )
		  {

/* 8 columns @ 32 or 64 texels per column */
			border_lr = tm_tsize >> 8;
			if ( border_lr == 0 )
				border_lr = 1;

			if ( lev->id.brd_data )
			  {
				if ( tm_tsize <= 16 )
					overlap = 1;
			  }
			 else if ( tm_ssize <= 16 )
				overlap = 1;
		  }

/* assignment statement intended */
		if ( corner = border_tb )
			corner = corner &&
						(tex->dim > DIM_PROXY_1D || tex->id.brd_data || tex->mm_enab);

		brd_numpage = border_tb + border_lr + corner;

/* put border pages at end of mipmap pages */
		if ( brd_numpage )
			lev->brd_page = lev->numpage - overlap;

		numpage = lev->numpage + brd_numpage - overlap;
		lev->totpage = numpage;
		tex->numpage += numpage;
	  }
}

/* The next step is to establish the internal format as one of
* the 12 formats implemented in the TRAMs, taking into account
* the number of TRAMs. We have to set the numcomp, compdepth,
* hx2, code, and db_possible parameters for each internal format.
*/
static void
set_internalformat(
	__TextureInfo *tex,
	int numtram)
{
	tex->id.hx2 = tex->id.db_possible = 0;

#ifndef IRISGL
	switch ( tex->id.internalformat )
	  {
		case GL_LUMINANCE4_EXT:
			 tex->id.hx2 = 1;
			 tex->id.db_possible = 1;
			 break;
		case GL_LUMINANCE8_EXT:
			 tex->id.db_possible = 1;
			 break;
		case GL_LUMINANCE12_EXT:
			 break;
		case GL_LUMINANCE4_ALPHA4_EXT:
			if ( numtram == 4 )
				tex->id.hx2 = 1;
			tex->id.db_possible = 1;
			break;
		case GL_LUMINANCE8_ALPHA8_EXT:
			if ( numtram == 4 )
				tex->id.db_possible = 1;
			break;
		case GL_LUMINANCE12_ALPHA12_EXT:
			if ( numtram == 1 )
			  {
				tex->id.compdepth = 1;
				tex->id.internalformat = GL_LUMINANCE8_ALPHA8_EXT;
			  }
			break;
		case GL_RGB4_EXT:
			if ( numtram == 4 )
			  {
				tex->id.db_possible = 1;
				tex->id.hx2 = 1;
			  }
	   	break;
		case GL_RGB8_EXT:
			if ( numtram == 4 )
				tex->id.db_possible = 1;
			 else
/* redefine component depth */
			  {
				tex->id.compdepth = 0;
				tex->id.internalformat = GL_RGB4_EXT;
			  }
			break;
		case GL_RGB12_EXT:

/* redefine component depth */
			if ( numtram == 1 )
			  {
				tex->id.compdepth = 0;
				tex->id.internalformat = GL_RGB4_EXT;
			  }
			break;
		case GL_RGBA4_EXT:
			if ( numtram == 4 )
			  {
				tex->id.db_possible = 1;
				tex->id.hx2 = 1;
			  }
			break;
		case GL_RGBA8_EXT:
			if ( numtram == 4 )
				tex->id.db_possible = 1;
			 else
/* redefine component depth */
			  {
				tex->id.compdepth = 0;
				tex->id.internalformat = GL_RGBA4_EXT;
			  }
			break;
		case GL_RGBA12_EXT:

/* redefine component depth */
			if ( numtram == 1 )
			  {
				tex->id.internalformat = GL_RGBA4_EXT;
				tex->id.compdepth = 0;
			  }
			break;
		default:
			break;
	  }
#endif
}
