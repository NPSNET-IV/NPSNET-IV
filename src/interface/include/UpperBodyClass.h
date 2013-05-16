// *********************************************************************
// File    : UpperBodyClass.h
// Author  : Scott McMillan
//         : Naval Postgraduate School
//         : Code CSMs
//         : Monterey, CA 93943
//         : mcmillan@cs.nps.navy.mil
// Project : NPSNET - Individual Combatants
// Created : 11 August 1995
// Summary : UpperBodyClass definitions
// Modified: 
// *********************************************************************

#ifndef __UPPERBODY_CLASS_H_
#define __UPPERBODY_CLASS_H_

#include <Performer/pf.h>

#include "input_device.h"
#include "RifleClass.h"
#include "ArmClass.h"
#include "FastrakClass.h"

//======================================================================

class UpperBodyClass : public input_device
{
private:
   int valid;

   FastrakClass *fastrak_unit;
   ArmClass *right_arm, *left_arm;
   RifleClass *rifle;

   FSTK_stations torso_sensor;
   FSTK_stations right_sensor;
   FSTK_stations  left_sensor;
   FSTK_stations rifle_sensor;

   // Fastrak related coordinate systems
   pfMatrix H_tx_to_ts, H_tx_to_rws, H_tx_to_lws, H_tx_to_rs;

   // Graphical model related coordinate systems
   pfMatrix H_uw_to_rw, H_uw_to_lw;
   pfMatrix H_hull_to_uw;

   void outputHMatrix(pfMatrix H_mat);

public:
   UpperBodyClass(const char *cfg_filename);
   ~UpperBodyClass();

   // functions that will override input_device virtual members
   int  exists() { return valid; }
   void get_all_inputs(pfChannel *);
   int  calibrate(const NPS_VALUATOR,
                  const NPS_CALIBRATION,
                  const float value = 0.0);

   int  set_multival(const NPS_MULTIVAL tag,
                     void *values);
   int  get_multival(const NPS_MULTIVAL tag,
                     void *values);
};

#endif
