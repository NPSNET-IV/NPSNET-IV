#include "formation.h"

formationClass::formationClass(int number) 
{
   nbr = number;
   type = J_LINE;
   temp_type = NOSIGNAL;
   bearing = new float[nbr];
   range = new float[nbr];
   heading =   goalHeading = 0.0f;
   spacing = goalSpacing = 1.0f;
   goalBearing = new float[nbr];
   goalRange = new float[nbr];

   for(int ix = 0; ix < number; ix++) {
      bearing[ix] = goalBearing[ix] = 90.0f;
      range[ix] = goalRange[ix] = 1.0f;
   }

   startTime = 0.0f;
   snapFormation(type);
   status = DONE;
   leadFlag = FALSE;

} // end formationClass::formationClass()


void formationClass::set_goal(int value)
{
   int ix;

   // assign goal parameters
   switch(value) {

      case COLUMN:
      case J_LINE:
      case ECHELON_RIGHT:
      case ECHELON_LEFT:
         float brg;
         switch(value) {
            case COLUMN:
               brg = 180.0f;
               break;
            case J_LINE:
               brg = 90.0f;
               break;
            case ECHELON_RIGHT:
               brg = 135.0f;
               break;
            case ECHELON_LEFT:
               brg = 225.0f;
               break;
         }

         for(ix = 0; ix < nbr; ix++) {
            goalBearing[ix] = brg;
            if(!ix) {
               goalRange[0] = goalSpacing;
            }
            else {
               goalRange[ix] = (ix+1) * goalRange[0];
            }
         }
         break;

      case WEDGE: // assumes 3/4 in team
         if(leadFlag) {
            goalBearing[0] = 90.0f;
            goalRange[0] = goalSpacing;
            goalBearing[1] = 120.0f;
            goalRange[1] = FOUR_DIV_SQRT3*goalRange[0];
         }
         else {
            goalBearing[0] = 45.0f;
            //goalRange[0] = goalSpacing * SQRT2;
            goalRange[0] = goalSpacing;
            goalBearing[1] = 60.0f;
            goalRange[1] = FOUR_DIV_SQRT3*goalRange[0];
            goalBearing[2] = 90.0f; 
            goalRange[2] = 3.0f*goalRange[0];
         }
         break;

      case(VEE): // assumes 3/4 in team
         if(leadFlag) {
            goalBearing[0] = 90.0f;
            goalRange[0] = goalSpacing;
            goalBearing[1] = 60.0f;
            goalRange[1] = FOUR_DIV_SQRT3*goalRange[0];
         }
         else {
            goalBearing[0] = 135.0f;
         //   goalRange[0] = goalSpacing * SQRT2;
            goalRange[0] = goalSpacing;
            goalBearing[1] = 120.0f;
            goalRange[1] = FOUR_DIV_SQRT3*goalRange[0];
            goalBearing[2] = 90.0f;
            goalRange[2] = 3.0f*goalRange[0];
         }
         break;

      // Spacing signals
      case CLOSE_UP:
         if(spacing >= 2.0f) { 
            goalSpacing = spacing - 1.0f;
         }
         break;

      case OPEN_UP:
         goalSpacing = spacing + 1.0f;
         break;

      case DISPERSE:
         // needs to be done still
         break;

      case MOVE_RIGHT:
         goalHeading = heading - 90.0f;
         if(goalHeading < 0.0f) {
            goalHeading += 360.0f;
         }
printf("setting goal for MOVE_RIGHT\n");
         break;

      case MOVE_LEFT:
         goalHeading = heading + 90.0f;
         if(goalHeading > 360.0f) {
            goalHeading -= 360.0f;
         }
         break;


      default:
         break;

   }
} // formationClass::set_goal()


void formationClass::set_position(int member, float brg, float rng, 
   float )
{  
   bearing[member] = brg; 
   range[member] = rng; 
} // end formationClass::set_position()


void formationClass::get_position(int member, float& brg, 
   float& rng, float& )
{
   brg = bearing[member]; 
   rng = range[member]; 
} // end formationClass::get_position()



int formationClass::snapFormation(int value)
{
   set_goal(value);
   if((signalType(value) == SPACING_SIGNAL)  
      || (signalType(value) == MOVEMENT_SIGNAL)) {
      set_goal(type);
   }
   temp_type = value;

   if((signalType(value) != SPACING_SIGNAL)  
      && (signalType(value) != MOVEMENT_SIGNAL)) {
      type = value;
   } 

   switch(type) {

      case (COLUMN):
      case(J_LINE):
      case(WEDGE):
      case(VEE):
      case(ECHELON_RIGHT):
      case(ECHELON_LEFT):
         spacing = goalSpacing;
         heading = goalHeading;
         for(int ix = 0; ix < nbr; ix++) {
            bearing[ix] = goalBearing[ix];
            range[ix] = goalRange[ix];
         }
         break;

      case MOVE_LEFT:
      case MOVE_RIGHT:
         heading = goalHeading;
         break;

      default:
         cerr << "unexpected napFormation() value\n";
         break;
   }
   // end switch 

   return(status = DONE);

} // formationClass::snapFormation()


int formationClass::setFormation(int value)
{
   int ix, done = TRUE;

   if(status == DONE) {
printf("GOAL set\n");
      set_goal(value);
      status = INPROGRESS;

      if(signalType(value) == SPACING_SIGNAL) {
         set_goal(type);
      }
      if((signalType(value) == SPACING_SIGNAL) || 
        (signalType(value) == MOVEMENT_SIGNAL)) {
         temp_type = value;
      }

      if((signalType(value) != SPACING_SIGNAL) 
         && (signalType(value != MOVEMENT_SIGNAL))) {
         type = value;
      }
   }

   switch(type) {

      case (COLUMN):
      case(J_LINE):
      case(WEDGE):
      case(VEE):
      case(ECHELON_RIGHT):
      case(ECHELON_LEFT):
         for(ix = 0; ix < nbr; ix++) {
            bearing[ix] += (goalBearing[ix] - bearing[ix])*0.1f;
            if((done) 
               && (fabs(goalBearing[ix] - bearing[ix]) > BEARING_TOL)) {
               done = FALSE;
            }
            range[ix] += (goalRange[ix] - range[ix])*0.1f;
            if((done) 
               && (fabs(goalRange[ix] - range[ix]) > RANGE_TOL)) {
               done = FALSE;
            }
         }
         if((temp_type == MOVE_LEFT) || (temp_type == MOVE_RIGHT)) {
            heading += (goalHeading - heading > 0.0f) ? 1.0f : -1.0f;
            if(heading < 0.0f) {
               heading += 360.0f;
            }
            else if(heading > 360.0f) {
               heading -= 360.0f;
            }
printf("setting heading %f %f\n", heading,goalHeading);
            if((done)
               && (fabs(heading - goalHeading) > 2.0f)) {
               done = FALSE;
printf("heading diff > 2.0f\n");
            }

         }
         
         break;

      default:
         cerr << "unexpected setFormation() value\n";
         break;

   } // end switch

   if(done) {
      for(ix = 0; ix < nbr; ix++) {
         bearing[ix] =  goalBearing[ix];
         range[ix] =  goalRange[ix];
      }
      spacing = goalSpacing;
printf("formation status set to DONE\n");
      status = DONE;
   }
   else {
      status = INPROGRESS;
   }

   return(status);

} // end formationClass::snapFormation()



