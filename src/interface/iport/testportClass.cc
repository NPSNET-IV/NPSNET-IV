#include <iostream.h>      // for iostream class definition
#include <stdlib.h>        // for exit definition
#include <ulocks.h>        // for semaphore definitions
#include <bstring.h>       // for bcopy definition
#include <sys/types.h>     // 
#include <sys/socket.h>    // for socket definitions
#include <sys/prctl.h>     // for sproc definition
#include <signal.h>        // for signal definitions and kill()
#include <unistd.h>        // for sysconf definition

// #include "input_device.h"  // for input_device class definition
#include "iportClass.h"    // for enum definitions for devices
#include "testportClass.h" // for testportClass definition
#include "ptpClass.h"      // for ptpClass difinition

void testport_update (void *tempThis);

testportClass::testportClass (const char *ptp_file) 
{
   long tck_per_sec;

   signal ( SIGCLD, SIG_IGN );
   tck_per_sec = sysconf (_SC_CLK_TCK);
   nap_interval = tck_per_sec / 30L;
   ptp = new ptpClass (ptp_file);
   child_pid = sproc (testport_update, PR_SALL, (void *) this);
   cerr << "\tTest Port pid: " << child_pid << endl;
}

testportClass::~testportClass()
{
    if (child_pid != -1) {
       kill (child_pid, SIGTERM);
    }

    delete ptp;
}

void 
testport_update (void *tempThis)
{
   static     void (*temp_signal)(int);

   testportClass *obj = (testportClass *) tempThis;

   // The following call is necessary for this child process to actually 
   // respond to SIGHUP signals when the parent process dies.            

   prctl(PR_TERMCHILD);

   if ((signal ( SIGTERM, (void (*)(int))i_sig_handler )) == SIG_ERR)
            perror("testportClass:\tError setting SIGTERM handler -\n   ");

   if ((signal ( SIGHUP, (void (*)(int))i_sig_handler )) == SIG_ERR)
            perror("testportClass:\tError setting SIGHUP handler -\n   ");

   if ( (temp_signal = signal ( SIGINT, (void (*)(int))i_sig_handler ))
      != SIG_DFL ) {
      if ((signal ( SIGINT, SIG_IGN )) == SIG_ERR)
         perror("testportClass:\tError setting SIGINT ignore -\n   ");
   }

   if ((signal ( SIGQUIT, (void (*)(int))i_sig_handler )) == SIG_ERR)
      perror("testportClass:\tError setting SIGQUIT handler -\n   ");

   while (1) {

      ussetlock (obj->iport_send_lock);
      obj->ptp->set_position (obj->send_buffer.xyz,
                              obj->send_buffer.flags);
      usunsetlock (obj->iport_send_lock);


      ussetlock (obj->iport_recv_lock);
      obj->ptp->get_velocity (obj->recv_buffer[INBUFF].velocity);
      obj->ptp->get_heading (obj->recv_buffer[INBUFF].pos.hpr[PF_H]);
      obj->ptp->get_status (obj->recv_buffer[INBUFF].flags);
      usunsetlock (obj->iport_recv_lock);

      sginap (obj->nap_interval);

   }
}
