


typedef struct {
  
  //the following field are used to communicate data from NPSNET to SUBCONTROL
  float sub_depth;    //actual
  float sub_course;   //actual
  float sub_speed;    //actual

  float sub_rudder_angle;    //actual
  float sub_sternplns_angle; //actual
  float sub_fwtrplns_angle;  //actual

  float sub_ordered_depth;
  float sub_ordered_course;
  float sub_ordered_speed;

  float sub_embtblow_onoff;
  float sub_mbtvents_openshut;

  float sub_scope_raiselower;
  float sub_scope_by;    //bearing that scope is looking at when mark is hit.
  float sub_scope_rh;    //range to object that scope is looking at when mark 
                         //is hit.

  float sub_selecttube_2or4;
  float sub_selecttube_1or3;
  float sub_selecttasm;

  //the following field represent the different stations communicating
  //from SUBCONTROL to NPSNET.  

  int station_id;  // identifies which type of station so other field are ignored.

  //helm station
  float helm_speed;

  //ood station
  float ood_depth;   //ordered
  float ood_course;  //ordered
  float ood_speed;   //ordered

  float ood_embtblow_onoff;
  float ood_mbtvents_openshut;

  float ood_scope_raiselower;
  float ood_scope_updown;
  float ood_scope_leftright;
  float ood_scope_mark;

  //weaps station
  float weaps_select_tube_2or4;
  float weaps_select_tube_1or3;

  float weaps_course_port;
  float weaps_shoot_port;
  float weaps_course_stbd;
  float weaps_shoot_stbd;
  float weaps_select_tasm;
  float weaps_shoot_tasm;

}SUB_MSG_DATA;
