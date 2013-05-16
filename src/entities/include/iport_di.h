
#ifndef __NPS_IPORT_DI__
#define __NPS_IPORT_DI__

#include "person.h"
#include "jack_di_veh.h"

#ifdef IPORT_INPUT

class IPORT_DI: public PERSON_VEH
  {
  friend int motion_Medic ( void *, IPORT_DI * );
  protected:

  pfSwitch
    *iport_rifle_sw, *jack_rifle_sw;
  pfDCS
    *rifle_dcs,
    *person_dcs;
  pfCoord
    Rifle_position;
  int
    last_fired;
  int
    last_secondary;
  int
    joint_index[MAXJOINTS];
  float
    oldspeed;
  unsigned int
    oldstatus;
  int
    force_pdu;
  void
    *motion;


  public:

  int
    medic_is_active;
 
  IPORT_DI(int,int,ForceID);
  void create_entity ( int, ForceID );
  void update ( DIDismount *, CONTROL & );
  void del_vehicle ();
  ctltype getControl ();
  ~IPORT_DI ();

#if 0
  void changestatus ( unsigned int );
  void changeWeaponStatus ( unsigned int );

  void walk(float speed);
  void normalize_angles();      /* added by Shirley 8/94 */


  int &index(int);
  void apply_angles ();

  movestat moveDR(float,float);
  movestat move();
  void entitystateupdate(EntityStatePDU *, sender_info &);
  void sendentitystate();
  vtype gettype();
#endif
 
  };
#endif


#endif
