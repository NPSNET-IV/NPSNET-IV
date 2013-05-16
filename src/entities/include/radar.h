
#ifndef __NPS_RADAR_H__
#define __NPS_RADAR_H__

#include "common_defs.h"

typedef struct {
   float radar_centerX;
   float radar_centerY;
   float meters_per_pixelX;
   float meters_per_pixelY;
   float rot_amount;
   } radar_screen_info;


radar_screen_info *
draw_radar ( pfChannel *, pfVec3, float, float, float, HUD_VEH_REC *, int,
             float, float, float, float,
             int draw_rings = TRUE, int draw_ldmarks = TRUE,
             int draw_bbox = TRUE, int draw_veh_dir_ind = TRUE,
             int draw_veh_icons = TRUE,
             int use_veh_icons = TRUE, int elevation_coloring = FALSE,
             int forward_looking = TRUE, unsigned int alpha = 255,
             float viewportl = 0.0f, float viewportr = 1.0f,
             float viewportb = 0.0f, float viewportt = 1.0f );

#endif
