
#ifndef __NPS_DISPLAY_TYPES__
#define __NPS_DISPLAY_TYPES__

#include <pf.h>
#include "common_defs.h"
#include "display_const.h"
#include "input_manager.h"

typedef struct
   {
   pfCoord posture;
   pfCoord look;
   int type;
   float speed;
   float altitude;
   float thrust;
   int eyeview;
   HUD_VEH_REC hud_data[HUD_MAX_VEH];
   float rng_fltr;
   NPS_Input_Device input_control;
   int input_number;
   int texture;
   int wireframe;
   int antialias;
   int printscreen;
   } PASS_DATA;

typedef struct
   {
   int          veh_index;
   pfCoord      veh_posture;
   int          veh_type_index;
   vtype        veh_class_type;
   float        veh_speed;
   unsigned char veh_force;
   EntityID     veh_entity_id;
   EntityType   veh_entity_type;
   EntityType   matched_entity_type;
   char         matched_model_name[255];
   EntityMarking veh_marking;
   unsigned int veh_appearance;
   sender_info  veh_controller;
   } INFO_PDATA;

#endif

