// File: <vehicle.h>

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


#ifndef __NPS_VEH_CLASS__
#define __NPS_VEH_CLASS__

#include "entity_const.h"
#include "weapon.h"
#include "display_types.h"
#include "automateClass.h"

#ifndef NO_DVW
#define NO_PFSMOKE
#define NO_NPSSMOKE
#endif

#ifndef NO_PFSMOKE
#define NO_NPSSMOKE
#endif

enum netTypeEnum {
   disNet, hiResNet
};

typedef struct
  {
  int vid;      // Vehicle id to tether to, or pursue
  int reset;    // Flag for rigid body object to be reset
  int rigid_body; // Flag to indicate if we are using the rigid body
  float roll;
  float pitch;
  float thrust;  // Throttle settings and keyboard speed control
  float rudder;  // Added for rudder pedal functionality
  int rudder_present;  // Holds 0 if not there, 1 if present
  float sidethrust;
  float elevthrust;
  float heading;
  float current_time;
  float last_time;
  float delta_time;
  } CONTROL;

typedef struct
   {
   char     name[50];
   unsigned short dis_id;
   pfDCS    *dcs;
   pfCoord min_range;
   pfCoord max_range;
   pfCoord current_setting;
   unsigned char articulate[6]; // Booleans for X, Y, Z, H, P, R respectively
   } JNT_INFO;

typedef struct
   {
   pfCoord location[MAX_TRANSPORT];
   char comment[MAX_TRANSPORT][80];
   char filename[255];
   int current_count;
   int changed;
   } TRANSPORT_DATA;

class VEHICLE {

protected:

    // The position and orientation for the vehicle is maintained in a pfCoord
    static npsAutomate *vehAuto;
    CONTROL  cdata;
    int tethered;
    int paused;
    int targetting;
    pfCoord  posture;    // location and orientation of vehicle
    pfCoord  look;
    TRANSPORT_DATA transport_data;
    float    speed;
    float    altitude;
    float    thrust;
    pfVec3   velocity;
    pfVec3   acceleration;
    pfVec3   eyepoint;
    pfVec3   firepoint;
    pfSwitch *head_switch;
    pfSwitch *grenade_switch;
    int      num_heads;
    int      num_grenades;
    pfDCS    *weapon_dcs;
    int      num_joints;
    JNT_INFO joint[MAXJOINTS];
    float    firetimes[MAX_FIRE_INFO];
    pfDCS    *hull;
    pfSwitch *models;
    float    lastPDU;  // last time PDU sent/received for this vehicle
    pfCoord  drpos;     // computed DR position/orientation
    pfCoord  drparam;  //Dead Reckoning parameters velocity/accelerations
    EntityID disname;  // site host entity for dis
    int      icontype;      // index into the icon/vehtype array
    mtype    weapons[MAX_FIRE_INFO];
    EntityType    type;   // dis entity type
    ForceID force; //which side is it on
    unsigned int  status;  //32 bit field to discribe it's apperance
    DeadReckAlgorithm dralgo; //the dead Reckoning algorithm 
    EntityMarking marking;
    int displaying_model;
    int birds_eye;
    int snap_switch;
    int tether_model_off;
    int quiet_switch;
    int video_switch;
    netTypeEnum net_type;
    ctltype Control;
    sender_info controller;
    pfBox bounding_box;
    pfVec3 bounding_box_center;
    pfSwitch *bbox_geom;
    NPS_Input_Device input_control;
    int input_number;
    int old_trigger;
    int old_trigger_aux;
#ifndef NO_PFSMOKE
  unsigned int smoke_index, flame_index, trail_index, smoke_status, status_lock;  pfCoord old_posture, older_posture;
  void find_top(pfVec3 top);
  void find_dust_origin(pfVec3 dust_origin);
  void movePuffs(pfuSmoke *smoke, pfVec3 origin);
  void update_pfsmokes ( void );
  void setSmoke ( int the_index, int type );
#endif


public:

    int vid;    // local vehicle ID
    VEHICLE ( int, int, ForceID );
#ifndef NODUDE
    VEHICLE(int);
#endif // NODUDE
    virtual ~VEHICLE ();
    static void vehAuto_init (const char *);
    netTypeEnum get_net_type (void) {return net_type;}
    void set_net_type (netTypeEnum the_type) {net_type = the_type;}
    void snap_on ();
    void snap_off ();
    void set_look_hpr (pfVec3);
    void set_look_xyz (pfVec3);
    void del_vehicle ( void );  //public distructor

    virtual void create_entity ( int, ForceID ); //Initializes the pfDCS & pfSwitch
    virtual vtype gettype ();   // what am i
    virtual ctltype getControl (); // how am i controlled
    virtual vtype getCat ()  {return (gettype()); }// what veh category am i
    virtual vtype getSubcat () {return (gettype());}// what veh subcat am i 
    virtual void setSignal ( int ) {;}

    virtual movestat move ();// move the vehicle for the next frame
    virtual movestat moveDR ( float, float );     //move routine for DR vehicles
    virtual movestat movedead ();//crash routine
    virtual int &index ( int );
    virtual void apply_angles () {;}
    virtual void turn_on_grenades ();
    virtual void turn_off_grenades ();
    virtual void turn_on_heads ();
    virtual void turn_off_heads ();

    float *getposition ();       //return current position
    float *getorientation ();	// return currient orientation
    pfDCS *gethull ();

    void getposparm ( pfCoord *, pfVec3 * ); //stuff for firing of weapons
    EntityID getdisname ();
    EntityType get_DIS_entity_type ();
    ForceID getforce ();
    int get_tether_mode () { return tethered; }
    int get_targetting_mode () { return targetting; }
    int get_paused_mode () { return paused; }
    virtual void get_eyepoint ( pfVec3 & );
    virtual void get_firepoint ( pfVec3 & );
    virtual void get_weapon_view (pfCoord &);
    virtual void get_weapon_look (pfCoord &);
    virtual int get_video_switch () {return video_switch;}
    virtual int oktofire ( wdestype, float );
    int setfiretime ( wdestype, float );
    void basicentitystate ( EntityStatePDU * );
    virtual void sendentitystate ();
    void sendEPDU ( EntityStatePDU * );
    void updateDRparams ();
    virtual void entitystateupdate ( EntityStatePDU *, sender_info & );
    void fillinDetonationPDU ( DetonationPDU *, pfVec3 );
    void senddetonation ( pfVec3 );  //used when we crash
    virtual void changestatus ( unsigned int ); //change the status
    virtual void changeWeaponStatus ( unsigned int ); // change weapon status
    unsigned int getstatus ();
    void init_posture ( pfCoord & );
    void init_velocity ( pfVec3 );
    pfVec3 *getvelocity ();
    void assign_dralgo ( DeadReckAlgorithm );
    void assign_weapons ( mtype * );
    void assign_entity_type ( int );
    void toggle_model ();
    void teleport (pfCoord &,float &);
    void attach (pfCoord &, float &, float );
    virtual void turnoff (void);
    virtual void turnon (void);
    void bbox ( int );
    int toggle_bbox ();
    int birds_eye_view (int);
    int check_targets ( pfVec3 &, pfVec3 &, pfVec3 & );
    virtual int lase (void) {return FALSE;}
    virtual int check_collide(pfVec3 &,pfVec3 &, pfVec3 &, float &, float &);
    //friends
    friend void MUNITION::sendfire (int,VEHICLE *,int,pfCoord,pfCoord,float,int);
#ifndef NOSUB
    friend void TORPEDO_SL::sendfire(int,VEHICLE *,int,PASS_DATA *,float,int); 
    friend void MISSILE_SL::sendfire(int,VEHICLE *,int,PASS_DATA *,float,int);
#endif // NOSUB
    friend void acquire_veh_info ( INFO_PDATA * );
//    friend int motion_Medic ( void *, IPORT_DI * );

    // New input related functions for use with input_manager
    // Functions defined in vehicle_input.cc
    virtual void collect_inputs ();
    virtual int set_input_control ( const NPS_Input_Device,
                                    const int new_number = 0 );

    virtual void cycle_input_control ();
    virtual void process_keyboard_input ();
    virtual void process_keyboard_speed_settings ();
    virtual void process_fcs_input ();
    virtual void process_kaflight_input ();
    virtual void process_flybox_input ();
    virtual void process_leather_flybox_input ();
    virtual void process_basic_panel_input ();
    virtual void process_spaceball_input ();
#ifndef NO_IPORT
    virtual void process_testport_input ();
    virtual void process_uniport_input ();
#endif // NO_IPORT
    virtual void process_fakespace_input();
    virtual void process_lws_input ();
    virtual void process_odt_input ();
#ifndef NOUPPERBODY
    virtual void process_upperbody_input ();
#endif // NOUPPERBODY
    virtual void hi_res_rifle_on () {;}
    virtual void iport_rifle_on () {;}
    virtual void jack_rifle_on () {;}
    virtual void change_thrust ( const float );
    virtual void set_thrust ( const float );
    virtual void change_horizontal ( const float );
    virtual void set_horizontal ( const float );
    virtual void change_vertical ( const float );
    virtual void set_vertical ( const float );
    virtual void change_rudder ( const float );
    virtual void set_rudder ( const float );
    virtual void change_altitude ( const float );
    virtual void set_altitiude ( const float );
    virtual void change_gaze_vertical ( const float );
    virtual void set_gaze_vertical ( const float );
    virtual void change_gaze_horizontal ( const float );
    virtual void set_gaze_horizontal ( const float );
    virtual void fire_weapon ( const wdestype, const int num_times = 1 );

    virtual int find_joint ( unsigned short, JNT_INFO ** );
    virtual void update_joint ( JNT_INFO *, ArticulatParams * );
    virtual void update_articulations ( ArticulatParams *, int );

    virtual void class_specific_keyboard () {;}
    virtual void class_specific_fcs () {;}
    virtual void class_specific_kaflight () {;}
    virtual void class_specific_flybox () {;}
    virtual void class_specific_spaceball () {;}
#ifndef NO_IPORT
    virtual void class_specific_testport () {;}
    virtual void class_specific_uniport () {;}
#endif // NO_IPORT
    virtual void class_specific_lws () {;}
#ifndef NOUPPERBODY
    virtual void class_specific_upperbody () {;}
#endif // NOUPPERBODY
    virtual void class_specific_basic_panel() {;}

    void update_time ();
    virtual void fill_in_pass_data ( PASS_DATA * );
    virtual void process_state(int,int);
    virtual void process_input_control(int,int);
    virtual void process_birdseye(int,int);
    virtual void process_tether(int,int);
    virtual void process_pause(int,int);
    virtual void process_transport(int,int);
    virtual void process_targetting(int,int);


    virtual int transport ( float, float, float );
    virtual float get_altitude();



};

void vehicle_hud_draw ( pfChannel *, void *);

#endif
