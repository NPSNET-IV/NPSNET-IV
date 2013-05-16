

#ifndef GLOBAL_H
#define GLOBAL_H


typedef struct {
  
  //the following field are used to communicate data from NPSNET to SUBCONTROL
  float sub_depth;    //actual ,0000
  float sub_course;   //actual ,000
  float sub_speed;    //actual ,00

  float sub_rudder_angle;    //actual ,00
  float sub_sternplns_angle; //actual ,00
  float sub_fwtrplns_angle;  //actual ,00

  float sub_ordered_depth;  // 0000
  float sub_ordered_course; // 000
  float sub_ordered_speed;  //-3 - 4 (back full - flank)
  float sub_helm_ordered_speed;  //-3 - 4 (back full - flank)

  float sub_embtblow_onoff;   //1 0
  float sub_mbtvents_openshut; //1 0

  float sub_scope_raiselower;  //1 -1
  float sub_scope_by;    //bearing that scope is looking at when mark is hit.
  float sub_scope_rh;    //range to object that scope is looking at when mark 
                         //is hit.

  float sub_selecttube_2or4; //1 0
  float sub_selecttube_1or3; //1 0
  float sub_selecttlam;      //1 - 6

  //the following field represent the different stations communicating
  //from SUBCONTROL to NPSNET.  

  int station_id;  // identifies which type of station so other field are ignored.

  //helm station
  float helm_speed;  //-3 - 4 (back full - flank)

  //ood station
  float ood_depth;   //ordered
  float ood_course;  //ordered
  float ood_speed;   //ordered

  float ood_embtblow_onoff;    //1 0
  float ood_mbtvents_openshut; //1 0

  float ood_scope_raiselower;  //1 -1
  float ood_scope_updown;      //1 -1
  float ood_scope_leftright;   //1 -1
  float ood_scope_mark;        //1

  //weaps station
  float weaps_select_tube_2or4;  //1 0
  float weaps_select_tube_1or3;  //1 0

  float weaps_course_port;
  float weaps_shoot_port;
  float weaps_course_stbd;
  float weaps_shoot_stbd;
  float weaps_select_tlam;
  float weaps_shoot_tlam;

}SUB_MSG_DATA;

//COMMS------------------------------------------------
#include <idunetlib.h>
#include <idu.h>      
//COMMS------------------------------------------------

#endif
