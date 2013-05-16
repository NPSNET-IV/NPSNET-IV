#include "signals.h"
#include "bstring.h"


int signalType(int index)
{

   switch(index)
   {
      // Formation signals
      case COLUMN:
      case J_LINE:
      case WEDGE:
      case VEE:
      case ECHELON_RIGHT:
      case ECHELON_LEFT:
         return(FORMATION_SIGNAL);
         // no break

      // Unit signals
      case FIRETEAM:
      case SQUAD:
      case PLATOON:
         return(UNIT_SIGNAL);
         // no break

      // Spacing signals
      case CLOSE_UP:
      case OPEN_UP:
      case DISPERSE:
         return(SPACING_SIGNAL);
         // no break

      // Movement signals
      case J_FORWARD:
      case HALT:
      case J_DOWN:
      case SPEED:
      case SLOW:
      case MOVE_RIGHT:
      case MOVE_LEFT:
         return(MOVEMENT_SIGNAL);
         // no break

      case POINT_TO:
      case SALUTE:
      case LEADER_SALUTE:
      case FLAG_HOLD:
      case FLAG_SALUTE:
      case EYES_RIGHT:
      case EYES_STRAIGHT:
      case MEDIC_ARMS_UPRIGHT:
      case MEDIC_ARMS_KNEELING:
      case WALK_NO_WEAPON:
      case RUN_NO_WEAPON:
      case WALK_WEAPON_STOW:
      case WALK_WEAPON_DEPLOY:
      case RUN_WEAPON_DEPLOY:
      case SHOULDER_ARMS:
      case CROUCH:
      case FOXHOLE_OPEN:
      case PRONE_FIRING:
         return(MISC_SIGNAL);
         // no break

      default:
         //cerr << "signalType(): Unk index " << index << endl;
         return(NOSIGNAL);
         // no break
   }

} // end signalType()


signalClass::signalClass()
{
   index = NOSIGNAL;
   status = DONE;
   start_time = 0.0f;
   pdu_sent = TRUE;
   queued = 0;
} // end signalClass::SignalClass()


void signalClass::set_signal(int signal, JointMods *ujm, int handSignalFlag)
{
   if((handSignalFlag) && (ujm)) {
      if((!queued) &&  
         ((ujm->get_status() == DONE) || (!ujm->isTemporal()))) {
         ujm->set_index(signal);
         ujm->set_status(DONE);
         index = signal;
         status = DONE;
      }
      else if(queued < MAX_SIGNALS) {
         queued++;
         queuedSignals[queued-1] = signal;
      }
   }
   else  {
      index = signal;
      status = DONE;
   }

} // end signalClass::set_signal()


int signalClass::check_signal(JointMods *ujm, int handSignalFlag)
{
   if(handSignalFlag) { // execute a hand signal
      if((queued) 
         && (ujm->get_status() == DONE) && (ujm->isTemporal())) {
         ujm->set_index(index=queuedSignals[0]);
         ujm->set_status(DONE);
         pdu_sent = FALSE;  // set flag to send DIS packet
         for(int ix = 1; ix < queued; ix++) {
            if(queuedSignals[ix] != NOSIGNAL) {
               queuedSignals[ix-1] = queuedSignals[ix];
            }
         }

         queued--;
         queuedSignals[queued] = NOSIGNAL;      // reset slot to NOSIGNAL
         status = INPROGRESS;
      }
   }
   else {
      if((index) && (status != INPROGRESS)) {
         start_time = pfGetTime();
         status = INPROGRESS;
      }
      else if((status == INPROGRESS) && (pfGetTime() - start_time > 2.0f)) {
         //reset();
      }
   }
   return (status);

} // end handSignal::check_signal()

