#ifndef _JOINTMODS_H
#define _JOINTMODS_H

#include <stdio.h>
#include <iostream.h>
#include <ulocks.h>
#include <Performer/pf.h>

// Conversion factors
#define RTOD 57.29578049	// Radians to Degrees
#define DTOR 1.0 / RTOD		// Degrees to Radians

#define JOINT_INDEX_OFFSET 11
#define UPPER_JOINTS 12
#define PERM_SIGNAL 50          // 1st signal index of permanent joint mods 

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef X
#define X 0
#endif
#ifndef Y
#define Y 1
#endif
#ifndef Z
#define Z 2
#endif
#ifndef HEADING
#define HEADING 0
#endif
#ifndef PITCH
#define PITCH 1
#endif
#ifndef ROLL
#define ROLL 2
#endif

enum {
   LS0 = 0, LS1, LS2, LE0, LW0, LW1, LW2,
   RS0,     RS1, RS2, RE0, RW0, RW1, RW2,
   RCX,     RCY, RCZ, RCH, RCP, RCR,
   NUM_HIRES_VALUES
};

const unsigned int LS0_TAG = ~(0xFFFFFFFF & 0x01);
const unsigned int LS1_TAG = ~(0xFFFFFFFF & 0x02);
const unsigned int LS2_TAG = ~(0xFFFFFFFF & 0x03);
const unsigned int LE0_TAG = ~(0xFFFFFFFF & 0x04);
const unsigned int LW0_TAG = ~(0xFFFFFFFF & 0x05);
const unsigned int LW1_TAG = ~(0xFFFFFFFF & 0x06);
const unsigned int LW2_TAG = ~(0xFFFFFFFF & 0x07);
const unsigned int RS0_TAG = ~(0xFFFFFFFF & 0x08);
const unsigned int RS1_TAG = ~(0xFFFFFFFF & 0x09);
const unsigned int RS2_TAG = ~(0xFFFFFFFF & 0x0A);
const unsigned int RE0_TAG = ~(0xFFFFFFFF & 0x0B);
const unsigned int RW0_TAG = ~(0xFFFFFFFF & 0x0C);
const unsigned int RW1_TAG = ~(0xFFFFFFFF & 0x0D);
const unsigned int RW2_TAG = ~(0xFFFFFFFF & 0x0E);
const unsigned int RCX_TAG = ~(0xFFFFFFFF & 0x0F);
const unsigned int RCY_TAG = ~(0xFFFFFFFF & 0x10);
const unsigned int RCZ_TAG = ~(0xFFFFFFFF & 0x11);
const unsigned int RCH_TAG = ~(0xFFFFFFFF & 0x12);
const unsigned int RCP_TAG = ~(0xFFFFFFFF & 0x13);
const unsigned int RCR_TAG = ~(0xFFFFFFFF & 0x14);

// upper body joints indices from TIPS model
enum  {WAIST = 8, BELLY, UPWAIST, 
   NECK,ATLANTO_OCCIPITAL,
   JLEFT_EYEBALL,JRIGHT_EYEBALL,
   LEFT_CLAVICLE, LEFT_SHOULDER,LEFT_ELBOW,LEFT_WRIST,
   RIGHT_CLAVICLE, RIGHT_SHOULDER,RIGHT_ELBOW,RIGHT_WRIST};

// same as upperbody joints but less JOINT_INDEX_OFFSET 
enum {UNECK=0,UATLANTO_OCCIPITAL,
   UJLEFT_EYEBALL,UJRIGHT_EYEBALL,
   ULEFT_CLAVICLE, ULEFT_SHOULDER,ULEFT_ELBOW,ULEFT_WRIST,
   URIGHT_CLAVICLE, URIGHT_SHOULDER,URIGHT_ELBOW,URIGHT_WRIST};

// Hand signal enumerated types
enum {DONE,START,INPROGRESS};

enum {FORMATION_SIGNAL,UNIT_SIGNAL,
   SPACING_SIGNAL,MOVEMENT_SIGNAL, MISC_SIGNAL};

enum {NOSIGNAL=0,COLUMN,J_LINE,WEDGE,VEE,ECHELON_RIGHT,ECHELON_LEFT,
   FIRETEAM,SQUAD,PLATOON,CLOSE_UP,OPEN_UP,DISPERSE,J_FORWARD,
   HALT,J_DOWN,SPEED,SLOW,MOVE_RIGHT,MOVE_LEFT, 
   POINT_TO,SALUTE,LEADER_SALUTE,FLAG_SALUTE,
   EYES_RIGHT,EYES_STRAIGHT,
   MEDIC_ARMS_UPRIGHT=50,MEDIC_ARMS_KNEELING,
   WALK_NO_WEAPON,RUN_NO_WEAPON,
   WALK_WEAPON_STOW,WALK_WEAPON_DEPLOY,RUN_WEAPON_DEPLOY,
   FLAG_HOLD, SHOULDER_ARMS, CROUCH, FOXHOLE_OPEN, PRONE_FIRING};

enum {LEFTARM, RIGHTARM};


class JointSet {            // class for overriding channel set data

public:
    JointSet() {override = FALSE; value[0] = value[1] = value[2] = 0.0f;}
    void set_flag(int val) {override = val;}
    int get_flag() {return (override);}
    void set_value(int index, float val) {value[index] = val;}
    float get_value(int index) {return (value[index]);}
    int isOverriden() {return ((override) ? TRUE : FALSE);}

private:
    int      override;       // TRUE, if override is desired
    float    value[3];       // values (up to 3 DOFs) to be used
};


extern int signalType(int);

// this class allows us to modify the upper body joints of jack soldier
class  JointMods {

   friend class JackSoldier;

public:
   JointMods() {status = DONE; motion_index = NOSIGNAL; }
   void set_status(int value) {status = value;}
   int get_status() {return (status);}
   void set_index(int value) {motion_index = value;}
   int get_index() {return (motion_index);}
   void set_joint(float rot, float elev, int DRFlag);
   int isTemporal() {return ((motion_index < PERM_SIGNAL) ? TRUE : FALSE);}
   void reset() {status = DONE; motion_index = NOSIGNAL; }
   void set_override_both_arms (int);
   void set_arm_joint_angle (int, int, float);
   float get_arm_joint_angle (int, int);
   

private:
   int status;               // status flag of motion
   int motion_index;         // index of motion
   int motion_type;          // type of motion
   float start_time;         // start time variable for motions
   JointSet upperJointSet[12]; // flags and joint angles for upper body

   void set_formationSignal();
   void set_unitSignal();
   void set_spacingSignal();
   void set_movementSignal();
   void set_miscSignal();
   int move_rifle();
   void orient_rifle(pfVec3 &orient);
   void override_part(int bodypart);

};

#endif
