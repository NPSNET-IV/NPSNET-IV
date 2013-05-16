// File: <jack_di_veh.h> 

#ifndef __JACK_DI_VEH_H__
#define __JACK_DI_VEH_H__

#ifdef JACK
#include "human_const.h"
#include "person.h"
#include "jacksoldier.h"
#include "formation.h"
#include "jointmods.h"
#include "signals.h"
#ifndef NODUDE
class JADECLASS;
#endif // NODUDE

const int LEAD = 0;

// model types for Jack soldiers/teams
enum jackmodelEnum {JACKFRIENDLY=85, JACKHOSTILE, JACKNORIFLE,
   JACKFRIENDLY_TEAM, JACKNORIFLE_TEAM, JACKFRIENDLY_UNIT,
   JACK_PASSREV,
   JACKFRIENDLY_SQUAD, JACKFRIENDLY_PLATOON, WOUNDED = 186};

enum moveEnum {WOUNDED_ONBOARD = 100 };

void alterMotion(int, float&);

class JACK_DI_VEH: public PERSON_VEH {

   friend class JACK_UNIT_VEH;
#ifndef NODUDE
   friend class JADECLASS;
#endif // NODUDE

protected:
   class JackSoldier *js;
   int appear;                 // one of the four
   int weapon_present;         // FALSE turns off rifle from model
   int weapon;                 // one of the two
   int Medic_update_flag;      // flag to force sendentitystate()'s
   int artic;	               // which articulation mode   
   pfSwitch *obj_switch;       // switch for objects used with DI 
   behaveEnum behavior;        // special behaviors assigned to the DI
   class JointMods *upperJointMods;   // class to override upper body joints
   class signalClass *signals;   

   // called only by move()
   virtual int checkSignal();
   void determineStatus(float&);
   void computeFirepoint();
   void updateJack();
   int MedicMode();
   void do_medic_mode(int& Medic_update_flag); 

   // called only by moveDR()
   void updateDRJack();
   void determineDRStatus(float);

   // called by various member functions 
   void SendPacket();
   void determineWeaponStatus(float);

public:
   JACK_DI_VEH(int,int,ForceID);
#ifndef NODUDE
   JACK_DI_VEH(pfSwitch *, pfDCS *, int, JNT_INFO*);
#endif // NODUDE
   virtual ~JACK_DI_VEH();
   void rot_elev_head() {}
   virtual vtype gettype() {return (JACK_DI);}
   virtual vtype getCat() {return (PERSON);}
   virtual void create_entity(int,ForceID); //Initializes the pfDCS & pfSwitch
#ifndef NODUDE
   void create_entity(pfSwitch *, int atype, ForceID, JNT_INFO * );
#endif // NODUDE
   void changestatus(unsigned int); // change the appearance status
   void changeWeaponStatus(unsigned int); //change the weapon status
   virtual void moveObject(behaveEnum, pfCoord&, float);
   movestat move();
   movestat moveDR(float,float);
   virtual void entitystateupdate(EntityStatePDU *, sender_info &);
   virtual void setSignal(int index);
   virtual void apply_hi_res ();
   virtual void set_hi_res (fixedDatumRecord *, unsigned int);
   virtual void hi_res_rifle_on ();
   virtual void iport_rifle_on ();
   virtual void jack_rifle_on ();

};

class JACK_TEAM_VEH: public JACK_DI_VEH {

   friend class JACK_UNIT_VEH;

protected:
   teamEnum teamType;          // type of team
   int teamCount;              // nbr of individuals in team
   pfDCS *teamDCS[TEAM_MAX];   // DCS of team individuals
   class formationClass *formation;
   
   // called by move() & moveDR()
   void moveObject(behaveEnum, pfCoord&, float);
   void placeTeamMember(int ix, const pfVec3 team_xyz);
   virtual int checkSignal();
   void executeSignal(int);

public:
   JACK_TEAM_VEH(int,int,ForceID);
   ~JACK_TEAM_VEH();
   vtype gettype() { return (JACK_TEAM); }
   vtype getSubcat() {return (JACK_DI);}
   teamEnum getTeamtype() { return(teamType); }
   int getTeamCount() {  return(teamCount); }
   void create_entity(int,ForceID); //Initializes the pfDCS & pfSwitch
   void locateTeamMember(int, pfVec3&);
   void locateTeamPart(int, const pfVec3&, pfVec3&);
   movestat move();
   movestat moveDR(float,float);
   void entitystateupdate(EntityStatePDU *, sender_info &);
   void setSignal(int index);
   virtual void fire_weapon ( const wdestype, const int );

};

#ifdef SS_INTERFACE
void jack_use_ss_interface ( int mode );
#endif

#endif

#endif

