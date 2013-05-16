// File: <lake.h>

/*
 * Copyright (c) 1995,
 *    Naval Postgraduate School
 *    Computer Graphics and Video Laboratory
 *    NPSNET Research Group
 *    npsnet@cs.nps.navy.mil
 *
 *
 *    Permission to use, copy, and modify this software and its
 * documentation for any non-commercial purpose is hereby granted
 * without fee, provided that (i) the above copyright notices and the
 * following permission notices appear in ALL copies of the software
 * and related documentation, and (ii) The Naval Postgraduate School
 * Computer Graphics and Video Laboratory and the NPSNET Research Group
 * be given written credit in your software's written documentation and
 * be given graphical credit on any start-up/credit screen your
 * software generates.
 *    This restriction helps justify our research efforts to the
 * sponsors who fund our research.
 *
 *    Do not redistribute this code without the express written
 * consent of the NPSNET Research Group. (E-mail communication and our
 * confirmation qualifies as written permission.) As stated above, this
 * restriction helps justify our research efforts to the sponsors who
 * fund our research.
 *
 *    This software was designed and implemented at U.S. Government
 * expense and by employees of the U.S. Government.  It is illegal
 * to charge any U.S. Government agency for its partial or full use.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * E-Mail addresses:
 *    npsnet@cs.nps.navy.mil
 *    URL: http://www.cs.nps.navy.mil/research/npsnet/
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *
 * Thank you to our sponsors:  ARL, ARPA, DMSO, STRICOM, TEC, TRAC and
 *                             NPS Direct Funding
 */
#ifndef NOSUB

#include "terrain.h"
#include "collision_const.h"
#include "lake.h"
#include "water.h"


static LakeData 	*lakeData;
static pfScene 		*lakeScene;
static int 		lakeActive = 0;
pfDCS *ocean_dcs;

static Water Original = {0};
static Water Buffer[3] = {0};
static pfSwitch *switchNode;

SharedWaterData *SharedWater;

extern          pfGroup      *G_fixed;  //NEW



/*--------------------------------------------------------------------*/
void
    ActivateLake()
{  
     if (lakeActive == 0)
       {
        lakeActive = 1;
        pfAddChild(lakeScene, lakeRoot);
       }
     else
       {
        lakeActive = 0;
        pfRemoveChild(lakeScene, lakeRoot);
       }
}
/*--------------------------------------------------------------------*/
static int 
    preDraw (pfTraverser *trav, void *data)
{
    (trav, trav);		/* for lint */
    if (SharedWater != NULL &&
	SharedWater->lakeData != NULL &&
	SharedWater->lakeData->activeBuffer != NULL)
	*SharedWater->lakeData->activeBuffer = int(long(data));
    return 0;
}

/*--------------------------------------------------------------------*/
static int 
    postDraw (pfTraverser *trav, void *data)
{
    (trav, data);		/* for lint */
    if (SharedWater != NULL &&
	SharedWater->lakeData != NULL &&
	SharedWater->lakeData->activeBuffer != NULL)
	*SharedWater->lakeData->activeBuffer = -1;
    return 0;
}
/*--------------------------------------------------------------------*/

static void
    ResetWaves(void)
{
    lakeData->wave[0].angle = -30.0f;
    lakeData->wave[0].length = 21.0f;
    lakeData->wave[0].height = 1.8f;
    lakeData->wave[0].speed = 1.0f/8.0f;
    lakeData->wave[0].phase = 0.0f;
    
    lakeData->wave[1].angle = 61.0f;
    lakeData->wave[1].length = 30.0f;
    lakeData->wave[1].height = 0.7f;
    lakeData->wave[1].speed = 1.0f/3.0f;
    lakeData->wave[1].phase = 0.0f;
    
    lakeData->wave[2].height = 0.0f;
    lakeData->wave[2].angle = 0.0f;
    lakeData->wave[2].length = 0.0f;
    lakeData->wave[2].speed = 0.0f;
    lakeData->wave[2].phase = 0.0f;
    
    lakeData->wave[3].height = 0.0f;
    lakeData->wave[3].angle = 0.0f;
    lakeData->wave[3].length = 0.0f;
    lakeData->wave[3].speed = 0.0f;
    lakeData->wave[3].phase = 0.0f;
}

/*--------------------------------------------------------------------*/
long
    LoadLakeDB (pfScene *scene)
{ 
    long i;
    void *arena;

    
   ocean_dcs = pfNewDCS();
    
SharedWater = (SharedWaterData *) pfCalloc(sizeof(SharedWaterData), 1, pfGetSharedArena());

    lakeRoot = pfNewGroup();

    lakeScene = scene;
    arena = pfGetSharedArena();
    
    lakeData = (LakeData *)pfMalloc(sizeof(LakeData), arena);
    
    lakeData->bindtex = 0;
    lakeData->activeBuffer = (int *)pfMalloc(sizeof(int), arena);

    SharedWater->lakeData = lakeData;

    /* get a list of all the textures used in this model */
    
    /* generate dynamic water */
    Original.w = 32;
    Original.a0 = 0.0f;
    Original.a1 = 390.0f;  //was 180
    Original.t0 = 0.1f;    //was 3.0
    Original.t1 = 250.0f;  //was 3990.0
    Original.dd = 3.0f;    //was 3.0
    Original.ad = 0.0f;
    Original.md = 0.3f;   //was 0.3
    strcpy(Original.name, "lake4.int");  //was water.rgb
    Original.scale = 0.01f;  //was 0.1f
    
    ResetWaves();
    
    /* copy wave definitions from shared area */
    for (i = 0; i < 4; i++)
	Original.wave[i] = lakeData->wave[i];
    
    pfSinCos(Original.wave[1].angle, &Original.tvy, &Original.tvx);
    Original.tvx *= Original.wave[1].speed*0.04f;  //was 0.08
    Original.tvy *= Original.wave[1].speed*0.04f;  //was 0.08
    
    /* water is structured as a switch with 3 children */
    switchNode = pfNewSwitch();

    for (i = 0; i < 3; i++)
    {
	pfGroup *groupNode;
	
        /* make a copy of the water definition */
	Buffer[i] = Original;
	
	/* create a new copy of the water geometry */
	groupNode = makeWater(&Buffer[i]);
	pfAddChild((pfGroup *)switchNode, (pfNode *)groupNode);
	
	/* associate draw-process call-back functions with new group */
	pfNodeTravFuncs(groupNode, PFTRAV_DRAW, preDraw, postDraw);
	pfNodeTravData(groupNode, PFTRAV_DRAW, (void *)i);

    }
    
    /* start with first buffer */
    if (lakeData->activeBuffer == NULL)
	printf(".");
    else
    {
	*lakeData->activeBuffer = 0;
	pfSwitchVal(switchNode, *lakeData->activeBuffer);
    }



    /* add water to scene */
    pfAddChild(lakeScene, ocean_dcs);
    pfAddChild ( ocean_dcs, lakeRoot );
    pfAddChild((pfGroup*)lakeRoot, (pfNode*)switchNode);

    set_intersect_mask(lakeRoot,(PERMANENT_MOVINGWATER<<PERMANENT_SHIFT),
                                  COLLIDE_DYNAMIC);
    return 1;
}

/*--------------------------------------------------------------------*/

void
    copyWater(Water *newwater, Water *oldwater)
{
    int i;
    
    for (i = 0; i < oldwater->v; i++)
    {
	/* copy vertex coordinates */
	newwater->vertex[i][PF_X] = oldwater->vertex[i][PF_X];
	newwater->vertex[i][PF_Y] = oldwater->vertex[i][PF_Y];
	newwater->vertex[i][PF_Z] = oldwater->vertex[i][PF_Z];
	
	/* copy vertex normal */
	newwater->normal[i][PF_X] = oldwater->normal[i][PF_X];
	newwater->normal[i][PF_Y] = oldwater->normal[i][PF_Y];
	newwater->normal[i][PF_Z] = oldwater->normal[i][PF_Z];
	
	/* copy vertex color */
	newwater->color[i][0] = oldwater->color[i][0];
	newwater->color[i][1] = oldwater->color[i][1];
	newwater->color[i][2] = oldwater->color[i][2];
	newwater->color[i][3] = oldwater->color[i][3];
	
	/* compute texture coordinate mapping */
	newwater->texture[i][PF_X] = oldwater->texture[i][PF_X];
	newwater->texture[i][PF_Y] = oldwater->texture[i][PF_Y];
    }
}

/*--------------------------------------------------------------------*/
pfGroup * makeWater(Water * water)
{
    int             i;
    int             j;
    float           r;
    void           *arena;
    pfGeoSet      *gset;
    pfGeoState     *gstate;
    pfGeode        *geode;
    pfGroup        *group;
    pfMaterial     *material;
    
    static Water *old = NULL;
    
    /* get pointer to shared-memory allocation arena */
    arena = pfGetSharedArena();
    
    /* determine number of vertices */
    for (i = 0, r = water->t0; r < water->t1; r += water->ad + water->md * r)
	++i;
    water->t = i + 1;
    water->v = water->t * (water->w + 1);
    
    printf("Water: t=%d, v=%d, w=%d\n", water->t, water->v, water->w);
    
    /* allocate and initialize vertex data */
    water->vertex  = (pfVec3 *)pfMalloc(water->v*sizeof(pfVec3), arena);
    water->normal  = (pfVec3 *)pfMalloc(water->v*sizeof(pfVec3), arena);
    water->color   = (pfVec4 *)pfMalloc(water->v*sizeof(pfVec4), arena);
    water->texture = (pfVec2 *)pfMalloc(water->v*sizeof(pfVec2), arena);
    
    /* determine range to vertices */
    water->range = (float *) pfMalloc(water->t * sizeof(float), arena);
    for (i=0, r=water->t0; r<water->t1; r+=water->ad+water->md*r, i++)
	water->range[i] = r;
    water->range[i] = water->t1;
    
    if (old == NULL)
    {
	old = water;
	
	/* initialize vertex data */
	for (i = 0; i <= water->w; i++)
	{
	    float           s;
	    float           c;
	    
	    float ratio = i /(float) water->w;
	    float rr = sinf(PF_PI*ratio);
	    
	    float		a = (water->a1 - water->a0) / (float)water->w;
	    
	    pfSinCos(i * a, &s, &c);
	    
	    for (j = 0; j < water->t; j++)
	    {
		float           shadow;
		int             k = i * water->t + j;
		
		/* water coordinates are biased toward origin */
		water->vertex[k][PF_X] = rr * c * water->range[j];
		water->vertex[k][PF_Y] = rr * s * water->range[j];
		water->vertex[k][PF_Z] = 2.0f; //was 0.0
		
		/* normal points up */
		water->normal[k][PF_X] = 0.0f;
		water->normal[k][PF_Y] = 0.0f;
		water->normal[k][PF_Z] = 1.0f;
		
		/* bright blue-green color */
		water->color[k][0] = 0.0f; //was0.621f;
		water->color[k][1] = 0.0f; //was0.933f;
		water->color[k][2] = 1.0f; //was1.0f;
		water->color[k][3] = 1.0f;
		
		/* dark blue-green of pacific ocean */
		//PFSCALE_VEC3(water->color[k], 0.5f, water->color[k]);
		
		/* apply random scale factor to water color */
		if ((random() % 6) == 0)
		{
		    shadow = 0.25f * (random() & 0xff) / 255.0f +0.75f;
		    PFSCALE_VEC3(water->color[k], shadow, water->color[k]);
		}
		
		
		
		/* compute texture coordinate mapping */
		water->texture[k][PF_X] = water->scale*water->vertex[k][PF_X];
		water->texture[k][PF_Y] = water->scale*water->vertex[k][PF_Y];
	    }
	}
	
    }
    else
	copyWater(water, old);
    
    /* initialize tetxure map */
    {
	static pfTexture *map = NULL;
	
	if (map == NULL)
	{
	    map = pfNewTex(arena);
            // pfFilePath("/n/bossie/work2/bacon/thesis/NpsnetIV.8/src/environment/npsnet");
	    if (!pfLoadTexFile(map, water->name))
		printf("error: unable to load texture \"%s\"\n", water->name);
	}
	
	water->map = map;
    }
    
#define CHUNKSIZE	4	/* 6 triangles */
    
    group = pfNewGroup();
    
    material = pfNewMtl(arena);
    pfMtlColor(material, PFMTL_AMBIENT, 0.6f, 0.6f, 0.6f);
    pfMtlColor(material, PFMTL_DIFFUSE, 0.1f, 0.1f, 0.1f);
    pfMtlColorMode(material, PFMTL_FRONT, PFMTL_CMODE_AMBIENT_AND_DIFFUSE);
    
    /* allocate and initialize new geo-state */
    gstate = pfNewGState(arena);
#ifdef	NOT_DEFINED
    pfGStateInherit(gstate, PFSTATE_LIGHTMODEL | PFSTATE_ENLIGHTING);
#endif
    pfGStateMode(gstate, PFSTATE_ENTEXTURE, PF_ON);
    pfGStateAttr(gstate, PFSTATE_TEXTURE, water->map);
    pfGStateAttr(gstate, PFSTATE_FRONTMTL, material);
    
    /* create VISKIT geometry */
    for (i = 0; i < water->w; i++)
    {
	long            chunk = 0, start, end;
	do
	{
	    ushort      *index;
	    int	k, *lengths;
	    
	    /* compute indices */
	    start = chunk * CHUNKSIZE;
	    end = start + CHUNKSIZE + 2;
	    if (end > water->t)
		end = water->t;
	    
	    /* allocate storage for vertex indices */
	    index = (ushort *)pfMalloc(2*(end - start)*sizeof(unsigned short), arena);
	    
	    for (j = 0, k = start; k < end; k++, j++)
	    {
		index[2 * j] = (i + 1) * water->t + k;
		index[2 * j + 1] = (i) * water->t + k;
	    }
	    
	    /*
	      fprintf(stderr, "%d: %d, %d\n", i, start, end);
	      */
	    
	    /* allocate and initialize new geo-data */
	    gset = pfNewGSet(arena);
	    pfGSetGState(gset, gstate);
	    pfGSetPrimType(gset, PFGS_TRISTRIPS);
	    pfGSetNumPrims(gset, 1);
	    lengths = (int*)pfMalloc(sizeof(int), arena);
	    lengths[0] = 2*(end - start);
	    pfGSetPrimLengths(gset, lengths);
	    
	    pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, water->vertex, index);
	    pfGSetAttr(gset, PFGS_NORMAL3, PFGS_PER_VERTEX, water->normal, index);
	    pfGSetAttr(gset, PFGS_COLOR4, PFGS_PER_VERTEX, water->color, index);
	    pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_PER_VERTEX, water->texture, index);
	    
	    geode = pfNewGeode();
	    pfAddGSet(geode, gset);
	    
	    pfAddChild(group, (pfNode *) geode);
	    chunk++;
	} while (end < water->t);
    }
    
    
    return group;
}

/*--------------------------------------------------------------------*/
void
    LakeSim (pfChannel *, float xposit, float yposit, float direction)
{
    int i;
    //pfVec3 xyz;
    //pfVec3 hpr;
    float now;
    //float sine;
    //float cosine;
       
    static int which = 0;
    
    /* identify next available buffer */
    which = (which + 1)%3;

    //update ocean_dcs
    pfDCSTrans (ocean_dcs, xposit, yposit, 0.0);
    pfDCSRot (ocean_dcs,  direction, 0.0, 0.0);

    /* update water parameters */
    for (i = 0; i < 4; i++)
       { 
	Buffer[which].wave[i] = SharedWater->lakeData->wave[i];
       
       }
   
    /* recompute water position */
    animateWater(&Buffer[which]);
    
    /* select buffer */
    pfSwitchVal(switchNode, which);
    
    /* get current wall-clock time (in seconds) */
    now = pfGetTime();


}


/*--------------------------------------------------------------------*/
void
    animateWater(Water * water)
{
    int             i;
    int             j;
    float           s;
    float           c;
    double		now = pfGetTime();
    double work;
    
    for (i = 0; i < water->v; i++)
	water->vertex[i][PF_Z] = 2.0f;  //was -3.0
    
    for (j = 0; j < 4; j++)
    {
	if (water->wave[j].height != 0.0f)
	{
	    work = now*water->wave[j].speed;
	    water->wave[j].phase = work - (int)work;
	    
	    pfSinCos(water->wave[j].angle, &s, &c);
	    
	    for (i = 0; i < water->v; i++)
	    {
		float d;
		float a;
		float ss;
		float cc;
		
		d =(c * water->vertex[i][PF_X] + s * water->vertex[i][PF_Y]);
		work = (water->wave[j].phase + d / water->wave[j].length);
		a = work - (int)work;
		a = 360.0f*a;
		pfSinCos(a, &ss, &cc);
		
		if (j == 0)
		{
		    water->normal[i][PF_X] = 0.0f + 0.2f * ss;
		    water->normal[i][PF_Y] = 0.0f + 0.2f * (ss + cc);
		    water->normal[i][PF_Z] = 1.0f - 0.2f * cc;
		}
		
		water->vertex[i][PF_Z] += 0.5f * water->wave[j].height * ss;
	    }
	}
    }
    
    for (i = 0; i < water->v; i++)
    {
	water->texture[i][PF_X] = water->scale*water->vertex[i][PF_X] +
	    water->tvx*now;
	water->texture[i][PF_Y] = water->scale*water->vertex[i][PF_Y] +
	    water->tvy*now;
    }

}
#endif
