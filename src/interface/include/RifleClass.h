// *********************************************************************
// File    : RifleClass.h
// Author  : Scott McMillan
//         : Naval Postgraduate School
//         : Code CSMs
//         : Monterey, CA 93943
//         : mcmillan@cs.nps.navy.mil
// Project : NPSNET - Individual Combatants
// Created : August 1995
// Summary : This file contains the definitions for a C++ class to
//         : manage the kinematics computations required to instrument
//         : the arms.
// Modified: 
// *********************************************************************

#ifndef __RIFLE_CLASS__
#define __RIFLE_CLASS__

#include <fstream.h>         // For C++ file I/O stuff
#include "Performer/pf.h"

//======================================================================
// RifleClass definitions
//======================================================================
class RifleClass
{
protected:
   int calibrated_flag;

   pfMatrix H_rs_to_r;       // rifle model wrt rifle sensor
   pfMatrix H_uw_to_ts;      // torso sensor wrt torso model

   pfVec3 p_r_to_ratt, p_r_to_latt;    // right and left hand attachment
   pfVec3 p_rs_to_ratt, p_rs_to_latt;  // positions wrt rifle and sensor CSs

   float arm_length;

   pfMatrix H_rs_to_npsrifle;// Matrix added to get firing aligned with
                             // NPSNET world coordinates in the
                             // treadport's WISE.
                             // WARNING: this assumes the emitter
                             // coordinate system is aligned with the
                             // hull DCS (x-right, y-forward, z-up).

public:
   RifleClass(ifstream &config_fileobj);
   ~RifleClass();

   int calibrate(pfMatrix H_tx_to_rs, pfMatrix H_tx_to_ts);

   void computeCoord(pfMatrix H_tx_to_rs,  pfMatrix H_tx_to_ts,
                     pfMatrix H_tx_to_rws, pfMatrix H_tx_to_lws,
                     pfMatrix H_uw_to_rw,  pfMatrix H_uw_to_lw,
                     pfCoord *coord);
   void computeTargetCoord(pfMatrix H_tx_to_rs,
                           pfMatrix H_tx_to_ts,
                           pfCoord *coord_hull_to_npsrifle);
};

#endif
