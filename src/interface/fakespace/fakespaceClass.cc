#include <iostream.h>     // for iostream class definition
#include <stdlib.h>       // for exit definition
#include <ulocks.h>       // for semaphore definitions
#include <bstring.h>      // for bcopy definition
#include <signal.h>       // for signal definitions and kill()
#include <string.h>
#include "fakespaceClass.h" 
#include <unistd.h>
#include <limits.h>

#ifndef NOFAKESPACE
#define NOGLCALLS
#include <boom.h>
static void BoomMatrixToPerformerMatrix(pfMatrix,  pfMatrix);
static boomrec *b;
#endif

fakespaceClass::fakespaceClass (char *tmpptr)
{
   fsInfoStruct *tmpInfo = (fsInfoStruct *)tmpptr;
   strncpy (tty, tmpInfo->tty, 32);
   strncpy (calfile, tmpInfo->calfile, 128);

   recv_buffer[FS_OUTBUFF].pos.xyz[PF_X] = 0.0f;
   recv_buffer[FS_OUTBUFF].pos.xyz[PF_Y] = 0.0f;
   recv_buffer[FS_OUTBUFF].pos.xyz[PF_Z] = 0.0f;
   recv_buffer[FS_OUTBUFF].pos.hpr[PF_H] = 0.0f;
   recv_buffer[FS_OUTBUFF].pos.hpr[PF_P] = 0.0f;
   recv_buffer[FS_OUTBUFF].pos.hpr[PF_R] = 0.0f;

#ifndef NOFAKESPACE
  if ( b = boom_open(tty, calfile,1) ) {
  cerr << "Opened Boom on " << tty
       << " with calfile " << calfile << endl;
  boom_setdatatype(b, BOOM_MATANGLES |
                      BOOM_EULER | BOOM_METERS | BOOM_BUTTONS);
  boom_opened_ok = 1;
  } 
  else {
     cerr << "DID NOT OPEN DEVICE ON " << tty
          << " WITH CALFILE " << calfile << endl;
     boom_close(b);
     boom_opened_ok = 0;
  }
#endif

}

fakespaceClass::~fakespaceClass ()
{
}   

void
fakespaceClass::get_all_inputs (pfChannel *)
{
#ifndef NOFAKESPACE
   pfMatrix   pf_boom_mat;

   if (b == NULL) {
      cerr << "Error opening Boom device." << endl;
      exit (0);
   }
   boom_read(b);
   BoomMatrixToPerformerMatrix(b->headmat, pf_boom_mat);


   pfGetOrthoMatCoord( pf_boom_mat, &(recv_buffer[FS_OUTBUFF].pos) );

  /*
cerr << "From fakespaceClass: "
     << recv_buffer[FS_OUTBUFF].pos.xyz[PF_X] << "  "
     << recv_buffer[FS_OUTBUFF].pos.xyz[PF_Y] << "  "
     << recv_buffer[FS_OUTBUFF].pos.xyz[PF_Z] << "  "
     << recv_buffer[FS_OUTBUFF].pos.hpr[PF_H] << "  "
     << recv_buffer[FS_OUTBUFF].pos.hpr[PF_P] << "  "
     << recv_buffer[FS_OUTBUFF].pos.hpr[PF_R] << endl;
cerr << "Unused: "
     << recv_buffer[FS_OUTBUFF]._unused.xyz[PF_X] << "  "
     << recv_buffer[FS_OUTBUFF]._unused.xyz[PF_Y] << "  "
     << recv_buffer[FS_OUTBUFF]._unused.xyz[PF_Z] << "  "
     << recv_buffer[FS_OUTBUFF]._unused.hpr[PF_H] << "  "
     << recv_buffer[FS_OUTBUFF]._unused.hpr[PF_P] << "  "
     << recv_buffer[FS_OUTBUFF]._unused.hpr[PF_R] << endl;
  */

   recv_buffer[FS_OUTBUFF].button1 = (boom_buttons(b) == BOOM_LBUT1);
   recv_buffer[FS_OUTBUFF].button2 = (boom_buttons(b) == BOOM_RBUT1);
#endif
}

int
fakespaceClass::button_pressed (const NPS_BUTTON button, int &value)
{
   int success = FALSE;

   if (button == NPS_FS_BUTTON_1) {
      success = TRUE;
      value = recv_buffer[FS_OUTBUFF].button1;
   }
   if (button == NPS_FS_BUTTON_2){
      success = TRUE;
      value = recv_buffer[FS_OUTBUFF].button2;
   }

   return success;
}

int
fakespaceClass::get_multival (const NPS_MULTIVAL multi_val, void *data)
{
   int success = FALSE;
   if (multi_val == NPS_MV_FS_COORD) {
      pfCoord *tmpCoord = (pfCoord *) data;
      pfCopyVec3 (tmpCoord->xyz, recv_buffer[FS_OUTBUFF].pos.xyz);
      pfCopyVec3 (tmpCoord->hpr, recv_buffer[FS_OUTBUFF].pos.hpr);
   }
   return success;
}

void
fakespaceClass::print_buffers ()
{
}


#ifndef NOFAKESPACE

/*
 * BoomMatrixToPerformerMatrix(pfMatrix boommat,  pfMatrix xfmat)
 * ---------------------------------------------------------------
 * The following code massages a Boom headmat
 * into the Performer coordinate system
 */
static void BoomMatrixToPerformerMatrix(pfMatrix boommat,  pfMatrix xfmat)
{
  
      xfmat[0][0] = boommat[0][0];
      xfmat[1][0] = -boommat[2][0];
      xfmat[2][0] = boommat[1][0];
      xfmat[3][0] = boommat[3][0];

      xfmat[0][1] = -boommat[0][2];
      xfmat[1][1] = boommat[2][2];
      xfmat[2][1] = -boommat[1][2];
      xfmat[3][1] = -boommat[3][2];

      xfmat[0][2] = boommat[0][1];
      xfmat[1][2] = -boommat[2][1];
      xfmat[2][2] = boommat[1][1];
      xfmat[3][2] = boommat[3][1];

      xfmat[0][3] = -boommat[0][2];
      xfmat[1][3] = boommat[2][2];
      xfmat[2][3] = -boommat[1][2];
      xfmat[3][3] = -boommat[3][2];
}

#endif
