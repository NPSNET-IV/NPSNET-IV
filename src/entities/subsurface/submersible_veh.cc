
#include <math.h>

#include "submersible_veh.h"
#include "common_defs2.h"
#include "terrain.h"
#include "netutil.h"
#include "macros.h"
#include "munitions.h"
#include "interface_const.h"
#include "collision_const.h"
#ifndef NOSUB
#include "entity.h"  
#include "lake.h" 
#include "vehicle.h"
#endif // NOSUB

#ifndef NO_DVW
#define NO_PFSMOKE
#define NO_NPSSMOKE
#endif

#ifndef NO_PFSMOKE
#define NO_NPSSMOKE
#endif

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;
extern          VEH_LIST     *G_vehlist;
extern volatile int           G_drivenveh;
extern          pfGroup      *G_fixed;
#ifndef NOSUB
extern          pfGroup      *lakeRoot;
#endif

//********************************************************************//
//****************** class SUBMERSIBLE_VEH ***************************//
//********************************************************************//

#ifndef NOSUB
SUBMERSIBLE_VEH::SUBMERSIBLE_VEH(int id,int atype, ForceID the_force)
:VEHICLE(id,atype,the_force)
{
 
 //initalize the submodel
 subbody.UUVBody_initialize();
 //subbody.set_position_to(7883.0, 7073.0, 20.0);
 // subbody.integrate_equations_of_motion();

 //initialize internal values
 Max_RPM = 1000.0;
 Min_RPM = 500.0;
 total_RPM = 0.0;
 total_planes = 0.0;
 total_rudder = 0.0;
 ordered_depth = 0.0;  // 0000
 ordered_course = 0.0; // 000
 ordered_speed = 0.0;  //-3 - 4 (back full - flank)
 helm_ordered_speed = 0.0;  //-3 - 4 (back full - flank)

 embtblow_onoff = 0.0;   //1 0
 mbtvents_openshut = 0.0; //1 0

 scope_raiselower = 0.0;  //1 0
 scope_updown = 0.0; //1 -1
 scope_leftright = 0.0; //1 -1
 scope_by = 0.0;    
 scope_rh = 0.0;   
 scope_mark = 0.0;                      

 selecttube_2or4 = 0.0; //1 0
 port_tube_shoot = 0.0;
 port_tube_course = 0.0; 
 selecttube_1or3 = 0.0; //1 0
 stbd_tube_shoot = 0.0;
 stbd_tube_course = 0.0; 
 selecttlam = 1.0;      //1 - 6
 tlam_shoot = 0.0; 
 tlam_course = 0.0;

 //Used to record data for gnu plot
 //   record_data_on ();
 //END Used to record data for gnu plot

 //COMMS-----------------------------------
 // Multicast Defaults
   int multicast = TRUE;
   u_short port;
   char group[25];
   u_char ttl = IDU_DEF_MC_TTL;
   port = 0;
   int loopback = FALSE;
   char net_interface[20];

    strncpy ( group, IDU_DEF_MC_GROUP,25 );   //COMMS
    strcpy ( net_interface, "" );             //COMMS

 if ( multicast )
      {
      if ( port == 0 )
         port = IDU_DEF_MC_PORT;
      net = new IDU_net_manager ( group, port, ttl, net_interface,
                                  loopback );
      }
   else
      {
      if ( port == 0 )
         port = IDU_DEF_BC_PORT;
      net = new IDU_net_manager ( port, net_interface, loopback );
      }

   if ( !net->net_open() )
      {
      cerr << "Could not open network." << endl;
      exit(0);
      }
   else
      {
      net->add_to_receive_list(Test_Type);
      net->add_to_receive_list(SUB_Helm_To_NPSNET_Type);
      net->add_to_receive_list(SUB_Ood_To_NPSNET_Type);
      net->add_to_receive_list(SUB_Weaps_To_NPSNET_Type);
      }

   cerr << endl << "Network open for submersible control:" << endl;
   if ( multicast )
      {
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
   else
      {
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

}

SUBMERSIBLE_VEH::~SUBMERSIBLE_VEH()
{
 // cerr << "Sub - destructor\n";
 net->net_close();  
}


movestat SUBMERSIBLE_VEH::move()
// move the vehicle for the next frame
{
   pfMatrix orient;
   pfVec3 up = { 0.0f, 0.0f, 1.0f };
   pfVec3 oldpos;
   static int paused;
   static int tethered;
   static pfVec3 ZeroVec = {0.0f,0.0f,0.0f};
   
   static NPSNETToSubIDU       NPSNET2Subidu;
   static int counter = 0;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tSUBMERSIBLE_VEH::move" << endl;
#endif

   pfCopyVec3(oldpos, posture.xyz);

   update_time ();
   collect_inputs ();

#ifdef TRACE
   cerr << "Moving a Submersible Vehicle\n";
#endif

 //----------------------------------------------------------------------------
  //TRY to read READ idu 

  if (net->read_idu(&idu, &type, idu_sender_info, swap_bufs) != NULL)
   {
   switch(type)
           {
           case (Test_Type):
             testidu = (TestIDU *)idu;
             cerr<<"Got a Test_type idu"<<endl;
             break;
           case (SUB_Helm_To_NPSNET_Type):
             helmidu = (HelmToNPSNETIDU *)idu;
             helmupdate(helmidu);
             break;
           case (SUB_Ood_To_NPSNET_Type):
             oodidu = (SubOodToNPSNETIDU *)idu;
             oodupdate(oodidu);
             break;
           case (SUB_Weaps_To_NPSNET_Type):
             weapsidu = (WeapsToNPSNETIDU *)idu;
             weapsupdate(weapsidu);
             break;
           default:
             //cerr << "\nUnknown type received: " << int(type)
             //     << endl << endl;
             break;
            }
     }

   if(!(paused || tethered || cdata.rigid_body))
      {
      // Move to the location we should be at after the last time interval
      posture.xyz[X] += velocity[X]*cdata.delta_time;
      posture.xyz[Y] += velocity[Y]*cdata.delta_time;
      posture.xyz[Z] += velocity[Z]*cdata.delta_time;

      // Determine depth
      altitude = -get_depth();
      
      // Determine speed
       speed = (sqrt(velocity[X]*velocity[X] + velocity[Y]*velocity[Y]));
     
      //Weapons control code
      if (port_tube_shoot)
        {
         G_dynamic_data->inital_torpedo_course = port_tube_course;
 // cerr<<"port tube course "<<port_tube_course<<endl;
         firebuttonhit ( PRIMARY, G_drivenveh, posture, posture);
         port_tube_shoot = 0.0;
        }
       if (stbd_tube_shoot)
        {
         G_dynamic_data->inital_torpedo_course = stbd_tube_course;
  // cerr<<"stbd tube course "<<stbd_tube_course<<endl;
         firebuttonhit ( PRIMARY, G_drivenveh, posture, posture);
         stbd_tube_shoot = 0.0;
        }
       if (tlam_shoot)
        {
         G_dynamic_data->inital_tlam_course = tlam_course;
         firebuttonhit ( SECONDARY, G_drivenveh, posture, posture);
         tlam_shoot = 0.0;
        }
      //Periscope code
      if (scope_raiselower != 0.0)  //scope raise ordered
        {
         G_dynamic_data->camera = TRUE;  
         if (look.xyz[Z] <= 18.0)  //scope max height is 18.0 m.
           look.xyz[Z] += 0.01;  //scope raise 0.01 each frame
         }
      else                              //scope lower ordered
         {
         if (look.xyz[Z] >= 0.0)  //scope min height is 0.0 m.
           look.xyz[Z] -= 0.01;  //scope lowers 0.01 each frame
         if (look.xyz[Z] == 0.0)  //scope lowered, disable scope
           G_dynamic_data->camera = FALSE;  
         }
      
       if (scope_updown != 0.0)
         {
          if (scope_updown == 1.0)
            look.hpr[PITCH] += KEY_PITCH_CHANGE;
          if (scope_updown == -1.0)
            look.hpr[PITCH] -= KEY_PITCH_CHANGE;
          scope_updown = 0.0;
         }
    
       if (scope_leftright != 0.0)
         {
          if (scope_leftright == 1.0)
            look.hpr[HEADING] += KEY_PITCH_CHANGE;
          if (scope_leftright == -1.0)
            look.hpr[HEADING] -= KEY_PITCH_CHANGE;
          scope_leftright = 0.0;
         }
       if (scope_mark == 1.0)
         {
          scope_by = look.hpr[HEADING];
          scope_mark = 1.0;   //reset
         }

      // Now see if we hit something in this time interval
      if (!G_static_data->stealth)
         {
         float theDepth = -get_depth();
         if ( check_collide(oldpos, posture.xyz,
                            posture.hpr,
                            speed, theDepth) )
            return(HITSOMETHING);
         }

      // Set the RPM
      if ((input_control == NPS_FCS) || (input_control == NPS_KAFLIGHT) || 
          (input_control == NPS_BASIC_PANEL) || (input_control == NPS_FLYBOX) )
         {
          //assuming that cdata.thrust is (-1.0 - 0.0 - 1.0) when using flight 
          //controls.  Just multiply this value by max RPM.
          //total_RPM is protected variable in this class.

          total_RPM = Max_RPM * cdata.thrust;
         
         }
       else  
         {  
         // Not using the flight control throttle
         
         //Assuming that cdata.thrust is either +5.0 or -5.0,
         //so just add to total rpm

         if (cdata.thrust > 0.0 )
           {
            total_RPM += (Max_RPM / 4.0);
            helm_ordered_speed++;
           }
         if (cdata.thrust < 0.0 )
           {
            total_RPM -= (Min_RPM / 2.0);
            helm_ordered_speed--;
           }

         cdata.thrust = 0.0f;
         }

       // Don't want to exceed the max rpm
       if (total_RPM > 1000)
          total_RPM = 1000;
       // Don't want to exceed the min rpm
       if (total_RPM < -500)
          total_RPM = -500; 
       // Don't want to exceen max helm_ordered_speed
        if (helm_ordered_speed > 4)
          helm_ordered_speed = 4; 
       // Don't want to exceen min helm_ordered_speed
        if (helm_ordered_speed < -3)
          helm_ordered_speed = -3; 
       //set model RPM
       subbody.AUV_port_rpm  = total_RPM;
       subbody.AUV_stbd_rpm  = total_RPM; 
 
//CUTcerr<<"thrust is set at "<<subbody.AUV_port_rpm <<endl;

       // Set the Planes
       if ((input_control == NPS_FCS) || (input_control == NPS_KAFLIGHT) || 
          (input_control == NPS_BASIC_PANEL) || (input_control == NPS_FLYBOX) )
          {
           //assuming that cdata.pitch is (-1.0 - 0.0 - 1.0) when using flight 
           //controls.  Just multiply this value by max planes.
           //total_planes is protected variable in this class.

           total_planes = 40 * cdata.pitch;
          }
       else  
          {  
          // Not using the flight control 
         
          //Assuming that cdata.pitch is either +1.0 or -1.0,
          //so just add to total planes

          total_planes += cdata.pitch;
          cdata.pitch = 0.0f;
          }

       // Don't want to exceed the max planes
       if (total_planes > 40)
          total_planes = 40;
       // Don't want to exceed the min planes
       if (total_planes < -40)
          total_planes = -40; 
       //set model planes convert into radians
       subbody.AUV_delta_planes = -total_planes * PI / 180.0;

//CUTcerr<<"planes are set at "<<total_planes <<endl;
   
       // Set the Rudder 
       if ((input_control == NPS_FCS) || (input_control == NPS_KAFLIGHT) || 
          (input_control == NPS_BASIC_PANEL) || (input_control == NPS_FLYBOX) )
          {
           //assuming that cdata.roll is (-1.0 - 0.0 - 1.0) when using flight 
           //controls.  Just multiply this value by max rudder.
           //total_rudder is protected variable in this class.

           total_rudder = 40 * cdata.roll;
          }
        else  
          {  
          // Not using the flight control 
         
          //Assuming that cdata.roll is either +1.0 or -1.0,
          //so just add to total rudder

          total_rudder += cdata.roll;
          cdata.roll = 0.0f;
          }
    
       // Don't want to exceed the max rudder
       if (total_rudder > 40)
          total_rudder = 40;
       // Don't want to exceed the min rudder
       if (total_rudder < -40)
          total_rudder = -40; 

       //set model rudder convert into radians
       subbody.AUV_delta_rudder = -total_rudder*PI/180.0;

//CUTcerr<<"rudder is set at "<< -total_rudder <<endl<<endl;
  
       //set model time
       subbody.AUV_time = cdata.current_time;

       //set NPSNET XYZ converting from model XYZ
      posture.xyz[X] =  subbody.AUV_y ;
      posture.xyz[Y] =  subbody.AUV_x;
      posture.xyz[Z] = - subbody.AUV_z;

       //set NPSNET HPR converting from model HPR
       posture.hpr[HEADING] = - subbody.AUV_psi*180/PI;
       posture.hpr[PITCH]   =  subbody.AUV_theta*180/PI;
       posture.hpr[ROLL]    =  subbody.AUV_phi*180/PI;

       //model computation
       subbody.integrate_equations_of_motion();
        
       // Make sure we are still in bounds, if not, stop it at the boundary.
       if (at_a_boundary(posture.xyz))
          {    
          posture.hpr[HEADING] = 
	       fmod(posture.hpr[HEADING] + 180.0f, 360.0f);
          pfNegateVec3(velocity,ZeroVec); 
          } 
       else  // Still in bounds
          {
          // Calculate the new NPSNET velocity vector using model velocities
          pfSetVec3 (velocity, subbody.AUV_y_dot, 
        	                subbody.AUV_x_dot, 
                               -subbody.AUV_z_dot );
          }
 
         pfMakeEulerMat ( orient, posture.hpr[HEADING],
                              posture.hpr[PITCH],
                              posture.hpr[ROLL]);
  
         pfFullXformPt3 ( up, up, orient );
         pfNormalizeVec3 ( up );
         pfScaleVec3 ( up, 5.0, up);
         pfAddVec3 ( eyepoint, posture.xyz, up );
         pfCopyVec3 ( firepoint, eyepoint );
  
         // Copy new posture values from pass_data into the vehicle's posture
         posture = posture;
 
         //put the vehicle in the correct location in space
         pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
         pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);

      } // End !paused !tethered !rigid_body

  //END READ idu input
//---------------------------------------------------------------------------
// If tethered, let the other vehicle calculate our position for us 
   if (tethered)
      {
      if (G_vehlist[cdata.vid].vehptr)
         {
         if ( tethered == 1)
            G_vehlist[cdata.vid].vehptr->teleport(posture,speed);
         else if ( tethered == 2)
            G_vehlist[cdata.vid].vehptr->attach(posture,speed,
                                                G_dynamic_data->attach_dis);
          posture = posture;
         }
      else
         {
#ifdef DATALOCK
         pfDPoolLock ( (void *) G_dynamic_data );
#endif
         G_dynamic_data->tethered = FALSE;
#ifdef DATALOCK
         pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         }
      }

//Used to record data for gnu plot
/*
if (loop < 1000 )
{
 cerr<<" writing"<<endl;


 record_data(subbody.AUV_time,subbody.AUV_x,subbody.AUV_y, subbody.AUV_z,subbody.AUV_phi, subbody.AUV_theta,
                  subbody.AUV_psi,subbody.AUV_u, subbody.AUV_v,subbody.AUV_w, subbody.AUV_p,subbody.AUV_q,
                  subbody.AUV_r, subbody.AUV_x_dot,subbody.AUV_y_dot, subbody.AUV_z_dot,
                  subbody.AUV_phi_dot,subbody.AUV_theta_dot, subbody.AUV_psi_dot,
                  subbody.AUV_delta_rudder,subbody.AUV_delta_planes,
                  subbody.AUV_port_rpm,          subbody.AUV_stbd_rpm,
                  subbody.AUV_bow_vertical, subbody.AUV_stern_vertical,
                  subbody.AUV_bow_lateral,  subbody.AUV_stern_lateral, 
                  subbody.AUV_ST1000_bearing,subbody.AUV_ST1000_range,
                  subbody.AUV_ST1000_strength,  subbody.AUV_ST725_bearing,
                  subbody.AUV_ST725_range, subbody.AUV_ST725_strength);
 cerr<<"done writing"<<endl;
}

if(loop ==1000)
 {
  cerr<<" close data file"<<endl;
  record_data_off ();
  cerr<<"done with close data file"<<endl;
 }
loop++;
*/
//END Used to record data for gnu plot

//COMMS--------------------------------------------------------
//send data to update if enough time has elapsed

 //increment counter
    counter++;

 if (counter > 40)
  {
   counter =0;
   //update NPSNET2Subidu
   NPSNET2Subidu.sub_depth = -altitude;
  
   if (posture.hpr[HEADING] < 0.0)
     NPSNET2Subidu.sub_course = -posture.hpr[HEADING];
   else
     NPSNET2Subidu.sub_course = 360.0 - posture.hpr[HEADING];

   NPSNET2Subidu.sub_speed = speed;
   

  NPSNET2Subidu.sub_rudder_angle = total_rudder;    //actual ,00
  NPSNET2Subidu.sub_sternplns_angle = total_planes; //actual ,00
  NPSNET2Subidu.sub_fwtrplns_angle = total_planes;  //actual ,00

  NPSNET2Subidu.sub_ordered_depth = ordered_depth;  // 0000
  NPSNET2Subidu.sub_ordered_course = ordered_course; // 000
  NPSNET2Subidu.sub_ordered_speed = ordered_speed;  //-3 - 4 (back full - flank)
  NPSNET2Subidu.sub_helm_ordered_speed = helm_ordered_speed;  //-3 - 4 (back full - flank)

  NPSNET2Subidu.sub_embtblow_onoff = embtblow_onoff;   //1 0
  NPSNET2Subidu.sub_mbtvents_openshut = embtblow_onoff; //1 0

  NPSNET2Subidu.sub_scope_raiselower = scope_raiselower;  //1 -1
  NPSNET2Subidu.sub_scope_by = -scope_by;  
  NPSNET2Subidu.sub_scope_rh = scope_rh;    //range to object that scope is looking at when mark 
                         //is hit.

  NPSNET2Subidu.sub_selecttube_2or4 = selecttube_2or4; //1 0
  NPSNET2Subidu.sub_selecttube_1or3 = selecttube_1or3; //1 0
  NPSNET2Subidu.sub_selecttlam = selecttlam;      //1 - 6

   if ( !(net->write_idu((char *)&NPSNET2Subidu,NPSNET_To_SUB_Type)) )
             cerr << "ERROR:\tCould not send NPSNET to SUB IDU." << endl;
  }
return(FINEWITHME); 

}

void SUBMERSIBLE_VEH::helmupdate(HelmToNPSNETIDU *helmidu)
{
 helm_ordered_speed = helmidu->helm_speed;
 switch (int(helm_ordered_speed))
      {
       case -3:
        total_RPM = -Min_RPM;
       break;
       case -2:
        total_RPM = -Min_RPM * 2/3;
       break;
       case -1:
        total_RPM = -Min_RPM *1/3;
       break;
       case 0:
        total_RPM = 0.0;
       break;
       case 1:
        total_RPM = Max_RPM * 1/4;
       break;
       case 2:
        total_RPM = Max_RPM * 1/2;
       break;
       case 3:
        total_RPM = Max_RPM * 3/4;
       break;
       case 4:
        total_RPM = Max_RPM;
       break;
      }
}


void SUBMERSIBLE_VEH::oodupdate(SubOodToNPSNETIDU *oodidu)
{
  ordered_depth = oodidu->ood_depth;   //ordered
  ordered_course = oodidu-> ood_course;  //ordered
  ordered_speed = oodidu->ood_speed;   //ordered

  embtblow_onoff = oodidu->ood_embtblow_onoff;    //1 0
  mbtvents_openshut = oodidu->ood_mbtvents_openshut; //1 0

  scope_raiselower = oodidu->ood_scope_raiselower;  //1 -1
  scope_updown = oodidu->ood_scope_updown;      //1 -1
  scope_leftright = oodidu->ood_scope_leftright;   //1 -1
  scope_mark = oodidu->ood_scope_mark;        //1

}

void SUBMERSIBLE_VEH::weapsupdate(WeapsToNPSNETIDU *weapsidu)
{
  selecttube_2or4 = weapsidu->weaps_select_tube_2or4;  //1 0
  selecttube_1or3 = weapsidu->weaps_select_tube_1or3;  //1 0

  port_tube_course = weapsidu-> weaps_course_port;
  port_tube_shoot = weapsidu->weaps_shoot_port;
  stbd_tube_course = weapsidu->weaps_course_stbd;
  stbd_tube_shoot = weapsidu->weaps_shoot_stbd;
  selecttlam = weapsidu->weaps_select_tlam;
   switch (int(selecttlam))
      {
       case 1:
        tlam_course = 0.0;
       break;
       case 2:
        tlam_course = 300.0;
       break;
       case 3:
        tlam_course = 240.0;
       break;
       case 4:
        tlam_course = 180.0;
       break;
       case 5:
        tlam_course = 120.0;
       break;
       case 6:
        tlam_course = 60.0;
       break;
      }
  tlam_shoot = weapsidu->weaps_shoot_tlam;
}


movestat SUBMERSIBLE_VEH::moveDR(float deltatime,float curtime)
//move routine for DR vehicles simple move
{
  float accelfac;  // 1/2*T used for accelerations
   static float oldspeed = 0.0f;
   float speed;

#ifdef TRACE
   cerr << "Dead Recon a Submersible Vehicle\n";
#endif

#ifndef NO_PFSMOKE
   update_pfsmokes();
#endif

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
      drpos.xyz[X] += velocity[X]*deltatime+accelfac*velocity[X];
      drpos.xyz[Y] += velocity[Y]*deltatime+accelfac*velocity[Y];
      drpos.xyz[Z] += velocity[Z]*deltatime+accelfac*velocity[Z];
      drparam.xyz[X] += accelfac*velocity[X];
      drparam.xyz[Y] += accelfac*velocity[Y];
      drparam.xyz[Z] += accelfac*velocity[Z];
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
  if (drpos.xyz[Z] > 0.0f)
     water_orient(drpos);

  if (vid != G_drivenveh)
  {
     posture = drpos;
     //put the vehicle in the correct location in space
     pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y],posture.xyz[Z]);
     pfDCSRot(hull,posture.hpr[HEADING],posture.hpr[PITCH], posture.hpr[ROLL]);

#ifdef DEBUG_SCALE
  pfDCSScale(hull,2.0f);
#endif

  }
  else if ( !G_static_data->stealth )
  {
    speed = pfLengthVec3(velocity);
    //this is us, have we exceeded the DR error?
    if ( (G_static_data->DIS_heartbeat_delta < (curtime - lastPDU)) ||
         (delta_send(&posture,&drpos)))
    {

#ifdef DEBUG
       cerr <<"X       " << posture.xyz[HEADING] << " " <<drpos.xyz[HEADING] <<
"\n";
       cerr <<"Y       " << posture.xyz[PITCH] << " " <<drpos.xyz[PITCH] << "\n";
       cerr <<"Z       " << posture.xyz[ROLL] << " " <<drpos.xyz[ROLL] << "\n";
       cerr <<"HEADING " << posture.hpr[HEADING] << " " <<drpos.hpr[HEADING] <<
"\n";
       cerr <<"PITCH   " << posture.hpr[PITCH] << " " <<drpos.hpr[PITCH] << "\n";
       cerr <<"ROLL    " << posture.hpr[ROLL] << " " <<drpos.hpr[ROLL] << "\n";
       cerr << "send a delta pdu\n";
       cerr << "acceleration" << acceleration[X] << " " << acceleration[Y]
                              << " " << acceleration[Z] << endl;

#endif
       sendentitystate();
       oldspeed = speed;
    }
    else if ( speed != oldspeed )
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
          }
       else if ( fabs(oldspeed-speed) > 50.0f )
          {
          sendentitystate();
          oldspeed = speed;
          }
       }

  }
      return(FINEWITHME);
}


vtype SUBMERSIBLE_VEH::gettype()
{
    return (SUBMERSIBLE);
}


int SUBMERSIBLE_VEH::check_collide(pfVec3 &startpnt, pfVec3 &endpnt,
                                   pfVec3 &orientation,
                                   float &, float &depth)
{
   long     isect;
   pfSegSet seg;
   pfHit    **hits[32];
   long     flags;
//   pfNode   *node;
   pfGeoSet *gset;
   pfVec3   pnt;
   pfMatrix xform;
   pfSeg *sp[2] ;
   pfCylinder the_barrel;
   long the_mask;
   pfVec3 eye_vec, foot_vec;
   pfVec3 start_eye, end_eye;
   pfVec3 start_pos, end_pos;
   pfMatrix Orient;
   pfVec3 tb_vec = { 0.0f, 0.0f, 1.0f };

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

   pfSetVec3 ( tb_vec, 0.0f, 0.0f, 1.0f );
   pfMakeEulerMat ( Orient, orientation[HEADING],
                            orientation[PITCH],
                            orientation[ROLL] );
   pfFullXformPt3 ( tb_vec, tb_vec, Orient );
   pfScaleVec3 ( eye_vec, EYE_OFFSET, tb_vec );
   pfScaleVec3 ( foot_vec, INTERSECT_OFFSET, tb_vec );
   pfAddVec3 ( start_eye, startpnt, eye_vec );
   pfAddVec3 ( end_eye, endpnt, eye_vec );
   pfAddVec3 ( start_pos, startpnt, foot_vec );
   pfAddVec3 ( end_pos, endpnt, foot_vec );

   
   //create a segment along the path we just followed
   pfSetVec3(seg.segs[0].dir,(end_pos[X] - start_pos[X]),
                        (end_pos[Y] - start_pos[Y]),
                        (end_pos[Z] - start_pos[Z]));
   pfSetVec3(seg.segs[1].dir,(end_eye[X] - start_eye[X]),
                        (end_eye[Y] - start_eye[Y]),
                        (end_eye[Z] - start_eye[Z]));

   seg.segs[0].length = pfLengthVec3(seg.segs[0].dir);
   seg.segs[1].length = pfLengthVec3(seg.segs[1].dir);

   PFCOPY_VEC3(seg.segs[0].pos,start_pos);
   PFCOPY_VEC3(seg.segs[1].pos,start_eye);

   //fix a performer bug
   if(seg.segs[0].dir[Y] == 0.0) seg.segs[0].dir[Y] = 0.01f;
   if(seg.segs[0].dir[X] == 0.0) seg.segs[0].dir[X] = 0.01f;
   if(seg.segs[1].dir[Y] == 0.0) seg.segs[1].dir[Y] = 0.01f;
   if(seg.segs[1].dir[X] == 0.0) seg.segs[1].dir[X] = 0.01f;

   pfNormalizeVec3(seg.segs[0].dir);
   pfNormalizeVec3(seg.segs[1].dir);
 
   sp[0] = &(seg.segs[0]);
   sp[1] = &(seg.segs[1]);

  pfCylAroundSegs ( &the_barrel, (const pfSeg **)sp, 2 );
#ifdef TRACE
   cerr << "Chck Pt 2 Number of Kids " << pfGetNumChildren(G_moving) <<"\n";
#endif


   //*** intersect test against the fixed objects ********************//

   if (depth < 1000.0f)
      {
      seg.mode = PFTRAV_IS_PRIM;
      seg.userData = NULL;
      seg.activeMask = 0x03;
      seg.isectMask = DESTRUCT_MASK;
      seg.bound = NULL;
      seg.discFunc = buildinghit;
      isect = pfNodeIsectSegs(G_fixed, &seg, hits);
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
      if ( flags )
         {
         pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
         pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
         }
      else
         {
         pfQueryHit(*hits[1], PFQHIT_POINT, &pnt);
         pfQueryHit(*hits[1], PFQHIT_XFORM, &xform);
         pfQueryHit(*hits[1], PFQHIT_FLAGS, &flags);
         }
      if (isect && (flags & PFHIT_POINT))
         {
         /*set the point at the intersectin point in the world COORDINATES*/
         pfFullXformPt3(posture.xyz, pnt, xform);

#ifdef DEBUG
         cout << "\n\nBuilding hit at " << posture.xyz[X] << " "
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
         }
      }

   //******* intersect test against the ground *********************//
   //NEW
   //check to see if below crush depth
   if (depth < -4000.0f)
    { 
     G_dynamic_data->dead = DYING;
     G_dynamic_data->cause_of_death = S_OBJECT_DEATH;  //WANT TO CHANGE TO 
     senddetonation(posture.xyz);                      //DEPTH_DEATH????
     return(HITSOMETHING);
    }

   //See if you have run into land. This code shoots a vector up and if
   //it does not hit ground then it does the same in the down direction.

   /* make a ray looking "UP" at terrain */
   seg.userData = NULL;
   seg.bound = NULL;
   seg.discFunc = NULL;
   PFCOPY_VEC3(seg.segs[0].pos, endpnt);
   seg.segs[0].pos[Z] += 2.5f;
   PFSET_VEC3(seg.segs[0].dir, 0.0f, 0.0f, 1.0f);
   pfNormalizeVec3(seg.segs[0].dir);
   seg.segs[0].length = 20000.0f;
   seg.mode = PFTRAV_IS_PRIM;
   seg.activeMask = 0x01;
   seg.isectMask = PERMANENT_MASK;
   isect = pfNodeIsectSegs(G_fixed, &seg, hits);  

   if (isect) 
     {
//      pfQueryHit(*hits[0], PFQHIT_NODE, &node);
//      the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);

      pfQueryHit(*hits[0], PFQHIT_GSET, &gset);
      the_mask = pfGetGSetIsectMask(gset);
 
      if ( int((the_mask & PERMANENT_MASK) >> PERMANENT_SHIFT)
          != PERMANENT_WATER)
        {   
         G_dynamic_data->dead = DYING;
         G_dynamic_data->cause_of_death = GROUND_DEATH;   
         //senddetonation(posture.xyz);               
         return(HITSOMETHING);
        }
      }
    else
      {

       /* make a ray looking "DOWN" at terrain */    
       //This ray only has to check a small distance so it is only
       //length of 10.0f
       PFCOPY_VEC3(seg.segs[0].pos, endpnt);
       seg.segs[0].pos[Z] += 2.5f;
       seg.segs[0].length = 10.0f;
       PFSET_VEC3(seg.segs[0].dir, 0.0f, 0.0f, -1.0f);
       pfNormalizeVec3(seg.segs[0].dir);
       isect = pfNodeIsectSegs(G_fixed, &seg, hits); 
      
       if (isect) 
        {
//         pfQueryHit(*hits[0], PFQHIT_NODE, &node);
//         the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);
         pfQueryHit(*hits[0], PFQHIT_GSET, &gset);
         the_mask = pfGetGSetIsectMask(gset);
         if ( int((the_mask & PERMANENT_MASK) >> PERMANENT_SHIFT)
            != PERMANENT_WATER)
           {  
            G_dynamic_data->dead = DYING;
            G_dynamic_data->cause_of_death = GROUND_DEATH;   
            //senddetonation(posture.xyz);                  
            return(HITSOMETHING);
           }//endif
         }//endif
        }//endelse
      

  //if we got here it did not hit anything 
  return FINEWITHME; 
   }

extern pfGroup *G_fixed;

float SUBMERSIBLE_VEH::get_depth()

{
 long        isect;
 pfSegSet    segment;
 pfHit       **hits[32];
 pfVec3      pnt; 
 float depth;

 /* make a ray looking "up" at terrain */

 /* find intersection with terrain */
 segment.userData = NULL;
 segment.bound = NULL;
 segment.discFunc = NULL;
 segment.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
 segment.activeMask = 0x01;
 segment.isectMask = (PERMANENT_MASK); 
 pfCopyVec3(segment.segs[0].pos, posture.xyz);
 segment.segs[0].pos[Z] += 0.1f;
 pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, 1.0f);
 segment.segs[0].length = fabs(segment.segs[0].pos[Z]) + 300.0f + 100.0f;
   
 isect = pfNodeIsectSegs(G_fixed, &segment, hits);

 if (isect)                                                                                 
   {
    pfQueryHit(*hits[0], PFQHIT_POINT, pnt);
    depth = pnt[Z] - posture.xyz[Z];
   }
 else
   {
    segment.userData = NULL;
    segment.bound = NULL;
    segment.discFunc = NULL;
    segment.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
    segment.activeMask = 0x01;
    segment.isectMask = (PERMANENT_MASK); 
    pfCopyVec3(segment.segs[0].pos, posture.xyz);
    segment.segs[0].pos[Z] -= 0.1f;
    pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, 1.0f);
    segment.segs[0].length = fabs(segment.segs[0].pos[Z]) + 300.0f + 10000.0f;
   
    isect = pfNodeIsectSegs(G_fixed, &segment, hits);

    if ( isect )
      {
       pfQueryHit(*hits[0], PFQHIT_POINT, pnt);
       depth = pnt[Z] - posture.xyz[Z];
      }
    else
       depth = 0.0f;
   }
   return depth;
}

#else
movestat SUBMERSIBLE_VEH::move()
// move the vehicle for the next frame
{
   pfMatrix orient;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 up = { 0.0f, 0.0f, 1.0f };
   float speed_setting = cdata.thrust * MAX_SPEED;
   pfVec3 oldpos;
   float altitude;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tSUBMERSIBLE_VEH::move" << endl;
#endif

   pfCopyVec3(oldpos,posture.xyz);

   update_time ();
   collect_inputs ();

#ifdef TRACE
   cerr << "Moving a Submersible Vehicle\n";
#endif

   if(!(paused || tethered || cdata.rigid_body))
      {
      // Move to the location we should be at after the last time interval
      posture.xyz[X] += velocity[X]*cdata.delta_time;
      posture.xyz[Y] += velocity[Y]*cdata.delta_time;
      posture.xyz[Z] += velocity[Z]*cdata.delta_time;

      altitude = 0.0f;
      // Now see if we hit something in this time interval
      if (!G_static_data->stealth)
         {
         if ( check_collide(oldpos, posture.xyz,
                            posture.hpr,
                            speed, altitude) )
            return(HITSOMETHING);
         }


      // Vehicle pitch is determined by the surface, so roll increments will
      // be used to increment the heading. Pitch increments are ignored.
      posture.hpr[HEADING] -= cdata.roll;
      if (posture.hpr[HEADING] < 0.0f)
         posture.hpr[HEADING] += 360.0f;

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
      // Don't want to exceed the max speed
      if (speed > MAX_SPEED)
         speed = MAX_SPEED;
      // Don't want to go backwards too fast
      if (speed < MAX_REVERSE)
         speed = MAX_REVERSE;

      // Now plant the sub back on the surface if it is above,
      // make sure we are still on the water, if not, stop.
      if (posture.xyz[Z] > 0.0f)
         water_orient(posture);

      // Calculate the new velocity vector
      pfMakeEulerMat ( orient, posture.hpr[HEADING],
                               posture.hpr[PITCH],
                               posture.hpr[ROLL]);

      pfFullXformPt3 ( direction, direction, orient );
      pfNormalizeVec3 ( direction );
      pfScaleVec3 ( velocity, speed, direction);
 
#ifdef DEBUG
      cerr << "GRND_ORIENT POSTURE X = " << posture.xyz[X] << "  Y = "
           << posture.xyz[Y] << "  Z = " << posture.xyz[Z] << endl;
      cerr << "                    H = " << posture.hpr[X] << "  P = "
           << posture.hpr[Y] << "  R = " << posture.hpr[Z] << endl;
#endif
 
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

/* // This segment of code is a hook for the implementing the
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

#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

   //put the vehicle in the correct location in space
   pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
   pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);

#ifdef DEBUG_SCALE
   pfDCSScale(hull,2.0f);
#endif

   thrust = cdata.thrust;

   return(FINEWITHME);

}

movestat SUBMERSIBLE_VEH::moveDR(float deltatime,float curtime)
//move routine for DR vehicles simple move
{
  float accelfac;  // 1/2*T used for accelerations
   static float oldspeed = 0.0f;
   float speed;

#ifdef TRACE
   cerr << "Dead Recon a Submersible Vehicle\n";
#endif

#ifndef NO_PFSMOKE
   update_pfsmokes();
#endif

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
      drpos.xyz[X] += velocity[X]*deltatime+accelfac*velocity[X];
      drpos.xyz[Y] += velocity[Y]*deltatime+accelfac*velocity[Y];
      drpos.xyz[Z] += velocity[Z]*deltatime+accelfac*velocity[Z];
      drparam.xyz[X] += accelfac*velocity[X];
      drparam.xyz[Y] += accelfac*velocity[Y];
      drparam.xyz[Z] += accelfac*velocity[Z];
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
  if (drpos.xyz[Z] > 0.0f)
     water_orient(drpos);

  if (vid != G_drivenveh)
  {
     posture = drpos;
     //put the vehicle in the correct location in space
     pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y],posture.xyz[Z]);
     pfDCSRot(hull,posture.hpr[HEADING],posture.hpr[PITCH], posture.hpr[ROLL]);

#ifdef DEBUG_SCALE
  pfDCSScale(hull,2.0f);
#endif

  }
  else if ( !G_static_data->stealth )
  {
    speed = pfLengthVec3(velocity);
    //this is us, have we exceeded the DR error?
    if ( (G_static_data->DIS_heartbeat_delta < (curtime - lastPDU)) ||
         (delta_send(&posture,&drpos)))
    {

#ifdef DEBUG
       cerr <<"X       " << posture.xyz[HEADING] << " " <<drpos.xyz[HEADING] <<
"\n";
       cerr <<"Y       " << posture.xyz[PITCH] << " " <<drpos.xyz[PITCH] << "\n";
       cerr <<"Z       " << posture.xyz[ROLL] << " " <<drpos.xyz[ROLL] << "\n";
       cerr <<"HEADING " << posture.hpr[HEADING] << " " <<drpos.hpr[HEADING] <<
"\n";
       cerr <<"PITCH   " << posture.hpr[PITCH] << " " <<drpos.hpr[PITCH] << "\n";
       cerr <<"ROLL    " << posture.hpr[ROLL] << " " <<drpos.hpr[ROLL] << "\n";
       cerr << "send a delta pdu\n";
       cerr << "acceleration" << acceleration[X] << " " << acceleration[Y]
                              << " " << acceleration[Z] << endl;

#endif
       sendentitystate();
       oldspeed = speed;
    }
    else if ( speed != oldspeed )
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
          }
       else if ( fabs(oldspeed-speed) > 50.0f )
          {
          sendentitystate();
          oldspeed = speed;
          }
       }

  }
      return(FINEWITHME);
}


vtype SUBMERSIBLE_VEH::gettype()
{
    return (SUBMERSIBLE);
}


int SUBMERSIBLE_VEH::check_collide(pfVec3 &startpnt, pfVec3 &endpnt,
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
   pfSeg *sp[2] ;
   pfCylinder the_barrel;
   long the_mask;
   pfVec3 incoming_vec;
   pfVec3 outgoing_vec;
   pfVec3 normal_vec;
   pfVec3 eye_vec, foot_vec;
   pfVec3 start_eye, end_eye;
   pfVec3 start_pos, end_pos;
   float temp_float;
   pfMatrix Orient;
   pfVec3 tb_vec = { 0.0f, 0.0f, 1.0f };
   int the_item_hit;

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

   pfSetVec3 ( tb_vec, 0.0f, 0.0f, 1.0f );
   pfMakeEulerMat ( Orient, orientation[HEADING],
                            orientation[PITCH],
                            orientation[ROLL] );
   pfFullXformPt3 ( tb_vec, tb_vec, Orient );
   pfScaleVec3 ( eye_vec, EYE_OFFSET, tb_vec );
   pfScaleVec3 ( foot_vec, INTERSECT_OFFSET, tb_vec );
   pfAddVec3 ( start_eye, startpnt, eye_vec );
   pfAddVec3 ( end_eye, endpnt, eye_vec );
   pfAddVec3 ( start_pos, startpnt, foot_vec );
   pfAddVec3 ( end_pos, endpnt, foot_vec );

   
   //create a segment along the path we just followed
   pfSetVec3(seg.segs[0].dir,(end_pos[X] - start_pos[X]),
                        (end_pos[Y] - start_pos[Y]),
                        (end_pos[Z] - start_pos[Z]));
   pfSetVec3(seg.segs[1].dir,(end_eye[X] - start_eye[X]),
                        (end_eye[Y] - start_eye[Y]),
                        (end_eye[Z] - start_eye[Z]));

   seg.segs[0].length = pfLengthVec3(seg.segs[0].dir);
   seg.segs[1].length = pfLengthVec3(seg.segs[1].dir);

   PFCOPY_VEC3(seg.segs[0].pos,start_pos);
   PFCOPY_VEC3(seg.segs[1].pos,start_eye);

   //fix a performer bug
   if(seg.segs[0].dir[Y] == 0.0) seg.segs[0].dir[Y] = 0.01f;
   if(seg.segs[0].dir[X] == 0.0) seg.segs[0].dir[X] = 0.01f;
   if(seg.segs[1].dir[Y] == 0.0) seg.segs[1].dir[Y] = 0.01f;
   if(seg.segs[1].dir[X] == 0.0) seg.segs[1].dir[X] = 0.01f;

   pfNormalizeVec3(seg.segs[0].dir);
   pfNormalizeVec3(seg.segs[1].dir);
 
   sp[0] = &(seg.segs[0]);
   sp[1] = &(seg.segs[1]);

  pfCylAroundSegs ( &the_barrel, (const pfSeg **)sp, 2 );
#ifdef TRACE
   cerr << "Chck Pt 2 Number of Kids " << pfGetNumChildren(G_moving) <<"\n";
#endif


   //*** intersect test against the fixed objects ********************//

   if (altitude < 1000.0f)
      {
      seg.mode = PFTRAV_IS_PRIM;
      seg.userData = NULL;
      seg.activeMask = 0x03;
      seg.isectMask = DESTRUCT_MASK;
      seg.bound = NULL;
      seg.discFunc = buildinghit;
      isect = pfNodeIsectSegs(G_fixed, &seg, hits);
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
      if ( flags )
         {
         pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
         pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
         }
      else
         {
         pfQueryHit(*hits[1], PFQHIT_POINT, &pnt);
         pfQueryHit(*hits[1], PFQHIT_XFORM, &xform);
         pfQueryHit(*hits[1], PFQHIT_FLAGS, &flags);
         }
      if (isect && (flags & PFHIT_POINT))
         {
         /*set the point at the intersectin point in the world COORDINATES*/
         pfFullXformPt3(posture.xyz, pnt, xform);

#ifdef DEBUG
         cout << "\n\nBuilding hit at " << posture.xyz[X] << " "
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
         }
      }

   //******* intersect test against the ground *********************//
   seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
   seg.userData = NULL;
   seg.activeMask = 0x03;
   seg.isectMask = PERMANENT_MASK;
   seg.bound = NULL;
   seg.discFunc = NULL;
   if (pfNodeIsectSegs(G_fixed, &seg, hits))
      {
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
      if ( flags )
          {
//          pfQueryHit(*hits[0], PFQHIT_NODE, &node);
          pfQueryHit(*hits[0], PFQHIT_GSET, &gset);
          pfQueryHit(*hits[0], PFQHIT_NORM, &normal);
          pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
          pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
          hit_index = 0;
          }
      else
          {
          pfQueryHit(*hits[1], PFQHIT_FLAGS, &flags);
//          pfQueryHit(*hits[1], PFQHIT_NODE, &node);
          pfQueryHit(*hits[1], PFQHIT_GSET, &gset);
          pfQueryHit(*hits[1], PFQHIT_NORM, &normal);
          pfQueryHit(*hits[1], PFQHIT_XFORM, &xform);
          pfQueryHit(*hits[1], PFQHIT_POINT, &pnt);
          hit_index = 1;
          }

//      the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);
    
      the_mask = pfGetGSetIsectMask(gset);
      the_item_hit = int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT);

      // find the orientation of the ground
      if ( ((the_item_hit == PERMANENT_DIRT) ||
            (the_item_hit == PERMANENT_BRIDGE)) &&
           (flags & PFHIT_NORM) )
         {
         speed = 0.0f;
         pfCopyVec3 ( endpnt, startpnt );
         return(HITSOMETHING);
         }      // end intersection test

      else if ( the_item_hit == PERMANENT_FOILAGE) 
         { // Hit the water or foilage
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

#endif
