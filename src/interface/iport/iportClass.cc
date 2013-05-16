#include <iostream.h>     // for iostream class definition
#include <stdlib.h>       // for exit definition
#include <ulocks.h>       // for semaphore definitions
#include <bstring.h>      // for bcopy definition
#include <signal.h>       // for signal definitions and kill()

#include "iportClass.h"   // for enum definitions for devices
#include "pdu.h"          // for fixedDatumRecord definition
#include "jointmods.h"    // for joint enums and definitions

iportClass::iportClass ()
{
#ifdef NOPERFORMER
   handle = usinit ("/tmp/iport.share.arena");
#else
   handle = pfGetSemaArena ();
#endif // NOPERFORMER
   iport_send_lock = usnewlock (handle);
   iport_recv_lock = usnewlock (handle);
   signal ( SIGCLD, SIG_IGN );
   bzero ( (void *) &(recv_buffer[INBUFF]), sizeof (fromIport) );
   bzero ( (void *) &(recv_buffer[OUTBUFF]), sizeof (fromIport) );
   bzero ( (void *) &(send_buffer), sizeof (toIport) );
   send_buffer.normal[2] = 1.0f;
   child_pid = -1;
}

iportClass::~iportClass ()
{
   usfreelock (iport_send_lock, handle);
   usfreelock (iport_recv_lock, handle);
}   

void
iportClass::get_all_inputs (pfChannel *)
{
   ussetlock (iport_recv_lock);
   bcopy ( (void *) &(recv_buffer[INBUFF]), (void *) &(recv_buffer[OUTBUFF]),
          sizeof (fromIport) );
//   recv_buffer[INBUFF].trigger_press = FALSE;
   usunsetlock (iport_recv_lock);
//   print_buffers ();
}

int
iportClass::button_pressed (const NPS_BUTTON button, int &value)
{
   int success = FALSE;

   if (button == NPS_IPORT_TRIGGER) {
      value = recv_buffer[OUTBUFF].trigger_press;
      success = TRUE;
   }
   if (button == NPS_IPORT_TRIGGER_AUX) {
      value = recv_buffer[OUTBUFF].trigger_press_aux;
      success = TRUE;
   }

   return success;
}

int
iportClass::valuator_set ( const NPS_VALUATOR type, float &value)
{
   int success = FALSE;

   if (type == NPS_IPORT_TWIST) {
      value = recv_buffer[OUTBUFF].pos.hpr[0];
      success = TRUE;
   }

   return success;
}

int
iportClass::get_state (const NPS_STATE state_val, int &value)
{
   int success = FALSE;

   if (state_val == NPS_STATE_IPORT) {
      value = recv_buffer[OUTBUFF].flags;
      success = TRUE;
   }
   else {
      value = 0;
   }

   return success;
}

int
iportClass::get_multival (const NPS_MULTIVAL multi_val, void *data)
{
   int success = FALSE;

   if (multi_val == NPS_MV_IPORT_RECEIVE) {

      ussetlock (iport_recv_lock);
      bcopy ( (void *) &(recv_buffer[INBUFF]), (void *) data,
             sizeof(fromIport) );
      recv_buffer[INBUFF].trigger_press = FALSE;
      usunsetlock (iport_recv_lock);

      success = TRUE;
   }
   else if (multi_val == NPS_MV_IPORT_VELOCITY) {
      bcopy ( (void *) (recv_buffer[OUTBUFF].velocity),
              (void *) data,
              sizeof (pfVec3) );
      success = TRUE;
   }

   else if (multi_val == NPS_MV_IPORT_LOOK) {
      bcopy ( (void *) (recv_buffer[OUTBUFF].gaze),
              (void *) data,
              sizeof (pfVec3) );
      success = TRUE;
   }

   else if (multi_val == NPS_MV_IPORT_RIFLE) {
      bcopy ( (void *) &(recv_buffer[OUTBUFF].rifle),
              (void *) data,
              sizeof (pfCoord) );
      success = TRUE;
   }
  
   else if (multi_val == NPS_MV_UPPERBODY) {
      float *jlist = recv_buffer[OUTBUFF].jlist;
      fixedDatumRecord *ourList = (fixedDatumRecord *)data;
      ourList[LS0].datum = jlist[8];
      ourList[LS1].datum = jlist[6];
      ourList[LS2].datum = jlist[7];
      ourList[LW0].datum = 0.0f;
      ourList[LW1].datum = 0.0f;
      ourList[LW2].datum = 0.0f;
      ourList[LE0].datum = jlist[14];

      ourList[RS0].datum = jlist[12];
      ourList[RS1].datum = jlist[10];
      ourList[RS2].datum = jlist[11];
      ourList[RW0].datum = 0.0f;
      ourList[RW1].datum = 0.0f;
      ourList[RW2].datum = 0.0f;
      ourList[RE0].datum = jlist[16];

      for (int ix = 0; ix <= NUM_HIRES_VALUES - 1; ix++) {
          while (ourList[ix].datum >= 360.0f) {
             ourList[ix].datum -= 360.0f;
          }
          while (ourList[ix].datum < 0.0f) {
             ourList[ix].datum += 360.0f;
          }
          ourList[ix].datum *= DTOR;
      }

      // for (int junk = 0; junk <= 16; junk ++) {
      //    cerr << " ourList[" << junk << "] = " << ourList[junk].datum << endl;
      // }

      success = TRUE;
   }

   return success;
}

int
iportClass::set_multival (const NPS_MULTIVAL multi_val, void *data)
{
   int         success = FALSE;

   if (multi_val == NPS_MV_IPORT_SEND) {

      ussetlock (iport_send_lock);
      bcopy ( (void *) data, (void *) &send_buffer, sizeof(toIport) );
      usunsetlock (iport_send_lock);

      success = TRUE;
   }

   return success;
}

void
iportClass::print_buffers ()
{
  cerr << "-=-=-=-=- Receive Buffer INBUFF -=-=-=-=-" << endl
       << "Position:" << endl
       << "X: " << recv_buffer[INBUFF].pos.xyz[0] << "  "
       << "Y: " << recv_buffer[INBUFF].pos.xyz[1] << "  "
       << "Z: " << recv_buffer[INBUFF].pos.xyz[2] << endl
       << "H: " << recv_buffer[INBUFF].pos.hpr[0] << "  "
       << "P: " << recv_buffer[INBUFF].pos.hpr[1] << "  "
       << "R: " << recv_buffer[INBUFF].pos.hpr[2] << endl
       << "Velocity:" << endl
       << "X: " << recv_buffer[INBUFF].velocity[0] << "  "
       << "Y: " << recv_buffer[INBUFF].velocity[1] << "  "
       << "Z: " << recv_buffer[INBUFF].velocity[2] << endl
       << "Flags: " << recv_buffer[INBUFF].flags << endl << endl
       << "-=-=-=-=- Receive Buffer OUTBUFF -=-=-=-=-" << endl
       << "Position:" << endl
       << "X: " << recv_buffer[OUTBUFF].pos.xyz[0] << "  "
       << "Y: " << recv_buffer[OUTBUFF].pos.xyz[1] << "  "
       << "Z: " << recv_buffer[OUTBUFF].pos.xyz[2] << endl
       << "H: " << recv_buffer[OUTBUFF].pos.hpr[0] << "  "
       << "P: " << recv_buffer[OUTBUFF].pos.hpr[1] << "  "
       << "R: " << recv_buffer[OUTBUFF].pos.hpr[2] << endl
       << "Velocity:" << endl
       << "X: " << recv_buffer[OUTBUFF].velocity[0] << "  "
       << "Y: " << recv_buffer[OUTBUFF].velocity[1] << "  "
       << "Z: " << recv_buffer[OUTBUFF].velocity[2] << endl
       << "Flags: " << recv_buffer[INBUFF].flags << endl << endl
       << "-=-=-=-=-=-=-=- Send Buffer -=-=-=-=-=-=-=-" << endl
       << "Position:" << endl
       << "X: " << send_buffer.xyz[0] << "  "
       << "Y: " << send_buffer.xyz[1] << "  "
       << "Z: " << send_buffer.xyz[2] << endl
       << "Normal:" << endl
       << "X: " << send_buffer.normal[0] << "  "
       << "Y: " << send_buffer.normal[1] << "  "
       << "Z: " << send_buffer.normal[2] << endl 
       << "Flag: " << send_buffer.flags << endl
       << endl
       << "^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-"
       << endl << endl;
}


void
i_sig_handler(int sig, ...)
{
   if ((signal ( sig, SIG_IGN )) == SIG_ERR) {
      perror("iportClass:\tSignal Error -\n   ");
   }

   switch ( sig ) {
      case SIGTERM:
         exit(0);
         break;
      case SIGHUP:
         if ( getppid() == 1 ) {
            cerr << "DEATH NOTICE:" << endl;
            cerr << "\tParent process terminated."
                 << endl;
            cerr << "\tiportClass terminating to prevent orphan process."
                 << endl;
            exit(0);
         }
         break;
      case SIGINT:
         cerr << "DEATH NOTICE: iportClass exitting due to interrupt "
              << "signal." << endl;
         exit(0);
         break;
      case SIGQUIT:
         cerr << "DEATH NOTICE: iportClass exitting due to quit "
              << "signal." << endl;
         exit(0);
         break;
      default:
         cerr << "DEATH NOTICE: iportClass exitting due to signal:  "
              << sig << endl;
         exit(0);
         break;
   }
   signal ( sig, (void (*)(int))i_sig_handler );
}

