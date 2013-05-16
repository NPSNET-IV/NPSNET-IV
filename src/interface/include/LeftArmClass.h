// *********************************************************************
// File    : LeftArmClass.h
// Author  : Scott McMillan
//         : Naval Postgraduate School
//         : Code CSMs
//         : Monterey, CA 93943
//         : mcmillan@cs.nps.navy.mil
// Project : NPSNET - Individual Combatants
// Created : August 1995
// Summary : This file contains the definitions for a C++ class to
//         : manage the kinematics computations required to instrument
//         : the left arm.
// Modified: 
// *********************************************************************

#ifndef __NPS_LEFTARM_CLASS__
#define __NPS_LEFTARM_CLASS__

//======================================================================
// LeftArmClass definitions
//======================================================================

class LeftArmClass : public ArmClass
{
private:

public:
   LeftArmClass();
   ~LeftArmClass();

   int calibrate(pfMatrix H_ts_to_sh);
   int calibrate(pfMatrix H_ws_side, pfMatrix H_ws_front,
                 pfMatrix H_ts_side, pfMatrix H_ts_front,
                 pfMatrix H_ts_to_sh,
                 pfVec3 p_shoulder);

   int inverseKinematics5(pfMatrix H_tx_to_ws,
                          pfMatrix H_tx_to_ts,
                          float angles[5]);
   int inverseKinematics5adj(pfMatrix H_tx_to_ws,
                             pfMatrix H_tx_to_ts,
                             float angles[5]);
};

#endif
