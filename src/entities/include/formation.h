#ifndef _FORMATION_H_
#define _FORMATION_H_

#include <math.h>
#include "jointmods.h"

const float BEARING_TOL = 0.5f;
const float RANGE_TOL = 0.2f;

const float SQRT2 = 1.4142136f;
const float SQRT3 = 1.7320508f;
const float SQRT5 = 2.236068f;
const float THREE_DIV_SQRT2 = 3.0f/SQRT2;
const float FOUR_DIV_SQRT3 = 4.0f/SQRT3;

class formationClass {

protected:
   int type;
   int temp_type;
   int status;
   int nbr;
   int leadFlag;
   int formationMode;
   float *bearing;
   float *range;
   float spacing;
   float heading;
   float *goalBearing;
   float *goalRange;
   float goalSpacing;
   float goalHeading;
   float startTime;

public:
   formationClass(int number);

   void set_goal(int value);  
   void set_status(int) {status = type;}
   void set_formationMode(int value) {formationMode = value;}
   void set_bearing(int member, float brg) {bearing[member] = brg; }
   void set_range(int member, float rng) {range[member] = rng;}
   void set_spacing(float spc) {spacing = spc;}
   void set_position(int member, float brg, float rng, float spc);
   void set_leadFlag(int value) {leadFlag = value;}

   void mod_bearing(int member, float brg) {bearing[member] += brg;}
   void mod_range(int member, float rng) {range[member] += rng;}
   void mod_spacing(float spc) {spacing += spc;}
   void mod_heading(float hdg) {heading += hdg;}

   int get_type() {return (type);}
   int get_status() {return (status);}
   int get_leadFlag() {return (leadFlag);}
   float get_bearing(int member) {return (bearing[member]);}
   float get_range(int member) {return (range[member]);}
   float get_spacing() {return (spacing);}
   float get_heading() {return (heading);}
   void get_position(int member, float& brg, float& rng, float& spc);

   int snapFormation(int value);
   int setFormation(int value);
};

#endif
