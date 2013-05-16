
#include <iostream.h>
#include <bstring.h>

#include "person.h"
#include "appearance.h"
#include "jointmods.h"
#include "human_const.h"
#include "manager.h"
#include "jack_di_veh.h"
#include "DI_comm.h"
#include "munitions.h"
#include "interface_const.h"

// #define HIRESTEST


// Globals used by this library
extern volatile DYNAMIC_DATA    *G_dynamic_data;
extern volatile  MANAGER_REC    *G_manager;
extern volatile float            G_curtime;


void
PERSON_VEH::go_standing ()
{
   if ( posture_chg_ok() ) {
      float speed = pfLengthVec3(velocity);
      unsigned int new_status;

      if(speed == 0.0f) {
         new_status=Appearance_LifeForm_UprightStill;
      }
      else if(speed <= 3.0f) {
         new_status=Appearance_LifeForm_UprightWalking;
      }
      else if(speed > 3.0f) {
         new_status=Appearance_LifeForm_UprightRunning;
      }

      switch(status & Appearance_LifeForm_Weapon1_Mask) {
         case Appearance_LifeForm_Weapon1_Deployed:
            new_status |= Appearance_LifeForm_Weapon1_Deployed;
            break;
         case Appearance_LifeForm_Weapon1_Stowed:
            new_status |= Appearance_LifeForm_Weapon1_Stowed;
            break;
         default:
            new_status |= Appearance_LifeForm_Weapon1_Stowed;
      }

      status = new_status;
      posture_chg_time = G_curtime;
   }
}


void
PERSON_VEH::go_kneeling ()
{
   if ( posture_chg_ok() ) {
      float speed = pfLengthVec3(velocity);
      unsigned int new_status;

      new_status=Appearance_LifeForm_Kneeling;

      switch(status & Appearance_LifeForm_Weapon1_Mask) {
         case Appearance_LifeForm_Weapon1_Deployed:
            new_status |= Appearance_LifeForm_Weapon1_Deployed;
            break;
         case Appearance_LifeForm_Weapon1_Stowed:
            new_status |= Appearance_LifeForm_Weapon1_Stowed;
            break;
         default:
            new_status |= Appearance_LifeForm_Weapon1_Stowed;
      }

      status = new_status;
      posture_chg_time = G_curtime;
   }
}


void
PERSON_VEH::go_prone()
{
   if ( posture_chg_ok() ) {
      float speed = pfLengthVec3(velocity);
      unsigned int new_status;

      if (speed == 0.0f) {
         new_status=Appearance_LifeForm_Prone;
      }
      else {
         new_status=Appearance_LifeForm_Crawling;
      }

      switch(status & Appearance_LifeForm_Weapon1_Mask) {

         case Appearance_LifeForm_Weapon1_Deployed:
            new_status |= Appearance_LifeForm_Weapon1_Deployed;
            break;
         case Appearance_LifeForm_Weapon1_Stowed:
            new_status |= Appearance_LifeForm_Weapon1_Stowed;
            break;
         default:
            new_status |= Appearance_LifeForm_Weapon1_Stowed;
      }
      status = new_status;
      posture_chg_time = G_curtime;
   }
}


void
PERSON_VEH::toggle_weapon_status()
{
   if ( posture_chg_ok() ) {
      switch(status & Appearance_LifeForm_Weapon1_Mask) {
         case Appearance_LifeForm_Weapon1_Stowed:
           // Weapon is stowed, so deploy weapon
           changeWeaponStatus(Appearance_LifeForm_Weapon1_Deployed);
           break;
         case Appearance_LifeForm_Weapon1_Deployed:
           // Weapon is deployed, so stow weapon
           changeWeaponStatus(Appearance_LifeForm_Weapon1_Stowed);
           break;
         default:
           cerr << "Invalid weapon mode default position stowed\n";
           changeWeaponStatus(Appearance_LifeForm_Weapon1_Stowed);
      }

      posture_chg_time = G_curtime;
   }
}


void
PERSON_VEH::class_specific_keyboard()
{
   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;
   int control_pressed = FALSE;
   int shift_pressed = FALSE;

   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_CONTROLKEY);
   control_pressed = (counter > 0);
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_SHIFTKEY);
   shift_pressed = (counter > 0);

   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_Q);
   // Ctrl-Q
   if ( counter > 0 ) {
      setSignal(CLOSE_UP);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_G);
   // Ctrl-G
   if ( counter > 0 ) {
      setSignal(SPEED);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_H);
   // Ctrl-H
   if ( counter > 0 ) {
      setSignal(SLOW);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_J);
   // Ctrl-J
   if ( counter > 0 ) {
      setSignal(MOVE_RIGHT);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_K);
   // Ctrl-K
   if ( counter > 0 ) {
      setSignal(MOVE_LEFT);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_W);
   // Ctrl-W
   if ( counter > 0 ) {
      setSignal(OPEN_UP);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_F);
   // Ctrl-F
   if ( counter > 0 ) {
      setSignal(J_DOWN);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_A);
   // Ctrl-A
   if ( counter > 0 ) {
      setSignal(J_FORWARD);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_D);
   // Ctrl-D
   if ( counter > 0 ) {
      setSignal(POINT_TO);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,char(127));
   // Ctrl-8
   if ( counter > 0 ) {
      setSignal(SQUAD);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_S);
   // Ctrl-S
   if ( counter > 0 ) {
      setSignal(HALT);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,char(31));
   // Ctrl-7
   if ( counter > 0 ) {
      setSignal(FIRETEAM);
   }
   

   // Some keys have the same value for them as do their control-key
   // counterparts.
   if ( control_pressed ) {
      l_input_manager->query_button(counter,NPS_KEYBOARD,'9');
      if ( counter > 0 ) {
         setSignal(PLATOON);
      }
      if ( !shift_pressed ) {
         l_input_manager->query_button(counter,NPS_KEYBOARD,'*');
         if ( counter > 0 ) {
            setSignal(WOUNDED_ONBOARD);
         }
      }
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_N);
   if ( counter > 0 ) {
      setSignal(ECHELON_RIGHT);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_O);
   if ( counter > 0 ) {
      setSignal(ECHELON_LEFT);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_P);
   if ( counter > 0 ) {
      setSignal(SALUTE);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_R);
   if ( counter > 0 ) {
      setSignal(VEE);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_T);
   if ( counter > 0 ) {
      setSignal(COLUMN);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_U);
   if ( counter > 0 ) {
      setSignal(WEDGE);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_V);
   if ( counter > 0 ) {
      setSignal(J_LINE);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'7');
   if ( counter > 0 ) {
      changestatus(Appearance_DamageDestroyed);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'5');
   if ( counter > 0 ) {
      toggle_weapon_status();
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'8');
   if ( counter > 0 ) {
      go_kneeling();
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'9');
   if ( !control_pressed && (counter > 0) ) {
      go_prone();
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'e');
   if ( counter > 0 ) {
      go_standing();
   }


   if ( shift_pressed && !control_pressed ) {
      l_input_manager->query_button(counter,NPS_KEYBOARD,'*');
      if ( counter > 0 ) {
         setSignal(NOSIGNAL);
      }
   }

   if ( !shift_pressed && !control_pressed ) {
      l_input_manager->query_button(counter,NPS_KEYBOARD,'*');
      if ( counter > 0 ) {
         changestatus(Appearance_LifeForm_Medic_Mask);
      }
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'Z');
   if ( counter > 0 ) {
      lase ();
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'X');
   if ( counter > 0 ) {
      video_switch = !video_switch;
cerr << "Caught key 'X', video_switch: " << video_switch << endl;
   }

#ifdef HIRESTEST
   static float            modangle = 0.0f;
  
   for (int ix = 0; ix < MIN_NUM_HIRES_JOINTS; ix++) {
      hiResJointAngles[ix].datum = modangle * DTOR;
   }

   numHiResJointAngles = MIN_NUM_HIRES_JOINTS;

   set_net_type (hiResNet);

   okToSendHiRes = TRUE;
   okToUpdateHiRes = TRUE;
//   immed_transition = TRUE;

   modangle += 0.5f;
   if (modangle > 360) {
      modangle = 0.0f;
   }
#endif // HIRESTEST

  
}


void
PERSON_VEH::process_flybox_input ()
{
   float value = 0.0f;
   int counter = 0;
   NPS_SWITCH_VAL setting;
   static NPS_SWITCH_VAL old_rusetting = NPS_SWITCH_RELEASED;
   static NPS_SWITCH_VAL old_rmsetting = NPS_SWITCH_RELEASED;
   input_manager *l_input_manager = G_manager->ofinput;

   l_input_manager->query_button(counter,NPS_FLYBOX,
                                 NPS_STCK_TRIGGER,input_number);
     if ( counter > 0 ) {
      fire_weapon ( PRIMARY, counter );
   }
   l_input_manager->query_valuator(value,NPS_FLYBOX,
                                   NPS_STCK_HORIZONTAL,input_number);
   if ( value != 0.0f ) {
      set_horizontal(value);
   }
   l_input_manager->query_valuator(value,NPS_FLYBOX,
                                   NPS_STCK_VERTICAL,input_number);
   if ( value != 0.0f ) {
      change_gaze_vertical(-value);
   }
   l_input_manager->query_valuator(value,NPS_FLYBOX,
                                   NPS_STCK_TWIST,input_number);
   if ( value != 0.0f ) {
      change_gaze_horizontal(-value);
   }
   l_input_manager->query_valuator(value,NPS_FLYBOX,
                                   NPS_LEVER_RIGHT,input_number);
//   l_input_manager->calibrate(NPS_STCK,NPS_DEAD_ZONE,NPS_FLYBOX,
//                              value,input_number);
   l_input_manager->query_valuator(value,NPS_FLYBOX,
                                   NPS_LEVER_LEFT,input_number);
   set_thrust(value);


   l_input_manager->query_button(counter,NPS_FLYBOX,
                                 NPS_MOMLEFT_BUTTON,input_number);
   if ( counter > 0 ) {
      set_gaze_vertical(0.0f);
   }
   l_input_manager->query_button(counter,NPS_FLYBOX,
                                 NPS_MOMRIGHT_BUTTON,input_number);
   if ( counter > 0 ) {
      set_gaze_horizontal(0.0f);
   }
   
 
   l_input_manager->query_switch(setting,NPS_FLYBOX,
                                 NPS_SWITCH_TOPLEFT,input_number);
   if ( setting == NPS_SWITCH_PRESSED ) {
      go_standing();
   }
   l_input_manager->query_switch(setting,NPS_FLYBOX,
                                 NPS_SWITCH_TOPRIGHT,input_number);
   if ( setting == NPS_SWITCH_PRESSED ) {
      if ( old_rusetting != setting ) {
         setSignal(J_FORWARD);
         old_rusetting = setting;
      }
   }
   else if ( setting == NPS_SWITCH_RELEASED ) {
      if ( old_rusetting != setting ) {
         old_rusetting = setting;
      }
   }
   l_input_manager->query_switch(setting,NPS_FLYBOX,
                                 NPS_SWITCH_MIDRIGHT,input_number);
   if ( setting == NPS_SWITCH_PRESSED ) {
      if ( old_rmsetting != setting ) {
         setSignal(SALUTE);
         old_rmsetting = setting;
      }
   }
   else if (setting == NPS_SWITCH_RELEASED ) {
      if ( old_rmsetting != setting ) {
         old_rmsetting = setting;
      }
   }
   l_input_manager->query_switch(setting,NPS_FLYBOX,
                                 NPS_SWITCH_MIDLEFT,input_number);
   if ( setting == NPS_SWITCH_PRESSED ) {
      go_kneeling();
   }
   l_input_manager->query_switch(setting,NPS_FLYBOX,
                                 NPS_SWITCH_BOTLEFT,input_number);
   if ( setting == NPS_SWITCH_PRESSED ) {
      go_prone();
   }      
   l_input_manager->query_switch(setting,NPS_FLYBOX,
                                 NPS_SWITCH_BOTRIGHT,input_number);
   if (setting == NPS_SWITCH_PRESSED) {
      if ( posture_chg_ok() ) {
         changeWeaponStatus(Appearance_LifeForm_Weapon1_Deployed);
      }
   }
   else if (setting == NPS_SWITCH_RELEASED) {
      if (posture_chg_ok()) {
         changeWeaponStatus(Appearance_LifeForm_Weapon1_Stowed);
      }
   }

                                 
}

void 
PERSON_VEH::class_specific_fcs()
{

   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;

   l_input_manager->query_button(counter,NPS_FCS,
                                 NPS_THRTL_BUTTON_7,input_number);
   if (counter > 0) {
#ifndef NONPSSHIP
      if (G_dynamic_data->pick_mode != SHIP_PICK)
#endif // NONPSSHIP
        go_prone();
   }

   l_input_manager->query_button(counter,NPS_FCS,
                                 NPS_THRTL_BUTTON_6,input_number);
   if (counter > 0) {
      go_kneeling();
   }

   l_input_manager->query_button(counter,NPS_FCS,
                                 NPS_THRTL_BUTTON_5,input_number);
   if (counter > 0) {
      go_standing();
   } 

   l_input_manager->query_button(counter,NPS_FCS,
                                 NPS_THRTL_BUTTON_4,input_number);
   if ( (counter > 0) && posture_chg_ok() ) {
      setSignal(J_FORWARD);
      posture_chg_time = G_curtime;
   }

   l_input_manager->query_button(counter,NPS_FCS,
                                 NPS_THRTL_BUTTON_3,input_number);
   if ( (counter > 0) && posture_chg_ok() ) {
      status = Appearance_LifeForm_Medic_Mask;
      posture_chg_time = G_curtime;
   }

   l_input_manager->query_button(counter,NPS_FCS,
                                 NPS_THRTL_BUTTON_2,input_number);
   if ( (counter > 0) && posture_chg_ok() ) {
      setSignal(SALUTE);
      posture_chg_time = G_curtime;
   }


   // NOTE:  Since the base vehicle class traps STCK MID BUTTON,
   //    a tertiary weapon file will also take place in addtion to
   //    the code below.  If this is not desired, the virtual function
   //    "fire_weapon" will have to be defined for this class.
   l_input_manager->query_button(counter,NPS_FCS,
                                 NPS_STCK_MID_BUTTON,input_number);
   if (counter > 0) {
      toggle_weapon_status();
   }


}

#ifndef NO_IPORT

void
PERSON_VEH::class_specific_testport ()
{
   int            state;
   input_manager *l_input_manager = G_manager->ofinput;
   
   l_input_manager->query_state (state, NPS_TESTPORT, NPS_STATE_IPORT);

   switch (state) {
      case NOSIGNAL:
         if (posture_chg_ok () ) {
            go_standing ();
            setSignal (NOSIGNAL);
            posture_chg_time = G_curtime;
         }
         break;
      default:
         if (posture_chg_ok () ) {
            setSignal (state);
            posture_chg_time = G_curtime;
         }
         break;
   }

#ifndef NOHIRESNET
   static float            modangle = 0.0f;
   
   for (int ix = 0; ix < MIN_NUM_HIRES_JOINTS; ix++) {
      hiResJointAngles[ix].datum = modangle * DTOR;
   }

   numHiResJointAngles = MIN_NUM_HIRES_JOINTS;

   set_net_type (hiResNet);

   okToSendHiRes = TRUE;
   okToUpdateHiRes = TRUE;
//   immed_transition = TRUE;

   modangle += 0.5f;
   if (modangle > 360) {
      modangle = 0.0f;
   }
#endif // NOHIRESNET
                                 
}

void
PERSON_VEH::class_specific_uniport ()
{
   input_manager *l_input_manager = G_manager->ofinput;
   int iport_state;
   int theResult = FALSE;

   l_input_manager->query_state ( iport_state, NPS_UNIPORT,
                                 NPS_STATE_IPORT);

   theResult = l_input_manager->query_multival ( (void *) &iport_rifle_coord,
                                                NPS_UNIPORT,
                                                NPS_MV_IPORT_RIFLE);

   if (theResult && iport_rifle_dcs) {
      iport_rifle_on ();
//      cerr << iport_rifle_coord.xyz[0] << "  "
//           << iport_rifle_coord.xyz[1] << "  "
//           << iport_rifle_coord.xyz[2] << endl;
      iport_rifle_coord.xyz[2]  += posture.xyz[2];
      pfDCSCoord (iport_rifle_dcs, &iport_rifle_coord);
   }

   theResult = l_input_manager->query_multival ( (void *)hiResJointAngles,
                                                NPS_UNIPORT,
                                                NPS_MV_UPPERBODY);

   if (theResult && iport_rifle_dcs) {
      okToUpdateHiRes = TRUE;
   }

   switch (iport_state) {
      case PRONE:
         go_prone ();
         jack_rifle_on ();
         okToSendHiRes = FALSE;
         okToUpdateHiRes = FALSE;
         set_net_type (disNet);
         break;
      case KNEELING:
         go_kneeling ();
         jack_rifle_on ();
         okToSendHiRes = FALSE;
         okToUpdateHiRes = FALSE;
         set_net_type (disNet);
         break;
      case STANDING:
         go_standing ();
         break;
      default:
         break;
   }
}



#endif // NO_IPORT

#ifndef NOUPPERBODY
void
PERSON_VEH::class_specific_upperbody ()
{
   input_manager *l_input_manager = G_manager->ofinput;

   l_input_manager->query_multival ( (void *)hiResJointAngles,
                                    NPS_UPPERBODY, NPS_MV_UPPERBODY);
/*
cerr << "hiResJointAngles: " << endl << "\t"
     << hiResJointAngles[RCX].datum << "  "
     << hiResJointAngles[RCY].datum << "  "
     << hiResJointAngles[RCZ].datum << "  "
     << hiResJointAngles[RCH].datum << "  "
     << hiResJointAngles[RCP].datum << "  "
     << hiResJointAngles[RCR].datum << endl;

   l_input_manager->query_multival ( (void *) hiResJointAngles,
                                    NPS_UPPERBODY,
                                    NPS_MV_RIFLE);

   for (int ix = 0; ix <= 2; ix++) {
      gun_pos.xyz[ix] = hiResJointAngles[RCX + ix].datum;
      gun_pos.hpr[ix] = hiResJointAngles[RCH + ix].datum;
   }

   gun_pos.xyz[PF_Z] += 1.0;
*/

   numHiResJointAngles = NUM_HIRES_VALUES;

   set_net_type (hiResNet);
   hi_res_rifle_on ();

   okToSendHiRes = TRUE;
   okToUpdateHiRes = TRUE;

}
#endif // NOUPPERBODY

void
PERSON_VEH::process_keyboard_speed_settings ()
{
   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;

   l_input_manager->query_button(counter,NPS_KEYBOARD,'s');
   if ( counter > 0 ) {
      set_thrust ( 0.0f );
      pfSetVec3 ( velocity, 0.0f, 0.0f, 0.0f );
      pfSetVec3 ( acceleration, 0.0f, 0.0f, 0.0f );
      speed = 0.0f;
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'a');
   if ( counter > 0 ) {
      change_thrust ( float(counter)*KEY_PERSON_SPEED_CHANGE );
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'d');
   if ( counter > 0 ) {
      change_thrust ( float(-counter)*KEY_PERSON_SPEED_CHANGE );
   }
}

void
PERSON_VEH::fire_weapon ( const wdestype weapon_num, const int /* num_times */ )
{
   input_manager *l_input_manager = G_manager->ofinput;
   if ( (input_control == NPS_UPPERBODY) || (input_control == NPS_UNIPORT) ) {
      if (l_input_manager->verify_device (NPS_UPPERBODY) ) {
/*         l_input_manager->query_multival ( (void *) &gun_pos,
                                          NPS_UPPERBODY,
                                          NPS_MV_TARGET_RIFLE);
*/
         firebuttonhit (weapon_num, vid, posture, gun_pos);
      }
      else {
         pfCoord noLook;
         bzero ( (void *) &noLook, sizeof(pfCoord));
         firebuttonhit ( weapon_num, vid, iport_rifle_coord, noLook );
      }
   }
   else {
      firebuttonhit ( weapon_num, vid, posture, look );
   }
}
