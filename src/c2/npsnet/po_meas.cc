// File: <po_meas.cc>
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

//  Changes made 26 April 1995 by Sam Kirby(SAK). Modifications of classes.
//  The modifications were made to catch PDU's in real time and construct
//  measures rather than read it from a file.

#include <iostream.h>
#define __PO_LOCAL__
#include "globals.h"

#include "basic.h"  //SAK.I need this to clobber the original from MODSAF.  This
                    //is needed because of name conflicts,

#include "terrain.h"

extern "C"{
#include "libpo.h"  //SAK.  Modsaf stuff. 

#include "libroute.h" //SAK.  Has conversion routines for routes to points.  
                      //  Used in building lines.
}

#include "address.h"    //  SAK.

#include "menu_funcs.h"

extern float64 POF_nps_northing_shift;
extern float64 POF_nps_easting_shift;

static int    L_numblocks = 0;


/*********************** PO_MEASURES_CLASS   *************************/

//SAK.  This constructor instantiates the class with the actual pdu that 
//  describes it.
PO_MEASURES_CLASS::PO_MEASURES_CLASS(SP_DescribeObjectVariant describeObject,
   int length)
{
   char    *temp;                   //  Will store pdu as string.

   firstTime = 1;                   //  Set flag.

   //  Convert the pdu into a string.  
   temp = (char *) &describeObject;
   pdu = (char *)pfMalloc((length+1)*sizeof(char),pfGetSharedArena());
   //pdu = new char[length + 1];       Allocate storage.  I don't just want 
                                   //  a pointer.

   //  Copy as string and put a terminator at the end.
   for (int i = 0; i < length; i++){
      pdu[i] = temp[i];
   }
   
   pdu[length] = '\0';
}  //  End PO_MEASURES_CLASS constructor.  

//  Constructor when no pdu given.
PO_MEASURES_CLASS :: PO_MEASURES_CLASS()
{
   pdu = NULL;
}


//  Destructor.  I think this takes care of memory leaks.
PO_MEASURES_CLASS :: ~PO_MEASURES_CLASS ()
{
   if(pfGetNumParents(polys)){
      //  The node is in the scene graph so remove it.
      pfRemoveChild(pfGetParent(polys,0),polys);
   }

   // Delete it from memory.
   pfDelete(polys);
   //delete []pdu;      //SAK. Get rid of the entire string.
   pfFree(pdu);
}  //  End of destructor.


//  Construct the measure.  This is a virtual function and should only
//  be called from the derived classes. 
pfGroup *PO_MEASURES_CLASS ::make_meas()
{
   cerr<<"You should never call this function, only the derived classes."<<
      endl;
   return(NULL);
}


//  Virtual function to create text node.  Should only be called
//  from derived classes.
void PO_MEASURES_CLASS::create_text_node(float*,PO_COLOR,char*)
{
   cerr<<"You should never call this function, create_text_node."<<endl;
}

//  This function updates a po measure.  It is a virtual function and 
//  should only be called from the derived classes.
void PO_MEASURES_CLASS::update(SP_DescribeObjectVariant,int)
{
   cerr<<"You should never call this function, create_text_node."<<endl;
}

//  Virtual function.  Used for dragging po measures.  Should only be used
//  from derived classes.
void PO_MEASURES_CLASS::movePO(float, float, ushort)
{
   cerr<<"Shouldn't call this virtual function"<<endl;
}

//  Virtual function.  Used for sending the dragged pdu information to the
//  network.  Should only be used from derived classes.
void PO_MEASURES_CLASS::dragToNet(float, float)
{
   cerr<<"Shouldn't call this virtual function.  dragToNet."<<endl;
}

//  Virtual function.  Used when dragging po measures.  This function
//  is used when the measure is dropped and should only be called
//  from derived classes.
void PO_MEASURES_CLASS::dragUpdatePO(float, float, ushort)
{
   cerr<<"Shouldn't call this virtual function"<<endl;
}

//  Virtual function.  Used when the member values of a po measure have been
//  changed.  When the member values have changed, this function then builds
//  the SP_DescribeObjectVariant and puts it in pdu.  This is a virtual
//  function and should be used only with the derived classes.
void PO_MEASURES_CLASS::updatePDU(){

   cerr<<"Virtual function: updatePDU, should only be called in derived class."
      <<endl;
}

//  Virtual function.  This function will take the current value of of the pdu
//  member and send it out on the network.  It is a virtual function and should
//  only be called by the derived classes.
void PO_MEASURES_CLASS::sendPDU(){

   cerr<<"Virtual function: sendPDU, should only be called in derived class."<<
      endl;
}



/*********************** PO_LINE_CLASS       *************************/
//SAK.  Make the line measure from the pdu which describes it.
PO_LINE_CLASS :: PO_LINE_CLASS(SP_DescribeObjectVariant describeObject, 
   int length): PO_MEASURES_CLASS(describeObject, length)
{
   ROUTE_LIST *route;                //  Used to convert routes to points.

   type = PO_LINE;
   firstTime = 1;
   difference[X] = 0;
   difference[Y] = 0;

   //  Must convert the way the points are sent in the PDU into a form I can
   //  use as vertices with performer.
   route = rt_line_class_to_route_list(&L_ctdb,
      &describeObject.variant.line);

   rt_list_roads_to_points(route);
   rt_list_merge_sections(route);

   //  The next part chops up the pdu taking out members for the measure.
   
   dashed = (ushort)describeObject.variant.line.dashed;
   style = (ushort)describeObject.variant.line.style;
   color = (PO_COLOR)describeObject.variant.line.color;
   numpoint = (int)(route->route.secpoints.num_pts);
   thickness = (ushort)describeObject.variant.line.thickness;
   closed = (ushort)describeObject.variant.line.closed;
   splined = (ushort)describeObject.variant.line.splined;
   width = (float)describeObject.variant.line.width;
   beginArrowHead = (ushort)describeObject.variant.line.beginArrowHead;
   endArrowHead = (ushort)describeObject.variant.line.endArrowHead;

   for (int i = 0; i < numpoint; i++){
      points[i][X] = (float)route->route.secpoints.points[i].point[0] -
                     POF_nps_easting_shift;
      points[i][Y] = (float)route->route.secpoints.points[i].point[1] -
                     POF_nps_northing_shift;
      points[i][Z] = gnd_level2(points[i]);
   }

   polys = make_meas();

}  //  End of PO_LINE_CLASS constructor.

//  This constructor returns a PO_LINE_CLASS without a pdu.  It assigns
//  the member functions from the parameters.
PO_LINE_CLASS :: PO_LINE_CLASS(ushort Dashed, ushort Style, PO_COLOR Color,
   ushort Thickness, ushort Closed, ushort Splined, float Width, ushort
   BeginArrowHead, ushort EndArrowHead, int Numpoint, pfVec3* Points,
   ushort quick):PO_MEASURES_CLASS()
{
  
   type = PO_LINE;
   firstTime = 1;
   difference[X] = 0;
   difference[Y] = 0;

   //  The next part chops up the pdu taking out members for the measure.
   
   dashed = Dashed;
   style = Style;
   color = Color;
   numpoint = Numpoint;
   thickness = Thickness;
   closed = Closed;
   splined = Splined;
   width = Width;
   beginArrowHead = BeginArrowHead;
   endArrowHead = EndArrowHead;

   for (int i = 0; i < numpoint; i++){
      points[i][X] = Points[i][X];
      points[i][Y] = Points[i][Y];
      points[i][Z] = Points[i][Z];
   }

   if (Numpoint > 1){
      if (quick){
         polys = pfNewGroup();
         pfAddChild(polys, makeQuickLine());
      }
      else{
         polys = make_meas();
      }
   }
   else{
      polys = NULL;
   }

}  //  End of PO_LINE_CLASS constructor.

PO_LINE_CLASS::~PO_LINE_CLASS()
{
   if(pfGetNumParents(polys)){
      //  The node is in the scene graph so remove it.
      pfRemoveChild(pfGetParent(polys,0),polys);
   }

   // Delete it from memory.
   pfDelete(polys);
   //delete []pdu;      //SAK. Get rid of the entire string.
   pfFree(pdu);
}

void PO_LINE_CLASS::addPoint(pfVec3 point){

//cerr<<"in PO_LINE_CLASS::addPoint"<<endl;
//   if (pfGetParent(polys,0) == NULL){
   if (polys == NULL){
      cerr<<"poly's par is null. error"<<endl;
   }
   else {
//cerr<<"about to remove"<<endl;
      pfRemoveChild(pfGetParent(polys,0),polys);   //SAK.  Remove the old.
//cerr<<"about to delete"<<endl;
      pfDelete(polys);
   }

//cerr<<"about to copy"<<endl;
   pfCopyVec3(points[numpoint], point);
   numpoint++;

   if (numpoint > 1){
      polys = make_meas();
   }
   else{
      polys = NULL;
   }

}

//SAK.  Update the line measure from the pdu which describes it.  Basically, like
//the constructor except new measure isn't created.
void PO_LINE_CLASS :: update(SP_DescribeObjectVariant describeObject,
   int length)
{

   ROUTE_LIST *route;       //  Route in PDU format.
   char    *temp; 

   if (pfGetParent(polys,0) == NULL){
      cerr<<"poly's par is null. error"<<endl;
      exit(0);
   }

   //  Set pickedGroup which is used to ensure that pointers to a gset
   //  are not accessed after being deleted.  Needed when highlighting
   //  it basically serves as a flag to prevent crashing when a highlighted
   //  object gets an update pdu.
   //if (polys == pickedGroup){
   //   pickedGroup = NULL;
   //}

   pfRemoveChild(pfGetParent(polys,0),polys);   //SAK.  Remove the old.
  
   //  If I'm dragging the line, all I want to do is update the polys with
   //  a quick line. I used an unused field in the ModSAF packet.  I
   //  change the line and put a quickline version in.  This could pose
   //  problems if I didn't get the "drop" pdu, because the line would
   //  in essence be left in the low resolution representation.  Should
   //  ensure we don't drop packets on the ground.  Doesn't work with
   //  roads due to the way I grab my points.

   //  I check to see if the group is highlighted, i.e. being dragged.  If so
   //  I ignore the update.  Basically update pdus don't interupt a drag.  Last
   //  drag wins.
   if (polys != pickedGroup){    //  Do the update.
      
      //  Delete it from memory.
      pfDelete(polys);          //  Get rid of the old group.

      //  This unused field is used to send whether or not the measure is
      //  being dragged.
      if (describeObject.variant.line._unused_10 == 1){
     
         for (int i = 0; i < numpoint; i++){
            points[i][X] = (float)describeObject.variant.line.points[i].
               variant.location.x;
            points[i][Y] = (float)describeObject.variant.line.points[i].
               variant.location.y;
            points[i][Z] = gnd_level2(points[i]);
         }

         difference[X] = 0.0;
         difference[Y] = 0.0;

         polys = pfNewGroup();
         //  A line which I can move in real time.
         pfAddChild(polys, makeQuickLine()); 

         updatePDU();

      }
      else{
         //delete []pdu;             //  SAK. Get rid of the entire string.
         pfFree(pdu);

         //  Add the new.
         //  I will convert the pdu into a string and ensures it terminates 
         //  with the '\0'.  In the constructor this is done in the parent
         //  class.

         temp = (char *) &describeObject;

         pdu = (char *)pfMalloc((length+1)*sizeof(char),pfGetSharedArena());
         //pdu = new char[length + 1];
         for (int i = 0; i < length; i++){
            pdu[i] = temp[i];
         }
         pdu[length] = '\0';

         //  Conversion of route to points.
         route = 
            rt_line_class_to_route_list(&L_ctdb, &describeObject.variant.line);
         rt_list_roads_to_points(route);
         rt_list_merge_sections(route);

         //  Take the members out of the PDU.
         type = PO_LINE;
         dashed = (ushort)describeObject.variant.line.dashed;
         style = (ushort)describeObject.variant.line.style;
         color = (PO_COLOR)describeObject.variant.line.color;
         numpoint = (int)(route->route.secpoints.num_pts);
         thickness = (ushort)describeObject.variant.line.thickness;
         closed = (ushort)describeObject.variant.line.closed;
         splined = (ushort)describeObject.variant.line.splined;
         width = (float)describeObject.variant.line.width;
         beginArrowHead = (ushort)describeObject.variant.line.beginArrowHead;
         endArrowHead = (ushort)describeObject.variant.line.endArrowHead;

         for (i = 0; i < numpoint; i++){
            points[i][X] = (float)route->route.secpoints.points[i].point[0] -
                           POF_nps_easting_shift;
            points[i][Y] = (float)route->route.secpoints.points[i].point[1] - 
                           POF_nps_northing_shift;
            points[i][Z] = gnd_level2(points[i]);
         }
   
         polys = make_meas();   //  The pfGroup actually representing the object.
   
      }  //  End of else.
   } //  End of if, (polys != pickedGroup).
}  //  End of update.



//  This function makes the pfGroup which represents the line.  This
//  representation is a single line.  But picking does not work through
//  a line so a polygon backboard is needed at the same position of the line.
pfGroup * PO_LINE_CLASS::make_meas()
{
   

   pfGroup *the_group;
   pfGeoSet *gset;
   pfGeoState *gst;
   pfGeode *geode,*geode2;
   pfVec3 *coords;
   
   void *arena;
 
   float angle,                     // Angle between line points.
      msin,mcos;                    // Sine and cosine of that angle.

   float Xmiddle, Ymiddle;          //  The increment shift up the
                                    //  centerline.

   int numvert=0,                   //  Vertex number in line.
      num_seg,                      //  Number of line sub segments.
      seg_cnt;                      //  Segment count between points.

   int *length_vec;                //  Used with performer primitives.

   int ix,kx;

   //  Get the shared memory.
   arena = pfGetSharedArena();
   

   //  From the points of the line I will chop up the line into a bunch of 
   //  segments.  In this way my line can follow the terrain elevation.
   //  First figure out how many segments there are.
   num_seg = 0;
   for(ix=0;ix<(numpoint-1);ix++){
      num_seg += (int)(PFDISTANCE_PT3(points[ix],points[ix+1])/
         PO_MEAS_LENGTH)+1;
   }

   if(closed){
      num_seg += (int)(PFDISTANCE_PT3(points[numpoint-1],points[0])/
         PO_MEAS_LENGTH)+1;
   }
 
   //  Now allocate the storage with the correct number of segs.
   if(NULL==(coords=(pfVec3 *)pfMalloc((num_seg*2+2)*sizeof(pfVec3),arena)))
   {
      return(NULL);
   }

   //  This part of make_meas creates a line which is a line strip primitive.
   //  it creates a single vertice for each segment of the line. 
   //  It bases the line thickness on the desired thickness
   //  of the line.  Numvert starts at zero.  Now I probably allocate too
   //  much storage for coords because my number of vertices is basically
   //  cut in half.  The line is also elevated by LINE_HEIGHT.
   //  The function actually makes two lines, one that is visible and one
   //  that is used as a backboard for picking.

   PFCOPY_VEC3(coords[0], points[0]);        //  First point in this sequence
                                             //  is the original point.

   coords[0][Z] =  gnd_level2(coords[0]) + LINE_HEIGHT; 

   numvert++;

   //  Generate the segment points for the performer primitive.
   for(ix=0;ix<(numpoint-1);ix++){
      //  Get the angle between the lines main points.
      angle = pfArcTan2((points[ix+1][Y] - points[ix][Y]),
         (points[ix+1][X] - points[ix][X]));

      pfSinCos(angle,&msin,&mcos);         //  Used to increment line segs.
      Xmiddle = mcos * PO_MEAS_LENGTH;     //  Incremental shift.
      Ymiddle = msin * PO_MEAS_LENGTH;
              
      //  The number of sub-segments a line is divided into.
      seg_cnt = (int)(PFDISTANCE_PT3(points[ix],points[ix +1])/ 
         PO_MEAS_LENGTH);                 
    
      //  Cycle through all points on centerline.
      for (kx=1;kx<seg_cnt-1;kx++ ){ 
         coords[numvert][X] = coords[numvert -1][X] + Xmiddle;
         coords[numvert][Y] = coords[numvert -1][Y] + Ymiddle;
         coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
         numvert++;
      }  //  End of kx for loop. 

      //Ensure that last point is in fact the next point.  i.e the segments
      //may not reach the last point due to rounding of seg_cnt. 
      coords[numvert][X] = points[ix+1][X];
      coords[numvert][Y] = points[ix+1][Y];
      coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT; 
      numvert++;
  
   }  //  End of ix for loop.

   //  Add the last line back to the start.
   if (closed){               
      PFCOPY_VEC3(coords[numvert], points[numpoint-1]);      //  Last point.
      coords[numvert][Z] =  gnd_level2(coords[numvert]) + LINE_HEIGHT; 
      numvert++;

      //  Get the angle between the points.
      angle = pfArcTan2((points[0][Y] - points[numpoint-1][Y]),
         (points[0][X] - points[numpoint-1][X]));

      pfSinCos(angle,&msin,&mcos);         //  Signs and cosines for sgmnt.
      Xmiddle = mcos * PO_MEAS_LENGTH;     //  Incremental shift.
      Ymiddle = msin * PO_MEAS_LENGTH;       

      //  The number of sub-segments a line is divided into.
      seg_cnt = (int)(PFDISTANCE_PT3(points[numpoint-1],points[0])/ 
         PO_MEAS_LENGTH);                 
    
      //  Cycle through all points on centerline, and assign
      //  to the vector.  
      for (kx=1;kx<seg_cnt-1;kx++ ){ 
         coords[numvert][X] = coords[numvert-1][X] + Xmiddle;
         coords[numvert][Y] = coords[numvert-1][Y] + Ymiddle;
         coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
         numvert++;
      }  //  End of kx for loop. 
 
      //Ensure it's connected.
      coords[numvert][X] = points[0][X];
      coords[numvert][Y] = points[0][Y];
      coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT; 
      numvert++;

   }  //  End of if (closed).

   // Set the primitive attributes.
   
   gset = pfNewGSet(arena);
   
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, L_color[color], NULL);
   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, coords, NULL);
   pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);

   
   pfGSetPrimType(gset, PFGS_FLAT_LINESTRIPS);    
   pfGSetNumPrims(gset, 1);
   
   if(NULL==(length_vec = (int *) pfMalloc(sizeof(int),arena))){
      return(NULL);
   }
   length_vec[0] = numvert;

   pfGSetPrimLengths(gset,length_vec);
   pfGSetLineWidth(gset, thickness);       //  Numbers of pixels wide.
                                                
   // Create a new geostate from shared memory, enable transparency.
   gst = pfNewGState(arena);

   pfGStateMode(gst, PFSTATE_ENTEXTURE, 0);
   pfGStateMode(gst, PFSTATE_TRANSPARENCY, 1);

   pfGSetGState(gset, gst);

   //  The geode with the visible line.
   geode = pfNewGeode();
   pfAddGSet (geode, gset);

   //  Make a vertical backboard to use for picking.
   geode2 = pfNewGeode();
   geode2 = make_vertical();

  the_group = pfNewGroup();

  //  Naming of nodes is used when picking to determine what kind of 
  //  nodes have been picked when the pick is queried.
  pfNodeName(geode, "highlight");
  pfNodeName(the_group, "picked_group");

  pfAddChild(the_group,geode);
  pfAddChild(the_group,geode2);

  return(the_group);
}   //  End of PO_LINE_CLASS's make_meas.


//  This function constructs a vertucal, tranparent tristrip which is
//  used for picking lines.  Performer 1.2 has a documented bug in
//  which it cannot pick lines.  To get around this I put this transparent
//  strip behind the visible line.  The basic algorithm is the same
//  as the line above.
pfGeode * PO_LINE_CLASS::make_vertical()
{
   const float LINE_HEIGHT = 10.0;
   const float VERT_THICK = 1.0;

   static pfVec4 transparent = {0.0, 0.0, 0.0, 0.0};

   pfGeoSet *gset;
   pfGeoState *gst;
   pfGeode     *geode;
   pfVec3 *coords;

   void *arena;

   float angle,                     // Angle between line points.
      msin,mcos;                    // Sine and cosine of that angle.

   int numvert=0,                   //  Vertex number in line.
      num_seg,                      //  Number of line sub segments.
      seg_cnt;                      //  Segment count between points.

   int *length_vec;                //  Used with performer primitives.

   int ix,kx;

   //  Get the shared memory.
   arena = pfGetSharedArena();
   

   //  From the points of the line I will chop up the line into a bunch of 
   //  segments.  In this way my line can follow the terrain elevation.
   //  First figure out how many segments there are.
   num_seg = 0;
   for(ix=0;ix<(numpoint-1);ix++){
      num_seg += (int)(PFDISTANCE_PT3(points[ix],points[ix+1])/
         PO_MEAS_LENGTH)+1;
   }

   if(closed){
      num_seg += (int)(PFDISTANCE_PT3(points[numpoint-1],points[0])/
         PO_MEAS_LENGTH)+1;
   }
 
   //  Now allocate the storage.
   if(NULL==(coords=(pfVec3 *)pfMalloc((num_seg*2+2)*sizeof(pfVec3),arena)))
   {
      return(NULL);
   }

   //  This creates triangle strips which are oriented vertically in 
   //  essence creating "walls".  It divides the line between each set
   //  of points an finds a top post and bottom post which
   //  are used as a vector of vertices to define the triangle strip.

   // Prime the line by puting the start point in.
   PFCOPY_VEC3(coords[numvert],points[0]);
   coords[numvert][Z] = gnd_level2(coords[numvert]) - VERT_THICK +
      LINE_HEIGHT;
   numvert++;

   PFCOPY_VEC3(coords[numvert],coords[numvert-1]);
   coords[numvert][Z] += 2.0 * VERT_THICK;
   numvert++;

   //Step thought the segments and generate the inside points of segments.
   for(ix=0;ix<(numpoint-1);ix++){
      angle = pfArcTan2((points[ix+1][Y] - points[ix][Y]),
         (points[ix+1][X] - points[ix][X]));
      seg_cnt = (int)(PFDISTANCE_PT3(points[ix],points[ix +1])/
         PO_MEAS_LENGTH);
      pfSinCos(angle,&msin,&mcos);

      for (kx=1;kx<seg_cnt;kx++ ){
         // The down side of the triangle.
         coords[numvert][X] = mcos * PO_MEAS_LENGTH*kx + points[ix][X];
         coords[numvert][Y] = msin * PO_MEAS_LENGTH*kx + points[ix][Y];
         coords[numvert][Z] = gnd_level2(coords[numvert]) - VERT_THICK +
            LINE_HEIGHT;
         numvert++;

         //The upside of the triangle.
         PFCOPY_VEC3(coords[numvert],coords[numvert-1]);
         coords[numvert][Z] += 2.0 * VERT_THICK;
         numvert++;

      }  //  End of kx for.
    
     // Get the end point.
     PFCOPY_VEC3(coords[numvert],points[ix+1]);
     coords[numvert][Z] = gnd_level2(points[ix+1]) - VERT_THICK +
        LINE_HEIGHT;
     numvert++;

     PFCOPY_VEC3(coords[numvert],coords[numvert-1]);
     coords[numvert][Z] += 2.0 * VERT_THICK;
     numvert++;

   }  //  End of ix for.

   if(closed){      //  The last leg connecting to start.
      angle = pfArcTan2((points[0][Y] - points[(numpoint-1)][Y]),
         (points[0][X] - points[(numpoint-1)][X]));
      seg_cnt = (int)(PFDISTANCE_PT3(points[(numpoint-1)],points[0])/ 
         PO_MEAS_LENGTH);

      pfSinCos(angle,&msin,&mcos);

      for (kx=0;kx<seg_cnt;kx++ ){
         coords[numvert][X] = mcos * PO_MEAS_LENGTH * kx + points[ix][X];
         coords[numvert][Y] = msin * PO_MEAS_LENGTH * kx + points[ix][Y];
         coords[numvert][Z] = gnd_level2(coords[numvert]) - VERT_THICK +
            LINE_HEIGHT;
         numvert++;

         coords[numvert][X] = coords[numvert-1][X];
         coords[numvert][Y] = coords[numvert-1][Y];
         coords[numvert][Z] = coords[numvert-1][Z] + 2.0 * VERT_THICK ;
         numvert++;
      }  //  End of kx for.

      //  Get the end point which sould be the first point.  Finish off the 
      //  strip.
      PFCOPY_VEC3(coords[numvert],coords[0]);
      numvert++;

      PFCOPY_VEC3(coords[numvert],coords[1]);
      numvert++;

   }  //  End of if.

   //  Make the performer stuff to build the measure.

   gset = pfNewGSet(arena);

   //  Set the attributes.
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, transparent, NULL);
   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, coords, NULL);
   pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);

   pfGSetPrimType(gset, PFGS_TRISTRIPS);        
   pfGSetNumPrims(gset, 1);

   if(NULL==(length_vec = (int *) pfMalloc(sizeof(int),arena))){
      return(NULL);
   }
   length_vec[0] = numvert;

   pfGSetPrimLengths(gset,length_vec);

   // Create a new geostate from shared memory,
   // enable transparency.
   gst = pfNewGState(arena);

   pfGStateMode (gst, PFSTATE_ALPHAFUNC, PFAF_NOTEQUAL);
   pfGStateMode (gst, PFSTATE_ALPHAREF, 0);
   pfGStateMode (gst, PFSTATE_TRANSPARENCY, PFTR_MS_ALPHA | PFTR_NO_OCCLUDE);

   pfGSetGState(gset, gst);

   geode = pfNewGeode();

   pfAddGSet (geode, gset);

   return(geode);

}  //  End of make_vertical.


//  This function is used when dragging and dropping po measures.
//  For moving lines, the point of the line which is picked is saved.
//  Then as the cursor is dragged the line is shifted by an amount
//  equivilent to the difference in the cursors world position
//  and the point picked on the line.  The function builds a temporary
//  version of the line which can be moved in real time.
void PO_LINE_CLASS::movePO(float xpos, float ypos, ushort outToNet)
{ 
   pfGeode* temp;                   //  Temporary representation of line.
   static pfVec3 startpoint;

   if (firstTime){
      startpoint[X] = xpos;
      startpoint[Y] = ypos;
      startpoint[Z] = gnd_level2(startpoint);
      firstTime = 0;
   }
   difference[X] = xpos - startpoint[X];
   difference[Y] = ypos - startpoint[Y];

   //  I didn't make a mistake here, this is an assignment which will return
   //  a null if it fails.  I'm looking to assign temp to the second child
   //  of polys.  If that child doesn't exist I don't want to continue with 
   //  next portion of the conditional. I want it as = NOT == 
   if (temp = (pfGeode*)pfGetChild(polys, 2) ){
      pfRemoveChild(polys, temp);                 //  Out with the old.
      pfDelete(temp);
   }

   temp = makeQuickLine();         //  A line which I can move in real time.
   pfAddChild(polys, temp);        //  In with the new. 

   if (outToNet) dragToNet(xpos, ypos);

}  //  End of movePO.


//  This function sends an update packet to the net which says the line is being
//  dragged.
void PO_LINE_CLASS::dragToNet(float, float){

   const unsigned short DESCRIBE_OBJECT_HEADER_SIZE = 36;

   SP_PersistentObjectPDU *outPDU;

   char *temp;

   int length;

   if(NULL == (outPDU = (SP_PersistentObjectPDU *) pfMalloc(
      sizeof(SP_PersistentObjectPDU), pfGetSharedArena()) )  )
   {
      cerr<<"Error allocating storage in sendPDU."<<endl;
   }

   temp = (char *) &(outPDU->variant.describeObject); 

   outPDU->version = poProtocol;                  //  We should have already
                                                  //  captured this.
   outPDU->kind = SP_describeObjectPDUKind;
   outPDU->exercise = L_exercise;
   outPDU->database = L_database;

   length = PRO_PO_LINE_CLASS_SIZE(numpoint) + DESCRIBE_OBJECT_HEADER_SIZE;
  
   //Dupe ModSAF.  I increase the sequence number while keeping the owner the
   //same.  This way the changes occur but ModSAF will maintain the entities
   outPDU->variant.describeObject.sequenceNumber++; 

   for (int i = 0; i < sizeof(SP_DescribeObjectVariant); i++){
      temp[i] = pdu[i];
   }

   // Since drag, I will only change the location.  I will also use the _unused_8
   // flag to show I'm dragging this entity.
   outPDU->variant.describeObject.variant.line._unused_10 = 1;
   for (i=0; i < numpoint; i++){
      outPDU->variant.describeObject.variant.line.points[i].pointNumber =
         (short) i;
      outPDU->variant.describeObject.variant.line.points[i].pointType =
         (SP_PointType)SP_PTLocation;
      outPDU->variant.describeObject.variant.line.points[i].variant.location.x =
         (long)points[i][X] + difference[X] + POF_nps_easting_shift;
      outPDU->variant.describeObject.variant.line.points[i].variant.location.y =
         (long)points[i][Y] + difference[Y] + POF_nps_northing_shift;
   }

   outPDU->length = length;   

   net_write((char*)outPDU, length); 

}  //  End of dragToNet.


//  This function is called during the "drop" portion of a drag and drop
//  of a po_measure.  It handles any needed action after the mouse button
//  has been released.  It also makes the permanent changes to the po
//  measure.
void PO_LINE_CLASS::dragUpdatePO(float, float, ushort outToNet)
{
//cerr<<"in dragUpdatePO "<<endl;
   firstTime = 1;
   pfGroup* polysParent;

   //  Make the changes to the line's points.
   for (int i = 0; i < numpoint; i++){
      points[i][X] = points[i][X] + difference[X];
      points[i][Y] = points[i][Y] + difference[Y];
      points[i][Z] = gnd_level2(points[i]);
   }

   //  I've made changes to the points, I must now remove the old poly
   //  representation of the measure and replace it with a new rep.
   if (pfGetParent(polys,0) == NULL){
      cerr<<"poly's par is null. error"<<endl;
      exit(0);
   }

   //  Must do this as a flag, in case the measure was highlighted.  I can't
   //  just delete it, because pointers in the highlighting code may still
   //  need the measure.  This is a flag for those functions.
   if (polys == pickedGroup){
      pickedGroup = NULL;
   }

   //  SAK.  remove the old.
   polysParent = pfGetParent(polys,0);
   pfRemoveChild(polysParent,polys);             
   pfDelete(polys);

   //  Build and add the new.
   polys = make_meas();
   pfAddChild(polysParent, polys);

   updateText();

   if (outToNet){
      updatePDU();
      sendPDU();
   }

}  //  End of dragUpdatePO.

void PO_LINE_CLASS::updateText(){

   SP_DescribeObjectVariant* linePDU,
                            *textPDU;

   if (pdu){

      linePDU = (SP_DescribeObjectVariant*)pdu; 

      for (int i=0; i<poCount; i++){
         textPDU = (SP_DescribeObjectVariant*)poArray[i].poPointer->pdu;
         if ((textPDU != NULL) && (textPDU->mclass == SP_objectClassText)){

            if((textPDU->variant.text.associatedObject.simulator.site ==
               linePDU->objectID.simulator.site) && 
               (textPDU->variant.text.associatedObject.simulator.host ==
               linePDU->objectID.simulator.host) &&
               (textPDU->variant.text.associatedObject.object ==
               linePDU->objectID.object) ) {

               poArray[i].poPointer->dragUpdatePO(difference[X],
                  difference[Y], 1); 

            }  
         }

      } // for

   }  //  if (pdu)

}  //  End of updateText.


//  This function is used to update the pdu member describing a po measure, when
//  members of that measure have changed.  The function updates the pdu values
//  with the new member values.
void PO_LINE_CLASS::updatePDU(){

   SP_DescribeObjectVariant *temp;
   
   if (pdu == NULL){
      
      //cerr<<"No pdu exists in updatePDU."<<endl;

      if(NULL == (temp = (SP_DescribeObjectVariant *) pfMalloc(
         sizeof(SP_DescribeObjectVariant), pfGetSharedArena()) )  )
      {
         cerr<<"Error allocating storage in updatePDU"<<endl;
      }
      temp->databaseSequenceNumber = 0;           //  This may be wrong.

      temp->objectID.simulator.site = poSite;
      temp->objectID.simulator.host = poHost;
      temp->objectID.object = object++;

      temp->worldStateID.simulator.site = 0;
      temp->worldStateID.simulator.host = 0;
      temp->worldStateID.object = 0;

      temp->owner.site = slaveSim.site;
      temp->owner.host = slaveSim.host;

      temp->sequenceNumber = 0;

      temp->mclass = SP_objectClassLine;

      temp->missingFromWorldState = 0;

      temp->variant.point.overlayID.simulator.site = OVERLAY_SITE;
      temp->variant.point.overlayID.simulator.host = OVERLAY_HOST;
      temp->variant.point.overlayID.object = OVERLAY_OBJECT;

      pdu = (char*) temp;

   }
   else{
      temp = (SP_DescribeObjectVariant *) pdu;
   }

   //Dupe ModSAF.  I increase the sequence number while keeping the owner the
   //same.  This way the changes occur but ModSAF will maintain the entities
   temp->sequenceNumber++; 
   
   //  Change the fields of the pdu.
   temp->variant.line.style = (SP_LineStyle) style;
   temp->variant.line.color = (SP_OverlayColor) color;
   temp->variant.line.dashed = (unsigned) dashed;
   temp->variant.line.thickness = (unsigned char)thickness;
   temp->variant.line.width = (float)width;
   temp->variant.line.closed = (unsigned)closed;
   temp->variant.line.splined = (unsigned)splined;
   temp->variant.line.beginArrowHead = (SP_ArrowHeadStyle)beginArrowHead;
   temp->variant.line.endArrowHead = (SP_ArrowHeadStyle)endArrowHead;
   temp->variant.line.pointCount = (unsigned char)numpoint;

   //  This unused field is used to send whether or not the measure is
   //  being dragged.
   temp->variant.line._unused_10 = 0;          

   for (int i=0; i < numpoint; i++){
      temp->variant.line.points[i].pointNumber = (short) i;
      temp->variant.line.points[i].pointType = (SP_PointType)SP_PTLocation;
      temp->variant.line.points[i].variant.location.x = (long)points[i][X];
      temp->variant.line.points[i].variant.location.y = (long)points[i][Y];
   }
   
}  //  End of PO_LINE_CLASS::updatePDU.

//  This function outputs the current pdu describing the po measure to the
//  network.  I copy the SP_describeObject stuff via characters using the
//  temporary pointer temp.  poProtocol should already be set elsewhere.
void PO_LINE_CLASS::sendPDU(){

   const unsigned short DESCRIBE_OBJECT_HEADER_SIZE = 36;

   SP_PersistentObjectPDU *outPDU;
   char *temp;
   unsigned short length;
   int i;

   if(NULL == (outPDU = (SP_PersistentObjectPDU *) pfMalloc(
      sizeof(SP_PersistentObjectPDU), pfGetSharedArena()) )  )
   {
      cerr<<"Error allocating storage in sendPDU."<<endl;
   }

   outPDU->version = poProtocol;             //  We should have already
                                             //  captured this.
   outPDU->kind = SP_describeObjectPDUKind;
   outPDU->exercise = L_exercise;
   outPDU->database = L_database;

   length = DESCRIBE_OBJECT_HEADER_SIZE + PRO_PO_LINE_CLASS_SIZE(numpoint);
 
   temp = (char *) &(outPDU->variant.describeObject);  //  Copy as chars.
   
   //  I can't do 
   //  "outPDU->variant.describeObject = (SP_DescribeObjectVariant) pdu;"
   //  outright, so I will copy it instead as characters.  I will also count
   //  the length.
   for (i = 0; i < sizeof(SP_DescribeObjectVariant); i++){
      temp[i] = pdu[i];
   }

   for (i=0; i < numpoint; i++){
      outPDU->variant.describeObject.variant.line.points[i].pointNumber =
         (short) i;
      outPDU->variant.describeObject.variant.line.points[i].pointType =
         (SP_PointType)SP_PTLocation;
      outPDU->variant.describeObject.variant.line.points[i].variant.location.x =
         (long)points[i][X] + POF_nps_easting_shift;
      outPDU->variant.describeObject.variant.line.points[i].variant.location.y =
         (long)points[i][Y] + POF_nps_northing_shift;
   }

   outPDU->length = length;

   net_write((char*)outPDU, length); 

}  //  End of PO_LINE_CLASS::sendPDU.



//  This function creates a lower resolution version of a line which can 
//  be moved in real time.  It's the same basic algorithm,  there are just
//  fewer segemnts in between the line's main points.
pfGeode * PO_LINE_CLASS::makeQuickLine()
{
   const float LINE_HEIGHT = 10.0;

   pfGeoSet *gset;
   pfGeoState *gst;
   pfGeode     *geode; 
   pfVec3 *coords;

   void *arena;
 
   float angle,                     // Angle between line points.
      msin,mcos;                    // Sine and cosine of that angle.

   float Xmiddle, Ymiddle;          //  The increment shift up the centerline.

   int numvert=0,                   //  Vertex number in line.
      num_seg,                      //  Number of line sub segments.
      seg_cnt;                      //  Segment count between points.

   int *length_vec;                //  Used with performer primitives.

   int ix,kx;

   const float COURSE = 10.0;       //  COURSEness of the measure.

   pfVec3 tempPoints[C2_SEGMENTS];

   for (int i = 0; i < numpoint; i++){
      tempPoints[i][X] = points[i][X] + difference[X];
      tempPoints[i][Y] = points[i][Y] + difference[Y];
      tempPoints[i][Z] = gnd_level2(tempPoints[i]);
   }

   //  Get the shared memory.
   arena = pfGetSharedArena();
   
   //  From the points of the line I will chop up the line into a bunch of 
   //  segments.  In this way my line can follow the terrain elevation.
   //  First figure out how many segments there are.
   num_seg = 0;
   for(ix=0;ix<(numpoint-1);ix++){
      //SAK.  Much courser line.
      num_seg += (int)(PFDISTANCE_PT3(tempPoints[ix],
      tempPoints[ix+1])/(PO_MEAS_LENGTH*COURSE))+1;  
   }      //Basically COURSE times less the resolution.

   if(closed){
      num_seg += (int)(PFDISTANCE_PT3(tempPoints[numpoint-1],tempPoints[0])/
         (PO_MEAS_LENGTH * COURSE))+1;                //SAK. Courser line.
   }
 
   //  Now allocate the storage.
   if(NULL==(coords=(pfVec3 *)pfMalloc((num_seg*2+2)*sizeof(pfVec3),arena))){
      return(NULL);
   }
   
   //  See make_meas for line measure.

   PFCOPY_VEC3(coords[0], tempPoints[0]);   //  First point in this sequence
                                            //  is the original point.
   coords[0][Z] =  gnd_level2(coords[0]) + LINE_HEIGHT; 

   numvert++;

   //  Step through each point and generate the segments in between.
   for(ix=0;ix<(numpoint - 1);ix++){

      //  Get the angle between the points.
      angle = pfArcTan2((tempPoints[ix+1][Y] - tempPoints[ix][Y]),
         (tempPoints[ix+1][X] - tempPoints[ix][X]));

      pfSinCos(angle,&msin,&mcos);         //  Used to increment line segs.
      Xmiddle = mcos * PO_MEAS_LENGTH * COURSE;     //  Incremental shift.
      Ymiddle = msin * PO_MEAS_LENGTH * COURSE;
              
      //  The number of sub-segments a line is divided into.  Adjusted for
      //  courseness.
      seg_cnt = (int)(PFDISTANCE_PT3(tempPoints[ix],tempPoints[ix +1])/ 
         (PO_MEAS_LENGTH * COURSE)) ;                 
    
      //  Cycle through all points on centerline, use the point as a vertex.
      for (kx=1;kx<seg_cnt-1;kx++ ){ 
         coords[numvert][X] = coords[numvert -1][X] + Xmiddle;
         coords[numvert][Y] = coords[numvert -1][Y] + Ymiddle;
         coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
         numvert++;
      }  //  kx for loop.

      //Ensure that last point is in fact the next point.  i.e the segments
      //may not reach the last point due to rounding of seg_cnt. 
      coords[numvert][X] = tempPoints[ix+1][X];
      coords[numvert][Y] = tempPoints[ix+1][Y];
      coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
      numvert++; 
   }  //  End of ix for loop.

    

   if (closed){               //  Add the last line back to the start. 
      //  Last point.
      PFCOPY_VEC3(coords[numvert], tempPoints[numpoint-1]);      
      coords[numvert][Z] =  gnd_level2(coords[numvert]) + LINE_HEIGHT; 
      numvert++;

      //  Get the angle between the points.
      angle = pfArcTan2((tempPoints[0][Y] - tempPoints[numpoint-1][Y]),
         (tempPoints[0][X] - tempPoints[numpoint-1][X]));

      pfSinCos(angle,&msin,&mcos);                  //  Signs and cosines for sgmnt.
      Xmiddle = mcos * PO_MEAS_LENGTH * COURSE;     //  Incremental shift.
      Ymiddle = msin * PO_MEAS_LENGTH * COURSE;       

      //  The number of sub-segments a line is divided into.  Adjusted for
      //  courseness.
      seg_cnt = (int)(PFDISTANCE_PT3(tempPoints[numpoint-1],tempPoints[0])/ 
         (PO_MEAS_LENGTH * COURSE));                 
    

      //  Cycle through all points on centerline, make the shift and assign
      //  to the vector.  
      for (kx=1;kx<seg_cnt-1;kx++ ){ 
         coords[numvert][X] = coords[numvert - 1][X] + Xmiddle;
         coords[numvert][Y] = coords[numvert - 1][Y] + Ymiddle;
         coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
         numvert++;
      }  //  End of kx for loop.
 
      //Ensure it's connected.
      coords[numvert][X] = tempPoints[0][X];
      coords[numvert][Y] = tempPoints[0][Y];
      coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
      numvert++; 
   }  //  End of if (closed).

   // Build performer stuff.
  
   gset = pfNewGSet(arena);

   //  Set the primitives.
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, L_color[color], NULL);
   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, coords, NULL);
   pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);

   pfGSetPrimType(gset, PFGS_FLAT_LINESTRIPS);   
   pfGSetNumPrims(gset, 1);

   if(NULL==(length_vec = (int *) pfMalloc(sizeof(int),arena))){
     return(NULL);
   }
   length_vec[0] = numvert;

   pfGSetPrimLengths(gset,length_vec);
   pfGSetLineWidth(gset, thickness);           //  Numbers of pixels wide my
                                               //  Line is.

   // Create a new geostate from shared memory,
  
   gst = pfNewGState(arena);

   pfGStateMode(gst, PFSTATE_ENTEXTURE, 0);
   pfGStateMode(gst, PFSTATE_TRANSPARENCY, 1);

   pfGSetGState(gset, gst);

   geode = pfNewGeode();
   pfAddGSet (geode, gset);
 
   return(geode);
}  //  End of makeQuickLine.


void PO_LINE_CLASS::setPoint(int index, pfVec3 value){
   if ( (index >= 0) && (index < numpoint)){

      points[index][X] = value[X];
      points[index][Y] = value[Y];
      points[index][Z] = gnd_level2(points[index]);

   }
}  //  End of setPoint.



/*********************** PO_TEXT_CLASS      *************************/

PO_TEXT_CLASS :: PO_TEXT_CLASS (PO_TYPE, int)
: PO_MEASURES_CLASS ()
{
  type = PO_TEXT;
}

//SAK
PO_TEXT_CLASS  :: PO_TEXT_CLASS(SP_DescribeObjectVariant describeObject,
   int length): PO_MEASURES_CLASS(describeObject, length)
{
   type = PO_TEXT;
   size = (SP_TextSize) describeObject.variant.text.size;
   color = (PO_COLOR) describeObject.variant.text.color;
   length = (short) describeObject.variant.text.length;
   alignment = (SP_TextAlignment) describeObject.variant.text.alignment;
   horizontalOffset = (short) describeObject.variant.text.horizontalOffset;
   verticalOffset = (short) describeObject.variant.text.verticalOffset;
   associatedObject = (SP_ObjectID) describeObject.variant.text.associatedObject;
   associatedPointNumber = 
      (short) describeObject.variant.text.associatedPointNumber;

   for (int x=0; x<length; x++)
      text[x] = (char) describeObject.variant.text.text[x];

   points[0][X] = (float) describeObject.variant.text.location.x - 
                  POF_nps_easting_shift;
   points[0][Y] = (float) describeObject.variant.text.location.y -
                   POF_nps_northing_shift;
   points[0][Z] = gnd_level2(points[0]) + TEXT_OFFSET;

   polys = make_meas();
}  //  End of PO_TEXT_CLASS constructor.


// SAK.
void PO_TEXT_CLASS::update(SP_DescribeObjectVariant describeObject, int length){

   char    *temp;

   if (pfGetParent(polys,0) == NULL){
      cerr<<"poly's par is null. error"<<endl;
      exit(0);
   }

   pfRemoveChild(pfGetParent(polys,0),polys);        //SAK.  remove the old.

  
   //  Delete old from memory.
   pfDelete(polys);
   //delete []pdu;                                     //SAK. Prevent mem leak.
   pfFree(pdu);

   //  Add the new.  I will convert the pdu into a string.  There may be 
   //  something in string.h which may do this easier but 
   //  the below routine just does it.  It makes the string the right size
   //  and ensures it terminates with the '\0'.

   temp = (char *) &describeObject;
   pdu = new char[length + 1];
   for (int i = 0; i < length; i++){
      pdu[i] = temp[i];
   }

   pdu[length] = '\0';

   type = PO_TEXT;
   size = (SP_TextSize) describeObject.variant.text.size;
   color = (PO_COLOR) describeObject.variant.text.color;
   length = (short) describeObject.variant.text.length;
   alignment = (SP_TextAlignment) describeObject.variant.text.alignment;
   horizontalOffset = (short) describeObject.variant.text.horizontalOffset;
   verticalOffset = (short) describeObject.variant.text.verticalOffset;
   associatedObject = (SP_ObjectID) describeObject.variant.text.associatedObject;
   associatedPointNumber =
      (short) describeObject.variant.text.associatedPointNumber;

   for (int x=0; x<length; x++)
      text[x] = (char) describeObject.variant.text.text[x];

   points[0][X] = (float) describeObject.variant.text.location.x - 
                  POF_nps_easting_shift;
   points[0][Y] = (float) describeObject.variant.text.location.y -
                  POF_nps_northing_shift;
   points[0][Z] = gnd_level2(points[0]) + TEXT_OFFSET;

   polys = make_meas();

}  //  End of update.
      
  
//  Both the minefiedl and the point have enbeded text, this helps to build 
//  a text node.
void PO_TEXT_CLASS::create_text_node(pfVec3 pos,PO_COLOR tcolor,char *letters)
{
   pfCopyVec3(points[0],pos);
   //points[0][Z] += PO_MEAS_SIZE + 3.0f;
   strcpy(text,letters);
   color = tcolor;
}
  
  


//  Text is fun, here is what we do, we define a predraw node call back that 
//  prints out the selected text from the the array of text messages available.
//  The function returns a prune so nothing below is rendered
//  sounds simple huh?
pfGroup * PO_TEXT_CLASS::make_meas()
{
   pfGroup *the_group;
   pfGeoSet *gset;
   pfGeode *geode;
   pfVec3 *coords;

   void *arena;

   //Allocate all the geometry needed, since we have to cull the point we make a
   //simple point to represnt the locaction to ensure that it gets put in the
   //right spot but we are never going to draw it.

   //  Get the shared memory.
   arena = pfGetSharedArena();
   gset = pfNewGSet(arena);

   if(NULL==(coords=(pfVec3 *)pfMalloc(sizeof(pfVec3),arena))){
      return(NULL);
   }

   pfCopyVec3(coords[0],points[0]);

   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, coords, NULL);
   pfGSetPrimType(gset, PFGS_POINTS);
   pfGSetNumPrims(gset, 1);

   // Add it to the group.
   geode = pfNewGeode();
   pfAddGSet (geode, gset);
   the_group = pfNewGroup();
   pfAddChild(the_group,geode);

   // If this is the first text string allocate space.
   if(L_textblock == NULL){
      //  Set the counter.
      L_textcnt = 0;
      // First text block, allocate some memory.
      if(NULL==(L_textblock=
         (TEXTBLOCK *) pfMalloc(sizeof(TEXTBLOCK)*BLOCK_ALLOC,arena))){
         return(NULL);
      }

      L_numblocks = BLOCK_ALLOC;
    
   }
   else{
      // If there is no more space, allocate some more.
      if(L_numblocks == L_textcnt){
         //we need somemore blocks
         L_numblocks += BLOCK_ALLOC;
         if(NULL==(L_textblock=
            (TEXTBLOCK*)pfRealloc(L_textblock,sizeof(TEXTBLOCK)*L_numblocks))){
          return(NULL);
         }
      }
   }  //  End of else.
  
   // Build the record out of shared memory.
   L_textblock[L_textcnt].inview = FALSE;
   pfCopyVec3(L_textblock[L_textcnt].location,points[0]);

   //Cpack is faster at runtime, so do the conversion.
   L_textblock[L_textcnt].c.comp[0] = 0xFF;
   L_textblock[L_textcnt].c.comp[1] = (unsigned char) (255*L_color[color][2]);
   L_textblock[L_textcnt].c.comp[2] = (unsigned char) (255*L_color[color][1]);
   L_textblock[L_textcnt].c.comp[3] = (unsigned char) (255*L_color[color][0]);

   strcpy(L_textblock[L_textcnt].text,text);
  
   // Attach the callback geode.
   pfNodeTravFuncs(the_group,PFTRAV_DRAW,NULL,place_text);

   // The data is the index into the array.  
   pfNodeTravData(the_group,PFTRAV_DRAW,(void *)&(L_textblock[L_textcnt]));

   L_textcnt++;

   return(the_group);  
}  //  End of make_meas for PO_TEXT_CLASS.


/*
pfGroup * PO_TEXT_CLASS::make_meas()
//construct the measure
{
  pfGeoState  *gstate;
  pfGeoSet   *gset;
  pfBillboard *board;
  PO_TEXT_CLASS *textnode;
  void *arena;

  char textname[255] = "~kirbysa/A.rgb";

  arena = pfGetSharedArena();

  L_po_point_tex[0] = pfNewTex (arena);
cerr<<"image success "<< pfLoadTexFile (L_po_point_tex[0],textname) <<endl;
  L_po_tex_env = pfNewTEnv (arena);
  pfTEnvMode(L_po_tex_env,PFTE_MODULATE);

  pfGroup *the_group;

  //make a textured panel
  gstate = pfNewGState (pfGetSharedArena());
  pfGStateMode (gstate, PFSTATE_ENTEXTURE, 1);
  pfGStateAttr (gstate, PFSTATE_TEXENV, L_po_tex_env);
  pfGStateAttr (gstate, PFSTATE_TEXTURE, L_po_point_tex[0]);
  
  pfGStateMode (gstate, PFSTATE_ALPHAFUNC, PFAF_NOTEQUAL);
  pfGStateMode (gstate, PFSTATE_ALPHAREF, 0);


  gset = pfNewGSet(pfGetSharedArena());
  pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, L_verts, NULL);
  pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_PER_VERTEX, L_texcoords, NULL);
  pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, L_color[color], NULL);
  pfGSetPrimType(gset, PFGS_QUADS);
  pfGSetNumPrims(gset, 1);

  pfGSetGState (gset, gstate);

  //add it to a pfBillboard
  board = pfNewBboard();
  pfAddGSet(board,gset);
  pfBboardPos(board,0,points[0]);

  the_group = pfNewGroup();
  pfAddChild(the_group,board);

  return(the_group);
}

*/

void PO_TEXT_CLASS::dragUpdatePO(float diffX, float diffY, ushort outToNet)
{
   pfGroup* polysParent;
  
   points[0][X] = points[0][X] + diffX;
   points[0][Y] = points[0][Y] + diffY;
   points[0][Z] = gnd_level2(points[0]) + LINE_HEIGHT;

   //  I've made changes to the points, I must now remove the old poly
   //  representation of the measure and replace it with a new rep.
   if (pfGetParent(polys,0) == NULL){
      cerr<<"poly's par is null. error"<<endl;
      exit(0);
   }

   //  Must do this as a flag, in case the measure was highlighted.  I can't
   //  just delete it, because pointers in the highlighting code may still
   //  need the measure.  This is a flag for those functions.
   if (polys == pickedGroup){
      pickedGroup = NULL;
   }

   //  SAK.  remove the old.
   polysParent = pfGetParent(polys,0);
   pfRemoveChild(polysParent,polys);             
   pfDelete(polys);

   //  Build and add the new.
   polys = make_meas();
   pfAddChild(polysParent, polys);

   if (outToNet){
      updatePDU();
      sendPDU();
   }

}  //  End of dragUpdatePO.



//  This function is used to update the pdu member describing a po measure, when
//  members of that measure have changed.  The function updates the pdu values
//  with the new member values.
void PO_TEXT_CLASS::updatePDU(){

   SP_DescribeObjectVariant *temp;
   
   if (pdu == NULL){
      
      //cerr<<"No pdu exists in updatePDU."<<endl;

      if(NULL == (temp = (SP_DescribeObjectVariant *) pfMalloc(
         sizeof(SP_DescribeObjectVariant), pfGetSharedArena()) )  )
      {
         cerr<<"Error allocating storage in updatePDU"<<endl;
      }
      temp->databaseSequenceNumber = 0;           //  This may be wrong.

      temp->objectID.simulator.site = poSite;
      temp->objectID.simulator.host = poHost;
      temp->objectID.object = object++;

      temp->worldStateID.simulator.site = 0;
      temp->worldStateID.simulator.host = 0;
      temp->worldStateID.object = 0;

      temp->owner.site = slaveSim.site;
      temp->owner.host = slaveSim.host;

      temp->sequenceNumber = 0;

      temp->mclass = SP_objectClassText;

      temp->missingFromWorldState = 0;

      temp->variant.text.overlayID.simulator.site = OVERLAY_SITE;
      temp->variant.text.overlayID.simulator.host = OVERLAY_HOST;
      temp->variant.text.overlayID.object = OVERLAY_OBJECT;

      pdu = (char*) temp;

   }
   else{
      temp = (SP_DescribeObjectVariant *) pdu;
   }

   //Dupe ModSAF.  I increase the sequence number while keeping the owner the
   //same.  This way the changes occur but ModSAF will maintain the entities
   temp->sequenceNumber++; 
   
   temp->variant.text.location.x = (long)points[0][X];
   temp->variant.text.location.y = (long)points[0][Y];
  
   
}  //  End of PO_TEXT_CLASS::updatePDU.

//  This function outputs the current pdu describing the po measure to the
//  network.  I copy the SP_describeObject stuff via characters using the
//  temporary pointer temp.  poProtocol should already be set elsewhere.
void PO_TEXT_CLASS::sendPDU(){

   const unsigned short DESCRIBE_OBJECT_HEADER_SIZE = 36;

   SP_PersistentObjectPDU *outPDU;
   SP_DescribeObjectVariant *tempPDU;
   char *temp;
   unsigned short length;

   tempPDU = (SP_DescribeObjectVariant*)pdu;

   if(NULL == (outPDU = (SP_PersistentObjectPDU *) pfMalloc(
      sizeof(SP_PersistentObjectPDU), pfGetSharedArena()) )  )
   {
      cerr<<"Error allocating storage in sendPDU."<<endl;
   }

   outPDU->version = poProtocol;             //  We should have already
                                             //  captured this.
   outPDU->kind = SP_describeObjectPDUKind;
   outPDU->exercise = L_exercise;
   outPDU->database = L_database;

   length = DESCRIBE_OBJECT_HEADER_SIZE +
      PRO_PO_TEXT_CLASS_SIZE(tempPDU->variant.text.length);
 
   temp = (char *) &(outPDU->variant.describeObject);  //  Copy as chars.
   
   //  I can't do 
   //  "outPDU->variant.describeObject = (SP_DescribeObjectVariant) pdu;"
   //  outright, so I will copy it instead as characters.  I will also count
   //  the length.
   for (int i = 0; i < sizeof(SP_DescribeObjectVariant); i++){
      temp[i] = pdu[i];
   }

   outPDU->variant.describeObject.variant.text.location.x = 
      (long) points[0][X] + POF_nps_easting_shift;
   outPDU->variant.describeObject.variant.text.location.y = 
      (long) points[0][Y] + POF_nps_northing_shift;

   outPDU->length = length;

   net_write((char*)outPDU, length); 

}  //  End of PO_TEXT_CLASS::sendPDU.




/*********************** PO_POINT_CLASS      *************************/

//  This constructor builds the measure from the pdu received.
PO_POINT_CLASS  :: PO_POINT_CLASS(SP_DescribeObjectVariant describeObject,
   int length): PO_MEASURES_CLASS(describeObject, length)
{
   type = PO_POINT;
   firstTime = 1;

   //  Take information from pdu to build measure.
   style = (ushort) describeObject.variant.point.style;
   color = (PO_COLOR) describeObject.variant.point.color;
   dashed = (ushort) describeObject.variant.point.dashed;
   direction = (uint) describeObject.variant.point.direction;
   
   for (int x=0; x < SP_maxPointNameLength; x++)
      text[x] = (char) describeObject.variant.point.text[x];

   points[0][X] = (float) describeObject.variant.point.location.x -
                  POF_nps_easting_shift;
   points[0][Y] = (float) describeObject.variant.point.location.y -
                  POF_nps_northing_shift;
   points[0][Z] = gnd_level2(points[0]);

   polys = make_meas();

}  //  End of PO_POINT_CLASS constructor.


//  This constructor enables the construction of a point without having
//  received a PDU.  It receives the points member values as parameters
//  rather than by pdu.
PO_POINT_CLASS  :: PO_POINT_CLASS(ushort Style, PO_COLOR Color,
   ushort Dashed, uint Direction, char * Text, pfVec3 Point)
   : PO_MEASURES_CLASS()
{
   char dummy[] = {'\0'};

   type = PO_POINT;
   firstTime = 1;

   style = (ushort)Style;
   color = Color;
   dashed = Dashed;
   direction = Direction;
   if (!Text) Text=dummy;

   strcpy(text, Text); 
   //for (int x=0; x < strlen(Text); x++) text[x] = (char) Text[x];
   //text[x] = '\0';

   pfCopyVec3(points[0],Point);
 
   polys = make_meas();

}  //  End of PO_POINT_CLASS constructor.

//  This constructor enables the construction of a point without having
//  received a PDU.  It differ's from the above function in that it
//  takes a file name as a parameter to build the measure from.  It also
//  doesn't have nor use many of the classes members.  It is used to build
//  a point with the drag and drop functionality but won't use much of
//  the other features of the point class.  This can be used for a point
//  which isn't a normal po measure.
PO_POINT_CLASS  :: PO_POINT_CLASS(pfVec4 color, const char* fileName, pfVec3
   point): PO_MEASURES_CLASS()
{ 
   type = PO_NO_TYPE;

   pfCopyVec3(points[0],point);
 
   polys = makePickableTriStrip(color, fileName, point);   //  This function is
                                                           //  in menu_funcs.h
//cerr<<"leaving point constructor"<<endl;

}

//  SAK.  Like the other classes this constructor is basically the same as the 
//  constructor except it doesn't generate a new measure.  It will remove and
//  delete the old performer stuff and build new performer stuff to put on.
void PO_POINT_CLASS::update(SP_DescribeObjectVariant describeObject, int length)
{
   char    *temp;

   if (pfGetParent(polys,0) == NULL){
      cerr<<"poly's par is null. error"<<endl;
      exit(0);
   }

   //  pickedGroup is used as a flag so if a highlighted measure is updated
   //  the flag will tell highlighting functions the highlight is valid 
   //  anymore.  Otherwise, since updates delete the old performer stuff for
   //  the measure, highlight functions may have hanging pointers.
   //if (polys == pickedGroup){
   //   pickedGroup = NULL;
   //}

   pfRemoveChild(pfGetParent(polys,0),polys);         //  SAK.  remove the old.
  
   //  I check to see if the group is highlighted, i.e. being dragged.  If so
   //  I ignore the update.  Basically update pdus don't interupt a drag.  Last
   //  drag wins.
   if (polys != pickedGroup){    //  Do the update.
      
      //  If I'm dragging the point, all I want to do is move it.
      //  This unused field is used to send whether or not the measure is
      //  being dragged.
      if (describeObject.variant.point._unused_8 == 1){

         pfSwitch* ptSwitch;
         pfBillboard* board, *board2;
         pfDCS* DCS;

         points[0][X] = describeObject.variant.point.location.x;
         points[0][Y] = describeObject.variant.point.location.y;
         points[0][Z] = gnd_level2(points[0]);

         ptSwitch = (pfSwitch*)pfGetChild(polys, 0);
         pfSwitchVal(ptSwitch, 0);

         board = (pfBillboard*)pfGetChild(ptSwitch, 0);
         board2 = (pfBillboard*)pfGetChild(ptSwitch, 1);

         //  Move DCS and billboards.
         DCS = (pfDCS*)pfGetChild(polys, 1);
         pfDCSTrans(DCS, points[0][X], points[0][Y], points[0][Z]);

         pfBboardPos(board,0,points[0]);
         pfBboardPos(board2,0,points[0]);

         updatePDU();
   
      }

      else{   //  Otherwise I may have to change many things.

         //  Delete it from memory.
         pfDelete(polys);
         //delete []pdu;                             //  SAK. Prevent mem leak.
         pfFree(pdu);

         type = PO_POINT;
         firstTime = 1;                            // Flag.

         //  Add the new.
         //  I will convert the pdu into a string.   It makes the
         //  string the right size and ensures it terminates with the '\0'.
cerr<<"Length:"<<length<<endl;
         temp = (char *) &describeObject;
         pdu = (char *)pfMalloc((length+1)*sizeof(char),pfGetSharedArena());
         //pdu = new char[length + 1];

         for (int i = 0; i < length; i++){
            pdu[i] = temp[i];
         }
         pdu[length] = '\0';
   
         style = (ushort) describeObject.variant.point.style;
         color = (PO_COLOR) describeObject.variant.point.color;
         dashed = (ushort) describeObject.variant.point.dashed;
         direction = (uint) describeObject.variant.point.direction;
   
         for (int x=0; x < SP_maxPointNameLength; x++)
            text[x] = (char) describeObject.variant.point.text[x];

         points[0][X] = (float) describeObject.variant.point.location.x -
                        POF_nps_easting_shift;
         points[0][Y] = (float) describeObject.variant.point.location.y -
                        POF_nps_northing_shift;
         points[0][Z] = gnd_level2(points[0]);

         polys = make_meas();
      }  //  End of else.
   }  //  End of if, (polys != pickedGroup).  
} //  End of update for PO_POINT_CLASS.
      

//  For points we make a textured billbaord the will rotate to face us.
//  if there is any text, we place the text above the marker.  The points
//  we make are relatively simple so a simple file holds the geometry. 
//  According to which type of point is made we make the point with a 
//  different file.  Ideally, this should be changed and made more object
//  oriented rather than using the below switch statement.
pfGroup * PO_POINT_CLASS::make_meas()
{
   pfGroup *the_group;

   switch (style){
      case SP_PSgeneral:
         the_group = make_pt("general.lst");
         break;
      case SP_PScoordinating:
         the_group = make_pt("coord.lst");
         break;
      case SP_PScontact:
         the_group = make_pt("contact.lst");
         break;
      default:
         the_group = make_pt("other.lst");
         cerr << "Unsupported point encountered." << endl;
         break;
   }
   return(the_group);
}  //  End of make_meas for PO_POINT_CLASS.



//  For points we make a linestrip using a file for the point locations.  I
//  wish it could be that simple.  This function puts the points on a billboard
//  which is what we desire.  But performer 1.2 has a bug which prevents
//  picking through a billboard.  So a transparent backboard is added which
//  provides the geometry to pick.  We put them together in a group the go
//  up the group to find what we highlight.  But it's not even that simple.
//  we can't use performer's highlight with a billboard, so we use two geosets
//  one in the highlight color, one not.  Then we pick with a switch.  Its
//  overly complicated, but we had the luck of working where several performer
//  1.2 bugs merged.  New version of performer this will get much prettier.
pfGroup * PO_POINT_CLASS::make_pt(const char* infileName){

   pfGeoState  *gstate, *gstate2;
   pfGeoSet   *gset, *gset2, *gset3;
   pfBillboard *board,*board2;
   pfGeode *geode;
   pfDCS *DCS;
   pfSwitch *ptSwitch;
   pfGroup *the_group;
   pfVec3 *pointVerts, *pointVerts2;

   static pfVec3 axis = {0.0, 0.0, 1.0};
   static pfVec4 transparent = {0.0, 0.0, 0.0, 0.0};
   static pfVec4 HLColor = {1.0, 0.0, 1.0, 1.0};

   //PO_TEXT_CLASS *textnode;

   int numPrims, numPrims2, numVertices = 0, numVertices2 = 0;

   FILE *infile;
 
   int *length_vec, *length_vec2;

   char fileName[PF_MAXSTRING];
   char quadfileName[PF_MAXSTRING];

   pfFindFile ( (char *)infileName, fileName, R_OK );

   //  Open the file which contains the geometry of the point.  The file
   //  consists of the number of primitives, the number of points in each
   //  and the actual points.
   if(NULL == (infile =  fopen( fileName, "r"))){
      cerr<<"Unable to open "<<fileName<<endl;
      return(FALSE);
   }

   //  Get the number of primitives.  Each primitive can be thought of as
   //  a connected strip.  Having variable number of primitives allows
   //  me to "lift my pencil".  The modeling is extremely simple but nice
   //  for the uses here.  These numbers are used in the construction of
   //  performer primitives.
   fscanf(infile, "%d", &numPrims);


   if(NULL == (length_vec = (int *) pfMalloc(sizeof(int)*numPrims,
      pfGetSharedArena() ))){ return (NULL);
   }

   //  Get the number of vertices for each primitive and sums the total number
   //  of vertices.
   for (int i = 0; i < numPrims; i++){
      fscanf(infile, "%d", &(length_vec[i]) );
      numVertices += length_vec[i];
   }

   pointVerts = (pfVec3*) pfMalloc(( (numVertices) * sizeof(pfVec3) ),
      pfGetSharedArena() );

   //  Get the actual vertices.
   for (i = 0; i < numVertices; i++){
      fscanf(infile, "%f %f %f", &(pointVerts[i][X]), &(pointVerts[i][Y]),
         &(pointVerts[i][Z]) );
   }

   //  Build the primitives, we must build the point in two colors, and a
   //  backboard.

   fclose(infile);

   gstate = pfNewGState (pfGetSharedArena());
   pfGStateMode (gstate, PFSTATE_ENTEXTURE, 0);
   pfGStateMode(gstate, PFSTATE_TRANSPARENCY, 1);
   //  Block pixels of 0 alpha for transparent textures. 
   pfGStateMode (gstate, PFSTATE_ALPHAFUNC, PFAF_NOTEQUAL);
   pfGStateMode (gstate, PFSTATE_ALPHAREF, 0);
  
   //  Normal colored geoset.
   gset = pfNewGSet(pfGetSharedArena());
   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts, NULL);
   pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, L_color[color], NULL);
   //pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OVERALL, axis, NULL);
   pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);  

   pfGSetPrimType(gset, PFGS_LINESTRIPS);
   pfGSetNumPrims(gset, numPrims);
   pfGSetPrimLengths(gset, length_vec); 
   pfGSetLineWidth(gset, 3.0);
   pfGSetGState (gset, gstate);

   //  The highlighted colored geoset.
   gset3 = pfNewGSet(pfGetSharedArena());
   pfGSetAttr(gset3, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts, NULL);
   pfGSetAttr(gset3, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset3, PFGS_COLOR4, PFGS_OVERALL, HLColor, NULL);
   //pfGSetAttr(gset3, PFGS_NORMAL3, PFGS_OVERALL, axis, NULL);
   pfGSetAttr(gset3, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);  

   pfGSetPrimType(gset3, PFGS_LINESTRIPS);
   pfGSetNumPrims(gset3, numPrims); 
   pfGSetPrimLengths(gset3, length_vec);    
   pfGSetLineWidth(gset3, 3.0);
   pfGSetGState (gset3, gstate);

   //  Get the points for the backboard quad.  It's just a rectangle.
   //  But the functionality allows any shape to be read in.
   pfFindFile ( "quad.lst", quadfileName, R_OK );
   if(NULL == (infile =  fopen( quadfileName, "r"))){
      printf("Unable to open the file quad.lst");
      return(FALSE);
   }
   fscanf(infile, "%d", &numPrims2);

   if(NULL == (length_vec2 = (int *) pfMalloc(sizeof(int)*numPrims2,
      pfGetSharedArena() ))){ return (NULL);
   }

   for (i = 0; i < numPrims2; i++){
      fscanf(infile, "%d", &(length_vec2[i]) );
      numVertices2 += length_vec2[i];
   }

   pointVerts2 = (pfVec3*) pfMalloc(( (numVertices2) * sizeof(pfVec3) ),
      pfGetSharedArena() );

   for (i = 0; i < numVertices2; i++){
      fscanf(infile, "%f %f %f", &(pointVerts2[i][X]), &(pointVerts2[i][Y]),
         &(pointVerts2[i][Z]) );
   }

   fclose(infile);

   //  Build the geostate for the quad.
   gstate2 = pfNewGState (pfGetSharedArena());
   pfGStateMode (gstate2, PFSTATE_TRANSPARENCY, PFTR_MS_ALPHA | PFTR_NO_OCCLUDE);
   pfGStateMode (gstate2, PFSTATE_ENTEXTURE, 0);
   // Alpha function to block pixels of 0 alpha for transparent textures. 
   pfGStateMode (gstate2, PFSTATE_ALPHAFUNC, PFAF_NOTEQUAL);
   pfGStateMode (gstate2, PFSTATE_ALPHAREF, 0);
   
   //  Make the geoset.
   gset2 = pfNewGSet(pfGetSharedArena());
   pfGSetAttr(gset2, PFGS_COORD3, PFGS_PER_VERTEX, pointVerts2, NULL);
   pfGSetAttr(gset2, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   //pfGSetAttr(gset2, PFGS_NORMAL3, PFGS_OVERALL, normal, NULL);
   pfGSetAttr(gset2, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset2, PFGS_COLOR4, PFGS_OVERALL, transparent, NULL);
 
   pfGSetPrimType(gset2, PFGS_QUADS);
   pfGSetNumPrims(gset2, numPrims2);
   pfGSetPrimLengths(gset2, length_vec2);       //HARDWIRED BAD!!! JUST FOR TEST!! 
   pfGSetGState (gset2, gstate2);

   //  Make billboards, we need two, one for each color of point.
   board = pfNewBboard();
   board2 = pfNewBboard();

   pfBboardAxis(board, axis);
   pfBboardMode(board, PFBB_ROT, PFBB_AXIAL_ROT);
   pfAddGSet(board,gset);

   pfBboardAxis(board2, axis);
   pfBboardMode(board2, PFBB_ROT, PFBB_AXIAL_ROT);
   pfAddGSet(board2,gset3);

   //  Make geode for the backboard.
   geode = pfNewGeode();
   pfAddGSet(geode,gset2);

   //  Need DCS to place the backboard correctly.
   DCS = pfNewDCS();
   pfAddChild(DCS, geode);

   //  Put everything in its correct position.
   pfBboardPos(board,0,points[0]);
   pfBboardPos(board2,0,points[0]);
   pfDCSTrans(DCS, points[0][X], points[0][Y], points[0][Z]);

   //  We need a switch to choose will geoset (highlighted or not) we are
   //  displaying.
   ptSwitch = pfNewSwitch();
   pfAddChild(ptSwitch,board);
   pfAddChild(ptSwitch,board2);
   pfSwitchVal( ptSwitch, 0);

   //  The overall group containing all this mess.  Add the visible portion and 
   //  the backboard for picking.
   the_group = pfNewGroup();
   pfAddChild(the_group,ptSwitch);
   pfAddChild(the_group, DCS);
   

int textLength = strlen(text);
float xoffset = -(textLength * 0.1);
pfVec3 textOffset = {xoffset, 0.0, 3.5};

   //  If there is any text, make a text node out of it.
   if(text[0] != '&'){
      textnode = new PO_TEXT_CLASS (PO_TEXT,1);
      //textnode->create_text_node(points[0],color,text);
      textnode->create_text_node(textOffset,color,text);

   //   THIS IS A POTENTIAL MEMORY LEAK ******************
      //pfAddChild(the_group,textnode->make_meas());
      pfAddChild(DCS,textnode->make_meas());
   }

   //  Name what we need for picking.
   pfNodeName(ptSwitch, "point_switch");
   pfNodeName(the_group, "picked_group");

   return(the_group);
  
}  //  End of make_pt.


//  This function is used for dragging a point measure.  Since we already
//  have two versions of the point (highlighted and not) we will move one
//  temporarily and keep the other stationary but display both.
void PO_POINT_CLASS::movePO(float xpos, float ypos, ushort outToNet)
{
   pfSwitch* ptSwitch;
   pfBillboard* board;
   pfVec3 tempPoints;

   tempPoints[X] = xpos;
   tempPoints[Y] = ypos;
   tempPoints[Z] = gnd_level2(tempPoints);

   ptSwitch = (pfSwitch*)pfGetChild(polys, 0);
   pfSwitchVal(ptSwitch, PFSWITCH_ON);
   board = (pfBillboard*)pfGetChild(ptSwitch, 1);

   //  Move the highlighted version with the mouse world coordinates.
   pfBboardPos(board,0,tempPoints);

   //  Send pdus telliing object is being dragged.
   if (outToNet)dragToNet(xpos, ypos);
  
}  //  End of movePO.


void PO_POINT_CLASS::dragToNet(float xpos, float ypos){

   const unsigned short DESCRIBE_OBJECT_HEADER_SIZE = 36;

   SP_PersistentObjectPDU *outPDU;

   char *temp;

   int length;

   if(NULL == (outPDU = (SP_PersistentObjectPDU *) pfMalloc(
      sizeof(SP_PersistentObjectPDU), pfGetSharedArena()) )  )
   {
      cerr<<"Error allocating storage in sendPDU."<<endl;
   }

   temp = (char *) &(outPDU->variant.describeObject); 

   outPDU->version = poProtocol;                  //  We should have already
                                                  //  captured this.
   outPDU->kind = SP_describeObjectPDUKind;
   outPDU->exercise = L_exercise;
   outPDU->database = L_database;

   length = PRO_PO_POINT_CLASS_SIZE + DESCRIBE_OBJECT_HEADER_SIZE;
  
   //Dupe ModSAF.  I increase the sequence number while keeping the owner the
   //same.  This way the changes occur but ModSAF will maintain the entities
   outPDU->variant.describeObject.sequenceNumber++; 

   for (int i = 0; i < sizeof(SP_DescribeObjectVariant); i++){
      temp[i] = pdu[i];
   }

   // Since drag, I will only change the location.  I will also use the _unused_8
   // flag to show I'm dragging this entity.
   outPDU->variant.describeObject.variant.point._unused_8 = 1;
   outPDU->variant.describeObject.variant.point.location.x = (long) xpos +
      POF_nps_easting_shift;
   outPDU->variant.describeObject.variant.point.location.y = (long) ypos +
      POF_nps_northing_shift;

   outPDU->length = length;   

   net_write((char*)outPDU, length); 

}  //  End of dragToNet.


//  This is the next portion of the drag and drop for a point measure.  This
//  function does what is needed after the point is dropped.  The switch is
//  adjusted to display only the normal color and the backboard DCS is modified
//  to the correct position.
void PO_POINT_CLASS::dragUpdatePO(float xpos, float ypos, ushort outToNet)
{
   pfSwitch* ptSwitch;
   pfBillboard* board, *board2;
   pfDCS* DCS;

   points[0][X] = xpos;
   points[0][Y] = ypos;
   points[0][Z] = gnd_level2(points[0]);

   ptSwitch = (pfSwitch*)pfGetChild(polys, 0);
   pfSwitchVal(ptSwitch, 0);

   board = (pfBillboard*)pfGetChild(ptSwitch, 0);
   board2 = (pfBillboard*)pfGetChild(ptSwitch, 1);

   //  Move DCS and billboards.
   DCS = (pfDCS*)pfGetChild(polys, 1);
   pfDCSTrans(DCS, points[0][X], points[0][Y], points[0][Z]);

   pfBboardPos(board,0,points[0]);
   pfBboardPos(board2,0,points[0]);

   if (outToNet){
      updatePDU();
      sendPDU();
   }
 
//cerr<<"leaving dragUpdatePO"<<endl;
}  //  End of dragUpdatePO.


//  This function is used to update the pdu member describing a po measure, when
//  members of that measure have changed.  The function updates the pdu values
//  with the new member values.
void PO_POINT_CLASS::updatePDU(){

   SP_DescribeObjectVariant *temp;
   
   if (pdu == NULL){
      //cerr<<"No pdu exists in updatePDU."<<endl;

      if(NULL == (temp = (SP_DescribeObjectVariant *) pfMalloc(
         sizeof(SP_DescribeObjectVariant), pfGetSharedArena()) )  )
      {
         cerr<<"Error allocating storage in updatePDU"<<endl;
      }
      temp->databaseSequenceNumber = 0;           //  This may be wrong.

      temp->objectID.simulator.site = poSite;
      temp->objectID.simulator.host = poHost;
      temp->objectID.object = object++;

      temp->worldStateID.simulator.site = 0;
      temp->worldStateID.simulator.host = 0;
      temp->worldStateID.object = 0;

      temp->owner.site = slaveSim.site;
      temp->owner.host = slaveSim.host;

      temp->sequenceNumber = 0;

      temp->mclass = SP_objectClassPoint;

      temp->missingFromWorldState = 0;

      temp->variant.point.overlayID.simulator.site = OVERLAY_SITE;
      temp->variant.point.overlayID.simulator.host = OVERLAY_HOST;
      temp->variant.point.overlayID.object = OVERLAY_OBJECT;

      pdu = (char*) temp;

   }
   else{
      temp = (SP_DescribeObjectVariant *) pdu;
   }

   //Dupe ModSAF.  I increase the sequence number while keeping the owner the
   //same.  This way the changes occur but ModSAF will maintain the entities
   temp->sequenceNumber++; 
   
   //  Change the fields of the pdu.
   
   temp->variant.point.style = (SP_PointStyle) style;
   temp->variant.point.color = (SP_OverlayColor) color;
   temp->variant.point.dashed = (unsigned) dashed;
   temp->variant.point.location.x = (long) points[0][X];
   temp->variant.point.location.y = (long) points[0][Y];
   temp->variant.point.direction = (SP_Angle) direction;
   strcpy(temp->variant.point.text, text);

   //  This unused field is used to send whether or not the measure is
   //  being dragged.
   temp->variant.point._unused_8 = 0;

}  //  End of PO_POINT_CLASS::updatePDU.

//  This function outputs the current pdu describing the po measure to the
//  network.  I copy the SP_describeObject stuff via characters using the
//  temporary pointer temp.  poProtocol should already be set elsewhere.
void PO_POINT_CLASS::sendPDU(){

   const unsigned short DESCRIBE_OBJECT_HEADER_SIZE = 36;

   SP_PersistentObjectPDU *outPDU;
   char *temp;
   unsigned short length;

   if(NULL == (outPDU = (SP_PersistentObjectPDU *) pfMalloc(
      sizeof(SP_PersistentObjectPDU), pfGetSharedArena()) )  )
   {
      cerr<<"Error allocating storage in sendPDU."<<endl;
   }

   outPDU->version = poProtocol;             //  We should have already
                                             //  captured this.
   outPDU->kind = SP_describeObjectPDUKind;
   outPDU->exercise = L_exercise;
   outPDU->database = L_database;

   length = PRO_PO_POINT_CLASS_SIZE + DESCRIBE_OBJECT_HEADER_SIZE;
   
   temp = (char *) &(outPDU->variant.describeObject);  //  Copy as chars.
   
   //  I can't do 
   //  "outPDU->variant.describeObject = (SP_DescribeObjectVariant) pdu;"
   //  outright, so I will copy it instead as characters.  I will also count
   //  the length.
   for (int i = 0; i < sizeof(SP_DescribeObjectVariant); i++){
      temp[i] = pdu[i];
   }

   outPDU->variant.describeObject.variant.point.location.x = 
      (long) points[0][X] + POF_nps_easting_shift;
   outPDU->variant.describeObject.variant.point.location.y = 
      (long) points[0][Y] + POF_nps_northing_shift;

   outPDU->length = length;

   net_write((char*)outPDU, length); 

}  //  End of PO_POINT_CLASS::sendPDU.


/*********************** PO_MINE_CLASS       *************************/

PO_MINE_CLASS  :: PO_MINE_CLASS(SP_DescribeObjectVariant describeObject,
    int length): PO_MEASURES_CLASS(describeObject, length)
{
   type = PO_MINE;
   firstTime = 1;

   //  Get other member values from the pdu.
   minefieldType = (ushort)describeObject.variant.minefield.minefieldType;
   style = (ushort)describeObject.variant.minefield.style;
   color = (PO_COLOR)describeObject.variant.minefield.color;
   munition = (uint) describeObject.variant.minefield.munition;
   detonator = (uint) describeObject.variant.minefield.detonator;
   size = (ushort) describeObject.variant.minefield.variant.area.size;
   rowWidth = (ushort)describeObject.variant.minefield.variant.area.rowWidth;
   density = (ushort) describeObject.variant.minefield.variant.area.density;
   origin[X] = (float) describeObject.variant.minefield.variant.area.origin.x;
   origin[Y] = (float) describeObject.variant.minefield.variant.area.origin.y;
   numpoint = (uint) describeObject.variant.minefield.variant.area.pointCount;

   for (int i=0; i < SP_maxPointNameLength; i++)
      text[i] = (char) describeObject.variant.minefield.text[i];

   for (i = 0; i < numpoint; i++){
      points[i][X] = describeObject.variant.minefield.variant.area.
         perimeter[i].x - POF_nps_easting_shift;
      points[i][Y] = describeObject.variant.minefield.variant.area.
         perimeter[i].y - POF_nps_northing_shift;
      points[i][Z] = gnd_level2(points[i]);  
   }

   polys = make_meas();

}  //  End of constructor for PO_MINE_CLASS.


//  Very similar to constructor except new measure is not constructed.  The
//  problem here is when I compare incoming pdu's with po measures that
//  have already been constructed.  There is a high rate of false hits, i.e.,
//  I find the measure which the pdu describes already exists AND that it
//  needs to be modified.  Practically every mine pdu coming across has to
//  be rebuilt which is wrong, especially since rebuilt they're the same 
//  measures.  so in this function I put a doublecheck, which checks each
//  field in the pdu.  It works it keeps the system fast but it's wrong.  Also
//  if the minefield is very small the incidence of false hits is much 
//  smaller which suggests to me the comparison has to deal with the length
//  of the pdu.  The update itself is just like the constructor without
//  creating the new measure.
void PO_MINE_CLASS::update(SP_DescribeObjectVariant describeObject, int length){

   unsigned drag = 1;
   char    *temp;
   int doublecheck = 0;

   //  Checking if I can remove the old, but I won't actually remove until 
   //  I have double checked that it needs to be modified.
   if (pfGetParent(polys,0) == NULL){
      cerr<<"poly's par is null. error"<<endl;
      exit(0);
   }

   //  Need this as a flag.  I need to know if the group I'm about to modify
   //  is being highlighted.  If so it will have pointers to it so I need to
   //  signal that it has been modified.  Also if we are dragging a minefield
   //  being updated we want to ensure we don't use the drag version.
   //if (polys == pickedGroup){
   //   pickedGroup = NULL;  
   // }

   //  Remove but don't delete yet.
   pfRemoveChild(pfGetParent(polys,0),polys);         //SAK.  remove the old.

//  This is a hack.  But for some reason, I think a race condition with the buffers
//  when I compare the PDU string with long PDUs they don't match.  Then when I
//  ASSIGN them in here they've changed so the don't match the NEXT TIME either.
//  So by the following checks I'll see if the measure has really changed but then
//  again the race condition may exist here as well.  This works.  You have
//  a better way, have at it.
   if (
   (minefieldType != (ushort)describeObject.variant.minefield.minefieldType) ||
   (style != (ushort)describeObject.variant.minefield.style) ||
   (color != (PO_COLOR)describeObject.variant.minefield.color) ||
   (munition != (uint) describeObject.variant.minefield.munition) ||
   (detonator != (uint) describeObject.variant.minefield.detonator) ||
   (size != (ushort) describeObject.variant.minefield.variant.area.size) ||
   (rowWidth != (ushort)describeObject.variant.minefield.variant.area.rowWidth)||
   (density != (ushort) describeObject.variant.minefield.variant.area.density) ||
   (origin[X] != (float) describeObject.variant.minefield.variant.area.origin.x) ||
   (origin[Y] != (float) describeObject.variant.minefield.variant.area.origin.y) ||
   (numpoint != (uint) describeObject.variant.minefield.variant.area.pointCount)||
   (drag !=  describeObject.variant.minefield._unused_13)
   ) doublecheck = 1;

   for (int i=0; i < SP_maxPointNameLength; i++){
      if (text[i] != (char) describeObject.variant.minefield.text[i])
         doublecheck = 1;
   }

   for (i = 0; i < numpoint; i++){
      if  (points[i][X] != describeObject.variant.minefield.variant.area.
         perimeter[i].x)
            doublecheck = 1;
      if  (points[i][Y] != describeObject.variant.minefield.variant.area.
         perimeter[i].y)
            doublecheck = 1; 
   }

   //  I really do need to update the measure.  Also I check if this group
   //  is being highlighted.  If it is I ignore the update.  i.e, a drag will
   //  never be interupted by an update.  Most recent drag wins.
   if (doublecheck && (polys != pickedGroup)) { 
      pfDelete(polys);                      //  Delete it from memory.

      //  If I'm dragging the mine, all I want to do is update the polys with
      //  a quick mine. I used an unused field in the ModSAF packet.  I
      //  change the mine and put a quickmine version in.  This could pose
      //  problems if I didn't get the "drop" pdu, because the line would
      //  in essence be left in the low resolution representation.  Should
      //  ensure we don't drop packets on the ground. 

      //  This unused field is used to send whether or not the measure is
      //  being dragged.
//cerr<<"the pdu "<<describeObject.variant.minefield._unused_13<<endl;
      if (describeObject.variant.minefield._unused_13 == 1){
//cerr<<"in the drag update"<<endl;     
         for (int i = 0; i < numpoint; i++){
            points[i][X] = (float)describeObject.variant.minefield.variant.
               area.perimeter[i].x;
            points[i][Y] = (float)describeObject.variant.minefield.variant.
               area.perimeter[i].y;
            points[i][Z] = gnd_level2(points[i]);
         }

         origin[X] = (float)describeObject.variant.minefield.variant.
               area.origin.x;
         origin[Y] = (float)describeObject.variant.minefield.variant.
               area.origin.y;

         difference[X] = 0.0;
         difference[Y] = 0.0;

         polys = pfNewGroup();
         //  A line which I can move in real time.
         pfAddChild(polys, makeQuickMine()); 

         updatePDU();

      }
      else{
//cerr<<"in the else part"<<endl;
         //delete []pdu;                         //SAK. Prevent mem leak.
         pfFree(pdu);

         //  This unused field is used to send whether or not the measure is
         //  being dragged.  Need to reset it here.  This is set to not being
         //  dragged. It's needed because if I drag minefield and ModSAF updates
         //  as I'm dragging, I may not get to updateDrag, so the drag flag
         //  won't get reset.
         describeObject.variant.minefield._unused_13 = 0;

          //  Add the new.
          //  I will convert the pdu into a string.  There may be something in string.h
         //  which may do this easier but the below routine just does it.  It makes the
          //  string the right size and ensures it terminates with the '\0'.

         temp = (char *) &describeObject;
         pdu = (char *)pfMalloc((length+1)*sizeof(char),pfGetSharedArena());
         //pdu = new char[length + 1];

         for (i = 0; i < length; i++){
            pdu[i] = temp[i];
         }
         pdu[length] = '\0';

         type = PO_MINE;

         //  Get the rest out of the pdu.
         minefieldType = (ushort)describeObject.variant.minefield.minefieldType;
         style = (ushort)describeObject.variant.minefield.style;
         color = (PO_COLOR)describeObject.variant.minefield.color;
         munition = (uint) describeObject.variant.minefield.munition;
         detonator = (uint) describeObject.variant.minefield.detonator;
         size = (ushort) describeObject.variant.minefield.variant.area.size;
         rowWidth = (ushort)describeObject.variant.minefield.variant.area.rowWidth;
         density = (ushort) describeObject.variant.minefield.variant.area.density;
         origin[X] = (float) describeObject.variant.minefield.variant.area.origin.x;
         origin[Y] = (float) describeObject.variant.minefield.variant.area.origin.y;
         numpoint = (uint) describeObject.variant.minefield.variant.area.pointCount;

         for (i=0; i < SP_maxPointNameLength; i++)
            text[i] = (char) describeObject.variant.minefield.text[i];

         for (i = 0; i < numpoint; i++){
            points[i][X] = 
               describeObject.variant.minefield.variant.area.perimeter[i].x -
               POF_nps_easting_shift;
            points[i][Y] =
               describeObject.variant.minefield.variant.area.perimeter[i].y -
               POF_nps_northing_shift;
            points[i][Z] = gnd_level2(points[i]);      
         }
         polys = make_meas();
      }  // End of else.
   }  //  End of if (doublecheck)
}  //  End update.



//  Mines are just closed lines with a text label in the middle.  For mines
//  I don't use a single line representation, rather I use a "ribbon"  which
//  is a tristrip.  To do this I again chop the lines between each main point.
//  I then also offset from a centerline to give me vertuces a set distance 
//  left or right of centerline.
pfGroup * PO_MINE_CLASS::make_meas()
{
   const float LINE_HEIGHT = 10.0;
   const uint thickness = 5;

   pfGroup *the_group;
   pfGeoSet *gset;
   pfGeoState *gst;
   pfGeode *geode;
 
   pfVec3 *coords;
   pfVec3 centerline;

   void *arena;
  
   float angle, msin, mcos;
   float angle1, msin1, mcos1;
   float angle2, msin2, mcos2;

   float Xleft, Xright,
      Yleft, Yright,
      Xmiddle, Ymiddle;

   int numvert=0, num_seg, seg_cnt;
   int ix,kx;

   int *length_vec;

  // PO_TEXT_CLASS* textnode;
 
   //  Get the shared memory
   arena = pfGetSharedArena();

   //  Generate the points.  First figure out how many are there.
   num_seg = 0;
   for(ix=0;ix<(numpoint-1);ix++){
      num_seg += (int)(PFDISTANCE_PT3(points[ix],points[ix+1])/PO_MEAS_LENGTH)+1;
   }

   //  All minefields are closed so add the segments.
   num_seg += (int)(PFDISTANCE_PT3(points[numpoint-1],points[0])/
      PO_MEAS_LENGTH)+1;
  
   //  Allocate the storage
   if(NULL==(coords=(pfVec3 *)pfMalloc(((num_seg +2)*2+2)*sizeof(pfVec3),
      arena))){
         return(NULL);
   }

   //  This part of make_meas creates a line which is a horizontal strip.
   //  It uses an algorithm similar to that of the vertical line except rather
   //  than creating vertices for a top and bottom post, it creates vertices 
   //  for a left and right side of the line.  It then bases the vertices on 
   //  the desired thickness of the line.  Numvert starts at zero.  SAK.

   //  Step through each point and generate the segments in between.
   for(ix=0;ix<(numpoint-1);ix++){

      PFCOPY_VEC3(centerline, points[ix]);  //  First point in this sequence
                                            //  is the original point.
      centerline[Z] =  gnd_level2(centerline) + LINE_HEIGHT; 

      //  Get the angle between the points.
      angle = pfArcTan2((points[ix+1][Y] - points[ix][Y]),
         (points[ix+1][X] - points[ix][X]));

      angle1 = angle + 90.0;               //  Left side.
      angle2 = angle - 90.0;               //  Right side.

      pfSinCos(angle,&msin,&mcos);         //  Get signs and cosines to be
      pfSinCos(angle1,&msin1,&mcos1);      //  used to increment centerline
      pfSinCos(angle2,&msin2,&mcos2);      //  segment and offset from 
                                           //  centerline.

      Xleft = mcos1 * thickness;           //  Find the shifts.  Remember
      Xright = mcos2 * thickness;          //  thickness is a member in the
      Yleft = msin1 * thickness;           //  class.
      Yright = msin2 * thickness; 
      Xmiddle = mcos * PO_MEAS_LENGTH;     //  Incremental shift.
      Ymiddle = msin * PO_MEAS_LENGTH;
              
      //  The number of sub-segments a line is divided into.
      seg_cnt = (int)(PFDISTANCE_PT3(points[ix],points[ix +1])/ 
        PO_MEAS_LENGTH);                 
    
      //  Cycle through all points on centerline, make the shift and assign
      //  to the vector.  
      for (kx=0;kx<seg_cnt;kx++ ){ 
         coords[numvert][X] = centerline[X] + Xleft;
         coords[numvert][Y] = centerline[Y] + Yleft;
         coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
         numvert++;

         coords[numvert][X] = centerline[X] + Xright;
         coords[numvert][Y] = centerline[Y] + Yright;
         coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
         numvert++;

         centerline[X] += Xmiddle;
         centerline[Y] += Ymiddle;
         centerline[Z] = gnd_level2(centerline) + LINE_HEIGHT;
            
      }  //  End of kx for loop.
 
      //  This may spill over the space allocated for the measure.  Finish
      //  the line.
      coords[numvert][X] = points[ix + 1][X] + Xleft;
      coords[numvert][Y] = points[ix + 1][Y] + Yleft;
      coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
      numvert++;

      coords[numvert][X] = points[ix + 1][X] + Xright;
      coords[numvert][Y] = points[ix + 1][Y] + Yright;
      coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
      numvert++; 
  
   }  //  ix for loop.

   //  Add the last line back to the start.
        
   PFCOPY_VEC3(centerline, points[numpoint-1]);           //  Last point.
   centerline[Z] =  gnd_level2(centerline) + LINE_HEIGHT; 

   //  Get the angle between the points.
   angle = pfArcTan2((points[0][Y] - points[numpoint-1][Y]),
      (points[0][X] - points[numpoint-1][X]));
   angle1 = angle + 90.0;               //  Left side.
   angle2 = angle - 90.0;               //  Right side.

   pfSinCos(angle,&msin,&mcos);         //  Get signs and cosines to be
   pfSinCos(angle1,&msin1,&mcos1);      //  used to increment centerline
   pfSinCos(angle2,&msin2,&mcos2);      //  segment and offset from 
                                        //  centerline.

   Xleft = mcos1 * thickness;           //  Find the shifts.  Remember
   Xright = mcos2 * thickness;          //  thickness is a member in the
   Yleft = msin1 * thickness;           //  class.
   Yright = msin2 * thickness; 
   Xmiddle = mcos * PO_MEAS_LENGTH;     //  Incremental shift.
   Ymiddle = msin * PO_MEAS_LENGTH;       

   //  The number of sub-segments a line is divided into.
   seg_cnt = (int)(PFDISTANCE_PT3(points[numpoint - 1],points[0])/ 
      PO_MEAS_LENGTH);                 
    
   //  Cycle through all points on centerline, make the shift and assign
   //  to the vector.  
   for (kx=0;kx<seg_cnt;kx++ ){ 
      coords[numvert][X] = centerline[X] + Xleft;
      coords[numvert][Y] = centerline[Y] + Yleft;
      coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
      numvert++;

      coords[numvert][X] = centerline[X] + Xright;
      coords[numvert][Y] = centerline[Y] + Yright;
      coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
      numvert++;

      centerline[X] += Xmiddle;
      centerline[Y] += Ymiddle;
      centerline[Z] = gnd_level2(centerline) + LINE_HEIGHT;        
   }  //  kx for loop. 

   //This may spill over the space allocated for the measure.  Finish the line.
   coords[numvert][X] = points[0][X] + Xleft;
   coords[numvert][Y] = points[0][Y] + Yleft;
   coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
   numvert++;

   coords[numvert][X] = points[0][X] + Xright;
   coords[numvert][Y] = points[0][Y] + Yright;
   coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
   numvert++; 

   //  Make the performer stuff with the calculated points.
   gset = pfNewGSet(arena);

   //  Set the primitive attributes.
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, L_color[color], NULL);
   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, coords, NULL);
   pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);

   pfGSetPrimType(gset, PFGS_TRISTRIPS);
   pfGSetNumPrims(gset, 1);
   if(NULL==(length_vec = (int *) pfMalloc(sizeof(int),arena))){
      return(NULL);
   }
   length_vec[0] = numvert;
   pfGSetPrimLengths(gset,length_vec);

   //  Create a new geostate from shared memory,
   // enable transparency
   gst = pfNewGState(arena);
   pfGStateMode(gst, PFSTATE_ENTEXTURE, 0);
   pfGStateMode(gst, PFSTATE_TRANSPARENCY, 1);

   pfGSetGState(gset, gst);

   //  Build the geode.
   geode = pfNewGeode();
   pfAddGSet (geode, gset);

   //  Build the group.
   the_group = pfNewGroup();
   pfNodeName(geode, "highlight");          //  Name it for picking.
   pfNodeName(the_group, "picked_group");
   pfAddChild(the_group,geode);

   //Label the minefield
   {
    pfVec3  center_mine;
    PFSET_VEC3(center_mine,0.0f,0.0f,0.0f);

    for(ix=0;ix<numpoint;ix++){
       pfAddVec3(center_mine,points[ix],center_mine);
    }

    pfScaleVec3(center_mine,1.0f/numpoint,center_mine);
    center_mine[Z] =  gnd_level2(center_mine);
    textnode = new PO_TEXT_CLASS (PO_TEXT,1);
    textnode->create_text_node(center_mine,color,text);
    cerr<<"about to go add text"<<endl;
    //THIS IS A POTENTIAL MEMORY LEAK ******************
    pfAddChild(the_group,textnode->make_meas());
   }
   return(the_group);
}  //  End of make_meas for PO_MINE_CLASS.

//  This is used for dragging the minefield.  It will grab a point and then
//  calculate a difference between that point and the mouse world position.
//  It then shifts the entire minefield by that amount.  It uses a temporary
//  fast minefield so that it can be moved in real time.
void PO_MINE_CLASS::movePO(float xpos, float ypos, ushort outToNet)
{
   pfGeode* temp;
   static pfVec3 startpoint;
//cerr<<"in movePO"<<endl;
   //  Get the start point to base movement on.
   if (firstTime){
      startpoint[X] = xpos;
      startpoint[Y] = ypos;
      startpoint[Z] = gnd_level2(startpoint);
      firstTime = 0;
   }

   //  Get difference between where we started and the current mouse position.
   difference[X] = xpos - startpoint[X];
   difference[Y] = ypos - startpoint[Y];
//cerr<<"about to delete"<<endl;
   //  I didn't make a mistake here, this is an assignment which will return
   //  a null if it fails.  I'm looking to assign temp to the second child
   //  of polys.  If that child doesn't exist I don't want to continue with the
   //  next portion of the conditional. I want it as = NOT == 
   if (temp = (pfGeode*)pfGetChild(polys, 1) ){
      pfRemoveChild(polys, temp);                         //  Out with the old.
      pfDelete(temp);
   }
//cerr<<"done deleting"<<endl;
   temp = makeQuickMine();         //  A minefield I can move in real time.
//cerr<<"done in mackQuick about to add"<<endl;
   pfAddChild(polys, temp);        //  In with the new.  Add the quick minefield.
  
//cerr<<"done adding about to send to net"<<endl;
   if (outToNet) dragToNet(xpos, ypos);   
//cerr<<"about to leave movePO"<<endl;
}  //  End of movePO for PO_MINE_CLASS.

//  This function sends an update packet to the net which says the line is being
//  dragged.
void PO_MINE_CLASS::dragToNet(float, float){
//cerr<<"in drag to net"<<endl;
   const unsigned short DESCRIBE_OBJECT_HEADER_SIZE = 36;

   SP_PersistentObjectPDU *outPDU;

   SP_DescribeObjectVariant *tempPDU;

   char *temp;

   int length;
//cerr<<"about to get storage for pdu"<<endl;
   if(NULL == (outPDU = (SP_PersistentObjectPDU *) pfMalloc(
      sizeof(SP_PersistentObjectPDU), pfGetSharedArena()) )  )
   {
      cerr<<"Error allocating storage in sendPDU."<<endl;
   }

   tempPDU = (SP_DescribeObjectVariant *)pdu;

   temp = (char *) &(outPDU->variant.describeObject); 

   length = PRO_PO_MINEFIELD_CLASS_SIZE(&(tempPDU->variant.minefield)) +
      DESCRIBE_OBJECT_HEADER_SIZE;

   outPDU->version = poProtocol;                  //  We should have already
                                                  //  captured this.
   outPDU->kind = SP_describeObjectPDUKind;
   outPDU->exercise = L_exercise;
   outPDU->database = L_database;

   //Dupe ModSAF.  I increase the sequence number while keeping the owner the
   //same.  This way the changes occur but ModSAF will maintain the entities
   outPDU->variant.describeObject.sequenceNumber++; 
//cerr<<"about to transfer into temp"<<endl;
   for (int i = 0; i < sizeof(SP_DescribeObjectVariant); i++){
      temp[i] = pdu[i];
   }

   // Since drag, I will only change the location.  I will also use the _unused_8
   // flag to show I'm dragging this entity.
   outPDU->variant.describeObject.variant.minefield._unused_13 = 1;
   for (i=0; i < numpoint; i++){
      outPDU->variant.describeObject.variant.minefield.variant.area.perimeter[i].
         x = (long)(points[i][X] + difference[X]);
      outPDU->variant.describeObject.variant.minefield.variant.area.perimeter[i].
         y = (long)(points[i][Y] + difference[Y]);
   }

   outPDU->variant.describeObject.variant.minefield.variant.area.origin.
         x = origin[X] + difference[X] + POF_nps_easting_shift;
   outPDU->variant.describeObject.variant.minefield.variant.area.origin.
         y = origin[Y] + difference[Y] + POF_nps_northing_shift;

   outPDU->length = length;   
   net_write((char*)outPDU, length); 

}  //  End of dragToNet.


//  This is the drop portion of the drag and drop.  It modifies the points in
//  the po measure and cleans up after the drag.
void PO_MINE_CLASS::dragUpdatePO(float, float, ushort outToNet)
{
   
   pfGroup* polysParent;

   firstTime = 1; 
//cerr<<"in dragUpdatePO"<<endl;
   for (int i = 0; i < numpoint; i++){
      points[i][X] = points[i][X] + difference[X];
      points[i][Y] = points[i][Y] + difference[Y];
      points[i][Z] = gnd_level2(points[i]);
   }

   origin[X] = origin[X] + difference[X];
   origin[Y] = origin[Y] + difference[Y];

   if (pfGetParent(polys,0) == NULL){
      cerr<<"poly's par is null. error"<<endl;
      exit(0);
   }

   //  Flag used in highlighting to ensure nodes aren't deleted which are
   //  still refered to elsewhere.
   if (polys == pickedGroup){
      pickedGroup = NULL;
   }

   //  Remove the quick minefield and build the permanent one.
   polysParent = pfGetParent(polys,0);
   pfRemoveChild(polysParent,polys);              //SAK.  remove the old.
   pfDelete(polys);

   polys = make_meas();
   pfAddChild(polysParent, polys);

   if (outToNet){
      updatePDU();
      sendPDU();
   }

}  //  End of dragUpdatePO for PO_MINE_CLASS.

//  This function is used to update the pdu member describing a po measure, when
//  members of that measure have changed.  The function updates the pdu values
//  with the new member values.
void PO_MINE_CLASS::updatePDU(){
//cerr<<"in updatePDU"<<endl;
   SP_DescribeObjectVariant *temp;
   
   //  For now we'll crash with this.  In future we'll build it.
   if (pdu == NULL){
      cerr<<"No pdu exists in updatePDU."<<endl;
   }

   temp = (SP_DescribeObjectVariant *) pdu;
   //  This unused field is used to send whether or not the measure is
   //  being dragged.
   temp->variant.minefield._unused_13 = 0;

   //Dupe ModSAF.  I increase the sequence number while keeping the owner the
   //same.  This way the changes occur but ModSAF will maintain the entities
   temp->sequenceNumber++; 
   
   //  Change the fields of the pdu.
   temp->variant.minefield.style = (SP_LineStyle) style;
   temp->variant.minefield.color = (SP_OverlayColor) color;
   temp->variant.minefield.munition = (SP_ObjectType) munition;
   temp->variant.minefield.detonator = (SP_ObjectType) detonator;
   temp->variant.minefield.minefieldType = (SP_MinefieldType)minefieldType;
   temp->variant.minefield.variant.area.size = (unsigned short) size;
   temp->variant.minefield.variant.area.rowWidth = (unsigned short) rowWidth;
   temp->variant.minefield.variant.area.density = (unsigned short) density;
   temp->variant.minefield.variant.area.origin.x = (long) origin[X];
   temp->variant.minefield.variant.area.origin.y = (long) origin[Y];
   temp->variant.minefield.variant.area.pointCount = (unsigned short) numpoint;
   
   //  This unused field is used to send whether or not the measure is
   //  being dragged.
   temp->variant.minefield._unused_13 = 0;

   for (int i=0; i < numpoint; i++){
      temp->variant.minefield.variant.area.perimeter[i].x = (long)points[i][X];
      temp->variant.minefield.variant.area.perimeter[i].y = (long)points[i][Y];
   }

   for (i = 0; i < SP_maxPointNameLength; i++){
      temp->variant.minefield.text[i] = text[i]; 
   }
   
}  //  End of PO_MINE_CLASS::updatePDU.

//  This function outputs the current pdu describing the po measure to the
//  network.  I copy the SP_describeObject stuff via characters using the
//  temporary pointer temp.  poProtocol should already be set elsewhere.
void PO_MINE_CLASS::sendPDU(){
//cerr<<"in send pdu"<<endl;
   const unsigned short DESCRIBE_OBJECT_HEADER_SIZE = 36;
   SP_PersistentObjectPDU *outPDU;
   SP_DescribeObjectVariant *tempPDU;

   char *temp;
   unsigned short length;

   if(NULL == (outPDU = (SP_PersistentObjectPDU *) pfMalloc(
      sizeof(SP_PersistentObjectPDU), pfGetSharedArena()) )  )
   {
      cerr<<"Error allocating storage in sendPDU."<<endl;
   }

   outPDU->version = poProtocol;             //  We should have already
                                             //  captured this.
   outPDU->kind = SP_describeObjectPDUKind;
   outPDU->exercise = L_exercise;
   outPDU->database = L_database;

   tempPDU = (SP_DescribeObjectVariant *) pdu;
   length = DESCRIBE_OBJECT_HEADER_SIZE + 
      PRO_PO_MINEFIELD_CLASS_SIZE(&(tempPDU->variant.minefield));
 
   temp = (char *) &(outPDU->variant.describeObject);  //  Copy as chars.
   
   //  I can't do 
   //  "outPDU->variant.describeObject = (SP_DescribeObjectVariant) pdu;"
   //  outright, so I will copy it instead as characters.  I will also count
   //  the length.
   for (int i = 0; i < sizeof(SP_DescribeObjectVariant); i++){
      temp[i] = pdu[i];
   }

   outPDU->variant.describeObject.variant.minefield.variant.area.origin.
         x = origin[X] + POF_nps_easting_shift;
   outPDU->variant.describeObject.variant.minefield.variant.area.origin.
         y = origin[Y] + POF_nps_northing_shift;

   outPDU->length = length;
//cerr<<"in sendPDU:flag "<<tempPDU->variant.minefield._unused_13<<endl;
//cerr<<"in sendPDU length "<<length<<endl;
   net_write((char*)outPDU, length); 

}  //  End of PO_MINE_CLASS::sendPDU.


//  This function creates a minefield quickly which can be moved in real time.
//  It is a single line representation of the minefield with a course step
//  size.
pfGeode * PO_MINE_CLASS::makeQuickMine(){

   const float LINE_HEIGHT = 10.0;
   const float COURSE = 10.0;            //  A courseness factor for segments.

   pfGeoSet *gset;
   pfGeoState *gst;
   pfGeode     *geode;
   pfVec3 *coords;
   pfVec3 tempPoints[C2_SEGMENTS];

   void *arena;
  
   float angle,msin,mcos;
   float Xmiddle, Ymiddle;

   int numvert=0,num_seg,seg_cnt;
   int ix,kx;

   int *length_vec;
//cerr<<"in makeQuickMine"<<endl;
   for (int i = 0; i < numpoint; i++){
      tempPoints[i][X] = points[i][X] + difference[X];
      tempPoints[i][Y] = points[i][Y] + difference[Y];
      tempPoints[i][Z] = gnd_level2(tempPoints[i]);
   }
//cerr<<"about to get arena"<<endl;
   //  Get the shared memory.
   arena = pfGetSharedArena();
   
   //  From the points of the line I will chop up the line into a bunch of 
   //  segments.  In this way my line can follow the terrain elevation.

   //  First figure out how many segments there are.
   num_seg = 0;
   for(ix=0;ix<(numpoint-1);ix++){
      num_seg += (int)(PFDISTANCE_PT3(tempPoints[ix],
      tempPoints[ix+1])/(PO_MEAS_LENGTH*COURSE))+1;      //SAK.  Much courser line.
   }      //Basically COURSE times less the resolution.

   // All minefields are closed add the segments.
   num_seg += (int)(PFDISTANCE_PT3(tempPoints[numpoint-1],tempPoints[0])/
      (PO_MEAS_LENGTH * COURSE))+1;                //SAK. Courser line.
//cerr<<"num segments "<<num_seg<<endl;
   //  Now allocate the storage.
   if(NULL==(coords=(pfVec3 *)pfMalloc((num_seg*2+2)*sizeof(pfVec3),arena))){
//cerr<<"NULL IN STORAGE"<<endl;
      return(NULL);
   }
//cerr<<"done allocateing storage"<<endl;
   //  This part of make_meas creates a line which is a line strip
   //  rather than a  triangle strip.  

   PFCOPY_VEC3(coords[0], tempPoints[0]);     //  First point in this sequence
                                              //  is the original point.
   coords[0][Z] =  gnd_level2(coords[0]) + LINE_HEIGHT; 

   numvert++;
//cerr<<"about to start in loop"<<endl;
   //  Step through each point and generate the segments in between.
   for(ix=0;ix<(numpoint - 1);ix++){

      //  Get the angle between the points.
      angle = pfArcTan2((tempPoints[ix+1][Y] - tempPoints[ix][Y]),
         (tempPoints[ix+1][X] - tempPoints[ix][X]));

      pfSinCos(angle,&msin,&mcos);         //  Used to increment line segs.
      Xmiddle = mcos * PO_MEAS_LENGTH * COURSE;     //  Incremental shift.
      Ymiddle = msin * PO_MEAS_LENGTH * COURSE;
              
      //  The number of sub-segments a line is divided into.
      seg_cnt = (int)(PFDISTANCE_PT3(tempPoints[ix],tempPoints[ix +1])/ 
         (PO_MEAS_LENGTH * COURSE)) ;                 
    
      //  Cycle through all points on centerline, use the point as a vertex.
      for (kx=1;kx<seg_cnt-1;kx++ ){ 
         coords[numvert][X] = coords[numvert -1][X] + Xmiddle;
         coords[numvert][Y] = coords[numvert -1][Y] + Ymiddle;
         coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
         numvert++;
      }  //  kx for loop.

      //Ensure that last point is in fact the next point.  i.e the segments
      //may not reach the last point due to rounding of seg_cnt. 
      coords[numvert][X] = tempPoints[ix+1][X];
      coords[numvert][Y] = tempPoints[ix+1][Y];
      coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
      numvert++; 
   }  //  End of ix for loop.

   //  Add the last line back to the start.
//cerr<<"num vert out of ix loop "<<numvert<<endl;        
   PFCOPY_VEC3(coords[numvert], tempPoints[numpoint-1]);      //  Last point.
   coords[numvert][Z] =  gnd_level2(coords[numvert]) + LINE_HEIGHT; 
   numvert++;
//cerr<<"num vert pt 2 "<<numvert<<endl;  
   //  Get the angle between the points.
   angle = pfArcTan2((tempPoints[0][Y] - tempPoints[numpoint-1][Y]),
      (tempPoints[0][X] - tempPoints[numpoint-1][X]));

   pfSinCos(angle,&msin,&mcos);         //  Signs and cosines for sgmnt.
   Xmiddle = mcos * PO_MEAS_LENGTH * COURSE;     //  Incremental shift.
   Ymiddle = msin * PO_MEAS_LENGTH * COURSE;       

   //  The number of sub-segments a line is divided into.
   seg_cnt = (int)(PFDISTANCE_PT3(tempPoints[numpoint-1],tempPoints[0])/ 
      (PO_MEAS_LENGTH * COURSE));                 
    

   //  Cycle through all points on centerline, make the shift and assign
   //  to the vector.  
   for (kx=1;kx<seg_cnt-1;kx++ ){ 
      coords[numvert][X] = coords[numvert - 1][X] + Xmiddle;
      coords[numvert][Y] = coords[numvert - 1][Y] + Ymiddle;
      coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
      numvert++;
   }  //  End of kx for loop.
//cerr<<"num vert pt 3 "<<numvert<<endl;
//cerr<<"about to ensure its connected "<<endl; 
   //Ensure it's connected.
   coords[numvert][X] = tempPoints[0][X];
   coords[numvert][Y] = tempPoints[0][Y];
   coords[numvert][Z] = gnd_level2(coords[numvert]) + LINE_HEIGHT;
   numvert++; 

   //  Build the performer stuff.
   gset = pfNewGSet(arena);

   //  Set the primitives attributes.
   pfGSetAttr(gset, PFGS_COLOR4, PFGS_OVERALL, L_color[color], NULL);
   pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, coords, NULL);
   pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_OFF, NULL, NULL);
   pfGSetAttr(gset, PFGS_NORMAL3, PFGS_OFF, NULL, NULL);

   pfGSetPrimType(gset, PFGS_FLAT_LINESTRIPS);  //  Lines rather than tristrips.
   pfGSetLineWidth(gset, 10.0);
   pfGSetNumPrims(gset, 1);

   if(NULL==(length_vec = (int *) pfMalloc(sizeof(int),arena))){
      return(NULL);
   }
   length_vec[0] = numvert;

   pfGSetPrimLengths(gset,length_vec);

   // Create a new geostate from shared memory,
   // enable transparency
   gst = pfNewGState(arena);
   pfGStateMode(gst, PFSTATE_ENTEXTURE, 0);
   pfGStateMode(gst, PFSTATE_TRANSPARENCY, 1);

   pfGSetGState(gset, gst);

   geode = pfNewGeode();
   pfAddGSet (geode, gset);
 
   return(geode);

}  //  End of makeQuickMine.

#endif
