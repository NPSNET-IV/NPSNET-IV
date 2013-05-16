// File: <measures.h>

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


#ifndef __NPS_MEASURES_H__
#define  __NPS_MEASURES_H__


#define C2_SEGMENTS 100
//#define MEAS_SIZE  10.0f
//#define MEAS_LENGTH  50.0f
#define MEAS_SIZE  0.2f
#define MEAS_LENGTH  1.0f

enum M_TYPE {NO_TYPE,POINT, LINE, MULTI_LINE, AREA};
enum C2_TYPE {NO_C2,ENEMY,AXIS,LINE_OF_DEPARTURE,BOUNDRY,
               OBJECTIVE,OBSTACLES,OBS_POST};


class MEASURES_CLASS {
  
protected:
  M_TYPE type;  //what structure is it?
  C2_TYPE c2_type; //what kind is it?
  int numparts; //How many segments in the measure
  pfVec3  points[C2_SEGMENTS];  // an array of the points
  pfGroup *polys;  //the representation of the measure
   

public:
  MEASURES_CLASS();
  MEASURES_CLASS(C2_TYPE,int);
  ~MEASURES_CLASS();
  virtual void read_measure(FILE *); //read the measure off the disk and
                                           //build it
  void attach_measure(pfGroup *);  /*attach the to scene graph*/
  void remove_measure()  /*remove the node from the scene graph*/
                      {pfRemoveChild(pfGetParent(polys,0),polys);};

};

class POINT_CLASS : public MEASURES_CLASS {

protected:
  
public:
  POINT_CLASS(C2_TYPE,int);
  ~POINT_CLASS();
  void read_measure(FILE *); //read the measure off the disk
                                   //and build it
};

class LINE_CLASS : public MEASURES_CLASS {

protected:
  
public:
  LINE_CLASS(C2_TYPE,int);
  ~LINE_CLASS();
  void read_measure(FILE *); //read the measure off the disk
                                   //and build it
};

class MULTI_LINE_CLASS : public MEASURES_CLASS {

protected:
  
public:
  MULTI_LINE_CLASS(C2_TYPE,int);
  ~MULTI_LINE_CLASS();
  void read_measure(FILE *); //read the measure off the disk
                                   //and build it
};

class AREA_CLASS : public MEASURES_CLASS {

protected:
  
public:
  AREA_CLASS(C2_TYPE,int);
  ~AREA_CLASS();
  void read_measure(FILE *); //read the measure off the disk
                                   //and build it
};


//the normal functions
int remove_measures(void);
int open_measure_file(char *);
pfGroup *make_point_meas(C2_TYPE,pfVec3);
pfGroup *make_line_meas(C2_TYPE,pfVec3,pfVec3);
void display_measures();
void delete_measures();
void hide_measures();
void c2_measures();
 
   
#endif
