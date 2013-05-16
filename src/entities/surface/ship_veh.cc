
#include <math.h>
#include <string.h>
#include <bstring.h>
#include <fstream.h>

#include "ship_veh.h"
#include "person.h"
#include "common_defs2.h"
#include "appearance.h"
#include "sound.h"
#include "effects.h"
#include "nps_smoke.h"
#include "netutil.h"
#include "terrain.h"
#include "munitions.h"
#include "interface_const.h"
#include "collision_const.h"
#include "local_snd_code.h"

#ifndef NONPSSHIP
#include "fileio.h"

#include "im.tab.h"
#include "imstructs.h"
#include "imclass.h"
#include "imdefaults.h"
#endif

#define DEG_TO_RAD 0.017453292f
#define RAD_TO_DEG 57.29577951f
#define HALFPI M_PI/2.0f
#define METER_TO_INCH 39.370079 
#define INCH_TO_METER 0.0254f
#define MILE_TO_KILOMETER 1.6093471f
#define METER_TO_FOOT 3.2808f
#define MPS_TO_KNOTS 1.94f
#define MAX_OBJECTS 250
#define REAL_SMALL 0.0001f
#define SMALL 0.5
#define UPDATETIME 5.0f
#define IS_ZERO(f)   ((f > -REAL_SMALL) && (f < REAL_SMALL))

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;
extern          VEH_LIST     *G_vehlist;
extern          VEHTYPE_LIST *G_vehtype;
extern          pfGroup      *G_fixed;
extern          pfGroup      *G_moving;
extern volatile int           G_drivenveh;
extern volatile float         G_curtime;
#ifndef NONPSSHIP
extern          InitManager  *initman;
#endif

static pfVec3 ZeroVec = {0.0f,0.0f,0.0f};

//********************************************************************//
//****************** class SHIP_VEH **********************************//
//********************************************************************//

SHIP_VEH::SHIP_VEH(int id, int atype, ForceID the_force)
:VEHICLE(id,atype,the_force)
{
 
   //initialize internal values
   portRPM = stbdRPM = rudderAngle = 0.0f;
   //mass = 7262402.64;
   thrustPerRPM = 2509288.65; 
   propOffset = 8.0;
   rudderOffset = 40.0;
   momentInertia = 40800100000.25;

   ordered_rudder_angle = 0.0f;
   ordered_rpm = 0.0f;
   ordered_port_bell = 0.0f;
   ordered_stbd_bell = 0.0f;
   keyboard_order = FALSE;

#ifndef NONPSSHIP
   char  mfile[200];
   dralgo = DRAlgo_DRM_FPW;
   VEHTYPE_LIST *vtypea = &G_vehtype[atype];
   sprintf(mfile, "./models/SurfaceVeh/%s.dat.nps",vtypea->name);
   ifstream infile(mfile);
   infile>>mass>>bridgeview[0]>>bridgeview[1]>>bridgeview[2];
   infile.close();

   // initialize all mounted veh's to NULL
   for ( int ix = 0; ix < MAX_VEH; ix++ ){
      mounted_veh[ix] = NULL;
   }

 if (id == G_drivenveh && G_dynamic_data->control == NPS_SHIP_PANEL) {

   // Added so class can control panel device 
   input_control = NPS_SHIP_PANEL;

 //COMMS-----------------------------------
 // Multicast Defaults
   int panel_multicast = TRUE;
   
   u_short port = 0;
   char group[STRING32];
   u_char ttl = IDU_DEF_MC_TTL;
   int loopback = FALSE;
   char net_interface[20];
   union query_data local;

   //strncpy ( group, IDU_DEF_MC_GROUP,25 );   //COMMS
   strcpy ( net_interface, "" );             //COMMS
   
   initman->query(PANEL_PORT, &local, GET);
   port = (u_short) local.ints[0];
   
   initman->query(PANEL_TTL, &local, GET);
   ttl = (u_char) local.ints[0];
   
   initman->query(PANEL_GROUP, &local, GET);
   strncpy ( group, local.string, STRING32 );
   
   if ( panel_multicast ) {
      if ( port == 0 ) port = IDU_DEF_MC_PORT;
      net = new IDU_net_manager ( group, port, ttl, net_interface,
                                  loopback );
   }
   else {
      if ( port == 0 ) port = IDU_DEF_BC_PORT;
      net = new IDU_net_manager ( port, net_interface, loopback );
   }

   if ( !net->net_open() ) {
      cerr << "Could not open network." << endl;
      exit(0);
   }
   else {
      net->add_to_receive_list(Test_Type);
      net->add_to_receive_list(SHIP_Ood_To_NPSNET_Type);
   }

   cerr << endl << "Network open for ship control:"<< endl;
   if ( panel_multicast ) {
      cerr << "\tMode:     \tMulticast" << endl;
      cerr << "\tPort:     \t" << (int)port << endl;
      cerr << "\tGroup:    \t" << group << endl;
      cerr << "\tTTL:      \t" << (int)ttl << endl;
      cerr << "\tInterface:\t" << net_interface << endl;
      cerr << "\tLoopback: \t";
      if ( loopback )
         cerr << "ON" << endl;
      else
         cerr << "OFF" << endl;
      cerr << endl;
   }
   else {
      cerr << "\tMode:     \tBroadcast" << endl;
      cerr << "\tPort:     \t" << (int)port << endl;
      cerr << "\tInterface:\t" << net_interface << endl << endl;
      cerr << "\tLoopback: \t";
      if ( loopback )
         cerr << "ON" << endl;
      else
         cerr << "OFF" << endl;
      cerr << endl;
   }
  //COMMS------------------------------------------------------
 } //id == G=drivenveh
 else 
#endif
      {
    net = NULL;
 }
}//SHIP_VEH Constructor

SHIP_VEH::~SHIP_VEH()
{
// cerr << "Ship - destructor\n";
#ifndef NONPSSHIP
   if (vid == G_drivenveh && net) {
      net->net_close();
   }

   for ( int ix = 0; ix < MAX_VEH; ix++ ){
      if (mounted_veh[ix] != NULL) {
         ((PERSON_VEH *)mounted_veh[ix])->dismount();
         mounted_veh[ix] = NULL;
      }
   }
#endif
}


//Convert bell orders to an appropriate rpm
float bell_to_rpm(int bell)
{
   float rpm;
         switch(bell){
            case -3:  //back full bell (-100 rpms)
                rpm = -100.0f;
                break;
            case -2:  //back 2/3 bell (-66 rpms)
                rpm = -66.0f;
                break;
            case -1:  //back 1/3 bell (-33 rpms)
                rpm = -33.0f;
                break;
            case 0:  //All Stop (0 rpms)
                rpm = 0.0f;
                break;
            case 1:  //1/3 bell (38 rpms)
                rpm = 38.0f;
                break;
            case 2:  //2/3 bell (77 rpms)
                rpm = 77.0f;
                break;
            case 3:  //std bell (115 rpms)
                rpm = 115.0f;
                break;
            case 4:  //full bell (172 rpms)
                rpm = 172.0f;
                break;
            case 5:  //flk bell (230 rpms)
                rpm = 230.0f;
                break;
            default:
                rpm = -99.0f;
                break;
         } // end of switch
   return rpm;
}


movestat SHIP_VEH::move()
// move the vehicle for the next frame
{
#ifdef TRACE
   cerr << "CLASS TRACE:\tSHIP_VEH::move" << endl;
   cerr << "Moving a Sea Vehicle\n";
#endif

   pfMatrix orient;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 up = { 0.0f, 0.0f, 1.0f };
   pfVec3 oldpos;
   float speed_setting;

#ifndef NONPSSHIP
   int indicatedRPM = 0;
   float orderedRPM = 0.0f;
   float dt = 0.1f;
   static NPSNETToShipIDU       NPSNET2Shipidu;
   static int counter = 0;
#endif

   pfCopyVec3(oldpos,posture.xyz);

   update_time ();

//#ifndef NONPSSHIP
   if (input_control == NPS_SHIP_PANEL) {
   //TRY to READ net idu from control panel 
   if (net->read_idu(&idu, &type, idu_sender_info, swap_bufs) != NULL) {
      switch(type)
           {
           case (Test_Type):
             testidu = (TestIDU *)idu;
             cerr<<"Got a Test_type idu"<<endl;
             break;
           case (SHIP_Ood_To_NPSNET_Type):
             oodidu = (OodToNPSNETIDU *)idu;
             oodupdate(oodidu);
             break;
           default:
             //cerr << "\nUnknown type received: " << int(type)
             //     << endl << endl;
             break;
           } // end switch
   }// end if read_idu
   }
//#else
   else {
        speed_setting = cdata.thrust * MAX_SPEED;
   }
//#endif

   collect_inputs ();

   if(!(paused || tethered || cdata.rigid_body))
      {
#ifndef NONPSSHIP
      // Determine if using manuvering combinations (999)
      indicatedRPM = (int)ordered_rpm;
      if (ordered_rpm > 230.0f) {
         ordered_rpm = bell_to_rpm(int(ordered_port_bell));
         orderedRPM = bell_to_rpm(int(ordered_stbd_bell));
      }
      else if (ordered_port_bell==ordered_stbd_bell) {
              orderedRPM = ordered_rpm;
           }
           else {
              ordered_rpm = bell_to_rpm(ordered_port_bell);
              orderedRPM = bell_to_rpm(int(ordered_stbd_bell));
           }

//cerr<<"rpms P:"<<ordered_rpm<<" S:"<<orderedRPM<<endl;
      // Update engine & rudder to match ordered engins & rudder
      // using cdata.thrust to indicate engine combined orders
      if(portRPM != ordered_rpm)
	{
	   if(portRPM < ordered_rpm)
	      portRPM += 1.0f;
	   else
	      portRPM -= 1.0f;
	 }
    
       if(stbdRPM != orderedRPM)
	 {
	   if(stbdRPM < orderedRPM)
	      stbdRPM += 1.0f;
	   else
	      stbdRPM -= 1.0f;
	 }

       if(rudderAngle != ordered_rudder_angle)  
	 {
	   if(rudderAngle < ordered_rudder_angle)
	      rudderAngle += 0.1;
	   else 
	      rudderAngle -= 0.1f;
	 }

       // Set Thrust value to amount of ordered rudder, only when using
       // realistic ship movements
       thrust = ordered_rudder_angle / 30.0f;

       thrustLeft = thrustPerRPM * portRPM; 
       thrustRight = thrustPerRPM * stbdRPM;
       thrustTotal = thrustLeft + thrustRight;

       torqueLeft = thrustLeft * (-propOffset);
       torqueRight = thrustRight * propOffset;

       //rudderAngle stored as degrees, but used as radians
       torqueRudder = (thrustTotal/3.0 * sin(DEG_TO_RAD*(-rudderAngle)))
                       * rudderOffset;

       torqueTotal = torqueLeft + torqueRight + torqueRudder;

       //Find the angular acceleration using Newton's Second Law
       alpha = (double)torqueTotal/momentInertia;

       //Find the angular velocity
       omega = alpha * dt; 

       //Find the acceleration
       linacceleration = thrustTotal/mass;

       //Find the speed of the ship
       linspeed = linacceleration * dt; speed = linspeed; 

//      cerr<<"\t\t\tomega = "<<omega<<"\r";
    #ifdef TRACE
      cerr << "GRND_ORIENT POSTURE X = " << posture.xyz[X] << "  Y = "
           << posture.xyz[Y] << "  Z = " << posture.xyz[Z] << endl;
      cerr << "                    H = " << posture.hpr[X] << "  P = "
           << posture.hpr[Y] << "  R = " << posture.hpr[Z] << endl;

      cerr<<"portRPM = "<<portRPM<<endl;         
      cerr<<"stbdRPM = "<<stbdRPM<<endl;
      cerr<<"thrustLeft = "<<thrustLeft<<endl;         
      cerr<<"thrustRight = "<<thrustRight<<endl;
      cerr<<"thrustTotal = "<<thrustTotal<<endl;         
      cerr<<"torqueLeft = "<<torqueLeft<<endl;
      cerr<<"torqueRight = "<<torqueRight<<endl;
      cerr<<"torqueRudder = "<<torqueRudder<<endl;         
      cerr<<"torqueTotal = "<<torqueTotal<<endl;
      cerr<<"rudder angle = "<<rudderAngle<<endl;
      cerr<<"alpha = "<<alpha<<endl;         
      cerr<<"omega = "<<omega<<endl;
      cerr<<"linaccell = "<<linacceleration<<endl;
      cerr<<"linspeed = "<<linspeed<<endl;
      cerr<<"deltaTime = "<<cdata.delta_time<<endl;
    #endif

#else
      // If using flight controls, speed must be controlled different because
      // the throttle remains at a set location. While using the keyboard,
      // you only press a key at add or subtract speed
      if ((input_control == NPS_FCS) || (input_control == NPS_KAFLIGHT) || 
          (input_control == NPS_BASIC_PANEL) || (input_control == NPS_FLYBOX) )
         {
         if (speed < speed_setting)
            speed += FCS_SPEED_CHANGE;
         if (speed > speed_setting)
            speed -= FCS_SPEED_CHANGE;
         cdata.roll = cdata.pitch = 0.0f;
         }
      else
         {
         speed += cdata.thrust;
         if ((speed < KEY_SPEED_CHANGE) && (speed > NO_SPEED))
            speed -= 1.0f;
         cdata.thrust = 0.0f;
         }

      posture.hpr[HEADING] += cdata.roll;
      //Zero the control data inputs
      cdata.roll = 0;
#endif

      if (posture.hpr[HEADING] < 0.0f)
         posture.hpr[HEADING] += 360.0f;
      else if (posture.hpr[HEADING] > 360.0f)
              posture.hpr[HEADING] -= 360.0f;

      // Don't want to exceed the max speed
      if (speed > MAX_SPEED)
         speed = MAX_SPEED;
      // Don't want to go backwards too fast
      if (speed < MAX_REVERSE)
         speed = MAX_REVERSE;

      // Now plant the ship back on the surface and
      // make sure we are still on the water, if not, stop.
      if (!water_orient(posture))
         {
         grnd_orient(posture);
         speed = 0.0f;
         pfCopyVec3(velocity, ZeroVec);
         }
      else  // Still in bounds
         {
         // Calculate the new velocity vector
         pfMakeEulerMat ( orient, posture.hpr[HEADING],
                                  posture.hpr[PITCH],
                                  posture.hpr[ROLL]);

         pfFullXformPt3 ( direction, direction, orient );
         pfNormalizeVec3 ( direction );
         pfScaleVec3 ( velocity, speed, direction);
         }
 
      // Move to the location we should be at after the last time interval
      posture.xyz[X] += velocity[X]*cdata.delta_time;
      posture.xyz[Y] += velocity[Y]*cdata.delta_time;
      posture.xyz[Z] += velocity[Z]*cdata.delta_time;

#ifndef NONPSSHIP
      // This is done after this frame update to simulate delayed reaction
      posture.hpr[HEADING] += (RAD_TO_DEG * (omega * cdata.delta_time));
#endif

#ifdef TRACE
      cerr << "GRND_ORIENT POSTURE X = " << posture.xyz[X] << "  Y = "
           << posture.xyz[Y] << "  Z = " << posture.xyz[Z] << endl;
      cerr << "                    H = " << posture.hpr[X] << "  P = "
           << posture.hpr[Y] << "  R = " << posture.hpr[Z] << endl;
#endif
 
      altitude = 0.0f;
      // Now see if we hit something in this time interval
      if (!G_static_data->stealth)
         {
         if ( check_collide(oldpos, posture.xyz,
                            posture.hpr,
                            speed, altitude) )
            return(HITSOMETHING);
         }

      } // End !paused !tethered !rigid_body

   // If tethered, let the other vehicle calculate our position for us
   if (tethered)
      {
      if ((cdata.vid > -1) && G_vehlist[cdata.vid].vehptr)
         {
         if ( tethered == 1)
            G_vehlist[cdata.vid].vehptr->teleport(posture,speed);
         else if ( tethered == 2)
            G_vehlist[cdata.vid].vehptr->attach(posture,speed,
                                                G_dynamic_data->attach_dis);
         }
      else
         {
#ifdef DATALOCK
         pfDPoolLock ( (void *) G_dynamic_data );
#endif
         tethered = FALSE;
#ifdef DATALOCK
         pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         }
      }

/*
   // This segment of code is a hook for the implementing the
   //  rigid_body class for vehicle updates
   if (cdata.rigidbody)
      {
      if (cdata.reset)
         // make call to set rigid_body parameters to the pdata values.
      // do the normal updates to pdata
      }
*/

   pfMakeEulerMat ( orient, posture.hpr[HEADING],
                            posture.hpr[PITCH],
                            posture.hpr[ROLL]);

   pfFullXformPt3 ( up, up, orient );
   pfNormalizeVec3 ( up );
   pfScaleVec3 ( up, EYE_OFFSET, up);
   pfAddVec3 ( eyepoint, posture.xyz, up );
   pfCopyVec3 ( firepoint, eyepoint );

   altitude = ::get_altitude(posture.xyz);
   if ( (!tethered) && (!G_static_data->stealth) && (altitude < -0.1) )
      {
#ifdef DATALOCK
      pfDPoolLock ( (void *) G_dynamic_data );
#endif
//      G_dynamic_data->cause_of_death = UNDER_GROUND_DEATH;
//      G_dynamic_data->dead = DYING;
      posture.xyz[PF_Z] -= altitude;
#ifdef DATALOCK
      pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
      }

   check_targets ( posture.xyz, posture.hpr, 
                   look.hpr );

   //put the vehicle in the correct location in space
   pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
   pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);

#ifdef DEBUG_SCALE
   pfDCSScale(hull,2.0f);
#endif

#ifndef NONPSSHIP
   //COMMS--------------------------------------------------------
   //send data to update if enough time has elapsed

   //increment counter
   counter++;

   if((input_control == NPS_SHIP_PANEL) && (counter > 40 || keyboard_order)) {
      counter=0;
      //update NPSNET2Shipidu
  
      if(posture.hpr[HEADING] < 0.0)
         NPSNET2Shipidu.ship_course = -posture.hpr[HEADING];
      else
         NPSNET2Shipidu.ship_course = 360.0 - posture.hpr[HEADING];

      NPSNET2Shipidu.ship_speed = speed;
      NPSNET2Shipidu.ship_rudder_angle = RAD_TO_DEG * omega; //actual ,00

      NPSNET2Shipidu.ship_ordered_rudder_angle = ordered_rudder_angle;  // 0000
      NPSNET2Shipidu.ship_ordered_rpm = indicatedRPM; // 000
      NPSNET2Shipidu.ship_ordered_port_bell = ordered_port_bell;  //-3 - 5 (back full - flank)
      NPSNET2Shipidu.ship_ordered_stbd_bell = ordered_stbd_bell;  //-3 - 5 (back full - flank)
      NPSNET2Shipidu.space_holder = keyboard_order; // keyboard pressed
      keyboard_order = FALSE;

      if( !(net->write_idu((char *)&NPSNET2Shipidu,NPSNET_To_SHIP_Type)) )
          cerr << "ERROR:\tCould not send NPSNET to SHIP IDU." << endl;
   }// end if counter
#endif

   return(FINEWITHME);
}


vtype SHIP_VEH::gettype()
{
    return (SHIP);
}


//Dead reckoning function for ships. Same as generic npsnet
//function but takes into account changes in rudder angle
movestat SHIP_VEH::moveDR(float deltatime,float curtime)
{

  #ifdef VEHICLE_DR
    cerr<<"moveDR called for vid "<<vid<<endl;
  #endif

  float accelfac;          // 1/2*T used for accelerations
  float speed;
  static char text[100];
  static float oldspeed = 0.0f;
#ifndef NONPSSHIP
  static float oldrudang = 0.0f;
#endif

  #ifdef TRACE
   cerr << "Dead Recon a SHIP_VEH Class Vehicle\n";
  #endif

#ifndef NO_PFSMOKE
   update_pfsmokes();
#endif

  delTime = deltatime;

  if ( ((curtime - lastPDU) > G_static_data->DIS_deactivate_delta ) && 
       (vid != G_drivenveh))
  {
    //Deactivate the puppy
//    cerr <<"It has been to long since the last update VEHICLE\n";
    //not us, so we should deactiveate this vehcile
    return(DEACTIVE);
  }

  switch(dralgo)
  {
    case DRAlgo_Static: //it is fixed, so it does not move
      break;
    case DRAlgo_DRM_FPW: //Fixed orientation,Constant Speed
      drpos.xyz[X] += drparam.xyz[X]*deltatime;
      drpos.xyz[Y] += drparam.xyz[Y]*deltatime;
      drpos.xyz[Z] += drparam.xyz[Z]*deltatime;
      break;
    case DRAlgo_DRM_FVW: //Constant rotation,varable velocity
      accelfac = 0.5f*deltatime*deltatime;
      drpos.xyz[X] += drparam.xyz[X]*deltatime+accelfac*drparam.hpr[X];
      drpos.xyz[Y] += drparam.xyz[Y]*deltatime+accelfac*drparam.hpr[Y];
      drpos.xyz[Z] += drparam.xyz[Z]*deltatime+accelfac*drparam.hpr[Z];
      drparam.xyz[X] += accelfac * drparam.hpr[X];
      drparam.xyz[Y] += accelfac * drparam.hpr[Y];
      drparam.xyz[Z] += accelfac * drparam.hpr[Z];
      break;
    //I don't have a clue how to do the others, so we will default 
    case DRAlgo_Other:
    case DRAlgo_DRM_RPW:
    case DRAlgo_DRM_RVW:
    case DRAlgo_DRM_FPB:
    case DRAlgo_DRM_RPB:
    case DRAlgo_DRM_RVB:
    case DRAlgo_DRM_FVB:
    default:
      //move the default vehicle type in a stright line
      drpos.xyz[X] += velocity[X]*deltatime;
      drpos.xyz[Y] += velocity[Y]*deltatime;
      drpos.xyz[Z] += velocity[Z]*deltatime;
      break;
  }

  //Repositin the other vehicles in the list
  if (vid != G_drivenveh)
  {
#ifndef NONPSSHIP
     //rotate ship based upon ship's angular velocity
     drpos.hpr[HEADING] += (RAD_TO_DEG * (omega * deltatime));
//cerr<<"\t\tomega:"<<omega<<" heading:"<<posture.hpr[HEADING]<<"\r";
#endif //NONPSHIP
     posture = drpos;
     //put the vehicle in the correct location in space
     pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y],posture.xyz[Z]);
     pfDCSRot(hull,posture.hpr[HEADING],posture.hpr[PITCH], posture.hpr[ROLL]);
  #ifdef VEHICLE_DR
     cerr<<" vid "<<vid<<" translated"<<endl;
     cerr<<"Posture x coord is "<<posture.xyz[X]<<endl;
     cerr<<"Posture y coord is "<<posture.xyz[Y]<<endl;
     cerr<<"Posture z coord is "<<posture.xyz[Z]<<endl;
     cerr<<"Posture heading is "<<posture.hpr[HEADING]<<endl;
  #endif
  }
  //Check to see if we need to send an entity state PDU  
  else if ( !G_static_data->stealth )
  {
    speed = pfLengthVec3(velocity);

#ifndef NONPSSHIP
    //This is added to give the ship a wake behind it
    if (speed > 0) {
       status |= Appearance_TrailingEffectsSmall;
    }
    else {
       status &= (~Appearance_TrailingEffects_Mask);
    }
#endif //NONPSSHIP

    //this is us, have we exceeded the DR error?
    if((G_static_data->DIS_heartbeat_delta < (curtime - lastPDU)) || 
      (delta_send(&posture,&drpos))) 
    {

  #ifdef DEBUG_CERR
       cerr <<"X       " << posture.xyz[HEADING] << " " <<drpos.xyz[HEADING] << "\n";
       cerr <<"Y       " << posture.xyz[PITCH] << " " <<drpos.xyz[PITCH] << "\n";
       cerr <<"Z       " << posture.xyz[ROLL] << " " <<drpos.xyz[ROLL] << "\n";
       cerr <<"HEADING " << posture.hpr[HEADING] << " " <<drpos.hpr[HEADING] << "\n";
       cerr <<"PITCH   " << posture.hpr[PITCH] << " " <<drpos.hpr[PITCH] << "\n";
       cerr <<"ROLL    " << posture.hpr[ROLL] << " " <<drpos.hpr[ROLL] << "\n";
       cerr << "send a delta pdu\n";
       cerr << "acceleration" << acceleration[X] << " " << acceleration[Y]
                              << " " << acceleration[Z] << endl;
  #endif
       sendentitystate();
       oldspeed = speed;
       //lastPDU = curtime;
       if ( G_static_data->pdu_position && (G_static_data->pdupos_fd >= 0) )
          {
          sprintf ( text, "%8.2f %8.2f %8.2f %7.1f %7.1f %7.1f %14.2f\n",
                    posture.xyz[X], posture.xyz[Y], posture.xyz[Z],
                    posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL],
                    curtime );
          if (write (G_static_data->pdupos_fd, text, strlen(text)) == -1)
             {
             cerr << "Write to PDU position file failed.  Closing file.\n";
             close ( G_static_data->pdupos_fd );
             G_static_data->pdupos_fd = -1;
             }
          }
    }

    else if ( speed != oldspeed 
#ifndef NONPSSHIP
              || rudderAngle != oldrudang
#endif //NONPSSHIP
            )
       {
       if ( (!IS_ZERO(oldspeed)) && (IS_ZERO(speed)) )
          {
          sendentitystate();
          oldspeed = speed;
          }
       else if ( (IS_ZERO(oldspeed)) && (!IS_ZERO(speed)) )
          {
          sendentitystate();
          oldspeed = speed;
          //lastPDU = curtime;
          }
            // Send PDU's if Speed changes by 1/2 a Knot
       else if ( fabs(oldspeed-speed)*MPS_TO_KNOTS > 0.5f )  
          {
          sendentitystate();
          oldspeed = speed;
          //lastPDU = curtime;
          }
#ifndef NONPSSHIP
       else if ( rudderAngle != oldrudang )
          {  //Send PDU if rudder angle differs by 1.0 degrees
          if ( fabs(oldrudang-rudderAngle) > 1.0f )
             {
             sendentitystate();
             oldrudang = rudderAngle;
             }
          }
#endif //NONPSSHIP
       }
  }

#ifndef NONPSSHIP
  // Now that we are in place, update all mounted vehicles to proper location
  for ( int ix = 0; ix < MAX_VEH; ix++ ) {
     if (mounted_veh[ix] != NULL) {
        ((PERSON_VEH *)mounted_veh[ix])->reset_on_mounted_vid();
     }
  }// for loop
#endif // NONPSSHIP

  return(FINEWITHME);
}


movestat SHIP_VEH::movedead()
{
  float gnd_elev = ::get_altitude(posture.xyz);
  static float dead_time;
  static float grnd_slam_time;
  //static int hit;
  static int my_status;
  static int cause;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tVEHICLE::movedead" << endl;
#endif

   update_time ();
   collect_inputs();

#ifdef DATALOCK
  pfDPoolLock ( (void *) G_dynamic_data );
#endif
  my_status = G_dynamic_data->dead;
  cause = G_dynamic_data->cause_of_death;
#ifdef DATALOCK
  pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

  if ( my_status == DYING )
     {
     if ( G_static_data->sound )
        {
        LocationRec dummy_snd_pos;
        dummy_snd_pos.x = 0.0;
        dummy_snd_pos.y = 0.0;
        dummy_snd_pos.z = 0.0;
        if ( cause == MUNITION_DEATH )
           playsound(&dummy_snd_pos, &dummy_snd_pos, HIT_INDEX,-1);
        else if ( cause == FALLING_DEATH )
           playsound(&dummy_snd_pos, &dummy_snd_pos, FALLING_INDEX,-1);
        else
           playsound(&dummy_snd_pos, &dummy_snd_pos, DYING_INDEX,-1);
        }

        grnd_slam_time = G_curtime;
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        G_dynamic_data->dead = GRNDDEAD;
        my_status = GRNDDEAD;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        status = Appearance_DamageDestroyed | Appearance_SmokePlume;
        changestatus(status);
        pfCopyVec3(velocity,ZeroVec);
        sendentitystate();
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        cause = G_dynamic_data->cause_of_death; 
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        if ( cause != MUNITION_DEATH )
           senddetonation(posture.xyz);
        if ( cause == GROUND_DEATH )
           makecrater_pos(posture.xyz);
        use_next_fire_plume(posture.xyz[X], posture.xyz[Y],
                            posture.xyz[Z], 20.0f );
        makefireball(posture.xyz);
     }

  if ( my_status == GRNDDEAD )
     {
     posture.xyz[Z] = -15.0f;  // Sink the ship
     if ( grnd_slam_time + GROUND_SLAM_DELAY < G_curtime )
        {
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        G_dynamic_data->dead = DEAD;
        my_status = DEAD;
        dead_time = G_curtime;
        G_dynamic_data->cause_of_death = 0;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        }
     }
  else
     {
     if (dead_time + DEAD_TIME_DELAY < G_curtime)
        {
        bzero ( (void *)&(look), sizeof(pfCoord) );
        pfCopyVec3 ( posture.hpr, 
                     (float *)G_static_data->init_location.hpr );
        G_vehlist[vid].vehptr->
              transport(G_static_data->init_location.xyz[PF_X],
                        G_static_data->init_location.xyz[PF_Y],
                        G_static_data->init_location.xyz[PF_Z] );
	
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        G_dynamic_data->dead = ALIVE;
        my_status = ALIVE;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        status = ALIVE;
        changestatus(status);


        }
     }


   altitude = ::get_altitude(posture.xyz);

  //put the vehicle in the correct location in space
  pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
  pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);
     

  return FINEWITHME;
}



int SHIP_VEH::check_collide(pfVec3 &startpnt, pfVec3 &endpnt,
                                   pfVec3 &orientation,
                                   float &speed, float &altitude)
{
   long     isect;
   pfSegSet seg;
   pfHit    **hits[32];
   long     flags;
//   pfNode   *node;
   pfGeoSet *gset;
   pfVec3   normal;
   pfVec3   pnt;
   pfMatrix xform;
   int hit_index;
   pfSeg *sp[6] ;
   //pfCylinder the_barrel;
   long the_mask;
   pfVec3 incoming_vec;
   pfVec3 outgoing_vec;
   pfVec3 normal_vec;
   pfVec3 eye_vec, foot_vec, beam_vec;
   pfVec3 start_eye, end_eye;
   pfVec3 start_pos, end_pos;
   pfVec3 start_port, end_port;
   pfVec3 start_stbd, end_stbd;
   pfVec3 hitStbd, hitPort;
   float temp_float;
   pfMatrix Orient;
   pfVec3 tb_vec = { 0.0f, 0.0f, 1.0f };
   //pfSetVec3 ( tb_vec, 0.0f, 0.0f, 1.0f );
   pfVec3 lr_vec = { 1.0f, 0.0f, 0.0f };
   int the_item_hit, ix;

// cerr << "start startpnt = " << startpnt[X] << ", " << startpnt[Y]
//      << ", " << startpnt[Z] << endl;
// cerr << "start endpnt = " << endpnt[X] << ", " << endpnt[Y]
//      << ", " << endpnt[Z] << endl;
// cerr << "start velocity = " << velocity[X] << ", " << velocity[Y]
//      << ", " << velocity[Z] << endl;

   if (int(pfEqualVec3(startpnt,endpnt)))
      {
#ifdef TRACE
      cerr << " NOT MOVING -> NO collisions\n";
#endif
      return FINEWITHME;
      }

#ifdef TRACE
   cerr << "startpnt " << startpnt[X] << " " <<  startpnt[Y]
        << " " <<  startpnt[Z] << endl;

   cerr << "endpnt "<< endpnt[X] << " " <<  endpnt[Y]
        << " " <<  endpnt[Z] << endl;
#endif

   pfMakeEulerMat ( Orient, orientation[HEADING],
                            orientation[PITCH],
                            orientation[ROLL] );
   pfFullXformPt3 ( tb_vec, tb_vec, Orient );
   pfFullXformPt3 ( lr_vec, lr_vec, Orient );
   pfScaleVec3 ( eye_vec, EYE_OFFSET, tb_vec );
   pfScaleVec3 ( foot_vec, INTERSECT_OFFSET, tb_vec );
   pfScaleVec3 ( beam_vec, bounding_box.max[X], lr_vec );
   pfAddVec3 ( start_eye, startpnt, eye_vec );
   pfAddVec3 ( end_eye, endpnt, eye_vec );
   pfAddVec3 ( start_pos, startpnt, foot_vec );
   pfAddVec3 ( end_pos, endpnt, foot_vec );
   pfAddVec3 ( start_stbd, startpnt, beam_vec );
   pfAddVec3 ( end_stbd, endpnt, beam_vec );
   pfSubVec3 ( start_port, startpnt, beam_vec );
   pfSubVec3 ( end_port, endpnt, beam_vec );

   //create a segment along the path we just followed
   // Bow Vector at water line
   pfSetVec3(seg.segs[0].dir,(end_pos[X] - start_pos[X]),
                        (end_pos[Y] - start_pos[Y]),
                        (end_pos[Z] - start_pos[Z]));
   // Bow Vector at eye point
   pfSetVec3(seg.segs[1].dir,(end_eye[X] - start_eye[X]),
                        (end_eye[Y] - start_eye[Y]),
                        (end_eye[Z] - start_eye[Z]));
   // Forward pointing starboard Vector
   pfSetVec3(seg.segs[2].dir,(end_stbd[X] - start_stbd[X]),
                        (end_stbd[Y] - start_stbd[Y]),
                        (end_stbd[Z] - start_stbd[Z]));
   // Forward pointing port Vector
   pfSetVec3(seg.segs[3].dir,(end_port[X] - start_port[X]),
                        (end_port[Y] - start_port[Y]),
                        (end_port[Z] - start_port[Z]));
   // Backward pointing starboard Vector
   pfSetVec3(seg.segs[4].dir,-seg.segs[2].dir[X],
                             -seg.segs[2].dir[Y],
                              seg.segs[2].dir[Z]);
   // Backward pointing port Vector
   pfSetVec3(seg.segs[5].dir,-seg.segs[3].dir[X],
                             -seg.segs[3].dir[Y],
                              seg.segs[3].dir[Z]);
   if (speed > 0) { // Moving forward  
   seg.segs[0].length = pfLengthVec3(seg.segs[0].dir) + 
                        bounding_box.max[Y];
   seg.segs[1].length = pfLengthVec3(seg.segs[1].dir) + 
                        bounding_box.max[Y];
   seg.segs[2].length = pfLengthVec3(seg.segs[2].dir) + 
                        (0.5f*bounding_box.max[Y]);
   seg.segs[3].length = pfLengthVec3(seg.segs[3].dir) + 
                        (0.5f*bounding_box.max[Y]);
   seg.segs[4].length = pfLengthVec3(seg.segs[4].dir) + 
                        fabs(bounding_box.min[Y]);
   seg.segs[5].length = pfLengthVec3(seg.segs[5].dir) + 
                        fabs(bounding_box.min[Y]);
   }
   else { // Moving Backwards
   seg.segs[0].length = pfLengthVec3(seg.segs[0].dir) + 
                        fabs(bounding_box.min[Y]);
   seg.segs[1].length = pfLengthVec3(seg.segs[1].dir) + 
                        fabs(bounding_box.min[Y]);
   seg.segs[2].length = pfLengthVec3(seg.segs[2].dir) + 
                        fabs(bounding_box.min[Y]);
   seg.segs[3].length = pfLengthVec3(seg.segs[3].dir) + 
                        fabs(bounding_box.min[Y]);
   seg.segs[4].length = pfLengthVec3(seg.segs[4].dir) + 
                        (0.5f*bounding_box.max[Y]);
   seg.segs[5].length = pfLengthVec3(seg.segs[5].dir) + 
                        (0.5f*bounding_box.max[Y]);
   }

   PFCOPY_VEC3(seg.segs[0].pos,start_pos);
   PFCOPY_VEC3(seg.segs[1].pos,start_eye);
   PFCOPY_VEC3(seg.segs[2].pos,start_stbd);
   PFCOPY_VEC3(seg.segs[3].pos,start_port);
   PFCOPY_VEC3(seg.segs[4].pos,start_stbd);
   PFCOPY_VEC3(seg.segs[5].pos,start_port);

   for (ix=0; ix<6; ix++) {
      if(seg.segs[ix].dir[Y] == 0.0) seg.segs[ix].dir[Y] = 0.01f;
      if(seg.segs[ix].dir[X] == 0.0) seg.segs[ix].dir[X] = 0.01f;
      pfNormalizeVec3(seg.segs[ix].dir);
      sp[ix] = &(seg.segs[ix]);
   }

  //pfCylAroundSegs ( &the_barrel, (const pfSeg **)sp, 2 );
  //pfCylAroundSegs ( &the_barrel, sp, 4 );
#ifdef TRACE
   cerr << "Chck Pt 2 Number of Kids " << pfGetNumChildren(G_moving) <<"\n";
#endif


   //*** intersect test against the fixed objects ********************//
   if (altitude < 1000.0f)
      {
      seg.mode = PFTRAV_IS_PRIM;
      seg.userData = NULL;
      seg.activeMask = 0x3f;
      seg.isectMask = DESTRUCT_MASK;
      seg.bound = NULL;
      seg.discFunc = buildinghit;
      isect = pfNodeIsectSegs(G_fixed, &seg, hits);

      for(int ix = 0; ix<4; ix++){
         pfQueryHit(*hits[ix], PFQHIT_FLAGS, &flags);
         if(flags){
           pfQueryHit(*hits[ix], PFQHIT_POINT, &pnt);
           pfQueryHit(*hits[ix], PFQHIT_XFORM, &xform);
           if((ix==0) || (ix==1))
             the_item_hit = 1;
           else if(ix == 2)
             the_item_hit = 2;
           else if(ix == 3)
             the_item_hit = 3;
           break;
         }
      }

      if (isect && (flags & PFHIT_POINT)) {
         //Our starboard bow hits the object and repels us to port
         if (the_item_hit == 2) {
               pfSubVec3(hitStbd, start_port, endpnt);
               pfNormalizeVec3(hitStbd);
               pfScaleVec3(hitStbd, 0.3, hitStbd);
               pfCopyVec3(posture.xyz, endpnt);
               pfAddVec3(posture.xyz, posture.xyz, hitStbd);

               return(HITSOMETHING);
         }
         //Our port bow hits the object and repels us to starboard
         else if (the_item_hit == 3) {
               pfSubVec3(hitPort, start_stbd, endpnt);
               pfNormalizeVec3(hitPort);
               pfScaleVec3(hitPort, 0.3, hitPort);
               pfCopyVec3(posture.xyz, endpnt);
               pfAddVec3(posture.xyz, posture.xyz, hitPort);

               return(HITSOMETHING);
         }
         //We hit an object head on and therefore die
         else {
               //set the point at the intersectin point in the world COORDINATES
               pfFullXformPt3(posture.xyz, pnt, xform);

#ifdef DEBUG_CERR
         cout << "\n\nFixed Object hit at " << posture.xyz[X] << " "
                                        << posture.xyz[Y] << " "
                                        << posture.xyz[Z] << endl;
#endif
#ifdef DATALOCK
               pfDPoolLock ( (void *) G_dynamic_data );
#endif
               G_dynamic_data->dead = DYING;
               G_dynamic_data->cause_of_death = S_OBJECT_DEATH;
#ifdef DATALOCK
               pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
               senddetonation(posture.xyz);
               return(HITSOMETHING);
           
         } //else
      } // if isect
   } // -altitude < 1000;


   //*** intersect test against the moving objects ********************//
   seg.mode = PFTRAV_IS_PRIM;
   seg.userData = NULL;
   seg.activeMask = 0x3f;
   seg.isectMask = ENTITY_MASK;
   seg.bound = NULL;
   seg.discFunc = NULL;

   isect = pfNodeIsectSegs(G_moving, &seg, hits);
   for(ix = 0; ix<4; ix++){
      pfQueryHit(*hits[ix], PFQHIT_FLAGS, &flags);
      if(flags){
        pfQueryHit(*hits[ix], PFQHIT_POINT, &pnt);
        if((ix==0) || (ix==1))
          the_item_hit = 1;
        else if(ix == 2)
          the_item_hit = 2;
        else if(ix == 3)
          the_item_hit = 3;
        break;
      }
   }
#ifdef DEBUG_CERR
   cerr<<"Point hit is "<<pnt[X]<<endl;
   cerr<<"             "<<pnt[Y]<<endl;
   cerr<<"             "<<pnt[Z]<<endl;

#endif

   if (isect && (flags & PFHIT_POINT)) {
      //Our starboard bow hits the object and repels us to port
      if (the_item_hit == 2) {
               pfSubVec3(hitStbd, start_port, endpnt);
               pfNormalizeVec3(hitStbd);
               pfScaleVec3(hitStbd, 0.3, hitStbd);
               pfCopyVec3(posture.xyz, endpnt);
               pfAddVec3(posture.xyz, posture.xyz, hitStbd);

               return(HITSOMETHING);
      }
           //Our port bow hits the object and repels us to starboard
      else if (the_item_hit == 3) {
               pfSubVec3(hitPort, start_stbd, endpnt);
               pfNormalizeVec3(hitPort);
               pfScaleVec3(hitPort, 0.3, hitPort);
               pfCopyVec3(posture.xyz, endpnt);
               pfAddVec3(posture.xyz, posture.xyz, hitPort);

               return(HITSOMETHING);
      }      
      //We hit an object head on and therefore die
      else {
               //set the point at the intersectin point in the world COORDINATES
               pfFullXformPt3(posture.xyz, pnt, xform);

#ifdef DEBUG_CERR
         cout << "\n\nMoving Object hit at " << posture.xyz[X] << " "
                                        << posture.xyz[Y] << " "
                                        << posture.xyz[Z] << endl;
#endif
#ifdef DATALOCK
               pfDPoolLock ( (void *) G_dynamic_data );
#endif
               G_dynamic_data->dead = DYING;
               G_dynamic_data->cause_of_death = D_OBJECT_DEATH;
#ifdef DATALOCK
               pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
               senddetonation(posture.xyz);
               return(HITSOMETHING);
      } // else 
   }  // if isect

   //******* intersect test against the ground *********************//
   seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
   seg.userData = NULL;
   seg.activeMask = 0x3f;
   seg.isectMask = PERMANENT_MASK;
   seg.bound = NULL;
   seg.discFunc = NULL;
   if (pfNodeIsectSegs(G_fixed, &seg, hits))
      {
      for (ix=0; ix<6; ix++) {
         pfQueryHit(*hits[ix], PFQHIT_FLAGS, &flags);
         if ( flags ) {
//            pfQueryHit(*hits[ix], PFQHIT_NODE, &node);
            pfQueryHit(*hits[ix], PFQHIT_GSET, &gset);
            pfQueryHit(*hits[ix], PFQHIT_POINT, &pnt);
            pfQueryHit(*hits[ix], PFQHIT_NORM, &normal);
            break;
         }
      }
//      the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);
      the_mask = pfGetGSetIsectMask(gset);
      the_item_hit = int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT);

      // find the orientation of the ground
      if ( ((the_item_hit == PERMANENT_DIRT) ||
            (the_item_hit == PERMANENT_BRIDGE)) &&
           (flags & PFHIT_NORM) )
         {
#ifdef DEBUG_CERR
         cerr<<"\nHit the dirt or bridge with "<<ix<<endl;
#endif
         speed = 0.0f;
         pfSetVec3(velocity, 0.0f, 0.0f, 0.0f);
         pfCopyVec3 ( endpnt, startpnt );
         return(HITSOMETHING);
         }      // end intersection test

      else if ( the_item_hit == PERMANENT_FOILAGE)
         { // Hit the water or foilage
#ifdef DEBUG_CERR
         cerr<<"\nHit foliage\n"<<endl;
#endif
#ifdef DATALOCK
         pfDPoolLock ( (void *) G_dynamic_data );
#endif
         G_dynamic_data->dead = DYING;
         G_dynamic_data->cause_of_death = FOILAGE_DEATH;
#ifdef DATALOCK
         pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         return(HITSOMETHING);
         }

      else if ( (the_item_hit == PERMANENT_TUNNEL) &&
                (flags & PFHIT_NORM)
               && (flags & PFHIT_POINT) )
         {
#ifdef DEBUG_CERR
         cerr<<"\nHit the tunnel\n"<<endl;
#endif
         float ordered_heading;
         float pitch_angle, roll_angle, bearing_angle;
         pfMatrix orient;
         pfVec3 direction = { 0.0f, 1.0f, 0.0f };
         pfSubVec3 ( incoming_vec, startpnt, endpnt);
         pfNormalizeVec3 ( incoming_vec );
           normal_vec[X] = normal[X];
           normal_vec[Y] = normal[Y];
           normal_vec[Z] = normal[Z];

         pfNormalizeVec3 ( normal_vec );
         temp_float = 1.25f * pfDotVec3 ( normal_vec, incoming_vec );
         pfScaleVec3 ( outgoing_vec, temp_float, normal_vec );
         pfSubVec3 ( outgoing_vec, outgoing_vec, incoming_vec );
         pfNormalizeVec3 ( outgoing_vec );

         bearing_angle = pfArcTan2( -outgoing_vec[Y], -outgoing_vec[X] );
         pitch_angle = 1.5f * pfArcSin ( outgoing_vec[Z] );
         if (pitch_angle >= 90.0f)
            pitch_angle -= 90.0f;
         if (pitch_angle <= -90.0f)
            pitch_angle += 90.0f;
         roll_angle = 0.0f;
         ordered_heading = (bearing_angle + 90.0f);
         if (ordered_heading >= 360.0)
            ordered_heading -= 360.0f;
         if (ordered_heading < 0.0)
            ordered_heading += 360.0f;

         pfSetVec3 ( orientation, ordered_heading, pitch_angle, roll_angle );

         pfMakeEulerMat ( orient, orientation[HEADING],
                                  orientation[PITCH],
                                  orientation[ROLL]);
         pfFullXformPt3 ( direction, direction, orient );
         pfNormalizeVec3 ( direction );
         if ( hit_index == 1 )
            pfScaleVec3 ( normal, EYE_OFFSET + 5.0f, normal );
         else
            pfScaleVec3 ( normal, INTERSECT_OFFSET + 5.0f, normal );

         pfAddVec3 (endpnt, pnt, normal);
         altitude = endpnt[Z] - gnd_level(endpnt);;
         temp_float= 0.75f * pfLengthVec3(velocity);
         speed *= 0.75f;
         pfScaleVec3 ( velocity, temp_float, direction);
         }

      }

      //if we got here it did not hit anything
      return FINEWITHME;

}

void SHIP_VEH::sendentitystate()
{
#ifndef NONPSSHIP
  static long lastacceleration = 0;
#endif
  EntityStatePDU epdu;
  char tempstr[MARKINGS_LEN];

  basicentitystate(&epdu);

#ifndef NONPSSHIP
  if ((long)linacceleration != lastacceleration) //if ship is accelerating
     assign_dralgo(DRAlgo_DRM_FVW);
  else //We are not turning
     assign_dralgo(DRAlgo_DRM_FPW);
  lastacceleration = (long)linacceleration;

  epdu.dead_reckon_params.algorithm = dralgo;

  // Ship's need to calculate these
  epdu.dead_reckon_params.angular_velocity[HEADING] = omega;
  //epdu.dead_reckon_params.angular_velocity[PITCH] = 0.0f;
  //epdu.dead_reckon_params.angular_velocity[ROLL] = 0.0f;
#endif

  epdu.num_articulat_params = (unsigned char)0;

  epdu.entity_marking.character_set = CharSet_ASCII;

  if ( *(G_static_data->marking) )
     {
     strcpy ( (char *)epdu.entity_marking.markings, 
              (char *)G_static_data->marking );
     }
  else
     {
     strncpy ( tempstr, 
              (char *)G_static_data->hostname, MARKINGS_LEN-4 );
     strcpy ( (char *)epdu.entity_marking.markings, "NPS" );
     strcat ( (char *)epdu.entity_marking.markings, tempstr );
     }

  sendEPDU(&epdu);

#ifdef DEBUG_CERR
  cerr << "sent a Vehicle Entity State PDU\n";
#endif

  updateDRparams();
}


void SHIP_VEH::entitystateupdate(EntityStatePDU *epdu, sender_info &sender)
//update a ship from the network
//also assuming there is no articulated parameters
{
   VEHICLE::entitystateupdate(epdu, sender);

#ifndef NONPSSHIP
   omega = epdu->dead_reckon_params.angular_velocity[HEADING];

   // Now that we are in place, update all mounted vehicles to proper location
   for ( int ix = 0; ix < MAX_VEH; ix++ ) {
     if (mounted_veh[ix] != NULL) {
        ((PERSON_VEH *)mounted_veh[ix])->reset_on_mounted_vid();
     }
   }// for loop
#endif //NONNPSSHIP
}


void SHIP_VEH::oodupdate(OodToNPSNETIDU *oodidu)
{
  ordered_rudder_angle = oodidu->ood_rudder_angle;   //ordered
  ordered_rpm = oodidu->ood_rpm;  //ordered
  ordered_port_bell = oodidu->ood_port_bell;   //ordered
  ordered_stbd_bell = oodidu->ood_stbd_bell;   //ordered
  cdata.thrust = ordered_port_bell;
  cdata.roll = ordered_rudder_angle;
}


void SHIP_VEH::getbridgeview(pfVec3 &view) 
{ 
   pfCopyVec3(view, bridgeview); 
}


#ifndef NONPSSHIP
void SHIP_VEH::mount(VEHICLE *vehptr, int vid) 
{ 
   mounted_veh[vid] = vehptr;
}


void SHIP_VEH::dismount(int vid) 
{ 
   mounted_veh[vid] = NULL;
}
#endif // NONPSSHIP

