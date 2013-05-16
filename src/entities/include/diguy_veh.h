
#ifndef __NPS_DIGUY_VEH__
#define __NPS_DIGUY_VEH__

#include "vehicle.h"
#include "diguyTypeLookup.h"
#include "pdu.h"

#include "imclass.h"

enum diEnum {
   diDISv203,
   diDISv204
};

class DIGUY_VEH : public VEHICLE {

protected:

   static int init;
   static int posMech;

   static diguyTypeLookup *lookUp;

// These are so that we can use both 2.0.3 and 2.0.4 human enumerations

   static unsigned int diApperance;
   static unsigned int diStill;
   static unsigned int diWalking;
   static unsigned int diRunning;
   static unsigned int diKneeling;
   static unsigned int diProne;
   static unsigned int diCrawling;
   static unsigned int diSwimming;
   static unsigned int diParachuting;
   static unsigned int diJumping;

   static unsigned int weaponAppearance;
   static unsigned int weaponStowed;
   static unsigned int weaponDeployed;
   static unsigned int weaponFiring;

   static void set203Enum ();
   
//   char di_name[128];
   int handle;
   float diLastTime;
   pfNode *di_model;
   pfSwitch *di_switch;
   pfCoord gun_pos;
   unsigned int  oldstatus;
   float hiResTimeStamp;

   int target_lock;

   virtual void change_gaze_vertical (const float);
   virtual void change_gaze_horizontal (const float);

public:

   inline static unsigned int getDiApperance () { return diApperance;}
   inline static unsigned int getDiStill () { return diStill;}
   inline static unsigned int getDiWalking () { return diWalking;}
   inline static unsigned int getDiRunning () { return diRunning;}
   inline static unsigned int getDiKneeling () {return diKneeling;}
   inline static unsigned int getDiProne () { return diProne;}
   inline static unsigned int getDiCrawling () { return diCrawling;}
   inline static unsigned int getDiSwimming () { return diSwimming;}
   inline static unsigned int getDiParachuting () { return diParachuting;}
   inline static unsigned int getDiJumping () { return diJumping;}

   inline static unsigned int getWeaponAppearance () { return weaponAppearance;}
   inline static unsigned int getWeaponStowed () { return weaponStowed;}
   inline static unsigned int getWeaponDeployed () { return weaponDeployed;}
   inline static unsigned int getWeaponFiring () { return weaponFiring;};

   DIGUY_VEH(int,int,ForceID);
   virtual ~DIGUY_VEH();
   void del_vehicle(void);  //public distructor

   virtual movestat move();
   virtual movestat moveDR (float,float);
   virtual movestat movedead();

   virtual vtype gettype();
   virtual void sendentitystate();
   virtual void entitystateupdate(EntityStatePDU *, sender_info &);
   virtual int check_collide(pfVec3 &, pfVec3 &, pfVec3 &, float &, float &);
   virtual void init_posture(pfCoord &);
   virtual void process_keyboard_speed_settings ();
   virtual void class_specific_keyboard ();
   virtual void class_specific_upperbody ();
   virtual void fire_weapon ( const wdestype, const int);
   virtual int birds_eye_view(int);
   virtual int transport ( float, float, float);
   void get_weapon_view (pfCoord &);
   void get_weapon_look (pfCoord &);
   int get_target_lock (void);
   void get_firepoint ( pfVec3 & );
   void set_hr_mimic_info (fixedDatumRecord *, int);
   void turnoff();
   void turnon();
   int lase ();

   static int init_diguy (InitManager *);
   static int addType (int, char*);
   static void update_diguy ();

};

#endif

