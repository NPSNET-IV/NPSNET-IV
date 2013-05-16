// File: <jack_di.cc> 
//
// NPSNETIV Version 6.0  dtd 06-06-94
// Performer Version 1.0, 1.1 and 1.2 Simulation Code
// Written by:  Paul T. Barham, Roy D. Young, David R. Pratt & Randall Barker
// Naval Postgraduate School, Computer Science Department
// Code CS/Pratt, Monterey, CA 93940-5100 USA
//
// This source code is property of the Naval Postgraduate School.
// Copying, distributing and/or using all or part of this code for
// commercial gain without written permission is prohibited.
//
// Please do not distribute part or all of this source code without
// notifying one or all of the following people:
//
//    Paul Barham, barham@cs.nps.navy.mil
//    David Pratt, pratt@cs.nps.navy.mil
//    Michael Zyda, zyda@cs.nps.navy.mil
//
// This condition is so we can keep track of who has our code to
// assist in justifying our research efforts to our sponsors.
//
// We are also starting a NPSNET-IV mailing list for notification of
//    future changes.  To have your name added to the mailing list,
//    send email to barham@cs.nps.navy.mil.
//
// We do requeust that you give us credit in all projects / papers / 
// demonstrations that use our code.  Do not remove the 
// "Naval Postgraduate School" title from the graphics screen without
// written permission from one of the above listed people.
//
// Change Record:
// Date        Changes                                       Programmer
// 10-19-93    Official release 2.0 w/ documentation         All
// 03-15-94    Change release to 3.0 for addition of
//             articulated parts, animations, munition-
//             specific effects, round-world coordinates,
//             and excersise ID usage.                       Barham
// 06-06-94    Official release 4.0 with Multicast
//             and major upgrades for graphics speed.        Barham
// 07-07-94    Ported to Performer 1.2 for release 5.0.      Barker
// 07-18-94    Used the new Performer 1.2 meathod for        Barker
//             interection testing in release 6.0.
//             This fixed the front face testing
//             failure from release 5.0.            
//

#ifdef JACK

#include <stdio.h>
#include <stdlib.h>
#include <pf.h>
#include <pfdu.h>
#include <bstring.h>

#include "jack_di_veh.h"
#include "jack.h"
#include "human_const.h"
#include "common_defs2.h"
#include "appearance.h"
#include "disnetlib.h"
#include "netutil.h"
#include "terrain.h"
#include "conversion_const.h"
#include "collision_const.h"
#include "manager.h"

#ifndef NO_DVW
#define NO_PFSMOKE
#define NO_NPSSMOKE
#endif

#ifndef NO_PFSMOKE
#define NO_NPSSMOKE
#endif

// Globals used by this library
extern volatile DYNAMIC_DATA    *G_dynamic_data;
extern volatile STATIC_DATA     *G_static_data;
extern          VEH_LIST        *G_vehlist;
extern          VEHTYPE_LIST    *G_vehtype;
extern volatile int              G_drivenveh;
extern          DIS_net_manager *G_net;
#ifndef NOHIRESNET
extern          DIS_net_manager *G_hr_net;
extern volatile float            G_curtime;
#endif // NOHIRESNET
#ifndef NOUPPERBODY
extern volatile MANAGER_REC     *G_manager;
#endif // NOUPPERBODY
extern          pfGroup         *G_moving;

extern          short           *G_JackSoldier_active;
extern          int              G_num_soldiers;
extern          int              G_tot_soldiers;
extern          void             firebuttonhit(wdestype,int,pfCoord,pfCoord);

void send_silver_bullet(int target);
void getJackWristTx(JackSoldier *js,
                    pfMatrix H_rwrist, pfMatrix H_lwrist);

#if defined(JACK) && defined(SS_INTERFACE)
#include "idunetlib.h"
static int L_use_ss_interface = FALSE;
static IDU_net_manager *L_idu_net = NULL;

static char    *L_ss_group = "224.200.200.201";
static u_short  L_ss_port = 3201;
static u_char   L_ss_ttl = 3;
static char    *L_ss_interface = "";
static int      L_ss_loopback = TRUE;


void jack_use_ss_interface ( int mode )
{
   L_use_ss_interface = mode;
}
#endif

// Utility functions used for medic mode code
// A function to convert NPS (N=0,E=90) headings (in degrees)
// into "standard" (E=0,N=90) mathematical headings (in radians)
double nps_to_rad (double ang)
{
  return ((90.0 + ang) * DEG_TO_RAD);
}


// The reverse of the function above

double rad_to_nps (double ang)
{
  return ((ang * RAD_TO_DEG) - 90.0);
}
// Note: Given a "standard" angle A, and a distance D,
// the location (X,Y) = (D*cos(A),D*sin(A)) is at a
// distance of "D" along the heading "A"


// A function to do "standard" arc-tan computations
// Return the "standard" angle between (0,0) and (x,y)
double atanxy (double x, double y)
{
  return (atan2 (y, x));
}


// A function to determine the angle to "desire" given
// an initial angle, and a "probable" desired angle
double pangle (double ihd, double fhd)
{
    // Make 'final' angle smaller than initial
    while (fhd > ihd) fhd -= M_PI * 2.0;

    // Make 'final' angle JUST BIGGER than initial
    while (ihd > fhd) fhd += M_PI * 2.0;
    
    // Hack -- Reverse the spin direction if needed
    if (fhd - ihd > M_PI) fhd -= M_PI * 2.0;

    // Return the new final angle
    return (fhd);
}


void checkBackwardMotion(float speed, float& oldspeed,
   pfVec3& velocity, float heading)
{
   // Need to determine if going backwards
   pfVec2 vel, headingvec;
   float hdg;
   vel[X] = velocity[X];
   vel[Y] = velocity[Y];
   pfNormalizeVec2(vel);
   hdg = 90.0f+heading;
   if(hdg < 0.0f) {
      hdg += 360.0f;
   }
   else if (hdg > 360.0f) {
      hdg -= 360.0f;
   }

   pfSinCos(hdg, &headingvec[Y],&headingvec[X]);
   // heading is opposite for DR entities
   if(pfDotVec2(headingvec,vel) < 0.0f) {
      oldspeed = -speed;
   }
   else {
      oldspeed = speed;
   }
} // end checkBackwardMotion()


//************************************************************************//
//*************** class JACK_DI ******************************************//
//************************************************************************//
JACK_DI_VEH::JACK_DI_VEH(int id,int atype, ForceID the_force)
: PERSON_VEH(id,atype,the_force)
{
   status = Appearance_LifeForm_UprightStill | Appearance_LifeForm_Weapon1_Stowed;
   Medic_update_flag = 0;
   unitMember = FALSE;
   behavior = COMBAT;
   upperJointMods = new JointMods();
   signals = new signalClass();
   create_entity ( atype, the_force );

   jack_rifle_sw = NULL;
   iport_rifle_sw = NULL;
   sensor_rifle_sw = NULL;

   sensor_rifle_dcs = NULL;
   iport_rifle_dcs = NULL;

#ifndef NOUPPERBODY
   // move the rifle and attach to the upper torso of the jack model.
   char node_name[50];
   const char *root_name = pfGetNodeName(hull);

//pfPrint(hull,NULL,PFPRINT_VB_INFO, NULL );

   if (root_name == NULL) {
      cerr << "Error: no hull node name found for Jack." << endl;
   }

   // find new attachment point for the rifle on the upper torso
   //"Di2/High_waist/High_belly/High_upwaist"
   sprintf(node_name, "%s/%s", root_name, js->jack->jointsinfo[UPWAIST].name);

   pfDCS *torso_dcs = (pfDCS*)pfLookupNode(node_name, pfGetDCSClassType());
   if (torso_dcs == NULL) {
      cerr << "Error: could not find Jack's upwaist DCS node." << endl;
   } 

   // js->figptrs[0] is the rifle DCS node
   sprintf(node_name, "%s/%s", root_name, "AK_47");
   pfDCS *rifle_dcs = (pfDCS*)pfLookupNode(node_name, pfGetDCSClassType());

#if 0
   if ( rifle_dcs == NULL ) {
      sprintf(node_name, "%s/%s", root_name, "M60");
      rifle_dcs = (pfDCS*)pfLookupNode(node_name, pfGetDCSClassType());
      if (rifle_dcs) {
         cerr << "Jack has an M60." << endl;
      }
   }
#endif

   if (rifle_dcs) {
      pfGroup *parent = pfGetParent(rifle_dcs, 0);
      pfRemoveChild(parent, rifle_dcs);

      //pfSync();

      // insert a switch into the Jack model and turn the rifle off
      jack_rifle_sw = pfNewSwitch();

      pfAddChild(jack_rifle_sw, rifle_dcs);
      pfAddChild(parent, jack_rifle_sw);
      pfSwitchVal(jack_rifle_sw, PFSWITCH_ON);

      // clone the rifle and add it to the desired location in the model
      iport_rifle_dcs = (pfDCS *) pfClone(rifle_dcs, 0);
      iport_rifle_sw = pfNewSwitch();
      pfSwitchVal (iport_rifle_sw, PFSWITCH_OFF );
      pfMatrix theMat;
      pfMakeEulerMat ( theMat, 90.0f, 0.0f, 0.0f );
      pfSCS *scs = pfNewSCS ( theMat );
      
      while (pfGetNumChildren (iport_rifle_dcs) ) {
         pfNode *theChild = pfGetChild (iport_rifle_dcs, 0);
         pfRemoveChild (iport_rifle_dcs, theChild);
         pfAddChild (scs, theChild);
      }
      pfAddChild (iport_rifle_dcs, scs);
      pfAddChild (iport_rifle_sw, iport_rifle_dcs);
      pfAddChild (G_moving, iport_rifle_sw);

      sensor_rifle_dcs = (pfDCS *) pfClone(rifle_dcs, 0);
      sensor_rifle_sw = pfNewSwitch();
      pfAddChild(sensor_rifle_sw, sensor_rifle_dcs);
      pfAddChild(torso_dcs, sensor_rifle_sw);
      pfSwitchVal(sensor_rifle_sw, PFSWITCH_OFF);

      pfMatrix mat;
      pfMakeIdentMat(mat);
      pfDCSMat(sensor_rifle_dcs, mat);
   }
   else {
//      cerr << "Error did not find rifle in Jack model.\n";
//      pfPrint(hull,NULL,PFPRINT_VB_NOTICE, NULL);
      sensor_rifle_dcs = js->figptrs[0];
      iport_rifle_dcs = NULL;
   }
#endif // NOUPPERBODY

#if defined(JACK) && defined(SS_INTERFACE)
   if ( (id == G_drivenveh) && L_use_ss_interface ) {
      L_idu_net = new IDU_net_manager ( L_ss_group, L_ss_port, L_ss_ttl,
                                        L_ss_interface, L_ss_loopback );
      if ( !L_idu_net->net_open() ) {
         cerr << "JACK_DI_VEH constructor:\tCannot open IDU_net_manager!"
              << endl;
         L_use_ss_interface = FALSE;
         delete L_idu_net;
         L_idu_net = NULL;
      }
   }
#endif // defined(JACK) && defined(SS_INTERFACE)

} // end JACK_DI_VEH::JACK_DI_VEH()

#ifndef NODUDE
JACK_DI_VEH::JACK_DI_VEH(pfSwitch *whichSwitch, pfDCS *jadehull, int id, JNT_INFO *Joint)
: PERSON_VEH(id)
{
   status = Appearance_LifeForm_UprightStill | Appearance_LifeForm_Weapon1_Stowed;
   Medic_update_flag = 0;
   unitMember = FALSE;
   behavior = COMBAT;
   upperJointMods = new JointMods();
   signals = new signalClass();

   hull = jadehull;
   create_entity (whichSwitch, 85, 0 , Joint);

#if defined(JACK) && defined(SS_INTERFACE)
   if ( (id == G_drivenveh) && L_use_ss_interface ) {
      L_idu_net = new IDU_net_manager ( L_ss_group, L_ss_port, L_ss_ttl,
                                        L_ss_interface, L_ss_loopback );
      if ( !L_idu_net->net_open() ) {
         cerr << "JACK_DI_VEH constructor:\tCannot open IDU_net_manager!"
              << endl;
         L_use_ss_interface = FALSE;
         delete L_idu_net;
         L_idu_net = NULL;
      }
   }
#endif

} 

#endif // NODUDE

JACK_DI_VEH::~JACK_DI_VEH()
{
#if defined(JACK) && defined(SS_INTERFACE)
   if ( (vid == G_drivenveh) && L_use_ss_interface && (L_idu_net != NULL) ) {
      L_use_ss_interface = FALSE;
      delete L_idu_net;
      L_idu_net = NULL;
   }
#endif
   if(artic == JACK_ARTIC) {
      G_JackSoldier_active[js->soldiernum] = FALSE;  // Deactivate but don't delete
      G_num_soldiers --;
   }

   delete upperJointMods;
   delete signals;

}


void JACK_DI_VEH::changestatus(unsigned int stat)
{
  status = stat;
  if (status & Appearance_DamageDestroyed) {
     appear = JACKTTES_DEAD;
     pfSetVec3(velocity,0.0f,0.0f,0.0f);
     speed = 0.0f;
     upperJointMods->set_index(NOSIGNAL);
     upperJointMods->set_status(DONE);
     jack_rifle_on ();
     okToSendHiRes = FALSE;
     okToUpdateHiRes = FALSE;
     set_net_type (disNet);
  }

} // end JACK_DI_VEH::changestatus()


void JACK_DI_VEH::changeWeaponStatus(unsigned int weapstatus)
{
  unsigned int new_status, curr_status = status;

  switch(curr_status & Appearance_LifeForm_State_Mask)
    {
     case Appearance_LifeForm_Kneeling:
        new_status = Appearance_LifeForm_Kneeling;
        break;
     case Appearance_LifeForm_Prone:
        new_status=Appearance_LifeForm_Prone;
        break;
#ifdef DIS_2v4
     case Appearance_LifeForm_Crawling:
        new_status=Appearance_LifeForm_Crawling;
        break;
#endif
     case Appearance_LifeForm_UprightStill:
        new_status=Appearance_LifeForm_UprightStill;
        break;
#ifdef DIS_2v4
     case Appearance_LifeForm_UprightWalking:
        new_status=Appearance_LifeForm_UprightWalking;
        break;
     case Appearance_LifeForm_UprightRunning:
        new_status=Appearance_LifeForm_UprightRunning;
        break;
#endif
     default:
        cerr << "Invalid position mode\n";
     }

     status = new_status | weapstatus;

} // end JACK_DI_VEH::changeWeaponStatus()


void JACK_DI_VEH::SendPacket()
{
    pfVec3 vel;
//    pfVec2 v1,v2;
//    float spd,s,c;

    if((js) && (G_JackSoldier_active[js->soldiernum]==TRUE)) {

    // Reorient velocity vector so that soldier is always
    // aligned with it (hull DCS will match direction of vel
    // so soldier is always at 0 heading)

//    spd = pfLengthVec3(velocity);
//    pfSinCos(posture.hpr[HEADING]+90.0f,&s,&c);
//    pfSetVec2(v1,s,c);
//    pfSetVec2(v2,velocity[X],velocity[Y]);
//    pfNormalizeVec2(v2);

//    // Heading is 0 when facing North, vel heading = 90
//    if((spd != 0.0f) &&  (abs_speed < 0.0f))  {
//       spd = abs_speed; // Going backwards
//       //cerr << "in SendPacket() and going backwards\n";
//    }
//
//    pfSetVec3(vel,0.0f,spd,0.0f);

    pfSetVec3(vel,0.0f,speed,0.0f);
    
    js->request(appear, weapon, vel, 0.0f,
             posture.xyz, immed_transition);

    }
//  else
//      cerr << "js is null\n";

} // end JACK_DI_VEH:: SendPacket()


#ifndef NODUDE
void JACK_DI_VEH::create_entity(pfSwitch * whichSwitch, int atype, ForceID, JNT_INFO *Joint) {
#ifdef TRACE
   cerr << "adding vid "<<vid <<" JACK_DI::create_entity()\n";
#endif

   char name[256], sname[256];
   int found = FALSE;

   if(G_num_soldiers < JACK_MAXSOLDIER-50-1) {
       if(G_tot_soldiers  < JACK_MAXSOLDIER-50-1)  // new JackSoldier to be created
       {
          pfGroup* l_model = (pfGroup*) pfGetChild(pfGetChild(whichSwitch, 0),0);
          js = new JackSoldier(l_model);
          G_JackSoldier_active[js->soldiernum] = TRUE;
          G_num_soldiers++;
          G_tot_soldiers++;
          if( js) {
             //cerr << "     num_soldiers= " << G_num_soldiers
                    //<< " tot_soldiers = " << G_tot_soldiers << endl;
             found = TRUE;
          }
          else {
             cerr << "unable to create new JackSoldier aborting create_entity\n";
          }
       }
       else { // max # of JackSoldiers created, try reusing JackSoldiers
         found = FALSE;
         for(int sj=1; sj < JACK_MAXSOLDIER-50;  sj++) {
            if(G_JackSoldier_active[sj] == FALSE) {
                found = TRUE;
                //cout << "reusing soldier # "<< sj << endl;
                G_JackSoldier_active[sj] = TRUE;
                js = js->jack->table[sj];
                sprintf(name,"s%d",js->soldiernum);
                sprintf(sname,"s%d",js->soldiernum);
                js->soldier = (pfGroup*)pfGetChild(models,0);
                pfNodeName(js->soldier,name);
                strcpy(name,pfGetNodeName(js->soldier));

                for (int i = 0;i < js->jack->njoints;i++) {
                   sprintf(name,"s%d/%s",js->soldiernum,
                      js->jack->jointsinfo[i].name);
                   if ((js->jointptrs[i] = (pfDCS*)pfLookupNode(name, pfGetDCSClassType())) == NULL) {
                      cerr << "Cannot locate pfDCS named  "
                         << js->jack->jointsinfo[i].name << " in cloned graph.\n";
                   }
                }

                for (i = 0;i < js->jack->nfigs;i++) {
                   sprintf(name,"s%d/%s",js->soldiernum,
                      js->jack->figsinfo[i].name);
                   if ((js->figptrs[i] = (pfDCS*)pfLookupNode(name, pfGetDCSClassType())) == NULL)
                      cerr << "Cannot locate pfDCS named  "
                         << js->jack->figsinfo[i].name << " in cloned graph.\n";
                }
               G_num_soldiers++;
               G_tot_soldiers++;
               //cerr << "     Reused JackSoldier  "<< sj << "  num_soldiers "
                 //<< G_num_soldiers << "  tot_soldiers =  " << G_tot_soldiers << endl;
               break;
            }
         } // end for
       }
       if (!found) {
          artic = NO_ARTIC;
          cerr << "WARNING:\tNo other JACK soldiers available." << endl;
       }
       else {
          artic = JACK_ARTIC;
          Control = JACKML;
          delete JointVals;     // free up pre-allocated space
          bzero ( js->JointVals, MAXJOINTS);
          JointVals = js->JointVals;
          // Initialize position (causes rotation so DI faces forward)
          appear = JACKTTES_UPRIGHT;
          if((atype == JACKNORIFLE) && (atype == JACKNORIFLE_TEAM)) {
             weapon_present = FALSE;
          }
          else {
             weapon_present = TRUE;
          }
          weapon = JACKTTES_WEAPON_STOWED;
          immed_transition = FALSE;        // smooth position transitions
          quiet = FALSE;                   // want joint updates
          if(behavior == COMBAT) {
             weapon_dcs = js->figptrs[0] ; // if combat, use rifle position
          }

          // set non zero initial velocity to initialize position
          pfSetVec3(velocity,0.0f,1.0f,0.0f);

          if(vid == G_drivenveh) {
 
             // Use raw joint angle updates always to do overrides
             js->sb->mode = JACKML_SOLDIERMODE_JOINTANGLES;

             SendPacket();
             CheckJackUpdate(js,upperJointMods,0.0f,0.0f,FALSE, FALSE);

             // Turn off head/neck so we can see our own body
             int i;
             pfNode *current_head_parent, *current_neck_parent;
             pfNode *current_head, *current_neck;

             num_heads = 1;
             for ( i = 0; i < num_heads; i++ ) {
                  current_head = (pfNode *)js->jointptrs[12];
		  current_neck = (pfNode *)js->jointptrs[11];
                if ( pfGetNumParents(current_head) == long(0) ) {
                   cerr << "Head does not have a parent.!\n";
                }
                else if(pfGetNumParents(current_neck) == long(0) ) {
                   cerr << "Neck does not have a parent.!\n";
                }
                else {
                   current_head_parent = (pfNode *)pfGetParent(current_head,0);
                   current_neck_parent = (pfNode *)pfGetParent(current_neck,0);

                   if (( current_head_parent == NULL ) ||
                        (current_neck_parent == NULL )) {
                      cerr << "Did not grab parent of head or neck!!.\n";
                   }
                   else {
                      head_switch[i] = pfNewSwitch();
                      pfAddChild ( (pfGroup *)head_switch[i], current_neck);
                      //set_intersect_mask((pfGroup *)current_neck_parent,
                      //          PRUNE_MASK,COLLIDE_DYNAMIC);
                      if (!(pfReplaceChild ((pfGroup *)current_neck_parent,
                          current_neck, (pfNode *)head_switch[i] ))) {
                         cerr << "Unable to insert switch for head.\n";
                      }
                      else {
                         pfSwitchVal(head_switch[i], PFSWITCH_OFF);
                      }
                   }
                } // end else
              } // end for


           } // end if (vid == G_drivenveh)

           else {  // not driven veh
              //  use DCS updates
              //js->sb->mode = JACKML_SOLDIERMODE_JOINTDCS;
  
              // use angle updates -- DCS updates still seems buggy
              // and hand signals & medic mode don't work right
              js->sb->mode = JACKML_SOLDIERMODE_JOINTANGLES;

              SendPacket();
              CheckJackUpdate(js,upperJointMods,0.0f,0.0f,FALSE, TRUE);

              pfSetVec3(velocity,0.0f,0.0f,0.0f);
           }
      } // end else found a jack soldier to use

    } // end if G_tot_solduers > JACK_MAXSOLDIER-50-1
    else {
       artic = NO_ARTIC;
       cerr << "WARNING: vid " << vid << " Full, No other JACK soldiers available." << endl;
    }

    //cerr << "entity vid = " << vid << " created with artic = " << artic << endl;
   
}
#endif // NODUDE
void JACK_DI_VEH::create_entity(int atype, ForceID )
//fill in the fields that the constuctor does not
{

#ifdef TRACE
   cerr << "adding vid "<<vid <<" JACK_DI::create_entity()\n";
#endif

//   char name[256], sname[256];
   char name[256];
   const char *sname;
   int found = FALSE;
   if(G_num_soldiers < JACK_MAXSOLDIER-50-1) {
       if(G_tot_soldiers  < JACK_MAXSOLDIER-50-1)  // new JackSoldier to be created
       {
          js = new JackSoldier((pfGroup*)pfGetChild(models,0));
          G_JackSoldier_active[js->soldiernum] = TRUE;
          G_num_soldiers++;
          G_tot_soldiers++;
          if( js) {
             //cerr << "     num_soldiers= " << G_num_soldiers 
                    //<< " tot_soldiers = " << G_tot_soldiers << endl;
             found = TRUE;
          } 
          else {
             cerr << "unable to create new JackSoldier aborting create_entity\n";
          }
       }
       else { // max # of JackSoldiers created, try reusing JackSoldiers
         found = FALSE;
         for(int sj=1; sj < JACK_MAXSOLDIER-50-1;  sj++) {
            if(G_JackSoldier_active[sj] == FALSE) {
                found = TRUE;
                //cout << "reusing soldier # "<< sj << endl;
                G_JackSoldier_active[sj] = TRUE;
                js = js->jack->table[sj];
                js->soldier = (pfGroup*)pfGetChild(models,0);
                sprintf(name,"s%d",js->soldiernum);
                if (0 == (pfNodeName(js->soldier,name))) {
                   cerr << "Warning, not unique" << endl;
                }
                sname = pfGetNodeName(js->soldier);

                for (int i = 0;i < js->jack->njoints;i++) {
                   sprintf(name,"%s/%s",sname,
                      js->jack->jointsinfo[i].name);
                   if ((js->jointptrs[i] = (pfDCS*)pfLookupNode(name, pfGetDCSClassType())) == NULL) {
                      cerr << "JACK_VEH:Cannot locate pfDCS named  "
                         << js->jack->jointsinfo[i].name << " in cloned graph.\n";
                   }
                }

                for (i = 0;i < js->jack->nfigs;i++) {
                   sprintf(name,"%s/%s",sname,
                      js->jack->figsinfo[i].name);
                   if ((js->figptrs[i] = (pfDCS*)pfLookupNode(name, pfGetDCSClassType())) == NULL)
                      cerr << "JACK_VEH:Cannot locate pfDCS named  "
                         << js->jack->figsinfo[i].name << " in cloned graph.\n";
                }
               G_num_soldiers++;
               //cerr << "     Reused JackSoldier  "<< sj << "  num_soldiers "
                 //<< G_num_soldiers << "  tot_soldiers =  " << G_tot_soldiers << endl;
               break;
            }
         } // end for

       }
       if (!found) {
          artic = NO_ARTIC;
          cerr << "WARNING:\tNo other JACK soldiers available." << endl;
       }
       else {
          artic = JACK_ARTIC;
          Control = JACKML;
          delete JointVals;	// free up pre-allocated space
          bzero ( js->JointVals, MAXJOINTS);
          JointVals = js->JointVals;
          // Initialize position (causes rotation so DI faces forward) 
          appear = JACKTTES_UPRIGHT;
          if((atype == JACKNORIFLE) && (atype == JACKNORIFLE_TEAM)) {
             weapon_present = FALSE;
          }
          else {
             weapon_present = TRUE;
          }
          weapon = JACKTTES_WEAPON_STOWED;
          immed_transition = FALSE;        // smooth position transitions
          quiet = FALSE;                   // want joint updates
          if(behavior == COMBAT) {
             weapon_dcs = js->figptrs[0] ; // if combat, use rifle position
          }
        
          // set non zero initial velocity to initialize position
//          pfSetVec3(velocity,0.0f,1.0f,0.0f);

          // This is for taking care of a JackML problem that
          // does not allow joint overides unitl jack has moved.
          speed = 10.0f;

          if(vid == G_drivenveh) {
  
             // Use raw joint angle updates always to do overrides
             js->sb->mode = JACKML_SOLDIERMODE_JOINTANGLES;
             SendPacket();
             CheckJackUpdate(js,upperJointMods,0.0f,0.0f,FALSE, FALSE);
 
             pfNode *current_head_parent, *current_neck_parent;
             pfNode *current_head, *current_neck;

             num_heads = 1;
                current_head = (pfNode *)joint[HEAD_ANGLE].dcs;
                current_neck = (pfNode *)joint[NECK_ANGLE].dcs;
                if ( pfGetNumParents(current_head) == long(0) )
                   cerr << "Head does not have a parent.!\n";
                else if(pfGetNumParents(current_neck) == long(0) )
                   cerr << "Neck does not have a parent.!\n";
                else {
                   current_head_parent = (pfNode *)pfGetParent(current_head,0);
                   current_neck_parent = (pfNode *)pfGetParent(current_neck,0);
                   if (( current_head_parent == NULL ) || 
                        (current_neck_parent == NULL ))
                      cerr << "Did not grab parent of head or neck!!.\n";
                   else {
                      head_switch = pfNewSwitch();
                      pfAddChild ( (pfGroup *)head_switch, current_neck);
                      //set_intersect_mask((pfGroup *)current_neck_parent,
                      //          PRUNE_MASK,COLLIDE_DYNAMIC);  
                      if (!(pfReplaceChild ((pfGroup *)current_neck_parent, 
                          current_neck, (pfNode *)head_switch )))
                         cerr << "Unable to insert switch for head.\n";
                        
                      else
                         pfSwitchVal(head_switch, PFSWITCH_OFF);

                   }
                } // end else
           } // end if (vid == G_drivenveh)

           else {  // not driven veh
              //  use DCS updates   
              //js->sb->mode = JACKML_SOLDIERMODE_JOINTDCS;
   
              // use angle updates -- DCS updates still seems buggy
              // and hand signals & medic mode don't work right
              js->sb->mode = JACKML_SOLDIERMODE_JOINTANGLES;

              SendPacket();
              CheckJackUpdate(js,upperJointMods,0.0f,0.0f,FALSE, TRUE);

              pfSetVec3(velocity,0.0f,0.0f,0.0f);
           }
      } // end else found a jack soldier to use


    } // end if G_tot_solduers > JACK_MAXSOLDIER-50-1 
    else {
       artic = NO_ARTIC;
       cerr << "WARNING: vid " << vid << " Full, No other JACK soldiers available." << endl;
    }
 
    //cerr << "entity vid = " << vid << " created with artic = " << artic << endl;

   // This is a continuation of the JackML bug fix
   pfSetVec3(velocity,0.0f,0.0f,0.0f);
   speed = 0.0f;

} // end JACK_DI_VEH::create_entity()


int JACK_DI_VEH::MedicMode()
{
    // Hack - just check the mask bit
    if (status & Appearance_LifeForm_Medic_Mask) {
        return TRUE;
    }
    else {
        return FALSE;
    }
} // end JACK_DI_VEH::MedicMode()


void JACK_DI_VEH::determineWeaponStatus(float speed) 
{
   if(behavior == COMBAT) {

      switch (status & Appearance_LifeForm_Weapon1_Mask) {

         case Appearance_LifeForm_Weapon1_Deployed:
         // JackML won't deploy after nonzero vel posture
         // so use set hand angles
            if((speed > 0.0f) && (appear == JACKTTES_UPRIGHT) && 
               (net_type != hiResNet)) {
            //don't use JackML - override arm joint angles
               if((fabs(speed) >= 3.0f) &&
                  ((upperJointMods->get_index() == NOSIGNAL) ||
                  (!upperJointMods->isTemporal()))) {
                  upperJointMods->set_index(RUN_WEAPON_DEPLOY);
               }
               // at 3.0mps, use current signal_index because
               // speeding up, at 3mps he appears walking
               // slowing down,at 3mps he appears running
               else if((fabs(speed) < 3.0f) && 
                  ((upperJointMods->get_index() == NOSIGNAL) ||
                  (!upperJointMods->isTemporal())))  {
                  upperJointMods->set_index(WALK_WEAPON_DEPLOY);
               }
               if (net_type != hiResNet) {
                  weapon = JACKTTES_WEAPON_STOWED; // need to set neckFlag
               }
            }
            else {
               if(!upperJointMods->isTemporal()) { 
                  upperJointMods->set_index(NOSIGNAL);
                  upperJointMods->set_status(DONE);
               }
               if (net_type != hiResNet) {
                  weapon = JACKTTES_WEAPON_DEPLOYED;
               }
           }
           break;

        case Appearance_LifeForm_Weapon1_Stowed:
        default:
           weapon = JACKTTES_WEAPON_STOWED;
           if( (appear == JACKTTES_UPRIGHT) && 
              (speed > 0.0f) && (fabs(speed) <= 3.0f) &&
              ((upperJointMods->get_index() == NOSIGNAL) || 
              (!upperJointMods->isTemporal())) &&
              (net_type != hiResNet) ) {
              // don't use JackML - override arm joint angles
              upperJointMods->set_index(WALK_WEAPON_STOW);
           }
           else {
              if(!upperJointMods->isTemporal()) {                    
                  upperJointMods->set_index(NOSIGNAL);
                  upperJointMods->set_status(DONE);
              }
           }
           break;
      } // end switch

   } // end if

} // end determineWeaponStatus()


void JACK_DI_VEH::determineStatus(float& speed) 
{
   switch(status & Appearance_LifeForm_State_Mask) {

      case Appearance_LifeForm_Kneeling:
         appear = JACKTTES_KNEEL;
         break;

      case Appearance_LifeForm_Prone:  // Jack does crawling
#ifdef DIS_2v4
      case Appearance_LifeForm_Crawling:
      case Appearance_LifeForm_Swimming:
#endif
         if(speed == 0.0f)  {
            if((status & Appearance_LifeForm_Weapon1_Mask) ==
               Appearance_LifeForm_Weapon1_Deployed) {
               status = Appearance_LifeForm_Prone |
                  Appearance_LifeForm_Weapon1_Deployed;
             }
             else {
                      status = Appearance_LifeForm_Prone |
                        Appearance_LifeForm_Weapon1_Stowed;
             }
         }

         else if((speed != 0.0f) && (speed < 3.0f)) {
            if((status & Appearance_LifeForm_Weapon1_Mask) ==
               Appearance_LifeForm_Weapon1_Deployed) {
               status = Appearance_LifeForm_Crawling |
                  Appearance_LifeForm_Weapon1_Deployed;
            }
            else {
               status = Appearance_LifeForm_Crawling |
                  Appearance_LifeForm_Weapon1_Stowed;
            }
         }

         appear = JACKTTES_PRONE; // if prone  & vel > 0
         break;

      case Appearance_LifeForm_UprightStill:
#ifdef DIS_2v4
      case Appearance_LifeForm_UprightWalking:
      case Appearance_LifeForm_UprightRunning:
      case Appearance_LifeForm_Jumping:
      case Appearance_LifeForm_Parachuting:
#endif
         // If getting up from kneel/crawling, make him stop 1st
         // to avoid moving fwd while getting up and strange pivot
         // later when he stops
         if(((appear == JACKTTES_KNEEL) || (appear == JACKTTES_PRONE))
            && (speed > 0.0f)) {
            speed = 0.0f;
            velocity[PF_X] = 0.0f;
            velocity[PF_Y] = 0.0f;
            velocity[PF_Z] = 0.0f;
         }
 
         if(speed == 0.0f)  {
             if((status & Appearance_LifeForm_Weapon1_Mask) ==
                Appearance_LifeForm_Weapon1_Deployed) {
                status = Appearance_LifeForm_UprightStill |
                   Appearance_LifeForm_Weapon1_Deployed;
             }
             else {
                status = Appearance_LifeForm_UprightStill |
                   Appearance_LifeForm_Weapon1_Stowed;
             }
         }

         else if((speed != 0.0f) && (speed < 3.0f)) {
            if((status & Appearance_LifeForm_Weapon1_Mask) ==
               Appearance_LifeForm_Weapon1_Deployed) {
               status = Appearance_LifeForm_UprightWalking |
                  Appearance_LifeForm_Weapon1_Deployed;
            }
            else {
               status = Appearance_LifeForm_UprightWalking |
                  Appearance_LifeForm_Weapon1_Stowed;
            }
         }
         else if(speed > 3.0f) {
            if((status & Appearance_LifeForm_Weapon1_Mask) ==
               Appearance_LifeForm_Weapon1_Deployed) {
                  status = Appearance_LifeForm_UprightRunning |
                     Appearance_LifeForm_Weapon1_Deployed;
            }
            else {
               status = Appearance_LifeForm_UprightRunning |
                  Appearance_LifeForm_Weapon1_Stowed;
            }
         }

         appear = JACKTTES_UPRIGHT;
         break;

      default:
         if((status & Appearance_DamageDestroyed) !=
            Appearance_DamageDestroyed) {
            status = Appearance_LifeForm_UprightStill |
               Appearance_LifeForm_Weapon1_Stowed;
            appear = JACKTTES_UPRIGHT;
         }
         //else {
         //  cerr << "DI must be dead!\n";
         //}
         break;
   }

   if((status & Appearance_DamageDestroyed)==Appearance_DamageDestroyed) {
      appear = JACKTTES_DEAD;
      pfSetVec3(velocity,0.0f,0.0f,0.0f);
      speed = 0.0f;
      upperJointMods->set_index(NOSIGNAL);
      upperJointMods->set_status(DONE);
      //cerr << "Driven DI GUY is dead\n";
   }
   else if(!okToUpdateHiRes) {
      determineWeaponStatus(speed);
   } // end else

} // end JACK_DI_VEH::determineStatus()


void JACK_DI_VEH::computeFirepoint()
{
   pfMatrix orient, mat;
   pfVec3 up = { 0.0f, 0.0f, 1.0f };

   // Use JACK_DI figure DCS to determine body orientation for jack soldiers
   pfGetDCSMat(js->figptrs[1],orient);
   pfFullXformPt3 ( up, up, orient ); //
   pfAddVec3 ( eyepoint, posture.xyz, up ); // offset s/b about 1.0fm

   // For Jack soldiers, firepoint is simply tip of rifle, wherever it is
   pfVec4 length = {WEAPON_DCS2TIP_LENGTH,0.0f,0.05f,1.0f};
   pfGetDCSMat(js->figptrs[0], mat);
   pfXformVec4(length,length, mat);

   firepoint[X] = length[X];
   firepoint[Y] = length[Y];
   firepoint[Z] = length[Z];

} // end JACK_DI_VEH::computeFirepoint()


void JACK_DI_VEH::updateJack()
{
   int neckFlag;
   float eye_offset = 0.15f;
#ifndef NOUPPERBODY
   input_manager *l_input_manager = G_manager->ofinput;
#endif // NOUPPERBODY

   SendPacket();

   // fix neck snapping problem when upright unless weapon deployed
   neckFlag = ((appear==JACKTTES_UPRIGHT) 
         && (weapon != JACKTTES_WEAPON_DEPLOYED)) ? TRUE : FALSE;
   if(G_JackSoldier_active[js->soldiernum] == TRUE) {
      if (okToUpdateHiRes) {
         apply_hi_res ();
         if((status & Appearance_DamageDestroyed) !=
            Appearance_DamageDestroyed) {
            oldstatus = status;
            status = Appearance_LifeForm_UprightStill |
               Appearance_LifeForm_Weapon1_Stowed;
            appear = JACKTTES_UPRIGHT;
         }
         okToUpdateHiRes = FALSE;
      }
      CheckJackUpdate(js,upperJointMods, look.hpr[HEADING],
         look.hpr[PITCH],neckFlag, FALSE);
      upperJointMods->set_override_both_arms(FALSE);
#ifndef NOUPPERBODY
      if ( (input_control == NPS_UPPERBODY) || (input_control == NPS_UNIPORT) ||
           (input_control == NPS_ODT) ) {
         if (l_input_manager->verify_device (NPS_UPPERBODY) ) {
            pfMatrix H_rwrist, H_lwrist;
            getJackWristTx(js, H_rwrist, H_lwrist);
            l_input_manager->set_multival ( (void *) H_rwrist,
                                           NPS_UPPERBODY,
                                           NPS_MV_RIGHTHAND);
            l_input_manager->set_multival ( (void *) H_lwrist,
                                           NPS_UPPERBODY,
                                           NPS_MV_LEFTHAND);
            l_input_manager->query_multival ( (void *)hiResJointAngles ,
                                             NPS_UPPERBODY,
                                             NPS_MV_RIFLE);
            pfCoord rifle_coord;
            for (int ix = 0; ix <= 2; ix++) {
               rifle_coord.xyz[ix] = hiResJointAngles[RCX + ix].datum;
               rifle_coord.hpr[ix] = hiResJointAngles[RCH + ix].datum;
            }
            
            if (sensor_rifle_dcs) {
               pfDCSCoord(sensor_rifle_dcs, &rifle_coord);
            }
            l_input_manager->query_multival ( (void *)&gun_pos,
                                             NPS_UPPERBODY,
                                             NPS_MV_TARGET_RIFLE);
/*
            gun_pos.xyz[PF_Z] = rifle_coord.xyz[PF_X];
            gun_pos.xyz[PF_X] = rifle_coord.xyz[PF_Y];
            gun_pos.xyz[PF_Y] = rifle_coord.xyz[PF_Z];
            gun_pos.hpr[PF_P] = rifle_coord.hpr[PF_H];
            gun_pos.hpr[PF_R] = rifle_coord.hpr[PF_P];
            gun_pos.hpr[PF_H] = rifle_coord.hpr[PF_R];

            gun_pos.xyz[PF_Z] += 1.0;
*/
         }
      }
#endif // NOUPPERBODY

   }

   // Set look to last computed values for elevation of DI center
   // This is then offset in view.cc by a fixed height above ground
   // which varies according postures.

   if (G_dynamic_data->window_size == WIN_TWO_TPORT) {
      look.xyz[X] = 0.0f;
      look.xyz[Y] = 0.0f;
      look.xyz[Z] = 1.5;
      if (!birds_eye) {
         turnoff ();
      }
   }
   else {
      look.xyz[X] = js->gaze.xyz[X];
      look.xyz[Y] = js->gaze.xyz[Y];
      look.xyz[Z] = js->gaze.xyz[Z];
   }

   if(look.hpr[PITCH] != 0.0f) {
      // Assume neck rotates instead of head when pitch != 0
      float s,c, radius;
      radius = fabs(sinf(DTOR*look.hpr[PITCH])*eye_offset);
      pfSinCos(look.hpr[HEADING], &s, &c);
      look.xyz[X] -=  s*radius;
      look.xyz[Y] +=  c*radius;
      look.xyz[Z] -= sinf(DTOR*look.hpr[PITCH])*eye_offset;
   }

} // end updateJack()


void JACK_DI_VEH::moveObject(behaveEnum behave, pfCoord& loc, 
   float )
{
   pfMatrix mat;
   pfGetDCSMat(js->figptrs[1],mat);
   float s,c;

   switch(behave) {

      case(GUIDEON):
         pfSinCos(loc.hpr[HEADING]+11.0f,&s,&c); 
         if(upperJointMods->get_index() == FLAG_HOLD) {
            pfDCSTrans(weapon_dcs,loc.xyz[X]+ 0.25f*c,
               loc.xyz[Y]+ 0.25f*s, 
               loc.xyz[Z]+mat[3][2]-0.05f);
            pfDCSRot(weapon_dcs,loc.hpr[HEADING],0.0f,0.0f);
         }
         else if(upperJointMods->get_index() == FLAG_SALUTE) {
            pfDCSTrans(weapon_dcs,loc.xyz[X] + 0.25f*c,
               loc.xyz[Y]+0.25f*s, 
               loc.xyz[Z]+mat[3][2]+0.4f);
            pfDCSRot(weapon_dcs,loc.hpr[HEADING],100.0f,180.0f);
         }

         break;

      default:
         break;

   } // end switch

} // end JACK_DI_VEH::moveObject()


// move the vehicle for the next frame
movestat JACK_DI_VEH::move()
{

   pfVec3 oldpos;
   static int old_tethered;
   int hitSomething = FALSE;

   oldspeed = speed; // This is the only place this should be set!!!!

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tJACK_DI_VEH::move" << endl;
#endif


   update_time ();
   collect_inputs ();

#ifndef NOHIRESNET
   if ((vid == G_drivenveh) && (G_hr_net != NULL)) {
      if (okToSendHiRes) {
         DataPDU hrDPDU;
         hrDPDU.orig_entity_id = G_vehlist[vid].DISid;
#ifndef DIS_2v4
         hrDPDU.orig_force_id = (ForceID)G_dynamic_data->force;
#endif
         bzero ( (void *) &hrDPDU.recv_entity_id, sizeof (EntityID) );
#ifndef DIS_2v4
         bzero ( (void *) &hrDPDU.recv_force_id, sizeof (ForceID) );
#endif
#ifndef NONPSSHIP
         if (mounted_vid != vid) {
            hrDPDU.recv_entity_id = G_vehlist[mounted_vid].DISid;
            hrDPDU.request_id = HIRES_DATA_PDU_MOUNT_TAG; // id of DATA
         }
         else
#endif // NONPSSHIP
            hrDPDU.request_id = HIRES_DATA_PDU_TAG; // Need to set for id purposes
         hrDPDU.num_datum_fixed = numHiResJointAngles;
         bcopy ( (void *) hiResJointAngles, (void *) (hrDPDU.fixed_datum),
                sizeof (fixedDatumRecord) * numHiResJointAngles);
         switch ( G_dynamic_data->hr_network_mode ) {
            case NET_OFF:
            case NET_RECEIVE_ONLY:
               break;
            case NET_SEND_ONLY:
            case NET_SEND_AND_RECEIVE:
            default:
               G_hr_net->write_pdu ( (char *)&hrDPDU, DataPDU_Type);
               break;
         }
         okToSendHiRes = FALSE;
      }
   }
#endif // NOHIRESNET

#if defined(JACK) && defined(SS_INTERFACE)
   if ((vid == G_drivenveh) && L_use_ss_interface && (L_idu_net != NULL)) {
      char *idu;
      IDUType idutype;
      sender_info sender;
      int swapped_buffers;
      SSToJackIDU *interface_data;

      u_long  weapon_type, weapon_fire;

      while ( L_idu_net->read_idu(&idu, &idutype, sender, swapped_buffers) ) {
         switch (idutype) {
            case (SS_To_Jack_Type):
               interface_data = (SSToJackIDU *)idu;
               // Copy info into class variables
               speed  = interface_data->speed; // float
               posture.hpr[HEADING]  = interface_data->body_heading; // float
               look.hpr[HEADING]  = interface_data->view_heading; // float
               look.hpr[PITCH]  = interface_data->view_pitch; // float
               status  = interface_data->posture; // 32 bit enum
               weapon_type  = interface_data->weapon; // 32 bit enum
               weapon_fire = interface_data->fire; // 32 bit enum
               if(weapon_fire) {
                  if(weapon_fire & 0x01) {
                     firebuttonhit(PRIMARY, G_drivenveh,
                        posture, look);
                  }
                  if(weapon_fire & 0x02) { 
                     firebuttonhit(SECONDARY, G_drivenveh,
                        posture, look);
                  }
                  if(weapon_fire & 0x04) {
                     firebuttonhit(TERTIARY, G_drivenveh,
                        posture, look);
                  }
               }

               break;
            default:
               break;
         }
      }

   }
#endif


   // check limits of head motion
   for (int ix = 0; ix <= 2; ix++) {
      while (look.hpr[ix] >= 360.0f) {
          look.hpr[ix] -= 360.0f;
      }
      while (look.hpr[ix] < 0) {
          look.hpr[ix] += 360.0f;
      }
   }
/*
   float min_head = -90.0f;
   float max_head = 90.0f;
   if(look.hpr[HEADING] > max_head) {
      look.hpr[HEADING] = max_head;
   }
   else if(look.hpr[HEADING] < min_head) {
      look.hpr[HEADING] = min_head;
   }
   else if(look.hpr[PITCH] > max_head) {
      look.hpr[PITCH] = max_head;
   }
   else if(look.hpr[PITCH] < min_head) {
      look.hpr[PITCH] = min_head; 
   }
*/


   // Check to hand signal queue
   if(checkSignal()) {
      executeSignal(FALSE);
   }
   movement_index = upperJointMods->get_index();

   // MH - saves medic mode data
   unsigned int medicstatus = 0;

   // Hack -- Allow smooth "checking" of 'status'
   int temp_medic_mode;

#ifdef TRACE
   cerr << "Moving a JACK_DI_VEH\n";
#endif

   initMove(old_tethered,tethered);
   pfCopyVec3(oldpos,posture.xyz);

   // Medic Mode Stuff
   if(artic == JACK_ARTIC) {
      temp_medic_mode = MedicMode();

      // MH - the old song and dance...
      if(temp_medic_mode) {
         // Handle Medic Mode
         do_medic_mode(Medic_update_flag);
      }

      // MH - save the medical mode status - so it can be re-introduced 
      // after the status is reset below
      medicstatus = (status & Appearance_LifeForm_Medic_Mask);

      // MH - need to mask out these bits for now... screws up the switch
      // below (interference between medic and appearance bits)
      status &= ~Appearance_LifeForm_Medic_Mask;
   }

   if (!(paused || tethered || cdata.rigid_body) ) {

      if(!temp_medic_mode) {
         determineSpeed(cdata,input_control);
      }

      if(artic == JACK_ARTIC) {
         // MH - the old song and dance...
         status |= medicstatus;
         temp_medic_mode = MedicMode();
         status &= ~Appearance_LifeForm_Medic_Mask;
      }

      //move the GROUND vehicle type in the x-y plane
      // don't do this in medic mode

      if (!temp_medic_mode) {
         posture.xyz[X] += velocity[X]*cdata.delta_time;
         posture.xyz[Y] += velocity[Y]*cdata.delta_time;
      }

      determinePosture(cdata,oldpos,hitSomething);
      if(hitSomething) {
         return(HITSOMETHING);
      }


   } // End !paused !tethered !rigid_body

   // If tethered, let the other vehicle calculate our position for us
   if (tethered) {
      handleTether(cdata,tethered);
   }

   computeFirepoint();

   checkUnderground(tethered);

   // MH - first check to see if we are in a special medic animation
   if ((artic == JACK_ARTIC) && 
      ((status | medicstatus) == Appearance_LifeForm_Medic_5)) {
      // cerr << "trigger caught\n";
      appear = JACKTTES_MEDIC_PULSE;
   }
   else {
      determineStatus(speed);

      if((icontype == JACKNORIFLE) && (appear == JACKTTES_UPRIGHT)) {
         if(behavior != GUIDEON) {
            if(speed <= 3.0f) {
               if(!upperJointMods->get_index()) {
                  upperJointMods->set_index(WALK_NO_WEAPON);
               }
            }
            else if(speed > 3.0f) {
               if(!upperJointMods->get_index()) {
                  upperJointMods->set_index(RUN_NO_WEAPON);
               }
            }
         }
         else {
            if(!upperJointMods->get_index()) {
               upperJointMods->set_index(FLAG_HOLD);
            }
         }
      }
      else if((appear == JACKTTES_PRONE) 
         && (weapon == JACKTTES_WEAPON_DEPLOYED)) {
         upperJointMods->set_index(PRONE_FIRING);
      }
      else if(((appear != JACKTTES_PRONE) || 
         (weapon != JACKTTES_WEAPON_DEPLOYED)) &&
         (upperJointMods->get_index() == PRONE_FIRING)) {
          upperJointMods->set_index(NOSIGNAL);
      }
   }

   // put the medic mode status bit back in
   status |= medicstatus;

   // Check to hand signal queue
   if(checkSignal()) {
      executeSignal(FALSE);
   }

   //put the vehicle in the correct location in space
   placeVehicle();

   // update articulations using JackML
   if(artic == JACK_ARTIC) {
      updateJack();
   }

   check_targets ( posture.xyz, posture.hpr,
                   look.hpr );

   oldstatus = status;

#if defined(JACK) && defined(SS_INTERFACE)
   if ( (vid == G_drivenveh) && L_use_ss_interface && (L_idu_net != NULL) ) {
      JackToSSIDU outbound_data;

      outbound_data.position_X = posture.xyz[X];
      outbound_data.position_Y = posture.xyz[Y];
      outbound_data.position_Z = posture.xyz[Z];
      outbound_data.status = status;

      L_idu_net->write_idu ( (char *)&outbound_data, Jack_To_SS_Type );

   }
#endif
/*
cerr << posture.xyz[0] << "  "
     << posture.xyz[1] << "  "
     << posture.xyz[2] << endl;
cerr << posture.hpr[0] << "  "
     << posture.hpr[1] << "  "
     << posture.hpr[2] << endl;
cerr << look.xyz[0] << "  "
     << look.xyz[1] << "  "
     << look.xyz[2] << endl;
*/

   thrust = cdata.thrust;

   return(FINEWITHME);

} // end of JACK_DI_VEH::move()


void JACK_DI_VEH::do_medic_mode(int& Medic_update_flag)
{

   static int step = 0;        // The current step (see below)
   static double begun;        // The time at which this step was begun
   static double px, py;       // The "current" self-location stats
   static double hd, sp;       // The "current" heading and speed
   static double start;        // The time at initialization
   static JACK_DI_VEH *victim; // The victim we are chasing
   static double vhd;          // The victim's orientation
   static double vpx, vpy;     // The victim's location
   static double fpx, fpy;     // The desired location (near the victim)
   double now = pfGetTime();   // Access the current time (units?)
   static int victim_id;

   // Step 0 -- Initialize
   if (step == 0) {
      // cerr << "MM: step 0 -- Find a victim\n";

      // Save the start time
      start = begun = now;
           
      // Extract the "current" location
      px = posture.xyz[X];
      py = posture.xyz[Y];
           
      // Extract the "current" headings
      hd = nps_to_rad (posture.hpr[HEADING]);
          
      // Extract the "current" speed
      sp = pfLengthVec3 (velocity);

      // the victim finder function from netutil.cc
      extern int find_victim(void);
           
      // check for a victim
      victim_id = find_victim();
           
      // if its -1 (i.e. nobody), reset medic mode and return
      if (victim_id == -1) {

          // cerr << "Medic can not find anybody to help! Returning...\n";
          // Cancel Medic Mode
          status &= ~Appearance_LifeForm_Medic_Mask;

          // "Jump" to the end of this function
          //goto medic_bailout;
          return;
      }
           
      // assign the victim we have chosen
      victim = (JACK_DI_VEH *)(G_vehlist[victim_id].vehptr);

      // Extract the location of the victim
      vpx = victim->posture.xyz[X];
      vpy = victim->posture.xyz[Y];
           
      // Extract the orientation of the victim XXX in radians
      vhd = nps_to_rad (victim->posture.hpr[HEADING]);

      // End up XXX 1.0 meters and XXX Pi/2 radians from the body
      fpx = vpx + 1.0 * cos (vhd + M_PI_2);
      fpy = vpy + 1.0 * sin (vhd + M_PI_2);
           
      // We must start with step 10
      step = 10;
   }       

   static double ihd10, fhd10, dur10;

   // Step 10 -- Face the victim
   if (step == 10) {
      // cerr << "MM: step 10 -- Face the victim\n";
        
      // Start facing the way we are now
           ihd10 = hd;

      // Desire to face the "final" location
      fhd10 = atanxy (fpx - px, fpy - py);

      // Place 'fhd10' on the "proper" side of ihd10
      fhd10 = pangle (ihd10, fhd10);
          
      // Turn 180 degrees per second
      dur10 = fabs(fhd10 - ihd10) / M_PI;

      // Advance to the next step
      step = 11;

      // Hack -- Take account of zero length steps
      if (dur10 <= 0.0) step = 20;
   }

   // Step 11 -- Face the victim
   if (step == 11) {
      // cerr << "MM: step 10 -- Face the victim\n";
           
      // Turn gradually to the destination
      if (now < begun + dur10) {
          hd = ihd10 + (fhd10 - ihd10) * ((now - begun) / dur10);
      }
           
      // Turn fully
      else {
          // Teleport
          hd = fhd10;

          // Advance to the next step
          step = 20; begun = begun + dur10;
      }
   }
       
   static double dur20, ipx20, ipy20;

   // Step 20 -- walk to the desired location
   if (step == 20) {
      // cerr << "MM: step 20 - Approach the victim\n";

      // Find initial position
      ipx20 = px; ipy20 = py;

      // Set the speed to XXX 3.0 meters/second
      //sp = 3.0;
      sp = 8.0;
           
      // See how long it should take
      dur20 = hypot (fpx - ipx20, fpy - ipy20) / sp;
           
      // Advance to the next step
      step = 21;

      // Hack -- Take account of zero length steps
      if (dur20 <= 0.0) step = 30;
   }

   // Step 21 -- walk to the desired location
   if (step == 21) {
      // cerr << "MM: step 21 - Approach the victim\n";

      // Slide gradually to the destination
      if (now < begun + dur20) {
          px = ipx20 + (fpx - ipx20) * ((now - begun) / dur20);
          py = ipy20 + (fpy - ipy20) * ((now - begun) / dur20);
      }
           
      // Teleport part-way there
      else {
          // Teleport
          px = fpx; py = fpy;

          // Cancel velocity
          sp = 0.0;
               
          // Advance to the next step
          step = 30; begun = begun + dur20;
      }
   }
       
   static double ihd30, fhd30, dur30;

   // Step 30 -- Face the victim's waist
   if (step == 30) {
      // cerr << "MM: step 30 -- Face the victim\n";
           
      // Start facing the way we are now
      ihd30 = hd;

      // End up looking at the victim's waist
      fhd30 = atanxy (vpx - px, vpy - py);

      // Place 'fhd10' on the "proper" side of ihd10
      fhd30 = pangle (ihd30, fhd30);

      // Look a little lower than the waist of victim
      fhd30 += 0.35;
          
      // Turn 180 degrees per second
      dur30 = fabs(fhd30 - ihd30) / M_PI;

      // Advance to the next step
      step = 31;

      // Hack -- Take account of zero length steps
      if (dur30 <= 0.0) step = 40;
   }

   // Step 31 -- Face the victim's waist
   if (step == 31) {
      // cerr << "MM: step 31 -- Face the victim\n";
           
      // Turn gradually to the destination
      if (now < begun + dur30) {
          hd = ihd30 + (fhd30 - ihd30) * ((now - begun) / dur30);
      }
           
      // Turn fully
      else {
          // Turn fully
          hd = fhd30;

          // Advance to the next step
          step = 40; begun = begun + dur30;
      }
   }

   static double start40;
   // Step 40 -- trigger the animation
   if (step == 40) {
      //   cerr << "MM: step 40 -- Trigger animation\n";
           
      // set the appearance flag
      status = Appearance_LifeForm_Medic_5;

      // force this update to be sent out
      Medic_update_flag = 1;

      // advance to the next step
      start40 = pfGetTime();
      step = 41; start40 = pfGetTime();

   }

   // Step 41 -- wait for it to finish
   else if (step == 41) {
      // cerr << "MM: step 41 -- Wait for animation\n";
           
      // Advance to the next step if we're done the animation 
      // (give it a second to kick in before checking flag);
      if ((now > (start40 + 1))&&(!js->jpkt->intransition))
         step = 42; // begun = begun + 5.0f;        
   }

   static double start42;
   // Step 42 -- start the return transition
   if (step == 42) {
      // cerr << "MM: step 42 -- Trigger return to standing\n";

      // set the appearance flag
      status = Appearance_LifeForm_UprightStill;
      status |= Appearance_LifeForm_Medic_Mask;

      // force this update to be sent out
      Medic_update_flag = 1;

      // advance to the next step
      step = 43; start42 = pfGetTime();
   }

   // Step 43 -- wait for it to finish
   else if (step == 43) {
       // cerr << "MM: step 43 -- Wait for animation\n";

       // Advance to the next step if we're done the animation 
       // (give it a second to kick in before checking flag);
       if ((now > (start42 + 1))&&(!js->jpkt->intransition))
          step = 50; // begun = begun + 5.0;
   }
   
   // Step 50 -- Complete the Medic Block
   if (step == 50) {
       // cerr << "MM: step 50 -- Cancel Medic Mode\n";
           
       // Cancel Medic Mode
       status &= ~Appearance_LifeForm_Medic_Mask;

       // HERE IS WHERE WE HEAL THE VICTIM
       //send_silver_bullet(victim_id);
           
       // Set step to ZERO when done
       step = 0;
   }
       
   // Save the heading XXX was radians
   posture.hpr[HEADING] = rad_to_nps (hd);
   posture.hpr[PITCH] = 0.0;
   posture.hpr[ROLL] = 0.0;

   // Save the location
   posture.xyz[X] = px;
   posture.xyz[Y] = py;

   // Save the speed
   speed = sp;

   // Save the velocity
   velocity[X] = sp * cos (hd);
   velocity[Y] = sp * sin (hd);

   // No Z velocity
   velocity[Z] = 0.0;

   SendPacket();

   // Actually send the info
//   oldspeed = speed;  // Set member value for use in SendPacket()
   if (okToUpdateHiRes) {
      apply_hi_res ();
      okToUpdateHiRes = FALSE;
   }
   CheckJackUpdate(js, upperJointMods, look.hpr[HEADING],
      look.hpr[PITCH], FALSE, FALSE);
   upperJointMods->set_override_both_arms(FALSE); 
   // Bail out of the medic Mode Code
   //medic_bailout:

} // end of do_medic_mode()


void JACK_DI_VEH::updateDRJack() 
{
   int neckFlag;

#ifndef NOHIRESNET
   if ( ((G_curtime - hiResTimeStamp) > HI_RES_TIMEOUT) && 
        (vid != G_drivenveh)) {
// this is for non hard coded timeout
// G_static_data->DIS_deactivate_delta ) &&
      okToUpdateHiRes = FALSE;
      set_net_type (disNet);
      jack_rifle_on ();
   }
#endif // NOHIRESNET
     

//   cerr << "DR velocity: " << endl
//        << "X: " << velocity[PF_X] << "  "
//        << "Y: " << velocity[PF_Y] << "  "
//        << "Z: " << velocity[PF_Z] << endl
//        << "DR speed: " << speed << endl;

   SendPacket();

   neckFlag = ((appear == JACKTTES_UPRIGHT) 
      && (weapon != JACKTTES_WEAPON_DEPLOYED)) ? TRUE : FALSE; 

   if(G_JackSoldier_active[js->soldiernum] == TRUE) {

      if (okToUpdateHiRes) {
         apply_hi_res ();
      }

      CheckJackUpdate(js,upperJointMods, JointVals[HEAD_H],
         JointVals[HEAD_P], neckFlag, TRUE);
      upperJointMods->set_override_both_arms(FALSE);
   }

} // end JACK_DI_VEH::updateDRJack()


void JACK_DI_VEH::determineDRStatus(float speed) 
{

  switch(status & Appearance_LifeForm_State_Mask) {
     case Appearance_LifeForm_Kneeling:
        appear = JACKTTES_KNEEL;
        break;

     case Appearance_LifeForm_Prone:  // Jack does crawling
#ifdef DIS_2v4
     case Appearance_LifeForm_Crawling:
     case Appearance_LifeForm_Swimming:
#endif
        appear = JACKTTES_PRONE; // if prone  & vel > 0
        break;

     case Appearance_LifeForm_UprightStill:
#ifdef DIS_2v4
     case Appearance_LifeForm_UprightWalking:
     case Appearance_LifeForm_UprightRunning:
     case Appearance_LifeForm_Jumping:
     case Appearance_LifeForm_Parachuting:
#endif
        appear = JACKTTES_UPRIGHT;
        break;

  } // end switch

  if((status & Appearance_DamageDestroyed) == Appearance_DamageDestroyed) {
     appear = JACKTTES_DEAD;
     pfSetVec3(velocity,0.0f,0.0f,0.0f);
     speed = 0.0f;
     upperJointMods->set_index(NOSIGNAL);
     upperJointMods->set_status(DONE);
  }
  else {
     determineWeaponStatus(speed);
  }

} // end checkDRStatus()


movestat JACK_DI_VEH::moveDR(float deltatime,float curtime) 
//move routine for DR vehciles simple move
{
//   float speed;  // Why is this called speed when there is a memeber
                   // variable named the same thing?

#ifdef TRACE
   cerr << "DR a JACK_DI_VEH vid = " << vid << " artic = " << artic <<"\n";
#endif

#ifndef NO_PFSMOKE
   update_pfsmokes();
#endif

   if((artic != NO_ARTIC) && (!js)) { 
      artic = NO_ARTIC;
   }

   movement_index = upperJointMods->get_index();

   if(((curtime - lastPDU) > G_static_data->DIS_deactivate_delta ) && 
      (vid != G_drivenveh) 
      && (!unitMember)) {
//      if(artic == JACK_ARTIC) {
//         G_JackSoldier_active[js->soldiernum] = FALSE;  // Deactivate but don't delete
//         G_num_soldiers --;
//      }
      //cerr << "DEACTIVATING soldier time interval " << curtime - lastPDU << endl;
      return(DEACTIVE);
   }

   computeDR(deltatime);

   if (vid != G_drivenveh) {

      speed = pfLengthVec3(velocity);

//      checkBackwardMotion (speed, speed, velocity, position.hpr[PF_H])

      // MH - check to see if we need to trigger a special medical animation
      if (status & Appearance_LifeForm_Medic_Mask) {
         // if its a recognized animation, do it
         unsigned int check_state = status & Appearance_LifeForm_State_Mask;
         // the only one for now
         if (check_state == Appearance_LifeForm_Medic_5) {
            appear = JACKTTES_MEDIC_PULSE;
         }
         // otherwise get rid of the medic mode bit so the rest will work
         else {
            status &= ~Appearance_LifeForm_Medic_Mask;
         }

         // Currently parts of the medic animation  only work when using joint
         // angle updates (no new animation files last time).  So if use DCS 
         // updates, need to put in a lag to complete animation before returning
         // to DCS updates 
      }

      determineDRStatus(speed);
      if((artic == JACK_ARTIC) && (icontype == JACKNORIFLE) 
         && (appear == JACKTTES_UPRIGHT)) {
         if(behavior != GUIDEON) {

            if(speed <= 3.0f) {
               if(!upperJointMods->get_index()) {
                  upperJointMods->set_index(WALK_NO_WEAPON);
               }
            }
            else if(speed > 3.0f) {
               if(!upperJointMods->get_index()) {
                  upperJointMods->set_index(RUN_NO_WEAPON);
               }
            }
         }
         else {
            if(!upperJointMods->get_index()) {
               upperJointMods->set_index(FLAG_HOLD);
            }
         }

      }
      else {
         if((appear == JACKTTES_PRONE)
           && (weapon == JACKTTES_WEAPON_DEPLOYED)) {
           upperJointMods->set_index(PRONE_FIRING);
         }
         else if(((appear != JACKTTES_PRONE) ||
            (weapon != JACKTTES_WEAPON_DEPLOYED)) &&
            (upperJointMods->get_index() == PRONE_FIRING)) {
            upperJointMods->set_index(NOSIGNAL);
         }
      }

      // Update DR entity 
      posture = drpos;
      
#ifndef NONPSSHIP
      //int falling= TRUE;
      //pfVec3 c_norm = {0.0f, 0.0f, 1.0f};
      //determineZ(falling, &c_norm);
      updateRelPosVec();
      if (mounted_vid < vid) {
         reset_on_mounted_vid();
      }
//cerr<<"\t\t\tposture.xyz[Z]"<<posture.xyz[Z]<<endl;
//cerr<<"\t\t\tsnap_switch"<<snap_switch<<endl;
#endif

      if((!unitMember) && (speed > 0.0f)) {
         checkBackwardMotion(speed,speed,velocity,posture.hpr[HEADING]);
      }

      //put the vehicle in the correct location in space
      placeVehicle();

/*  doesn't work right yet - DCS updating seems buggy
      if(upperJointMods->get_status() == DONE) { 
         if (js->sb->mode != JACKML_SOLDIERMODE_JOINTDCS) {
            js->sb->mode = JACKML_SOLDIERMODE_JOINTDCS;
            cerr << "switching to faster JOINTDCS updates mode 
               << js->sb->mode << endl;
         }
      }
      else  {
         if(js->sb->mode != JACKML_SOLDIERMODE_JOINTANGLES) {
            js->sb->mode = JACKML_SOLDIERMODE_JOINTANGLES;
            cerr << "switching to slower JOINTANGLES updates mode 
	       << js->sb->mode << endl;
         }
      }
*/
      if((artic == JACK_ARTIC) && (!quiet)) {
         updateDRJack();
      }
   }
   else if ((!G_static_data->stealth ) &&
      (!unitMember)) {  // send EPDU if G_drivenveh and not part of a JACK_UNIT
      // This is us, have we exceeded the DR error or time limit?
      if(upperJointMods->get_index() != movement_index) { 
         movement_index = upperJointMods->get_index();
         sendentitystate();
         signals->set_pduFlag(TRUE);
      }
      else if((G_static_data->DIS_heartbeat_delta < (curtime - lastPDU)) || 
              (delta_send(&posture,&drpos)) 
         || (Medic_update_flag)) {
         time2Send(speed);
         Medic_update_flag = 0; // clear the medic update flag
      }
      else {
         reason2Send(speed);
      }
      oldstatus = status;

   } // End not G_drivenveh and not stealth

   //cerr << "done with moveDR for vid = " << vid << "\n";
   return(FINEWITHME);
} // end of JACK_DI_VEH::moveDR()


void JACK_DI_VEH::entitystateupdate(EntityStatePDU *epdu, sender_info &sender)
//get an entity state PDU with articulated parts
{
   PERSON_VEH::entitystateupdate(epdu,sender);

   // update only if a hand signal is not in progress
   if(upperJointMods->get_status() == DONE) {
      upperJointMods->set_index(atoi(epdu->dead_reckon_params.TBD));
   }
} // end of JACK_DI_VEH::entitystateupdate()


void JACK_DI_VEH::setSignal(int index)
{
   // This ensures that signals are not set... 
   if((appear != JACKTTES_PRONE) && (appear != JACKTTES_DEAD)) {
      // set only if no signal currently in progress (exc weapon positions)
      signals->set_signal(index, upperJointMods, TRUE);
   }
} // end of JACK_DI_VEH::setSignal()


int JACK_DI_VEH::checkSignal()
{
   return(signals->check_signal(upperJointMods,TRUE));
} // end of JACK_DI_VEH::checkSignal()

void JACK_DI_VEH::apply_hi_res ()
{
   upperJointMods->set_override_both_arms (TRUE);

   upperJointMods->set_arm_joint_angle (ULEFT_SHOULDER, 2, 
                                        hiResJointAngles[LS2].datum);
   upperJointMods->set_arm_joint_angle (ULEFT_SHOULDER, 1,
                                        hiResJointAngles[LS1].datum);
   upperJointMods->set_arm_joint_angle (ULEFT_SHOULDER, 0, 
                                        hiResJointAngles[LS0].datum);

   upperJointMods->set_arm_joint_angle (ULEFT_ELBOW, 0,
                                        hiResJointAngles[LE0].datum);

   upperJointMods->set_arm_joint_angle (ULEFT_WRIST, 2,
                                        hiResJointAngles[LW2].datum);
   upperJointMods->set_arm_joint_angle (ULEFT_WRIST, 1,
                                        hiResJointAngles[LW1].datum );
   upperJointMods->set_arm_joint_angle (ULEFT_WRIST, 0,
                                        hiResJointAngles[LW0].datum);

   upperJointMods->set_arm_joint_angle (URIGHT_SHOULDER, 2,
                                        hiResJointAngles[RS2].datum);
   upperJointMods->set_arm_joint_angle (URIGHT_SHOULDER, 1,
                                        hiResJointAngles[RS1].datum);
   upperJointMods->set_arm_joint_angle (URIGHT_SHOULDER, 0,
                                        hiResJointAngles[RS0].datum);

   upperJointMods->set_arm_joint_angle (URIGHT_ELBOW, 0,
                                        hiResJointAngles[RE0].datum);

   upperJointMods->set_arm_joint_angle (URIGHT_WRIST, 2,
                                        hiResJointAngles[RW2].datum);
   upperJointMods->set_arm_joint_angle (URIGHT_WRIST, 1,
                                        hiResJointAngles[RW1].datum);
   upperJointMods->set_arm_joint_angle (URIGHT_WRIST, 0,
                                        hiResJointAngles[RW0].datum);

   pfCoord rifle_coord;

   for (int ix = 0; ix <= 2; ix++) {
      rifle_coord.xyz[ix] = hiResJointAngles[RCX + ix].datum;
      rifle_coord.hpr[ix] = hiResJointAngles[RCH + ix].datum;
   }
   if (sensor_rifle_dcs) {
      pfDCSCoord(sensor_rifle_dcs, &rifle_coord);
   }
}

void JACK_DI_VEH::set_hi_res (fixedDatumRecord *angleArray,
                              unsigned int the_size)
{
/*
   // override Performer Jack model with new joint angles
   if (the_size < MIN_NUM_HIRES_JOINTS) {
      cerr << "Error:\tJACK_DI_VEH::set_hi_res not enought "
           << "angles in angle array."
           << endl;
   }
   else {
      bcopy (angleArray, hiResJointAngles, 
             sizeof (fixedDatumRecord) * MIN_NUM_HIRES_JOINTS);
      numHiResJointAngles = the_size;
   }
*/
   numHiResJointAngles = the_size;

   for (int ix = 0; ix < numHiResJointAngles; ix++) {
      switch (angleArray[ix].datum_id) {
         case LS0_TAG:
            hiResJointAngles[LS0].datum = angleArray[ix].datum;
            break;
         case LS1_TAG:
            hiResJointAngles[LS1].datum = angleArray[ix].datum;
            break;
         case LS2_TAG:
            hiResJointAngles[LS2].datum = angleArray[ix].datum;
            break;
         case LE0_TAG:
            hiResJointAngles[LE0].datum = angleArray[ix].datum;
            break;
         case LW0_TAG:
            hiResJointAngles[LW0].datum = angleArray[ix].datum;
            break;
         case LW1_TAG:
            hiResJointAngles[LW1].datum = angleArray[ix].datum;
            break;
         case LW2_TAG:
            hiResJointAngles[LW2].datum = angleArray[ix].datum;
            break;
         case RS0_TAG:
            hiResJointAngles[RS0].datum = angleArray[ix].datum;
            break;
         case RS1_TAG:
            hiResJointAngles[RS1].datum = angleArray[ix].datum;
            break;
         case RS2_TAG:
            hiResJointAngles[RS2].datum = angleArray[ix].datum;
            break;
         case RE0_TAG:
            hiResJointAngles[RE0].datum = angleArray[ix].datum;
            break;
         case RW0_TAG:
            hiResJointAngles[RW0].datum = angleArray[ix].datum;
            break;
         case RW1_TAG:
            hiResJointAngles[RW1].datum = angleArray[ix].datum;
            break;
         case RW2_TAG:
            hiResJointAngles[RW2].datum = angleArray[ix].datum;
            break;
#ifndef NOUPPERBODY
         case RCX_TAG:
            hiResJointAngles[RCX].datum = angleArray[ix].datum;
            break;
         case RCY_TAG:
            hiResJointAngles[RCY].datum = angleArray[ix].datum;
            break;
         case RCZ_TAG:
            hiResJointAngles[RCZ].datum = angleArray[ix].datum;
            break;
         case RCH_TAG:
            hiResJointAngles[RCH].datum = angleArray[ix].datum;
            break;
         case RCP_TAG:
            hiResJointAngles[RCP].datum = angleArray[ix].datum;
            break;
         case RCR_TAG:
            hiResJointAngles[RCR].datum = angleArray[ix].datum;
            break;
#endif // NOUPPERBODY
         default:
            cerr << "Unknown datum id: " << angleArray[ix].datum_id 
                 << " found in HiResDataPDU." 
                 << endl;
            break;
      }
   }
}


void JACK_DI_VEH::hi_res_rifle_on ()
{
   if (jack_rifle_sw) {
      pfSwitchVal(jack_rifle_sw, PFSWITCH_OFF);
   }
   if (iport_rifle_sw) {
      pfSwitchVal(iport_rifle_sw, PFSWITCH_OFF);
   }
   if (sensor_rifle_sw) {
      pfSwitchVal(sensor_rifle_sw, PFSWITCH_ON);
   }
}

void JACK_DI_VEH::iport_rifle_on ()
{
   if (jack_rifle_sw) {
      pfSwitchVal(jack_rifle_sw, PFSWITCH_OFF);
   }
   if (iport_rifle_sw) {
      pfSwitchVal(iport_rifle_sw, PFSWITCH_ON);
   }
   if (sensor_rifle_sw) {
      pfSwitchVal(sensor_rifle_sw, PFSWITCH_OFF);
   }
}

void JACK_DI_VEH::jack_rifle_on ()
{
   if (jack_rifle_sw) {
      pfSwitchVal(jack_rifle_sw, PFSWITCH_ON);
   }
   if (iport_rifle_sw) {
      pfSwitchVal(iport_rifle_sw, PFSWITCH_OFF);
   }
   if (sensor_rifle_sw) {
      pfSwitchVal(sensor_rifle_sw, PFSWITCH_OFF);
   }
}


//this is a major hack to let the people know that we have fixed them
void send_silver_bullet(int target)
{
  DetonationPDU Dpdu; //the PDU

  Dpdu.firing_entity_id.address.site = G_static_data->DIS_address.site;
  Dpdu.firing_entity_id.address.host = G_static_data->DIS_address.host;
  Dpdu.firing_entity_id.entity = G_vehlist[G_drivenveh].DISid.entity;
  Dpdu.munition_id.address.site = 0;
  Dpdu.munition_id.address.host = 0;
  Dpdu.munition_id.entity =  0;
  /*the event ID*/
  Dpdu.event_id.address.site = 0;
  Dpdu.event_id.address.host = 0;
  Dpdu.event_id.event = 0;

  /*what did we shoot*/
  bzero((char *)&Dpdu.burst_descriptor.munition.entity_kind, 
        sizeof(Dpdu.burst_descriptor.munition.entity_kind));
  Dpdu.burst_descriptor.munition.entity_kind = 99; //the silver bullet
  Dpdu.target_entity_id.address.site = 0;
  Dpdu.target_entity_id.address.host = 0;
  //who was the victim
  //Dpdu.target_entity_id = G_vehlist[target].vehptr->getdisname();

  Dpdu.target_entity_id.address.site = G_vehlist[target].DISid.address.site;
  Dpdu.target_entity_id.address.host = G_vehlist[target].DISid.address.host;
  Dpdu.target_entity_id.entity = G_vehlist[target].DISid.entity;

  // cerr << "SILVER BULLE  - vid "<< target << " " << G_vehlist[target].vehptr->vid<< "\n";
  // cerr << "SILVER BULLET - " << Dpdu.target_entity_id.address.site << endl;
  // cerr << "SILVER BULLET - " << Dpdu.target_entity_id.address.host << endl;
  // cerr << "SILVER BULLET - " << Dpdu.target_entity_id.entity << endl;
  Dpdu.burst_descriptor.warhead = WarheadMunition_HighExpAntiTank;
  Dpdu.burst_descriptor.fuze = FuzeMunition_ContactInstant;
  Dpdu.burst_descriptor.quantity = 1; /*One at a time*/
  Dpdu.burst_descriptor.rate = 0;

  /*movement parameters*/
  Dpdu.velocity.x = 0.0f;
  Dpdu.velocity.y = 0.0f;
  Dpdu.velocity.z = 0.0f;

  /*where we hit*/
//  float *loc;
//  loc = G_vehlist[target].vehptr->getposition();
//  Dpdu.location_in_world.x = loc[X];
//  Dpdu.location_in_world.y = loc[Y];
//  Dpdu.location_in_world.z = loc[Z];
   Dpdu.location_in_world.x = 0.0f;
   Dpdu.location_in_world.y = 0.0f;
   Dpdu.location_in_world.z = 0.0f;

  /*no Aticulated Parameters,yet*/
  Dpdu.num_articulat_params = 0;

 //We hit a building, so enter the coordinates
  Dpdu.location_in_entity.x = 0.0f;
  Dpdu.location_in_entity.y = 0.0f;
  Dpdu.location_in_entity.z = 0.0f;

  //we hit the dirt
  Dpdu.detonation_result = DetResult_EntityImpact;

  /*send it*/
    switch ( G_dynamic_data->network_mode )
       {
       case NET_OFF:
       case NET_RECEIVE_ONLY:
          break;
       case NET_SEND_ONLY:
       case NET_SEND_AND_RECEIVE:
       default:
          if (G_net->write_pdu((char *) &Dpdu, (PDUType )DetonationPDU_Type) 
               == FALSE){
             cerr << "write_pdu() failed\n";
          break;
       }

  }

}


void alterMotion(int index, float& speed)
{

   switch(index) {

      case (J_FORWARD):
         speed = 2.0f;
         break;

      case (HALT):
         speed = 0.0f;
         break;

      case (SPEED):
         if(speed < 9.0f)
            speed += 2.0f;
         else if(speed < 10.0f)
            speed += 1.0f;
         break;

      case (SLOW):
         if(speed > 0.0f)
            speed -= 1.0f;
         break;

      default:
         cerr << "alterMotion: unknown index\n";
   }

} // alterMotion

#ifndef NOUPPERBODY
//----------------------------------------------------------------------
//   Function: 
//    Summary: 
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
void getJackWristTx(JackSoldier *js,
                    pfMatrix H_rwrist, pfMatrix H_lwrist)
{
   pfMatrix H_upwaist;
   pfMatrix H_temp;

   pfMakeIdentMat(H_upwaist);

   pfGetDCSMat(js->jointptrs[LEFT_CLAVICLE], H_temp);
   pfMultMat(H_lwrist, H_temp, H_upwaist);
   pfGetDCSMat(js->jointptrs[LEFT_SHOULDER], H_temp);
   pfPreMultMat(H_lwrist, H_temp);
   pfGetDCSMat(js->jointptrs[LEFT_ELBOW], H_temp);
   pfPreMultMat(H_lwrist, H_temp);
   pfGetDCSMat(js->jointptrs[LEFT_WRIST], H_temp);
   pfPreMultMat(H_lwrist, H_temp);

   pfGetDCSMat(js->jointptrs[RIGHT_CLAVICLE], H_temp);
   pfMultMat(H_rwrist, H_temp, H_upwaist);
   pfGetDCSMat(js->jointptrs[RIGHT_SHOULDER], H_temp);
   pfPreMultMat(H_rwrist, H_temp);
   pfGetDCSMat(js->jointptrs[RIGHT_ELBOW], H_temp);
   pfPreMultMat(H_rwrist, H_temp);
   pfGetDCSMat(js->jointptrs[RIGHT_WRIST], H_temp);
   pfPreMultMat(H_rwrist, H_temp);
}
#endif // NOUPPERBODY

//************************************************************************//
//*************** class JACK_TEAM_VEH ************************************//
//************************************************************************//

JACK_TEAM_VEH::JACK_TEAM_VEH(int id, int atype, ForceID the_force)
: JACK_DI_VEH(id,atype,the_force)
{
   char tempstr[10];

   if (!*(G_static_data->marking) ) {
     strncpy ( tempstr, (const char *)G_static_data->hostname, MARKINGS_LEN-3 );
     strcpy ( (char *)G_static_data->marking, "DITeam" );
     strcat ( (char *)G_static_data->marking, tempstr );
   }

   teamCount = 0; // initialize as empty

   create_entity ( atype, the_force );

} // end JACK_TEAM_VEH::JACK_TEAM_VEH()

JACK_TEAM_VEH::~JACK_TEAM_VEH()
{
   delete upperJointMods;
   delete signals;
}

void JACK_TEAM_VEH::create_entity(int atype, ForceID )
//fill in the fields that the constuctor does not
{
   long entity_mask;

#ifdef TRACE
   cerr << "adding vid "<<vid << "  model " << atype 
        << " JACK_TEAM::create_entity()\n";
#endif

   char name[256], sname[256];
   int found = FALSE;

   if(G_num_soldiers < JACK_MAXSOLDIER-50-1) {
       if(G_tot_soldiers  < JACK_MAXSOLDIER-50-1)  // new JackSoldier to be created
       {
          js = new JackSoldier((pfGroup*)pfGetChild(models,0));
          G_JackSoldier_active[js->soldiernum] = TRUE;
          G_num_soldiers++;
          G_tot_soldiers++;
          cerr << "     num_soldiers= " << G_num_soldiers << " tot_soldiers = " 
             << G_tot_soldiers << endl;

          if(!teamCount) {
             if(int(G_vehtype[atype].distype.subcategory) > 100) {
                switch(int(G_vehtype[atype].distype.subcategory)) {

                   case FIRE_TEAM:
                      teamCount = 4;
                      break;
 
                   case SQUAD_TEAM:
                      teamCount = 13;
                      break;

                   case PLATOON_TEAM:
                      teamCount = 40;
                      break;

                   case COMPANY_TEAM:
                      teamCount = 121;
                      break;

                   default:
                      teamCount = 2;
                      break;
                }
             }
             else {             // subcategory is nbr of individuals 
                teamCount = int(G_vehtype[atype].distype.subcategory);
             }
          }

          entity_mask =
              (long(G_vehtype[atype].distype.entity_kind) << ENTITY_KIND_SHIFT); 

          formation = new formationClass(teamCount);

          for(int ix = 0; ix < teamCount-1; ix++) {
             teamDCS[ix] = pfNewDCS();
             pfNodeTravMask ( teamDCS[ix], PFTRAV_ISECT,
                entity_mask, PFTRAV_SELF|PFTRAV_DESCEND
                |PFTRAV_IS_UNCACHE, PF_SET);
             // replicate geometry at another location for another team member
             pfAddChild(teamDCS[ix],js->soldier);
             //attach it to the scene graph
             pfSync();
             pfAddChild(G_moving,teamDCS[ix]);
          }

          found = TRUE;
       }
       else { // max # of JackSoldiers created, try reusing JackSoldiers

         entity_mask =
            (long(G_vehtype[atype].distype.entity_kind) << ENTITY_KIND_SHIFT);

         for(int sj=1; sj < JACK_MAXSOLDIER-50;  sj++) {
            if(G_JackSoldier_active[sj] == FALSE) {
               found = TRUE;
               cout << "reusing soldier # "<< sj << endl;
               G_JackSoldier_active[sj] = TRUE;
               js = js->jack->table[sj]; 
               sprintf(name,"s%d",js->soldiernum);
               sprintf(sname,"s%d",js->soldiernum);
               js->soldier = (pfGroup*)pfGetChild(models,0);
               pfNodeName(js->soldier,name);
               strcpy(name,pfGetNodeName(js->soldier));

               for (int i = 0;i < js->jack->njoints;i++) {
                  sprintf(name,"s%d/%s",js->soldiernum,
                     js->jack->jointsinfo[i].name);
                  if ((js->jointptrs[i] = (pfDCS*)pfLookupNode(name, pfGetDCSClassType())) == NULL)
                     cerr << "Cannot locate pfDCS named " 
                        << js->jack->jointsinfo[i].name << "in cloned graph.\n";
               }

               for (i = 0;i < js->jack->nfigs;i++) {
                  sprintf(name,"s%d/%s",js->soldiernum,
                     js->jack->figsinfo[i].name);
                  if ((js->figptrs[i] = (pfDCS*)pfLookupNode(name, pfGetDCSClassType())) == NULL)
                     cerr << "Cannot locate pfDCS named  " 
                        << js->jack->figsinfo[i].name << " in cloned graph.\n";
               }

               G_num_soldiers++;
               G_tot_soldiers++;
               cerr << "    Reused JackSoldier " << sj << ": "
                    << " num_soldiers = " << G_num_soldiers
                    << " tot_soldiiers = " << G_tot_soldiers;
               break;
            }
         } // end for
      } // end else

      if(!found) {
         artic = NO_ARTIC;
      }
      else {
         Control = JACKML;
         bzero ( js->JointVals, MAXJOINTS);
         JointVals = js->JointVals;

         teamType = (teamEnum)G_vehtype[atype].distype.subcategory;

         if((atype == JACKNORIFLE_TEAM) && 
            (G_vehtype[atype].distype.specific == STRETCHER)) {
            const int WOUNDED = 186;
            weapon_present = FALSE;  // no rifle, use stretcher instead
            movement_index = MEDIC_ARMS_UPRIGHT;
            upperJointMods->set_index(MEDIC_ARMS_UPRIGHT);
            // Assume only 2 to a medic team
            formation->set_position(0,180.0f,2.7f,2.7f);
            behavior = TWOMAN_STRETCHER;

            pfGroup *stretcher, *wounded;
            obj_switch = pfNewSwitch();
            weapon_dcs = pfNewDCS();
            stretcher = (pfGroup*)pfdLoadFile("models/Misc/stretcher.flt");
            wounded = (pfGroup*) pfGetChild((pfSwitch *)
            pfClone(G_vehtype[WOUNDED].mswitch,0),0);
            if((stretcher) && (wounded)) {
               pfAddChild(obj_switch,stretcher);
               pfAddChild(obj_switch,wounded);
               pfAddChild(weapon_dcs,obj_switch);
               pfSwitchVal(obj_switch, 0);
               pfAddChild(G_moving,weapon_dcs);
            }
            else {
               cerr << "Unable to create stretcher or wounded\n";
            }
         }
         //else if(atype == JACKMORTAR_TEAM) {
            // need to fill in
         //}
         else {             // other types of teams in just a line
            weapon_present = TRUE;
            weapon_dcs = js->figptrs[0] ;
            weapon = JACKTTES_WEAPON_STOWED;
            for(int ix=0; ix < teamCount-1; ix++) {
               formation->set_position(ix,90.0f,ix+1,1.0f);
            }
         }

         appear = JACKTTES_UPRIGHT;
         weapon = JACKTTES_WEAPON_STOWED;
         immed_transition = FALSE; // smooth position transitions
         // set non zero initial velocity to initialize position
         pfSetVec3(velocity,0.0f,1.0f,0.0f);

         if(vid == G_drivenveh) {
             // Use raw joint angle updates always 
             js->sb->mode = JACKML_SOLDIERMODE_JOINTANGLES;

             SendPacket();
             CheckJackUpdate(js,upperJointMods,0.0f,0.0f,FALSE, FALSE);
         } // end if (vid == G_drivenveh)
         else  {  // not driven veh
 
            //  use DCS updates   
            //js->sb->mode = JACKML_SOLDIERMODE_JOINTDCS;

            // use angle updates -- DCS updates still seems buggy
            // and hand signals & medic mode don't work right
            js->sb->mode = JACKML_SOLDIERMODE_JOINTANGLES;

            SendPacket();
            CheckJackUpdate(js,upperJointMods,0.0f,0.0f,FALSE, TRUE);

            pfSetVec3(velocity,0.0f,0.0f,0.0f);
         }
      } // end if found
   } // end if (G_num_soldiers < JACK_MAXSOLDIER)

   else {
      artic = NO_ARTIC;
      Control = OTHER;
      cerr << "WARNING:\tNo other JACK soldiers available." << endl;
   }
} // end JACK_TEAM_VEH::create_entity()


void JACK_TEAM_VEH::moveObject(behaveEnum behave, pfCoord& team, 
   float speed)
{
   pfMatrix mat, weapmat;
   pfGetDCSMat(js->figptrs[1],mat);
   pfGetDCSMat(weapon_dcs,weapmat);   

   switch(behave) {
      case TWOMAN_STRETCHER:
         if(speed > 3.0f) {
            pfDCSTrans(weapon_dcs,(team.xyz[X] + posture.xyz[X])*0.5f,
               (team.xyz[Y] + posture.xyz[Y])*0.5f,
               (posture.xyz[Z]+mat[3][2]+3*weapmat[3][2])*0.25f);
         }
         else {
            pfDCSTrans(weapon_dcs,(team.xyz[X] + posture.xyz[X])*0.5f,
               (team.xyz[Y] + posture.xyz[Y])*0.5f,
               posture.xyz[Z]+mat[3][2]);
         }

         pfDCSRot(weapon_dcs,posture.hpr[HEADING]+90.0f,
            posture.hpr[PITCH],
            posture.hpr[ROLL]);
         break;

      default:
         break;

   } // end switch

} // end JACK_TEAM_VEH::moveObject()


void JACK_TEAM_VEH::locateTeamPart(int ix, const pfVec3& mainPart, 
   pfVec3& teamPart)
{
   pfVec3  delta_xyz, team_xyz;

   if((ix > 0) && (ix < teamCount)) { 
      pfSubVec3(delta_xyz, mainPart, posture.xyz); 
      locateTeamMember(ix, team_xyz);
      pfAddVec3(teamPart, team_xyz, delta_xyz);
   }

} // end JACK_TEAM_VEH::locateTeamPart()


void JACK_TEAM_VEH::locateTeamMember(int ix, pfVec3& team_xyz) 
{
   float sbrg,cbrg;
   pfVec3  delta_xyz;
   pfCoord team_coord;

   pfSinCos(formation->get_bearing(ix) -posture.hpr[HEADING],&sbrg,&cbrg);
   pfSetVec3(delta_xyz, sbrg*formation->get_range(ix),
      cbrg*formation->get_range(ix),0.0f);
   pfAddVec3(team_xyz, posture.xyz, delta_xyz);

   // if too far from main vehicle, compute new altitude
   if(formation->get_range(ix) > 1.0f) {

      pfCopyVec3(team_coord.hpr,posture.hpr);
      pfCopyVec3(team_coord.xyz,team_xyz);

      if (!grnd_orient(team_coord)) {
         team_xyz[Z] = posture.xyz[Z];
      }
      else {
         team_xyz[Z] = team_coord.xyz[Z];
      }
   }

} // end JACK_TEAM_VEH::locateTeamMember()


void JACK_TEAM_VEH::placeTeamMember(int ix, const pfVec3 team_xyz)
{
   pfDCSTrans(teamDCS[ix],team_xyz[X], team_xyz[Y], team_xyz[Z]);
   pfDCSRot(teamDCS[ix],posture.hpr[HEADING], posture.hpr[PITCH],
      posture.hpr[ROLL]);

} // end JACK_TEAM_VEH::placeTeamMember()


movestat JACK_TEAM_VEH::move()
// move the vehicle for the next frame
{
   pfVec3 oldpos;
   static int old_tethered;
   int hitSomething = FALSE;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tJACK_TEAM_VEH::move" << endl;
#endif

#ifdef TRACE
   cerr << "Moving a JACK_TEAM_VEH\n";
#endif

   // check limits of head motion
   if(look.hpr[HEADING] > 90.0f) {
      look.hpr[HEADING] = 90.0f;
   }
   else if(look.hpr[HEADING] < -90.0f) {
      look.hpr[HEADING] = -90.0f;
   }
   else if(look.hpr[PITCH] > 90.0f) {
      look.hpr[PITCH] = 90.0f;
   }
   else if(look.hpr[PITCH] < -90.0f) {
      look.hpr[PITCH] = -90.0f;
   }

   if((icontype == JACKNORIFLE_TEAM) &&
      (G_vehtype[icontype].distype.specific == STRETCHER)) {
      if(signals->get_index() == WOUNDED_ONBOARD) {   
         // if so, show guy on stretcher
         pfSwitchVal(obj_switch,1); 
      }
   }

   else if(checkSignal()) {
      executeSignal(FALSE);
   }

   initMove(old_tethered,tethered);
   pfCopyVec3(oldpos,posture.xyz);

   if (!(paused || tethered || cdata.rigid_body) ) {

      determineSpeed(cdata,input_control);

      //move the GROUND vehicle type in the x-y plane
      posture.xyz[X] += velocity[X]*cdata.delta_time;
      posture.xyz[Y] += velocity[Y]*cdata.delta_time;
 
      determinePosture(cdata,oldpos,hitSomething);
      if(hitSomething) {
         return(HITSOMETHING);
      }

 
   } // End !paused !tethered !rigid_body

   // If tethered, let the other vehicle calculate our position for us
   if (tethered) {
      handleTether(cdata,tethered);
   }

   computeFirepoint();  // use JACK_DI's function

   checkUnderground(tethered);

   determineStatus(speed);
   if((icontype == JACKNORIFLE_TEAM) &&
      (G_vehtype[icontype].distype.specific == STRETCHER)) {
      if(appear == JACKTTES_UPRIGHT) {
         upperJointMods->set_index(MEDIC_ARMS_UPRIGHT);
      }
      else if(appear == JACKTTES_KNEEL) {
         upperJointMods->set_index(MEDIC_ARMS_KNEELING);
      }
   }
   else if((appear == JACKTTES_PRONE)
      && (weapon == JACKTTES_WEAPON_DEPLOYED)) {
      upperJointMods->set_index(PRONE_FIRING);
   }
   else if(((appear != JACKTTES_PRONE) ||
      (weapon != JACKTTES_WEAPON_DEPLOYED)) &&
      (upperJointMods->get_index() == PRONE_FIRING)) {
       upperJointMods->set_index(NOSIGNAL);
   }


   if(((behavior == PASSING_REV1) || (behavior == PASSING_REV2))
      && (upperJointMods->get_index() != EYES_RIGHT)) {
      upperJointMods->set_index(SHOULDER_ARMS);
   }

   if(checkSignal()) {
      executeSignal(FALSE);
   }

   // update articulations using JackML
   updateJack();
   
   //put main vehicle in the correct location in space
   placeVehicle();
   
   // compute position of team members rel. to main vehicle
   pfCoord team;
   for(int ix = 0; ix < teamCount-1; ix++) {

      locateTeamMember(ix,team.xyz);
      placeTeamMember(ix,team.xyz);

      if((icontype == JACKNORIFLE_TEAM) &&
         (G_vehtype[icontype].distype.specific == STRETCHER)) {
         // move stretcher between 2 medic team members
         moveObject(TWOMAN_STRETCHER,team,speed);
      }
   }

   oldstatus = status;
   return(FINEWITHME);

} // end JACK_TEAM::move()


movestat JACK_TEAM_VEH::moveDR(float deltatime,float curtime) 
//move routine for DR vehciles simple move
{
   float speed;

#ifdef TRACE
   cerr << "Dead Recon a JACK_TEAM_VEH.\n";
#endif

   if((artic != NO_ARTIC) && (!js)) {
      artic = NO_ARTIC;
   }

   if((icontype == JACKNORIFLE_TEAM) &&
      (G_vehtype[icontype].distype.specific == STRETCHER)) {
      if(signals->get_index() == WOUNDED_ONBOARD) { 
         // if so, show guy on stretcher
         pfSwitchVal(obj_switch,1);
      }
   }
   else if(checkSignal()) {
      executeSignal(TRUE);
   }

   if(((curtime - lastPDU) > G_static_data->DIS_deactivate_delta ) && 
      (vid != G_drivenveh) &&
      (!unitMember)) {
      //Deactivate the puppy
      if(artic == JACK_ARTIC) {
         G_JackSoldier_active[js->soldiernum] = FALSE;  // Deactivate but don't delete
         G_num_soldiers --;
      }
      cout << "DEACTIVATING soldier time interval " << curtime - lastPDU << endl;
      return(DEACTIVE);
   }

   computeDR(deltatime);

   speed = pfLengthVec3(velocity);

   if (vid != G_drivenveh){
      determineDRStatus(speed);
      if((icontype == JACKNORIFLE_TEAM) && 
         (G_vehtype[icontype].distype.specific != STRETCHER)) {
         if(speed <= 3.0f) {
            if(!upperJointMods->get_index()) {
               upperJointMods->set_index(WALK_NO_WEAPON);
            }
         }
         else if(speed > 3.0f) {
            if(!upperJointMods->get_index()) {
               upperJointMods->set_index(RUN_NO_WEAPON);
            }
         }
      }
      else {
         if((appear == JACKTTES_PRONE)
           && (weapon == JACKTTES_WEAPON_DEPLOYED)) {
           upperJointMods->set_index(PRONE_FIRING);
         }
         else if(((appear != JACKTTES_PRONE) ||
            (weapon != JACKTTES_WEAPON_DEPLOYED)) &&
            (upperJointMods->get_index() == PRONE_FIRING)) {
            upperJointMods->set_index(NOSIGNAL);
         }

      }


      // Update DR entity 
      posture = drpos;

      if((!unitMember) && (speed > 0.0f)) {
         checkBackwardMotion(speed,abs_speed,velocity,posture.hpr[HEADING]);
      }

//  doesn't work right yet - DCS updating seems buggy
//     if(upperJointMods->get_status() == DONE) { 
//        if (js->sb->mode != JACKML_SOLDIERMODE_JOINTDCS) {
//           js->sb->mode = JACKML_SOLDIERMODE_JOINTDCS;
//cerr << "switching to faster JOINTDCS updates mode " << js->sb->mode << endl;
//        }
//     }
//     else  {
//        if(js->sb->mode != JACKML_SOLDIERMODE_JOINTANGLES) {
//           js->sb->mode = JACKML_SOLDIERMODE_JOINTANGLES;
//cerr << "switching to slower JOINTANGLES updates mode " << js->sb->mode << endl;
//       }
//     }

      if((artic == JACK_ARTIC) && (!quiet)) {
         updateDRJack();
      }

      //put the vehicle in the correct location in space
      placeVehicle();


      // compute position of team members rel. to main vehicle
      pfCoord team;
      for(int ix = 0; ix < teamCount-1; ix++) {

         locateTeamMember(ix, team.xyz);
         placeTeamMember(ix, team.xyz);

         // move stretcher between 2 medic team members
         if((icontype == JACKNORIFLE_TEAM) &&
         (G_vehtype[icontype].distype.specific == STRETCHER)) {
            moveObject(TWOMAN_STRETCHER,team,speed);
         }
      }
   }
   else if (( !G_static_data->stealth ) &&
      (!unitMember)) // So sendentitystate() only if G_drivenveh
   {

      // This is us, have we exceeded the DR error or time limit?
      int signalIndex = signals->get_index();
      if((upperJointMods->get_index()) || (signalIndex)) { 
         movement_index = upperJointMods->get_index() 
            + signalIndex*100;
         sendentitystate();
//         oldspeed = speed; 
         if((upperJointMods->get_index()) || (signals->get_index())) {
            signals->set_pduFlag(TRUE);
         }
      }
      else if((G_static_data->DIS_heartbeat_delta < (curtime - lastPDU)) 
         || (delta_send(&posture,&drpos))) {
         time2Send(speed);
      }
      else { 
         reason2Send(speed);
      }

      oldstatus = status;

   } // End not not stealth

   return(FINEWITHME);
} // end JACK_TEAM_VEH::moveDR()


void JACK_TEAM_VEH::entitystateupdate(EntityStatePDU *epdu, sender_info &sender)//get an entity state PDU with articulated parts
{
   PERSON_VEH::entitystateupdate(epdu,sender);

   movement_index = atoi(epdu->dead_reckon_params.TBD);
   signals->set_index(int(movement_index/100.0f) *100);
 
   // update only if a hand signal is not in progress
   int signalIndex = signals->get_index();
   if(upperJointMods->get_status() == DONE) {
       movement_index - signalIndex; 
      upperJointMods->set_index(movement_index - 
         signalIndex);
   }
} // end JACK_TEAM_VEH::entitystateupdate()


void JACK_TEAM_VEH::setSignal(int index) 
{

   // special case team signal which is treated like a hand signal
   if((signalType(index) == MISC_SIGNAL) 
      && (index != POINT_TO)
      && (signals->get_index() != index)) {
      signals->set_signal(index, upperJointMods, TRUE);
   }
   else  {
      if(index == J_DOWN) {
         changestatus(Appearance_LifeForm_Prone);
      }
      else {
         signals->set_signal(index, NULL, FALSE);
      }
   }

   signals->set_pduFlag(FALSE); // set flag to send DIS PDU
 
} // end JACK_TEAM_VEH::setSignal()


int JACK_TEAM_VEH::checkSignal()
{
   int returnValue;
   int index = signals->get_index();

   if((index == WOUNDED_ONBOARD) 
      && (icontype == JACKNORIFLE_TEAM) 
      && (G_vehtype[icontype].distype.specific == STRETCHER)) {

      // if so, show guy on stretcher
      pfSwitchVal(obj_switch,1);
      signals->reset();
      returnValue = DONE;
   }
   else if(index == J_DOWN) {
      if(!(status & Appearance_LifeForm_Prone)) {
         changestatus(Appearance_LifeForm_Prone);
      }
      signals->reset();
      returnValue = DONE;
   }
   else if((signals->get_type() == MISC_SIGNAL) 
      && (signals->check_signal(upperJointMods,TRUE))) {
      returnValue = INPROGRESS;
   }
   else if(signals->check_signal(NULL,FALSE))  { 
      returnValue = INPROGRESS;
   }
   else {
      returnValue = DONE;
   }

   return(returnValue);

} // end JACK_TEAM_VEH::checkSignal()


void JACK_TEAM_VEH::executeSignal(int DRFlag)
{
   int index = signals->get_index();

   switch(index) {

      case (COLUMN):
      case(J_LINE):
      case(WEDGE):
      case(VEE):
      case(ECHELON_RIGHT):
      case(ECHELON_LEFT):
      case (CLOSE_UP):
      case (OPEN_UP):
      case (DISPERSE):
         if(!immed_transition) {
            formation->setFormation(index);
         }
         else {
            formation->snapFormation(index);
         }
         break;

      case (MOVE_RIGHT):
      case (MOVE_LEFT):
         if(!immed_transition) {
            formation->setFormation(index);
         }
         else {
            formation->snapFormation(index);
         }
         posture.hpr[HEADING] = formation->get_heading();
         break;


      case (J_FORWARD):
      case (HALT):
      case (SPEED):
      case (SLOW):
         if(!DRFlag) {
            alterMotion(index, speed);
         }
         else {
            float spd = pfLengthVec3(velocity), s, c;
            alterMotion(index,  spd);
            pfSinCos(posture.hpr[HEADING],&s,&c);
            velocity[X] = spd*c;
            velocity[Y] = spd*s;
         }
         break;

      default:
         cout << "doing nothing for signal " << index;
         // do nothing;
         break;

   } // end switch

   if(formation->get_status() == DONE) {
      signals->reset();
   }
} // end JACK_TEAM_VEH::executeSignal()


void
JACK_TEAM_VEH::fire_weapon ( const wdestype weapon_num,
                             const int /* num_times */ )
{
   for ( int member = 0; member < getTeamCount()-1; member++ ) {
      firebuttonhit ( weapon_num, vid, posture, look );
   }
}


#endif  // ifdef JACK


