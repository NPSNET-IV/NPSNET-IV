

#ifndef GLOBAL_H
#define GLOBAL_H

//COMMS------------------------------------------------
//#include <idunetlib.h>
//#include <idu.h>      
//COMMS------------------------------------------------

typedef struct {
  
  //the following field are used to communicate data from NPSNET to SHIPCONTROL
  float ship_course;   //actual ,000
  float ship_speed;    //actual ,00
  float ship_rudder_angle;    //actual ,00

  float ship_ordered_rudder_angle; // 000
  float ship_ordered_rpm; // 000
  float ship_ordered_port_bell;  //-3 - 5 (back full - flank)
  float ship_ordered_stbd_bell;  //-3 - 5 (back full - flank)

  //the following field represent the different stations communicating
  //from SHIPCONTROL to NPSNET.  

  int station_id;  // identifies which type of station so other field are ignored.

  //ood station
  float ood_rudder_angle;  //ordered
  float ood_rpm;   //ordered
  float ood_port_bell;   //ordered
  float ood_stbd_bell;   //ordered

}SHIP_MSG_DATA;

#endif
