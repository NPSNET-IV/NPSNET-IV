// File: <measures.cc>

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
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *    npsnet-info@cs.nps.navy.mil
 *       Contact principle investigators.
 *       Overall research project information and funding.
 *       Requests for demonstations.
 *
 * Thank you to our sponsors:  ARL, STRICOM, TRAC, ARPA and DMSO.
 */

#ifndef NOC2MEASURES

#include "globals.h"
#include "measures.h"

static pfGroup *L_measures = NULL;  // the root of the measures tree
static pfVec4 L_red_color[1]     = {1.0f,0.0f,0.0f,0.3f};
static pfVec4 L_green_color[1]   = {0.0f,1.0f,0.0f,0.3f};
static pfVec4 L_blue_color[1]    = {0.0f,0.0f,1.0f,0.3f};
static pfVec4 L_magenta_color[1] = {1.0f,0.0f,1.0f,0.3f};
//static long   L_length_4[1] = {10};
//static long   L_length_8[1] = {18};
static long   L_length_4[1] = {1};
static long   L_length_8[1] = {2};


/*********************** MEASURES_CLASS   *************************/
MEASURES_CLASS :: MEASURES_CLASS (C2_TYPE meas_type, int cnt)
{
  c2_type = meas_type;
  numparts = cnt;  
}

MEASURES_CLASS :: ~MEASURES_CLASS ()
{
  if(pfGetNumParents(polys)){
    //the node is in the scene graph so remove it
    pfRemoveChild(pfGetParent(polys,0),polys);
  }
  //delete it from memory
  pfDelete(polys);
}

void MEASURES_CLASS :: attach_measure(pfGroup *parent)
//attach the to scene graph
{
printf("result from attach_measure %3d\n",
  pfAddChild(parent,polys));
}

void MEASURES_CLASS :: read_measure(FILE * infile)
{
  char dummy[255];
 
  fgets(dummy,255,infile); 
  //this is an error condition
  printf("Got a major error here\n");
}

/*********************** POINT_CLASS      *************************/

POINT_CLASS :: POINT_CLASS (C2_TYPE meas_type, int cnt)
: MEASURES_CLASS (meas_type,cnt)
{
  type = POINT;
}

void POINT_CLASS :: read_measure(FILE *infile)
//read the measure off the disk and build it
{
  int ix;
printf("Reading a point measure in\n");
  for(ix=0;ix<numparts;ix++){
    fscanf(infile,"%f %f %f ",
       &(points[ix][X]),&(points[ix][Y]),&(points[ix][Z]));
    //make sure the feature is on the ground
    points[ix][Z] = gnd_level2(points[ix]);
  }
  
  //depending on the type, build a red 4 side figure or a 6 sided blue one
  switch(c2_type){
    case ENEMY: //red square
      polys = make_point_meas(ENEMY,points[0]); 
      attach_measure(L_measures);
      break;
    case OBS_POST: //6 sided blue
      polys = make_point_meas(OBS_POST,points[0]); 
      attach_measure(L_measures);
      break;
    default: //There is a mistake
      printf("There is a bad Measures type %d %d\n",type,c2_type);
      break;
  }

}

/*********************** LINE_CLASS       *************************/

LINE_CLASS :: LINE_CLASS (C2_TYPE meas_type, int cnt)
: MEASURES_CLASS (meas_type,cnt)
{
  type = LINE;
}

void LINE_CLASS :: read_measure(FILE *infile)
//read the measure off the disk and build it
{
  int ix;
  for(ix=0;ix<numparts;ix++){
    fscanf(infile,"%f %f %f ",
      &(points[ix][X]),&(points[ix][Y]),&(points[ix][Z]));
    //make sure the feature is on the ground
    points[ix][Z] = gnd_level2(points[ix]);
  }

  switch(c2_type){
    case LINE_OF_DEPARTURE:
      polys = make_line_meas(LINE_OF_DEPARTURE,points[0],points[1]);
      attach_measure(L_measures);
      break;
    default: //There is a mistake
      printf("There is a bad Measures type %d %d\n",type,c2_type);
      break;
  }
}

/*********************** MULTI_LINE_CLASS *************************/

MULTI_LINE_CLASS :: MULTI_LINE_CLASS (C2_TYPE meas_type, int cnt)
: MEASURES_CLASS (meas_type,cnt)
{
  type = MULTI_LINE;
}

void MULTI_LINE_CLASS :: read_measure(FILE *infile)
//read the measure off the disk and build it
{
  int ix;

  polys = pfNewGroup();

  for(ix=0;ix<numparts;ix++){
    fscanf(infile,"%f %f %f ",
      &(points[ix][X]),&(points[ix][Y]),&(points[ix][Z]));
    //make sure the feature is on the ground
    points[ix][Z] = gnd_level2(points[ix]);
  }
  switch(c2_type){
    case AXIS:
    case BOUNDRY:
      for(ix=1;ix<numparts;ix++){
        pfAddChild(polys,make_line_meas(c2_type,points[ix-1],points[ix]));
      }
      attach_measure(L_measures);
      break;
    default: //There is a mistake
      printf("There is a bad Measures type %d %d\n",type,c2_type);
      break;
  }

}

/*********************** AREA_CLASS       *************************/

AREA_CLASS :: AREA_CLASS (C2_TYPE meas_type, int cnt)
: MEASURES_CLASS (meas_type,cnt)
{
  type = AREA;
}

void AREA_CLASS :: read_measure(FILE *infile)
//read the measure off the disk and build it
{
  int ix;

  polys = pfNewGroup();

  for(ix=0;ix<numparts;ix++){
    fscanf(infile,"%f %f %f ",
      &(points[ix][X]),&(points[ix][Y]),&(points[ix][Z]));
    //make sure the feature is on the ground
    points[ix][Z] = gnd_level2(points[ix]);
  }
  switch(c2_type){
    case OBJECTIVE:
    case OBSTACLES:
      for(ix=1;ix<numparts;ix++){
        pfAddChild(polys,make_line_meas(c2_type,points[ix-1],points[ix]));
      }
      pfAddChild(polys,make_line_meas(c2_type,points[0],points[numparts-1]));
      attach_measure(L_measures);
      break;
    default: //There is a mistake
      printf("There is a bad Measures type %d %d\n",type,c2_type);
      break;
  }

}

/************************ General Functions *************************/

pfGroup *make_line_meas(C2_TYPE c2_type,pfVec3 origin,pfVec3 end)
//make a line segment measure
{
  pfGroup *the_group;
  pfGeoSet *gset;
  pfGeoState *gst;
  pfGeode     *geode;
  void *arena;
  pfVec3 *coords;
  float angle,msin,mcos;
  int ix,jx=0,num_seg;
  long *length_vec;

  //get the shared memory
  arena = pfGetSharedArena();
  gset = pfNewGSet(arena);

  //set the colors
  switch(c2_type){
    case AXIS:
      pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, L_blue_color, NULL);
      break;
    case LINE_OF_DEPARTURE:
    case OBJECTIVE:
      pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, L_green_color, NULL);
      break;
    case BOUNDRY:
      pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, L_magenta_color, NULL);
      break;
    case OBSTACLES:
      pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, L_red_color, NULL);
      break;
    default:
     break;
  }

  //generate the points
  num_seg = (int)(PFDISTANCE_PT3(origin,end) / MEAS_LENGTH) +1;

  if(NULL==(coords=(pfVec3 *)pfMalloc((num_seg*2+2)*sizeof(pfVec3),arena))){
    return(NULL);
  }
  

  angle = pfArcTan2((end[Y] - origin[Y]),(end[X] - origin[X]));
  pfSinCos(angle,&msin,&mcos);
/*
printf("number of segments %4d length %10.4f\n",num_seg,(PFDISTANCE_PT3(origin,end)));
*/
  for (ix=0;ix<num_seg;ix++ ){
    coords[jx][X] = mcos * MEAS_LENGTH*ix + origin[X];
    coords[jx][Y] = msin * MEAS_LENGTH*ix + origin[Y];
    coords[jx][Z] = gnd_level2(coords[jx]) - 5.0f;

/*
printf("ix %3d jx %3d (%10.3f %10.3f %10.3f)\n",ix,jx,coords[jx][X],coords[jx][Y],coords[jx][Z]);
*/
    jx++;
    coords[jx][X] = mcos * MEAS_LENGTH*ix + origin[X];
    coords[jx][Y] = msin * MEAS_LENGTH*ix + origin[Y];
    coords[jx][Z] = gnd_level2(coords[jx]) + MEAS_SIZE;

/*
printf("ix %3d jx %3d (%10.3f %10.3f %10.3f)\n",ix,jx,coords[jx][X],coords[jx][Y],coords[jx][Z]);
*/
    jx++;
  }
  //get the end point
  PFCOPY_VEC3(coords[jx],end);
  coords[jx][Z] = gnd_level2(end) - 5.0f;
  jx++;
  PFCOPY_VEC3(coords[jx],end);
  coords[jx][Z] = gnd_level2(end) + MEAS_SIZE;

  // set the coordinate, normal and color arrays
  // and their cooresponding index arrays
  pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, coords, NULL);
  pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
  pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
  pfGSetPrimType(gset, PFGS_TRISTRIPS);
  pfGSetNumPrims(gset, 1);
  if(NULL==(length_vec = (long *) pfMalloc(sizeof(long),arena))){
    return(NULL);
  }
  length_vec[0] = (num_seg*2+2);
  pfGSetPrimLengths(gset,length_vec);  

  // create a new geostate from shared memory,
  // enable transparency
  gst = pfNewGState(arena);
  pfGStateMode(gst, PFSTATE_ENTEXTURE, 0);
  pfGStateMode(gst, PFSTATE_TRANSPARENCY, 1);

  pfGSetGState(gset, gst);

  //add it to the group
  geode = pfNewGeode();
  pfAddGSet (geode, gset);
  the_group = pfNewGroup();
  pfAddChild(the_group,geode);

  return(the_group);
}

pfGroup *make_point_meas(C2_TYPE c2_type,pfVec3 origin)
//make the pfGroup for a point measure
{
  pfGroup *the_group;
  pfGeoSet *gset;
  pfGeoState *gst;
  pfGeode     *geode;
  void *arena;
  pfVec3 *coords;
  int numprims;
  float delta_angle;
  int ix,jx=0;;


  //get the shared memory
  arena = pfGetSharedArena();
  gset = pfNewGSet(arena);

  //allocate the memory for the vertex arrays
  switch(c2_type){
    case ENEMY:
      pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, L_red_color, NULL); 
      pfGSetPrimLengths(gset,L_length_4 );
      numprims = 4;
      break;
    case OBS_POST:
      pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, L_blue_color, NULL); 
      pfGSetPrimLengths(gset,L_length_8 );
      numprims = 8;
      break;
    default:
      pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, L_red_color, NULL); 
      pfGSetPrimLengths(gset,L_length_8 );
      numprims = 8;
      break;
  }
  
  if(NULL == (coords = 
               (pfVec3 *)pfMalloc((numprims*2+2)*sizeof(pfVec3),arena))){
    return(NULL);
  }

  //generate the points
  delta_angle = 360.0f/numprims;
  for (ix=0;ix<=numprims;ix++ ){
    float msin,mcos;
    pfSinCos(ix*delta_angle,&msin,&mcos);
    coords[jx][X] = mcos * MEAS_LENGTH + origin[X]; 
    coords[jx][Y] = msin * MEAS_LENGTH + origin[Y]; 
    coords[jx][Z] = origin[Z] - 5.0f; 
/*
printf("ix %3d jx %3d (%10.3f %10.3f %10.3f)\n",ix,jx,coords[jx][X],coords[jx][Y],coords[jx][Z]);
*/
    jx++;
    coords[jx][X] = mcos * MEAS_LENGTH + origin[X];
    coords[jx][Y] = msin * MEAS_LENGTH + origin[Y];
    coords[jx][Z] = origin[Z] + MEAS_SIZE - 5.0f;
/*
printf("ix %3d jx %3d (%10.3f %10.3f %10.3f)\n",ix,jx,coords[jx][X],coords[jx][Y],coords[jx][Z]);
*/
    jx++;
  }

  // set the coordinate, normal and color arrays
  // and their cooresponding index arrays
  pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, coords, NULL);
  pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
  pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
  pfGSetPrimType(gset, PFGS_TRISTRIPS);
  pfGSetNumPrims(gset, 1);

  // create a new geostate from shared memory,
  // enable transparency
  gst = pfNewGState(arena);
  pfGStateMode(gst, PFSTATE_ENTEXTURE, 0);
  pfGStateMode(gst, PFSTATE_TRANSPARENCY, 1);

  pfGSetGState(gset, gst);

  //add it to the group
  geode = pfNewGeode();
  pfAddGSet (geode, gset);
  the_group = pfNewGroup(); 
  pfAddChild(the_group,geode);

  return(the_group);
}

int open_measure_file(char *filename)
//open up the measures file
{
  FILE *infile;
  int type,t_meas_type,count;
  C2_TYPE meas_type;
  char name[255];


  if(NULL == (infile = fopen(filename,"r"))){
    printf("Unable to open the file %s\n");
    return(FALSE);
  }

  while(4 == fscanf(infile,"%s %d %d %d ",name,&type,&t_meas_type,&count)){
    meas_type = (C2_TYPE)t_meas_type;
    switch(type){
      case POINT:  //single point feature
        {
        POINT_CLASS *meas;
        
        meas = new POINT_CLASS(meas_type,count); 
        meas->read_measure(infile);
        
        }
        break;
      case LINE:  //single line feature
        {
        LINE_CLASS *meas;
        
        meas = new LINE_CLASS(meas_type,count); 
        meas->read_measure(infile);
        
        }
        break;
      case MULTI_LINE:  //multiple line segments (up to C2_SEGMENTS) feature
        {
        MULTI_LINE_CLASS *meas;
        
        meas = new MULTI_LINE_CLASS(meas_type,count); 
        meas->read_measure(infile);
        
        }
        break;
      case AREA: //like MULTI_LINE, but closed
        {
        AREA_CLASS *meas;
        
        meas = new AREA_CLASS(meas_type,count); 
        meas->read_measure(infile);
        
        }
        break;
      default: //got a booboo
        return(FALSE);
    }
  }
  fclose(infile);

  //everything worked
  return(TRUE);
}


void hide_measures()
//hide the measures
{
printf("hide the measures\n");
  if((L_measures == NULL) || 
       (!strcmp((char *)G_static_data->c2_file,D_C2_FILE))){
    //there is no measure file defined
    printf("No measures are defined / loaded\n");
    return;
  }
  if(pfGetNumParents(L_measures) != 0)
    pfRemoveChild(G_measures,L_measures);
}

void display_measures()
//attach the measures to the scene graph
{
printf("display the measures\n");
  if(L_measures != NULL){
printf("Result from adding the L_Measures %3d\n",
    pfAddChild(G_measures,L_measures));
  }
  else{
    printf("The measures tree is empty\n");
  }
}


void c2_measures()
//if the root of the tree is NULL we have not read in the measures
{
  
  if(!(strcmp((char *)G_static_data->c2_file,D_C2_FILE))){
    //there is no measure file defined
    return;
  }

  if(L_measures == NULL){
    //read the file in
    L_measures = pfNewGroup();
printf("Opening a file %s\n",G_static_data->c2_file);
    open_measure_file((char *)G_static_data->c2_file);
    display_measures();
  }
  else{
    //just attach the node
    display_measures();
  }
}

void delete_measures()
//remove all of the current measuers
{
  hide_measures();
  pfDelete(L_measures);
  L_measures = NULL;
}

#endif
