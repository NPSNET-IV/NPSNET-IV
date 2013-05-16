#include "jointmods.h"


void JointMods::set_formationSignal() 
{

   float curr_time, time_diff;

   if(status == START) {
      start_time = (curr_time = (float)pfGetTime());
      time_diff = 0.0f;
      //cerr << "Setting FORMATION hand motion " << motion_index << endl;
   }
   else {
      curr_time =  (float)pfGetTime();
      time_diff = curr_time - start_time;
   }
   status = INPROGRESS;

   switch (motion_index) {

      case COLUMN:      
         if((status != DONE) && (time_diff <=  3.5f)) {
            //cerr << "*** Setting COLUMN\n";
            override_part(RIGHTARM);

            // Set Right arm Shoulder values
            if((time_diff < 1.5f) || (time_diff > 2.3f)) {
               upperJointSet[URIGHT_SHOULDER].set_value(0, 90.0f*DTOR);
            }
            else {
               upperJointSet[URIGHT_SHOULDER].set_value(0, 150.0f*DTOR);
            }
            upperJointSet[URIGHT_SHOULDER].set_value(2, 
               360.0f*DTOR*time_diff*0.3333333f);
         }

         if(time_diff > 3.5f) {
            reset();
         }
         break;

      case J_LINE:  
         //cerr << "*** Setting J_LINE\n";
         if((status != DONE) && (time_diff <=  2.0f)) {

            override_part(RIGHTARM);
            override_part(LEFTARM);

            // Set Right arm Shoulder values
            upperJointSet[URIGHT_SHOULDER].set_value(1, 90.0f*DTOR);

            // Set Left arm Shoulder values
            upperJointSet[ULEFT_SHOULDER].set_value(1,90.0f*DTOR);
         }

         if(time_diff > 2.0f) {
            reset();
         }
         break;

      case WEDGE:
         //cerr << "*** Setting WEDGE\n";
         if((status != DONE) && (time_diff <=  2.0f)) {

            override_part(RIGHTARM);
            override_part(LEFTARM);

            // Set Right arm Shoulder values
            upperJointSet[URIGHT_SHOULDER].set_value(0,-100.0f*DTOR);
            upperJointSet[URIGHT_SHOULDER].set_value(1, 45.0f*DTOR);

            // Set Left arm Shoulder values
            upperJointSet[ULEFT_SHOULDER].set_value(0, -100.0f*DTOR);
            upperJointSet[ULEFT_SHOULDER].set_value(1,  45.0f*DTOR);
         }

         if(time_diff > 2.0f) {
            reset();
         }
         break;

      case VEE:
         //cerr << "*** Setting VEE\n";
         if((status != DONE) && (time_diff <=  2.0f)) {

            override_part(RIGHTARM);
            override_part(LEFTARM);

            // Set Right arm Shoulder values
            upperJointSet[URIGHT_SHOULDER].set_value(0, -100.0f*DTOR);
            upperJointSet[URIGHT_SHOULDER].set_value(1,  135.0f*DTOR);

            // Set Left arm Shoulder values
            upperJointSet[ULEFT_SHOULDER].set_value(0,  -100.0f*DTOR);
            upperJointSet[ULEFT_SHOULDER].set_value(1, 135.0f*DTOR);
         }

         if(time_diff > 2.0f) {
            reset();
         }
         break;

      case ECHELON_RIGHT:       // echelon right
         //cerr << "*** Setting ECHELON_RIGHT\n";
         if((status != DONE) && (time_diff <=  2.0f)) {

            override_part(RIGHTARM);
            override_part(LEFTARM);

            // Set Right arm Shoulder values
            upperJointSet[URIGHT_SHOULDER].set_value(0, -100.0f*DTOR);
            upperJointSet[URIGHT_SHOULDER].set_value(1, 45.0f*DTOR);

            // Set Left arm Shoulder values
            upperJointSet[ULEFT_SHOULDER].set_value(0, -100.0f*DTOR);
            upperJointSet[ULEFT_SHOULDER].set_value(1, 135.0f*DTOR);

         }

         if(time_diff > 2.0f) {
            reset();
         }
         break;

      case ECHELON_LEFT:        // echelon left
         //cerr << "*** Setting ECHELON_LEFT\n";
         if((status != DONE) && (time_diff <=  2.0f)) {

            override_part(RIGHTARM);
            override_part(LEFTARM);

            // Set Left arm Shoulder values
            upperJointSet[ULEFT_SHOULDER].set_value(0, -100.0f*DTOR);
            upperJointSet[ULEFT_SHOULDER].set_value(1,  45.0f*DTOR);

            // Set Right arm Shoulder values
            upperJointSet[URIGHT_SHOULDER].set_value(0, -100.0f*DTOR);
            upperJointSet[URIGHT_SHOULDER].set_value(1, 135.0f*DTOR);
         }

         if(time_diff > 2.0f) {
            reset();
         }
         break;

      default:
         cerr << "unknown Formation signal index " << motion_index << endl;
         status = DONE;
         break;

   } // end switch

} // end JointMods::set_formationSignal()


void JointMods::set_unitSignal()
{
   float curr_time, time_diff;

   if(status == START) {
      start_time = (curr_time = (float)pfGetTime());
      time_diff = 0.0f;
   }
   else {
      curr_time =  (float)pfGetTime();
      time_diff = curr_time - start_time;
   }
   status = INPROGRESS;

   switch (motion_index) {

      case FIRETEAM:
         // cerr << "*** Setting FIRETEAM\n";
         if((status != DONE) && (time_diff <=  2.0f)) {

            override_part(RIGHTARM);
            override_part(LEFTARM);

            // Set Right Elbow/Shoulder values
            upperJointSet[URIGHT_ELBOW].set_value(0, 130.0f*DTOR);
            upperJointSet[URIGHT_SHOULDER].set_value(2, 30.0f*DTOR);
            upperJointSet[URIGHT_SHOULDER].set_value(0,  55.0f*DTOR); 
         
            // Do small rotation to current left elbow value
            // Channel set data says left elbow normally at 120.9 deg
            upperJointSet[ULEFT_ELBOW].set_value(0, 110.0f*DTOR);
         }

         if(time_diff > 2.0f) {
            reset();
         }
         break;

      case SQUAD:
         //cerr << "*** Setting SQUAD\n";
         if((status != DONE) && (time_diff <=  3.0f)) {
            override_part(RIGHTARM);

            // Set Right arm Shoulder values
            upperJointSet[URIGHT_SHOULDER].set_value(0, 90.0f*DTOR); 
            upperJointSet[URIGHT_SHOULDER].set_value(2, 90.0f*DTOR);

            if(((time_diff > 0.4f) && (time_diff < 0.7f)) || 
               ((time_diff > 1.0f) && (time_diff < 1.3f)) || 
               ((time_diff > 1.6f) && (time_diff < 1.9f)) || 
               ((time_diff > 2.2f) && (time_diff < 2.5))) {
           
               upperJointSet[URIGHT_WRIST].set_value(1, 69.0f*DTOR);
            }
            else {
               upperJointSet[URIGHT_WRIST].set_value(1, -30.0f*DTOR);   
            } 
         }

         if(time_diff > 3.0f) {
            reset();
         }
         break;
        
      case PLATOON:
         //cerr << "*** Setting PLATOON\n";
         if((status != DONE) && (time_diff <=  4.0f)) {

            override_part(RIGHTARM);
            override_part(LEFTARM);

            // Set Right arm Shoulder values

            if(time_diff <= 0.5f) {
               upperJointSet[URIGHT_SHOULDER].set_value(1, 180.0f*DTOR);
               upperJointSet[ULEFT_SHOULDER].set_value(1, 180.0f*DTOR);
 
               upperJointSet[ULEFT_SHOULDER].set_value(0, -90.0f*DTOR);
               upperJointSet[URIGHT_SHOULDER].set_value(0, -90.0f*DTOR);
            }
            else if((time_diff > 0.5f) && (time_diff <= 3.0f)) {  

               upperJointSet[URIGHT_SHOULDER].set_value(1, 
                  180.0f*(1.0f-time_diff/3.0f)*DTOR);
               upperJointSet[ULEFT_SHOULDER].set_value(1, 
                  180.0f*(1.0f-time_diff/3.0f)*DTOR);

               if(time_diff > 2.0f) {
                  upperJointSet[URIGHT_SHOULDER].set_value(0, 
                     90.0f*DTOR*(time_diff-2.0f)/1.0f);
                  upperJointSet[ULEFT_SHOULDER].set_value(0, 
                     90.0f*DTOR*(time_diff-2.0f)/1.0f);               
               }
               else {
                  upperJointSet[URIGHT_SHOULDER].set_value(0,
                     -90.0f*(1.0f-time_diff/2.0f)*DTOR);
                  upperJointSet[ULEFT_SHOULDER].set_value(0,
                     -90.0f*(1.0f-time_diff/2.0f)*DTOR);
               }
            }
            else {
               upperJointSet[URIGHT_SHOULDER].set_value(1, 0.0f*DTOR);
               upperJointSet[URIGHT_SHOULDER].set_value(0, 90.0f*DTOR);
               upperJointSet[ULEFT_SHOULDER].set_value(1, 0.0f*DTOR);
               upperJointSet[ULEFT_SHOULDER].set_value(0, 90.0f*DTOR);
            }
         }

         if(time_diff > 4.0f) {
            reset();
         }
         break;

      default:
         cerr << "unknown Unit signal " << motion_index << endl;
            reset();
         break;

   } // end switch

} // end JointMods::set_unitSignal()


void JointMods::set_spacingSignal()
{
   float curr_time, time_diff;

   if(status == START) {
      start_time = (curr_time = (float)pfGetTime());
      time_diff = 0.0f;
      //cerr << "Setting SPACING hand motion " << motion_index << endl;
   }
   else {
      curr_time =  (float)pfGetTime();
      time_diff = curr_time - start_time;
   }
   status = INPROGRESS;

   switch (motion_index) {

      case CLOSE_UP:
         //cerr << "*** Setting CLOSE_UP\n";
         if((status != DONE) && (time_diff <=  2.0f)) {

            override_part(RIGHTARM);
            override_part(LEFTARM);

            // Set start Left elbow values
            upperJointSet[ULEFT_ELBOW].set_value(0,  90.0f*DTOR);
            // Set start Right elbow values
            upperJointSet[URIGHT_ELBOW].set_value(0,  90.0f*DTOR);

            //movement for close spacing
            if(time_diff <= 0.5f) {
               upperJointSet[URIGHT_SHOULDER].set_value(0,-20.0f*DTOR);
               upperJointSet[ULEFT_SHOULDER].set_value(0,-20.0f*DTOR);
            }
            else if((time_diff > 0.5f) && (time_diff < 1.0f)) {
               upperJointSet[URIGHT_SHOULDER].set_value(0, 
                  (-20.0f + 40.0f*time_diff)*DTOR);
               upperJointSet[ULEFT_SHOULDER].set_value(0, 
                  (-20.0f + 40.0f*time_diff)*DTOR);
            }
            else {
              upperJointSet[URIGHT_SHOULDER].set_value(0, 20.0f*DTOR);
              upperJointSet[ULEFT_SHOULDER].set_value(0, 20.0f*DTOR);
            }

         }

         if(time_diff > 2.0f) {
            reset();
         }
         break;

      case OPEN_UP:
         //cerr << "*** Setting OPEN_UP\n";
         if((status != DONE) && (time_diff <=  2.0f)) {

            override_part(RIGHTARM);
            override_part(LEFTARM);

            // Set start values
            upperJointSet[ULEFT_ELBOW].set_value(0, 90.0f*DTOR);
            upperJointSet[URIGHT_ELBOW].set_value(0, 90.0f*DTOR);
  
            if (time_diff <= 0.5f) {
               upperJointSet[URIGHT_SHOULDER].set_value(0, 30.0f*DTOR);
               upperJointSet[ULEFT_SHOULDER].set_value(0, 30.0f*DTOR);
            }

            //movement for open spacing
            else if((time_diff > 0.5f) && (time_diff < 1.0f)) {
               upperJointSet[URIGHT_SHOULDER].set_value(0, 
                   (30.0f - 50.0f*time_diff/1.0f)*DTOR);
               upperJointSet[ULEFT_SHOULDER].set_value(0, 
                   (30.0f - 50.0f*time_diff/1.0f)*DTOR);
            }
            else {
               upperJointSet[URIGHT_SHOULDER].set_value(0, -20.0f*DTOR);
               upperJointSet[ULEFT_SHOULDER].set_value(0, -20.0f*DTOR);
            }
         }

         if(time_diff > 2.0f) {
            reset();
         }
         break;

      case DISPERSE:
         //cerr << "*** Setting DISPERSE\n";
         if((status != DONE) && (time_diff <=  2.0f)) {

            override_part(RIGHTARM);

            // Set Right arm Shoulder values
            upperJointSet[URIGHT_SHOULDER].set_value(0, 90.0f*DTOR);

            if(time_diff <= 0.5f) {
               upperJointSet[URIGHT_SHOULDER].set_value(2,180.0f*DTOR);
            } 
            //movement of arm over time
            else if((time_diff > 0.5f) && (time_diff <= 1.0f)) {
               upperJointSet[URIGHT_SHOULDER].set_value(2, 
                  (180.0f-180.0f*time_diff/1.0f)*DTOR);
            }
            else {
               upperJointSet[URIGHT_SHOULDER].set_value(2, 0.0f*DTOR);
            }
         }

         if(time_diff > 2.0f) {
            reset();
         }
         break;

      default:
         cerr << "unknown Spacing signal " << motion_index << endl;
            reset();
         break;

   } // end switch

} // end JointMods::set_spacingSignal()


void JointMods::set_movementSignal()
{
   float curr_time, time_diff;

   if(status == START) {
      start_time = (curr_time = (float)pfGetTime());
      time_diff = 0.0f;
      //cerr << "Setting MOVEMENT hand motion " << motion_index << endl; 
   }
   else {
      curr_time =  (float)pfGetTime();
      time_diff = curr_time - start_time;
   }
   status = INPROGRESS;

   switch (motion_index) {

      case J_FORWARD:   
         //cerr << "*** Setting J_FORWARD\n";
         if((status != DONE) && (time_diff <=  2.0f)) {

            override_part(RIGHTARM);
         
            //keep this position throughout motion
            upperJointSet[URIGHT_SHOULDER].set_value(0,  90.0f*DTOR);

             if (time_diff <= 0.5f) {
                // Set Right arm Shoulder values
                upperJointSet[URIGHT_SHOULDER].set_value(0, 50.0f*DTOR);
                upperJointSet[URIGHT_SHOULDER].set_value(2, 270.0f*DTOR);
             }        

             //movement of arm over time
             else if((time_diff > 0.5f) && (time_diff < 1.0f)) {
                upperJointSet[URIGHT_SHOULDER].set_value(2, 
                   (270.0f - 180.0f*time_diff)*DTOR);
                upperJointSet[URIGHT_SHOULDER].set_value(1,
                   45.0f*(1.0f-time_diff)*DTOR);
                upperJointSet[URIGHT_SHOULDER].set_value(0, 
                   (50.0f + 40.0f*time_diff)*DTOR);
             }
             else {
                upperJointSet[URIGHT_SHOULDER].set_value(2, 90.0f*DTOR);
             }
         }

         if(time_diff > 2.0f) {
            reset();
         }

         break;

      case HALT: 
         //cerr << "*** Setting MOVEMENT_HALT\n";
         if((status != DONE) && (time_diff <=  2.0f)) {

            override_part(RIGHTARM);

            // Set Right arm Shoulder values
            upperJointSet[URIGHT_SHOULDER].set_value(0, 90.0f*DTOR);
            upperJointSet[URIGHT_SHOULDER].set_value(2, 180.0f*DTOR);
         }

         if(time_diff > 2.0f) {
            reset();
         }

         break;

      case J_DOWN:    
         //cerr << "*** Setting J_DOWN\n";
         if((status != DONE) && (time_diff <=  1.5f)) {

            override_part(RIGHTARM);

            upperJointSet[URIGHT_SHOULDER].set_value(1, 135.0f*DTOR);

            //movement of arm over time
            if(time_diff <= 0.05f) {
               upperJointSet[URIGHT_SHOULDER].set_value(1, 135.0f*DTOR);
            }
            else if(time_diff < 1.0f) {
               upperJointSet[URIGHT_SHOULDER].set_value(1, 
                 (135.0f*DTOR*(1.0f-time_diff)));
            }
            else {
               upperJointSet[URIGHT_SHOULDER].set_value(1, 0.0f);
            }
         }

         if(time_diff > 1.5f) {
            reset();
         }

         break;

      case SPEED:  
         //cerr << "*** Setting SPEED\n";
         if((status != DONE) && (time_diff <=  6.0f)) {

            override_part(RIGHTARM);
           
            //this could be improved, I dont like how he finishes with hand up
            if((time_diff > 0.5f) && (time_diff < 1.0f)|| (time_diff > 2.0f)
               && (time_diff < 2.5f) || (time_diff > 3.0f) && (time_diff <
               3.5f) || (time_diff >4.0f) && (time_diff < 4.5f)) {
               upperJointSet[URIGHT_ELBOW].set_value(0, 180.0f*DTOR);
            }
            else  {
               upperJointSet[URIGHT_SHOULDER].set_value(2, 180.0f*DTOR);
            }
         }
        
         if(time_diff > 6.0f) {
            reset();
         }

         break;

      case SLOW:   
         //cerr << "*** Setting SLOW\n";
         if((status != DONE) && (time_diff <=  5.0f)) {

            override_part(RIGHTARM);

            upperJointSet[URIGHT_SHOULDER].set_value(0, -90.0f*DTOR);

            if((time_diff > 0.5f) && (time_diff < 1.0f)|| (time_diff > 2.0f)
               && (time_diff < 2.5f) || (time_diff > 3.0f) && (time_diff <
               3.5f) || (time_diff >4.0f) && (time_diff <4.5f)) {
               upperJointSet[URIGHT_SHOULDER].set_value(1, 90.0f*DTOR);
            }
            else { 
               upperJointSet[URIGHT_SHOULDER].set_value(1, 70.0f*DTOR);
            }
         }
        
         if(time_diff > 5.0f) {
            reset();
         }

         break;

      case MOVE_RIGHT:  
         //cerr << "*** Setting MOVE_RIGHT\n";

         if((status != DONE) && (time_diff <=  2.0f)) {

            override_part(RIGHTARM);

            // Set Right arm Shoulder value
            upperJointSet[URIGHT_SHOULDER].set_value(1, 90.0f*DTOR);
         }

         if(time_diff > 2.0f) {
            reset();
         }
         break;

      case MOVE_LEFT:
         //cerr << "*** Setting MOVE_LEFT\n";
         if((status != DONE) && (time_diff <=  2.0f)) {

            override_part(LEFTARM);

            // Set Left arm Shoulder value
            upperJointSet[ULEFT_SHOULDER].set_value(1, 90.0f*DTOR);
         }

         if(time_diff > 2.0f) {
            reset();
         }
         break;

      default:
         cerr << "unknown Movement signal index " << motion_index << endl;
            reset();
         break;

   } // end switch

} // end JointMods::set_movementSigna()


void JointMods::set_miscSignal() {

   float curr_time, time_diff;

   if(status == START) {
      start_time = (curr_time = (float)pfGetTime());
      time_diff = 0.0f;
   }
   else {
      curr_time =  (float)pfGetTime();
      time_diff = curr_time - start_time;
   }
   status = INPROGRESS;

   switch (motion_index) {

      case POINT_TO:  //This is a motion which really does not exist
         //cerr << "*** Setting POINT_TO\n";
         
         if((status != DONE) 
            && (time_diff <=  2.0f)) {

            override_part(RIGHTARM);

            // Set Right arm Shoulder values
            upperJointSet[URIGHT_SHOULDER].set_value(2, 90.0f*DTOR);
         }

         if(time_diff > 2.0f) {
           reset();
         }

         break;

      case SALUTE:
         //cerr << "*** Setting SALUTE\n";
         if((status != DONE)
            && (time_diff <=  2.5f)) {

            override_part(RIGHTARM);

            // Set Right arm Shoulder values
            upperJointSet[URIGHT_SHOULDER].set_value(2, 90.0f*DTOR);
            upperJointSet[URIGHT_SHOULDER].set_value(1, 60.0f*DTOR);
            upperJointSet[URIGHT_SHOULDER].set_value(0, 40.0f*DTOR);
           
            upperJointSet[URIGHT_ELBOW].set_value(0, 130.0f*DTOR);
            upperJointSet[URIGHT_WRIST].set_value(2, -60.0f*DTOR);
         }

         if(time_diff > 2.0f) {
           reset();
         }

         break;

      case LEADER_SALUTE:
         //cerr << "*** Setting LEADER_SALUTE\n";
         if((status != DONE) && (time_diff <=  4.0f)) {

            override_part(RIGHTARM);
            override_part(LEFTARM);
            upperJointSet[UATLANTO_OCCIPITAL].set_flag(TRUE);

            // Set Right arm Shoulder values
            upperJointSet[URIGHT_SHOULDER].set_value(2, 90.0f*DTOR);
            upperJointSet[URIGHT_SHOULDER].set_value(1, 80.0f*DTOR);
            upperJointSet[URIGHT_SHOULDER].set_value(0, 40.0f*DTOR);

            upperJointSet[URIGHT_ELBOW].set_value(0, 130.0f*DTOR);
            upperJointSet[URIGHT_WRIST].set_value(2, -60.0f*DTOR);

            // Do eyes right too
            upperJointSet[UATLANTO_OCCIPITAL].set_value(0, -45.0f*DTOR);

         }

         if(time_diff > 3.0f) {
           status = DONE;
           motion_index = WALK_NO_WEAPON;
         }

         break;

      case FLAG_SALUTE:
         //cerr << "*** Setting FLAG_SALUTE\n";
         if((status != DONE) && (time_diff <=  4.0f)) {

            override_part(RIGHTARM);
            override_part(LEFTARM);

            // Left arm swing with walk
            upperJointSet[ULEFT_ELBOW].set_value(0, 10.0f*DTOR);

            // Right arm curls with flag straight ahead under arm
            upperJointSet[URIGHT_WRIST].set_value(2, -20.0f*DTOR);
            upperJointSet[URIGHT_WRIST].set_value(1, -10.0f*DTOR);
            upperJointSet[URIGHT_ELBOW].set_value(0, 95.0f*DTOR);
            upperJointSet[URIGHT_SHOULDER].set_value(0,-20.0f*DTOR);
         }

         if(time_diff > 3.0f) {
           status = DONE;
           motion_index = FLAG_HOLD;
         }

         break;

      case EYES_RIGHT:
         //cerr << "*** Setting EYES_RIGHT \n";
         if((status != DONE) && (time_diff <=  4.0f)) {

            override_part(RIGHTARM);
            override_part(LEFTARM);
            upperJointSet[UATLANTO_OCCIPITAL].set_flag(TRUE);

            // let left arm swing with walk
            upperJointSet[ULEFT_ELBOW].set_value(0, 10.0f*DTOR);

            // Right arm curls with flag straight ahead under arm
            upperJointSet[URIGHT_WRIST].set_value(2, -20.0f*DTOR);
            upperJointSet[URIGHT_WRIST].set_value(1, -10.0f*DTOR);
            upperJointSet[URIGHT_ELBOW].set_value(0, 85.0f*DTOR);
            upperJointSet[URIGHT_SHOULDER].set_value(0, -20.0f*DTOR);

            // move head
            upperJointSet[UATLANTO_OCCIPITAL].set_value(0, -45.0f*DTOR);

         }

         if(time_diff > 3.0f) {
           status = DONE;;
           motion_index = SHOULDER_ARMS;
         }

         break;

      case EYES_STRAIGHT:
         //cerr << "*** Setting EYES_STRAIGHT\n";
         if((status != DONE) && (time_diff <=  4.0f)) {

            override_part(RIGHTARM);
            override_part(LEFTARM);
            upperJointSet[UATLANTO_OCCIPITAL].set_flag(TRUE);

            // let left arm swing with walk
            upperJointSet[ULEFT_ELBOW].set_value(0, 10.0f*DTOR);

            // right arm is straight with a wrist turn to hold rifle upright
            upperJointSet[URIGHT_SHOULDER].set_value(1, -5.0f*DTOR);
            upperJointSet[URIGHT_WRIST].set_value(1, -10.0f*DTOR);
            upperJointSet[URIGHT_WRIST].set_value(2, -60.0f*DTOR);
            upperJointSet[URIGHT_ELBOW].set_value(0, 5.0f*DTOR);

            // move head
            upperJointSet[UATLANTO_OCCIPITAL].set_value(0, 0.0f*DTOR);
         }

         if(time_diff > 3.0f) {
           reset();
         }

         break;


      case MEDIC_ARMS_UPRIGHT:

         override_part(RIGHTARM);
         override_part(LEFTARM);

         upperJointSet[URIGHT_SHOULDER].set_value(1, 13.0f*DTOR);
         upperJointSet[ULEFT_SHOULDER].set_value(1, 23.0f*DTOR);
         upperJointSet[URIGHT_SHOULDER].set_value(2, 5.0f*DTOR);
         upperJointSet[ULEFT_SHOULDER].set_value(2, 5.0f*DTOR);
         break;

      case MEDIC_ARMS_KNEELING:

         override_part(RIGHTARM);
         override_part(LEFTARM);

         upperJointSet[ULEFT_SHOULDER].set_value(0, 20.0f*DTOR);
         upperJointSet[ULEFT_SHOULDER].set_value(1, 20.0f*DTOR);
         upperJointSet[ULEFT_SHOULDER].set_value(2, 10.0f*DTOR);


         upperJointSet[URIGHT_SHOULDER].set_value(0, 20.0f*DTOR);
         upperJointSet[URIGHT_SHOULDER].set_value(1, 22.0f*DTOR);
         upperJointSet[URIGHT_SHOULDER].set_value(2, 30.0f*DTOR);
         break;

      case WALK_NO_WEAPON: // this needs work

         override_part(RIGHTARM);
         override_part(LEFTARM);

         upperJointSet[ULEFT_ELBOW].set_value(0, 0.0f*DTOR);
         upperJointSet[URIGHT_ELBOW].set_value(0, 0.0f*DTOR);
         break;

      case RUN_NO_WEAPON: // this needs work

         override_part(RIGHTARM);
         override_part(LEFTARM);

         // let arms swing with walk
         upperJointSet[ULEFT_ELBOW].set_value(0, 70.0f*DTOR);
         upperJointSet[URIGHT_ELBOW].set_value(0, 70.0f*DTOR);
         break;


      case WALK_WEAPON_STOW:

         override_part(RIGHTARM);
         override_part(LEFTARM);

         upperJointSet[ULEFT_SHOULDER].set_value(0, -10.0f*DTOR);
         upperJointSet[ULEFT_SHOULDER].set_value(1, 5.0f*DTOR);
         upperJointSet[ULEFT_SHOULDER].set_value(2, 5.0f*DTOR);
         upperJointSet[ULEFT_ELBOW].set_value(0, 95.0f*DTOR);
         upperJointSet[ULEFT_WRIST].set_value(2,-80.0f*DTOR);

         upperJointSet[URIGHT_ELBOW].set_value(0, 100.0f*DTOR);
         upperJointSet[URIGHT_SHOULDER].set_value(0, 45.0f*DTOR);
         upperJointSet[URIGHT_SHOULDER].set_value(2, 5.0f*DTOR);
         break;

      case WALK_WEAPON_DEPLOY:

         override_part(RIGHTARM);
         override_part(LEFTARM);

         upperJointSet[ULEFT_SHOULDER].set_value(0, 30.0f*DTOR);
         upperJointSet[ULEFT_SHOULDER].set_value(1, -20.0f*DTOR);
         upperJointSet[ULEFT_SHOULDER].set_value(2, 40.0f*DTOR);
         upperJointSet[ULEFT_ELBOW].set_value(0, 50.0f*DTOR);

         upperJointSet[URIGHT_ELBOW].set_value(0, 100.0f*DTOR);
         upperJointSet[URIGHT_SHOULDER].set_value(1, 5.0f*DTOR);
         break;

      case RUN_WEAPON_DEPLOY:

         //cerr << "*** Setting RUN_WEAPON_DEPLOY\n";

         override_part(RIGHTARM);
         override_part(LEFTARM);

         upperJointSet[ULEFT_SHOULDER].set_value(0, 30.0f*DTOR);
         upperJointSet[ULEFT_SHOULDER].set_value(1, -20.0f*DTOR);
         upperJointSet[ULEFT_SHOULDER].set_value(2, 40.0f*DTOR);
         upperJointSet[ULEFT_ELBOW].set_value(0, 65.0f*DTOR);

         upperJointSet[URIGHT_ELBOW].set_value(0, 115.0f*DTOR);
         upperJointSet[URIGHT_SHOULDER].set_value(1, 5.0f*DTOR);
         break;

      case FLAG_HOLD:
         //cerr << "*** Setting FLAG_HOLD\n";

         override_part(RIGHTARM);
         override_part(LEFTARM);

         // let left arm swing with walk
         upperJointSet[ULEFT_ELBOW].set_value(0, 10.0f*DTOR);

         // right arm is straight with a wrist turn to hold flag upright
         upperJointSet[URIGHT_SHOULDER].set_value(1, -5.0f*DTOR);
         upperJointSet[URIGHT_WRIST].set_value(1, -10.0f*DTOR);
         upperJointSet[URIGHT_WRIST].set_value(2, -60.0f*DTOR);
         upperJointSet[URIGHT_ELBOW].set_value(0, 5.0f*DTOR);
         break;

      case SHOULDER_ARMS:
         //cerr << "*** Setting SHOULDER_ARMS\n";

         override_part(RIGHTARM);
         override_part(LEFTARM);

         // Left arm swing with walk
         upperJointSet[ULEFT_ELBOW].set_value(0, 10.0f*DTOR);

         // Right arm curls with flag straight ahead under arm
         upperJointSet[URIGHT_WRIST].set_value(2, -20.0f*DTOR);
         upperJointSet[URIGHT_WRIST].set_value(1, -10.0f*DTOR);
         upperJointSet[URIGHT_ELBOW].set_value(0, 85.0f*DTOR);
         upperJointSet[URIGHT_SHOULDER].set_value(0, -20.0f*DTOR);
         break;

      case PRONE_FIRING:
         // Reset because animation leaves hand off rifle 
         //cerr << "joint set for PRONE_FIRING\n";
         upperJointSet[ULEFT_WRIST].set_flag(TRUE);
         upperJointSet[ULEFT_WRIST].set_value(1, 20.0f*DTOR);
         upperJointSet[ULEFT_WRIST].set_value(2, 180.0f*DTOR);
         break;

      default:
         cerr << "unknown Misc signal index " << motion_index << endl;
         reset();
         break;

   } // end switch

} // end JointMods::set_miscSignal()

void
JointMods::set_override_both_arms (int flag)
{
   upperJointSet[URIGHT_CLAVICLE].set_flag(flag);
   upperJointSet[URIGHT_SHOULDER].set_flag(flag);
   upperJointSet[URIGHT_ELBOW].set_flag(flag);
   upperJointSet[URIGHT_WRIST].set_flag(flag);
   upperJointSet[ULEFT_CLAVICLE].set_flag(flag);
   upperJointSet[ULEFT_SHOULDER].set_flag(flag);                   
   upperJointSet[ULEFT_ELBOW].set_flag(flag);                      
   upperJointSet[ULEFT_WRIST].set_flag(flag);

}

void
JointMods::set_arm_joint_angle (int jointIndex, int angleIndex, 
                                float angleValue)
{
   upperJointSet[jointIndex].set_value(angleIndex, angleValue);
}

float
JointMods::get_arm_joint_angle (int jointIndex, int angleIndex)
{
   return upperJointSet[jointIndex].get_value(angleIndex);
}

void 
JointMods::override_part(int bodypart) {

   if(bodypart == RIGHTARM) {
      upperJointSet[URIGHT_CLAVICLE].set_flag(TRUE);
      upperJointSet[URIGHT_SHOULDER].set_flag(TRUE);
      upperJointSet[URIGHT_ELBOW].set_flag(TRUE);
      upperJointSet[URIGHT_WRIST].set_flag(TRUE);
   }
   else if(bodypart == LEFTARM) {
      upperJointSet[ULEFT_CLAVICLE].set_flag(TRUE);
      upperJointSet[ULEFT_SHOULDER].set_flag(TRUE);
      upperJointSet[ULEFT_ELBOW].set_flag(TRUE);
      upperJointSet[ULEFT_WRIST].set_flag(TRUE);
   }

} // end JointMods::override_part()


// Given a hand motion index, this function determines whether the
// rifle needs to be repositioned to where the right hand is located
int JointMods::move_rifle()
{
   int ans = FALSE;

   switch(motion_index) {
      case J_LINE:
      case WEDGE:
      case VEE:
      case ECHELON_RIGHT:
      case ECHELON_LEFT:
      case PLATOON:
      case CLOSE_UP:
      case OPEN_UP:
      case WALK_WEAPON_STOW:
      case WALK_WEAPON_DEPLOY:
      case RUN_WEAPON_DEPLOY:
      case SHOULDER_ARMS:
      case EYES_RIGHT:
      case EYES_STRAIGHT:
         ans = TRUE;
         break;

      default:
         ans = FALSE;
         break;
   }

   return(ans);
} // end JointMods::move_rifle()


// Given a hand motion index, this function determines the proper
// orientation for the rifle in its new position
void JointMods::orient_rifle(pfVec3 &orient)
{

   switch(motion_index)
   {
      case WEDGE:
      case VEE:
      case ECHELON_RIGHT:
      case ECHELON_LEFT:
      case PLATOON:
      case CLOSE_UP:
      case OPEN_UP:
      case WALK_WEAPON_STOW:
      case WALK_WEAPON_DEPLOY:
      case RUN_WEAPON_DEPLOY: 
         orient[HEADING] = (orient[PITCH] = 0.0f);
         orient[ROLL] = -80.0f;
         break;

      case FIRETEAM:    // check for error
         cerr << "orient_rifle(): should never be here for FIRETEAM motion " << endl;
         break;

      case J_LINE:
         orient[HEADING] = (orient[PITCH] = 0.0f);
         orient[ROLL] = -70.0f;
         break;

      case SHOULDER_ARMS:
      case EYES_RIGHT:
      case EYES_STRAIGHT:
         orient[HEADING] = -10.0f;
         orient[PITCH] = 180.0f;
         orient[ROLL] = -140.0f;
         break;

      default:
         //cerr << "orient_rifle(): unknown hand motion index " 
         //    << motion_index << endl;
         break;

   }
} // end JointMods::orient_rifle()


void JointMods::set_joint(float rot, float elev, int DRFlag)
// This function sets angles for head (only for DR entities),
// and arms (incl shoulder and clavicle) for both driven and
// DR entities based on index of hand motions passed in. 
// If motionIndex = NOSIGNAL, arm joints are not overridden.
{
     int i,j;

     for(i=0; i < UPPER_JOINTS; i++) {

        // i=0 (index = NECK) is neck - skip setting it for now
        if((i == UATLANTO_OCCIPITAL) && (DRFlag)) {
           // Set head values - only for DR entity
           upperJointSet[i].set_flag(TRUE);
           upperJointSet[i].set_value(0, -rot*DTOR);
           upperJointSet[i].set_value(1, -elev*DTOR); 
        }
        else {

           // init clavicle, shoulder, arm flags
           if(upperJointSet[i].get_flag() != TRUE) {
              upperJointSet[i].set_flag(FALSE); 

              // init clavicle joint values to known good values
              // from channel set data; rest are set to 0.0f
              for(j = 0; j<3; j++) {

                 if((i == RIGHT_CLAVICLE-JOINT_INDEX_OFFSET) 
                    || (i == RIGHT_CLAVICLE-JOINT_INDEX_OFFSET))
                    upperJointSet[i].set_value(j, 0.18f);
                 else
                    upperJointSet[i].set_value(j, 0.0f);
              }
           }
        }
     } // end for loop

     if(motion_index) {

        if((isTemporal()) && (status == DONE)) { 
           // Set other values for arms according to motion indices.
           status = START;
        }
        motion_type = signalType(motion_index);

        if(((isTemporal()) && (status != DONE)) || (!isTemporal())) { 

           switch(motion_type) {

              case FORMATION_SIGNAL:
                 set_formationSignal();
                 break;

              case UNIT_SIGNAL:
                 set_unitSignal();
                 break;

              case SPACING_SIGNAL:
                 set_spacingSignal();
                 break;

              case MOVEMENT_SIGNAL:
                 set_movementSignal();
                 break;

              case MISC_SIGNAL:
                 set_miscSignal();
                 break;
         
              default:
                 printf("unknown motion type %d\n", motion_type);
                 status = DONE;
                 break;
           }
        }
     
     } // end if

     return;

} // end JointMods::set_joint()

