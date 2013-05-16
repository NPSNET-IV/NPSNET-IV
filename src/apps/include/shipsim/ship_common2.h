#include "shipglobals.h"

#ifndef _SHIP_COMMON2_H_
#define _SHIP_COMMON2_H_




typedef struct
{
  int shipID;
  int lighted;
  char name[20]; //Name of model file
  int numclones;
  EntityType distype;
  pfGroup * model;
  pfSwitch * mswitch;
  vtype cat;
} SHIPTYPE_LIST;


typedef struct
{
  float legTime;
  pfCoord initLegPosture;
  pfVec3 pbVelocity;
  float legSpeed;
  float pbPortRPM;
  float pbStbdRPM;
  float pbRudderAngle;
  float pbOmega;
  float pbDeltaTime;
} PLAYBACK_LIST;

typedef struct
 {
  VEHICLE* vehptr;
  vtype cat;
  int shipID;
  int type;
  int shipType;
  pfCoord startPos;
  EntityID DISid;
  int pbLeg;
  int addLeg;
  PLAYBACK_LIST pbList[MAXCOURSECHANGES];
  int first;
}SHIP_LIST;

typedef struct
{
  int id;
  int lighted;
  char name[100];
  pfGroup *model;
  pfSwitch *mSwitch;
} OBJECTTYPE_LIST;


typedef struct
{
  int id;
  pfSwitch *mSwitch;

} OBJECT_LIST;



#endif
