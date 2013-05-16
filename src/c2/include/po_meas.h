// File: <po_meas.h>

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


#ifndef __NPS_PO_MEAS_H_
#define __NPS_PO_MEAS_H_

#include "c2_const.h"
#include "p_po.h"  //  SAK.  Has many of the type defs.  SP_xxxx is probably
                   //  In here.  The original MODSAF version has some name
                   //  conflicts with the rest of NPSNET.  So I clobber it here.   
                          
#define PO_SEGMENTS 100
#define PO_MEAS_SIZE  0.2f
#define PO_MEAS_LENGTH  10.0f
#define TEXT_OFFSET 10.0f
#define BLOCK_ALLOC 50
#define C2_SEGMENTS 100

const float LINE_HEIGHT = 10.0;

enum PO_COLOR {DEFAULT_M,BLACK_M,YELLOW_M,RED_M,GREEN_M,BLUE_M,WHITE_M,HIGHLIGHT};
enum PO_TYPE {PO_NO_TYPE,PO_POINT, PO_LINE, PO_MINE, PO_TEXT, PO_BALL};

typedef struct {
  char inview;
  pfVec3 location;
  union {
   unsigned char comp[4];
   unsigned int color;
  } c;
  char text[1024];
} TEXTBLOCK;


#ifndef uchar
typedef unsigned char uchar;
#endif

#ifndef ushort
typedef unsigned short ushort;
#endif

#ifndef uint
typedef unsigned int uint;
#endif

class PO_MEASURES_CLASS {
  
protected:
   
   ushort     dashed;    //solid or dashed lines
   ushort     style;     //Which of the styles it is
   PO_COLOR   color;     //which of the colors is it 
   int       numpoint;  //How many segments in the measure
   pfVec3    points[C2_SEGMENTS];  // an array of the points
   pfVec3    difference;
   int firstTime;
  
  
  
public:
   PO_TYPE    type;      //what structure is it?
   pfGroup   *polys;    //the representation of the measure
   char    *pdu;        //SAK.  The last PDU as a string.

   //SAK.  Construct from describing PDU.
   PO_MEASURES_CLASS(SP_DescribeObjectVariant describeObject, int length);  
   PO_MEASURES_CLASS();
   ~PO_MEASURES_CLASS();

   //SAK.
   virtual void update(SP_DescribeObjectVariant describeObject, int length);
 
   virtual void movePO(float, float, ushort);
   virtual void dragToNet(float, float);
   virtual void dragUpdatePO(float, float, ushort);
   virtual void updatePDU();
   virtual void sendPDU();

   pfGroup * make_meas();       //and build it
   virtual void create_text_node(pfVec3 ,PO_COLOR ,char *);

};  //End of PO_MEASURES_CLASS.

class PO_TEXT_CLASS : public PO_MEASURES_CLASS {

protected:
  SP_TextSize size;
  short length;  //length of the text string
  char alignment; // where relative to the point is the label
  SP_PointLocation location;
  short horizontalOffset;
  short verticalOffset;
  SP_ObjectID associatedObject; 
  short associatedPointNumber;
  char text[1024]; // the string itself

public:

  //SAK.  Construct from describing PDU.
  PO_TEXT_CLASS(SP_DescribeObjectVariant describeObject, int length);  
 
  PO_TEXT_CLASS(PO_TYPE, int);
 
  ~PO_TEXT_CLASS();
  void update(SP_DescribeObjectVariant describeObject, int length);  //SAK.

  void create_text_node(pfVec3 ,PO_COLOR ,char *);
  void dragUpdatePO(float, float, ushort);
  void updatePDU();
  void sendPDU();
  pfGroup * make_meas();       
};

class PO_POINT_CLASS : public PO_MEASURES_CLASS {

protected:
  char text[1024]; // the text label
  uint  direction; // which direction the point is facing
  PO_TEXT_CLASS *textnode; //the text assocated with the point
  pfGroup* make_pt(const char*);
  
public:

   PO_POINT_CLASS(SP_DescribeObjectVariant describeObject, int length);  //SAK.  Construct
                                                      //  from describing PDU.
   PO_POINT_CLASS  :: PO_POINT_CLASS(ushort, PO_COLOR, ushort, uint, char * ,
      pfVec3);
   PO_POINT_CLASS  :: PO_POINT_CLASS(pfVec4, const char* , pfVec3);
   ~PO_POINT_CLASS();

   void update(SP_DescribeObjectVariant describeObject, int length);  //SAK.
   void movePO(float, float, ushort);
   void dragUpdatePO(float, float, ushort);
   void updatePDU();
   void sendPDU();
   void dragToNet(float, float);

  

  pfGroup * make_meas();       //and build it
};

class PO_LINE_CLASS : public PO_MEASURES_CLASS {

protected:
  ushort thickness, //How many pixels wide is the line
        closed,    //is it an area or a line
        splined;   //should we use a spline     
  float width;     //How many meters wide is the line
  ushort beginArrowHead, endArrowHead; //what kind of arrow at the ends 

    void updateText();
        
        

public:
   //SAK. Construct from describing PDU.     
   PO_LINE_CLASS(SP_DescribeObjectVariant describeObject, int length);  
      
   //SAK.  Construct from parameters rather than PDU.                                              
   PO_LINE_CLASS :: PO_LINE_CLASS(ushort, ushort, PO_COLOR,
      ushort, ushort, ushort, float, ushort, ushort, int, pfVec3*,
      ushort quick = 0);

   ~PO_LINE_CLASS();
   void update(SP_DescribeObjectVariant describeObject, int length);  //SAK.
   void movePO(float, float, ushort);
   void dragToNet(float, float);
   void dragUpdatePO(float, float, ushort);
   void updatePDU();
   void sendPDU();
   void addPoint(pfVec3);
   void setPoint(int, pfVec3);
   pfGeode*   makeQuickLine();

   pfGroup * make_meas();       //and build it
   pfGeode * make_vertical();
};

class PO_MINE_CLASS : public PO_MEASURES_CLASS {

protected:
   ushort minefieldType; // what type of mine field
   uint munition, detonator; // what type of mine are they
   ushort size,  //Number of mine cells /8 high
      rowWidth,  // Number of mine cells /8 across
      density;  //meters between cells
   pfVec3 origin;  //where the grid starts
   char text[1024];  // the name of the minefield
   PO_TEXT_CLASS *textnode; //the text assocated with the minefield
   pfGeode*   makeQuickMine();
  
 
  
public:
   //SAK.  Construct from describing PDU.
   PO_MINE_CLASS(SP_DescribeObjectVariant describeObject, int length); 
   void update(SP_DescribeObjectVariant describeObject, int length);  //SAK.
   ~PO_MINE_CLASS();

  
   void movePO(float, float, ushort);
   void dragToNet(float, float);
   void dragUpdatePO(float, float, ushort);
   void updatePDU();
   void sendPDU();

   pfGroup * make_meas();       //and build it
};




#endif


