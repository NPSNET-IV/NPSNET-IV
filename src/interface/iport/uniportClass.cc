#include <iostream.h>     // for iostream class deffinition
#include <stdlib.h>       // for exit deffinition
#include <ulocks.h>       // for semaphore deffinitions
#include <bstring.h>      // for bcopy deffinition
#include <sys/types.h>    // 
#include <sys/socket.h>   // for socket deffinitions
#include <sys/prctl.h>    // for sproc deffinition
#include <signal.h>       // for signal deffinitions and kill()

#include "input_device.h" // for input_device class deffinition
#include "iportClass.h"   // for enum deffinitions for devices
#include "uniportClass.h" // for uniportClass deffinition
#include "DI_comm.h"      // for uni-port communication structure structures
#include "Mcast.h"        // for multicast library deffinitons

void uniport_update (void *tempThis);

uniportClass::uniportClass () 
{
   int on = 1;
   
   uniport_detect = FALSE;

   if ( (mcast_CreateReceiveGroup (ISMS_GROUP, ISMS_PACKET_PORT, &mcg) ) != 0) {
      cerr << "ERROR:\tUnable to create Multicast Group for Uni-Port." << endl;
      exit (1);
   }

   talk_back = socket (AF_INET, SOCK_DGRAM, 0);
   setsockopt (talk_back, SOL_SOCKET, SO_REUSEPORT, &on, sizeof (on) );

   child_pid = sproc (uniport_update, PR_SALL, (void *) this);

   cerr << "\tUni-Port pid: " << child_pid << endl;

} // end of uniportClass Constuctor

uniportClass::~uniportClass()
{

   if (child_pid != -1) {
      kill ( child_pid, SIGTERM );
   }

   close (mcg.fd);
   close (talk_back);

} // end of uniportClass Destructor

void 
uniport_update (void *tempThis)
{
   DIPacket   message;
   DIResponse response;
   int        fired_temp;
   u_long     ip_response_port;
   static     void (*temp_signal)(int);

   uniportClass *obj = (uniportClass *) tempThis;

   // The following call is necessary for this child process to actually 
   // respond to SIGHUP signals when the parent process dies.            

   prctl(PR_TERMCHILD);

   if ((signal ( SIGTERM, (void (*)(int))i_sig_handler )) == SIG_ERR)
            perror("uniportClass:\tError setting SIGTERM handler -\n   ");

   if ((signal ( SIGHUP, (void (*)(int))i_sig_handler )) == SIG_ERR)
            perror("uniportClass:\tError setting SIGHUP handler -\n   ");

   if ( (temp_signal = signal ( SIGINT, (void (*)(int))i_sig_handler ))
      != SIG_DFL ) {
      if ((signal ( SIGINT, SIG_IGN )) == SIG_ERR)
         perror("uniportClass:\tError setting SIGINT ignore -\n   ");
   }

   if ((signal ( SIGQUIT, (void (*)(int))i_sig_handler )) == SIG_ERR)
      perror("uniportClass:\tError setting SIGQUIT handler -\n   ");

   ip_response_port = htons (ISMS_RESPONSE_PORT);

   mcast_Receive ( &(obj->mcg), sizeof (message), &message);
   obj->uniport_detect = TRUE;

   while (1) {

      mcast_Receive ( &(obj->mcg), sizeof (message), &message);

      fired_temp = message.rifle.fired;

      ussetlock (obj->iport_recv_lock);
      bcopy ( (void *) (message.velocity),
              (void *) (obj->recv_buffer[INBUFF].velocity),
              sizeof (pfVec3) );
      bcopy ( (void *) &(message.position),
              (void *) &(obj->recv_buffer[INBUFF].pos),
              sizeof (pfCoord) );
      bcopy ( (void *) (message.gaze.hpr),
              (void *) (obj->recv_buffer[INBUFF].gaze),
              sizeof (pfVec3) );
      bcopy ( (void *) &message.rifle.position,
              (void *) &obj->recv_buffer[INBUFF].rifle,
              sizeof (pfCoord) );
      bcopy ( (void *) message.jointlist,
              (void *) (obj->recv_buffer[INBUFF].jlist),
              sizeof (float) * DIJOINTCOUNT);
      obj->recv_buffer[INBUFF].trigger_press = message.rifle.fired;
      obj->recv_buffer[INBUFF].trigger_press_aux = message.rifle.secondary;
      obj->recv_buffer[INBUFF].flags = message.status;
      usunsetlock (obj->iport_recv_lock);

      ussetlock (obj->iport_send_lock);
      bcopy ( (void *) (obj->send_buffer.normal),
              (void *) (response.norm),
              sizeof (pfVec3) );
      bcopy ( (void *) (obj->send_buffer.xyz),
              (void *) (response.desired_position),
              sizeof (pfVec3) );
      response.status = obj->send_buffer.flags;
      usunsetlock (obj->iport_send_lock);

      obj->mcg.addr.sin_port = ip_response_port;

      if (sendto (obj->talk_back, &response, sizeof (response), 0,
                  &(obj->mcg.addr), obj->mcg.addrlen) < 0 )              {
         cerr << "ERROR:\tUnable to send response to Uni-Port." << endl;
      }
                 
   } // end of while (1)

} // end of uniport_update()
