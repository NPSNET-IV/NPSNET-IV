// File: <weapon.h>

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


// This is the Munitions Class definition
//	This is the base class for munitions
//		Derived classes are BULLETS, BOMBS, and MISSILES
// These are the staight line kill and dead missiles

#ifndef __WEAPONS_H__
#define __WEAPONS_H__

#include <pf.h>
#include "pdu.h"
#include "identify.h"
#include "draw.h"
#include "common_defs.h"

#ifndef NO_DVW
#define NO_PFSMOKE
#define NO_NPSSMOKE
#endif

#ifndef NO_PFSMOKE
#define NO_NPSSMOKE
#endif

class VEHICLE; //Needed for the friend functions

class DIDismount;
extern void
  dismount_firebuttonhit ( DIDismount *dismount, int vid,
                           wdestype designation );

class MUNITION {

protected:
    // The position and orientation for the munition is maintained in a pfCoord
    pfCoord posture;    // location and orientation of munition
    pfVec3 velocity;
    pfVec3 acceleration;
    pfDCS  *dcs;  //the DCS of the icon
    int firedfrom;
    int target;
    ForceID force;
    float lastPDU;  // last time PDU sent/received for this munition
    EntityID disname;  // site host entity for dis
    EntityType type;   // dis entity type
    unsigned int status;  //32 bit field to discribe it's apperance
    pfCoord drparam;  //Dead Reckoning parameters
    ushort event_id;  // the id of the launch event
    EFFECT_REC effect;

public:

    int mid;    // munition ID

    MUNITION(int id);
    ~MUNITION();

    void del_munition(void);
    virtual mtype gettype();   // what am i
    virtual movestat move(float,float);	// move the munition for the next frame
    virtual void launcheffect ();	// sfx when fire PDU received
    void impacteffect ();  //  sfx when detonate PDU received 
    movestat impactdetermination(pfVec3, pfVec3);  // determine if this munition hit anything
    void sendentitystate();
    void sendfire(int,VEHICLE *,int, pfCoord, pfCoord,
                  float, int target = -1);
    void changestatus(unsigned int); //change the status
    unsigned int getstatus();
 //send a fire PDU
    void fillinDetonationPDU(DetonationPDU *,pfVec3); // the standard part
    void senddetonation(pfVec3);// basic ground shot
    void senddetonation(DetonationResult);//air ball
    void senddetonation(pfVec3,pfVec3);//hit a building
    void senddetonation(pfVec3,pfVec3,int);//hit a vehicle
    void set_speed(float); //set the intial speed of a muniton

    friend void dismount_firebuttonhit ( DIDismount *, int, wdestype );

};

// BULLET derived type, inherits methods and public attributes from
// MUNITION.  
class BULLET : public MUNITION {

private:
   float travelled_dist;
public:
    BULLET(int);
    ~BULLET() {};
    movestat move(float,float);
    mtype gettype();
};

// BOMB derived type, inherits methods and public attributes from
// MUNITION.  
class BOMB : public MUNITION {

public:
    BOMB(int mid) : MUNITION (mid) {}; 
    ~BOMB() {};

    mtype gettype();
    movestat move(float,float);    // move the munition for the next frame
    void launcheffect();       //fx when droping a bomb, none
};


// MISSILE derived type, inherits methods and public attributes from
// MUNITION.  
class MISSILE : public MUNITION {

private:
   int video;
   float travelled_dist;
#ifndef NO_PFSMOKE
   int smoke_index;
   void setSmoke(void);
#endif

public:
    MISSILE(int mid, int my_target = -1 );
    ~MISSILE() {};
    void deactivate_missile ();
    movestat move(float,float);
    mtype gettype();
};

// This class added by Dan Bacon --  //NEW
// TORPEDO_SL derived type, inherits methods and public attributes from
// MUNITION.  Represents sub-launched torpedo.
class TORPEDO_SL : public MUNITION {

private:
    int video;                   // 0 for video torp off, 1 for on 
    float travelled_dist;        // torp will die at MAX_TORP_RANGE
    float initial_course;        // course the torpedo launched down
    int is_on_initial_course;    // 0 if not, 1 if on initial_course
    int is_underwater;           // 0 if above water, 1 if underwater
    int target_acquired;         // 0 if no target, 1 if target designated
    int target_id;               // id of current target vehicle
    int maritime_targets[250];   // array of eligible targets, all others 0

public:
    TORPEDO_SL(int mid, float launch_course);
    ~TORPEDO_SL() {};

    mtype gettype();
    void deactivate_torpedo ();  // kill torp and shut off video
    movestat move(float,float);  // move the munition for the next frame
    void launcheffect();         //fx when droping a bomb, none
    virtual void sendfire(int,VEHICLE *,int, PASS_DATA *pdata, 
                  float, int target = -1);
    // determine if this munition hit anything
    movestat impactdetermination(pfVec3, pfVec3);
};

#ifndef NOSUB
// This class added by Dan Bacon --   //NEW
// MISSILE_SL derived type, inherits methods and public attributes from
// MUNITION.  Represents sub-launched missile.
class MISSILE_SL : public MUNITION {

private:
    int video;                   // 0 for video torp off, 1 for on 
    float travelled_dist;        // torp will die at MAX_MISSILE_RANGE
    float initial_course;
    float speed;
    int is_underwater;           // 0 if above water, 1 if underwater
    int is_cruising;
    int is_on_initial_course;    // 0 if not, 1 if on initial_course
    int target_acquired;         // 0 if no target, 1 if target designated
    int target_id;               // id of current target vehicle
    int shore_targets[250];   // array of eligible targets, all others 0

public:
    MISSILE_SL(int mid, float launch_course);
    ~MISSILE_SL() {};

    mtype gettype();
    void deactivate_missile ();  // kill torp and shut off video
    movestat move(float,float);  // move the munition for the next frame
    void launcheffect();         //fx when droping a bomb, none
    virtual void sendfire(int,VEHICLE *,int, PASS_DATA *pdata, 
                  float, int target = -1);
    // determine if this munition hit anything
    movestat impactdetermination(pfVec3, pfVec3);
};
#endif // NOSUB
// This class added by Fred Lentz -- 
// TORPEDO_AL derived type, inherits methods and public attributes from
// MUNITION.  Represents air-launched torpedo.
class TORPEDO_AL : public MUNITION {

private:
    int video;                   // 0 for video torp off, 1 for on 
    float travelled_dist;        // torp will die at MAX_TORP_RANGE
    int is_underwater;           // 0 if above water, 1 if underwater
    int target_acquired;         // 0 if no target, 1 if target designated
    int target_id;               // id of current target vehicle
    int maritime_targets[250];   // array of eligible targets, all others 0

public:
    TORPEDO_AL(int mid);
    ~TORPEDO_AL() {};

    mtype gettype();
    void deactivate_torpedo ();  // kill torp and shut off video
    movestat move(float,float);  // move the munition for the next frame
    void launcheffect();         //fx when droping a bomb, none

    // determine if this munition hit anything
    movestat impactdetermination(pfVec3, pfVec3);
};


#endif
