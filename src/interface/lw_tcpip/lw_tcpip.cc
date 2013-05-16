// File: <lw_tcpip.cc>

#include <iostream.h>
#include <string.h>
#include <signal.h>
#include <strings.h>
#include <bstring.h>
#include <math.h>
#include <sys/time.h>
#include "lw_tcpip.h"
#include "alarmclock.h"


#define LW_HEARTBEAT 5.0

typedef struct {
   int packet_size;
   AER_rec data;
} NPS_TO_LW_PACKET;

typedef struct {
   int packet_size;
   DIS_WSIF_rec data;
} LW_TO_NPS_PACKET; 


int lw_tcpip::next_pos = 0;
sock_info_rec lw_tcpip::sock_info[MAX_LW] = { {0,0} };


static double get_time();

double get_time()
{
  struct timeval time ;
  struct timezone tzone;
  double secs, usecs;

  tzone.tz_minuteswest = 8100;
  gettimeofday(&time, &tzone);
  secs = (double)time.tv_sec;
  usecs = (double)time.tv_usec;
  usecs *= 0.000001;
  secs = secs + usecs;
  return secs;
}


int
lw_tcpip::init_shared_vars () 
{

   /* Set arena to be shared by parent and child only */
   if (usconfig(CONF_ARENATYPE, US_SHAREDONLY) == -1) {
      perror("ERROR:\tlw_tcpip configuring arena -\n   ");
      return (FALSE);
   }

   sprintf ( arena_name, "/usr/tmp/lw_tcpip_arena.%d", getpid() );

   if ( (arena = usinit(arena_name)) == NULL ) {
      perror("ERROR:lw_tcpip creating arena -\n   ");
   }
   else {
      /* Allocate shared variables */
      sender_running = (volatile int *)usmalloc(sizeof(int), arena);
      if ( sender_running == NULL ) {
         perror("ERROR:\tlw_tcpip unable to allocate shared flag -\n   ");
         return (FALSE);
      }
      *sender_running = FALSE;

      receiver_running = (volatile int *)usmalloc(sizeof(int), arena);
      if ( receiver_running == NULL ) {
         perror("ERROR:\tlw_tcpip unable to allocate shared flag -\n   ");
         return (FALSE);
      }
      *receiver_running = FALSE;

      receiver_pid = (volatile int *)usmalloc(sizeof(int), arena);
      if ( receiver_pid == NULL ) {
         perror("ERROR:\tlw_tcpip unable to allocate shared flag -\n   ");
         return (FALSE);
      }
      *receiver_pid = -1;

      sender_pid = (volatile int *)usmalloc(sizeof(int), arena);
      if ( sender_pid == NULL ) {
         perror("ERROR:\tlw_tcpip unable to allocate shared flag -\n   ");
         return (FALSE);
      }
      *sender_pid = -1;

      site = (short *)usmalloc(sizeof(short), arena);
      if ( site == NULL ) {
         perror("ERROR:\tlw_tcpip unable to allocate site variable -\n   ");
         return (FALSE);
      }
      *site = 0;

      host = (short *)usmalloc(sizeof(short), arena);
      if ( host == NULL ) {
         perror("ERROR:\tlw_tcpip unable to allocate host variable -\n   ");
         return (FALSE);
      }
      *host = 0;

      entity = (short *)usmalloc(sizeof(short), arena);
      if ( entity == NULL ) {
         perror("ERROR:\tlw_tcpip unable to allocate entity variable -\n   ");
         return (FALSE);
      }
      *entity = 0;

      send_packet = (volatile AER_rec *)
                    usmalloc(sizeof(AER_rec), arena);
      if ( send_packet == NULL ) {
         perror("ERROR:\tlw_tcpip unable to allocate send_packet -\n   ");
         return (FALSE);
      }

      recv_packet = (volatile DIS_WSIF_rec *)
                    usmalloc(sizeof(DIS_WSIF_rec), arena);
      if ( recv_packet == NULL ) {
         perror("ERROR:\tlw_tcpip unable to allocate recv_packet -\n   ");
         return (FALSE);
      }
 
      if ((recv_sema = usnewsema(arena, 1)) == NULL) {
         perror("ERROR:\tlw_tcpip unable to allocate recv semaphore -\n ");
         return (FALSE);
      }

      if ((send_sema = usnewsema(arena, 1)) == NULL) {
         perror("ERROR:\tlw_tcpip unable to allocate send semaphore -\n ");
         return (FALSE);
      }

      if ( (data_to_send = usnewlock(arena)) == NULL ) {
         perror("ERROR:\tlw_tcpip unable to allocate send lock -\n");
         return (FALSE);
      }
      usinitlock(data_to_send);

   }
   
   return TRUE;

}


lw_tcpip::lw_tcpip ( const unsigned short recvport,
                     const char *recvinterface,
                     const char *recvfromip,
                     const unsigned short sendport,
                     const char *sendinterface,
                     const char *sendtoip )
{
//cerr << "LW TCPIP CLASS CONSTRUCTOR DEBUG:" << endl;
//cerr << "\tRecv.Port = " << recvport << ", Recv.Intr = "
     //<< recvinterface << ", Recv.FromIP = " << recvfromip << endl;
//cerr << "\tSend.Port = " << sendport << ", Send.Intr = "
     //<< sendinterface << ", Send.FromIP = " << sendtoip << endl;

   sender_port = sendport;
   strcpy ( sender_interface, sendinterface );
   strcpy ( sender_ip, sendtoip );
   receiver_port = recvport;
   strcpy ( receiver_interface, recvinterface );
   strcpy ( receiver_ip, recvfromip );
   sender_running = NULL;
   receiver_running = NULL;
   sender_pid = NULL;
   receiver_pid = NULL;
   recv_sema = NULL;
   send_sema = NULL;
   recv_packet = NULL;
   send_packet = NULL;
   data_to_send = NULL;
   site = NULL;
   host = NULL;
   entity = NULL;
   valid = FALSE;
   if ( init_shared_vars() ) {
      signal ( SIGCLD, SIG_IGN );
      *sender_pid = sproc ( send_process, PR_SALL, (void *)this );
      sleep(1);
      *receiver_pid = sproc ( receive_process, PR_SALL, (void *)this );
      valid = TRUE;
   }
}


lw_tcpip::~lw_tcpip ()
{

   /* Signal child processes to die */
   if ( receiver_pid != NULL ) {
      kill(*receiver_pid,SIGTERM);
      sleep(1);
   }
   if ( sender_pid != NULL ) {
      kill(*sender_pid,SIGTERM);
   }

   /* Free up variables */
   if ( sender_running != NULL ) {
      usfree ( (void *)sender_running, arena );
      sender_running = NULL;
   }
   if ( receiver_running != NULL ) {
      usfree ( (void *)receiver_running, arena );
      sender_running = NULL;
   }
   if ( sender_pid != NULL ) {
      usfree ( (void *)sender_pid, arena );
      sender_running = NULL;
   }
   if ( receiver_pid != NULL ) {
      usfree ( (void *)receiver_pid, arena );
      sender_running = NULL;
   }
   if ( send_packet != NULL ) {
      usfree ( (void *)send_packet, arena );
      send_packet = NULL;
   }
   if ( recv_packet != NULL ) {
      usfree ( (void *)recv_packet, arena );
      recv_packet = NULL;
   }
   if ( site != NULL ) {
      usfree ( (void *)site, arena );
      site = NULL;
   }
   if ( host != NULL ) {
      usfree ( (void *)host, arena );
      host = NULL;
   }
   if ( entity != NULL ) {
      usfree ( (void *)entity, arena );
      entity = NULL;
   }

   if ( recv_sema != NULL ) {
      usfreesema ( recv_sema, arena );
      recv_sema = NULL;
   }
   if ( send_sema != NULL ) {
      usfreesema ( send_sema, arena );
      send_sema = NULL;
   }
   if ( data_to_send != NULL ) {
      usfreelock ( data_to_send, arena );
      data_to_send = NULL;
   }

   if ( arena != NULL ) {
      usdetach(arena);
      arena = NULL;
   }

   valid = FALSE;
}


int
lw_tcpip::communications_up ()
{
   if ( valid ) {
      return (*sender_running && *receiver_running);
   }
   else {
      return FALSE;
   }
}


int
lw_tcpip::sender_up ()
{
   if ( valid ) {
      return (*sender_running);
   }
   else {
      return FALSE;
   }
}


int
lw_tcpip::receiver_up ()
{
   if ( valid ) {
      return (*receiver_running);
   }
   else {
      return FALSE;
   }

}

void
terminate ( pid_t my_pid, int kill_process )
{
   int my_pos = 0;
   while ( (my_pos < lw_tcpip::next_pos) &&
           (lw_tcpip::sock_info[my_pos].process_id != my_pid) ) {
      my_pos++;
   }

   if (my_pos >= lw_tcpip::next_pos) {
      cerr << "ERROR:\tUnable to close socket at termination." << endl;
   }
   else {
      if ( lw_tcpip::sock_info[my_pos].socket_id != -1 ) {
         close(lw_tcpip::sock_info[my_pos].socket_id);
         cerr << "\tCLOSED socket " 
              << lw_tcpip::sock_info[my_pos].socket_id << endl;
      }
   }
   if ( kill_process ) { 
      exit(0);
   }
}


void
send_signal_handler(int sig, ...)
{
   pid_t my_pid;
   signal ( SIGTERM, SIG_IGN );
   signal ( SIGINT, SIG_IGN );
   signal ( SIGHUP, SIG_IGN );
   signal ( SIGPIPE, SIG_IGN );
   my_pid = getpid();
   switch ( sig ) {
      case SIGPIPE:
         break;
      case SIGTERM:
      case SIGINT:
         cerr << "\tLand Warrior send process " << my_pid
              << " terminating." << endl;
         terminate(my_pid,1);
         break;
      case SIGHUP:
         if ( getppid() == 1 ) {
            cerr << "\tLand Warrior send process " << my_pid
                 << " terminating." << endl;
            terminate(my_pid,1);
         }
         break;
   }
   signal ( SIGTERM, (void (*)(int))send_signal_handler );
   signal ( SIGINT, (void (*)(int))send_signal_handler );
   signal ( SIGHUP, (void (*)(int))send_signal_handler );


}

void
recv_signal_handler(int sig, ...)
{
   pid_t my_pid;
   signal ( SIGTERM, SIG_IGN );
   signal ( SIGINT, SIG_IGN );
   signal ( SIGHUP, SIG_IGN );
   signal ( SIGPIPE, SIG_IGN );
   my_pid = getpid();
   switch ( sig ) {
      case SIGPIPE:
         break;
      case SIGTERM:
      case SIGINT:
         cerr << "\tLand Warrior receive process " << my_pid
              << " terminating." << endl;
         terminate(my_pid,1);
         break;
      case SIGHUP:
         if ( getppid() == 1 ) {
            cerr << "\tLand Warrior receive process " << my_pid
                 << " terminating." << endl;
            terminate(my_pid,1);
         }
         break;
   }
   signal ( SIGTERM, (void (*)(int))recv_signal_handler );
   signal ( SIGINT, (void (*)(int))recv_signal_handler );
   signal ( SIGHUP, (void (*)(int))recv_signal_handler );
}



void
send_process ( void *tempthis ) 
{
/* Client to send Laze information to LW system */

   IP_CO_SockCommClass *socket_send;
   lw_tcpip *L_lw_tcpip = (lw_tcpip *)tempthis;
   static void (*temp_signal)(int);
   AlarmClockClass my_timer;
   NPS_TO_LW_PACKET outgoing;
   pid_t my_pid;
   int my_lw_pos;
   double current_time = 0.0;
   double last_trans_time = 0.0;
   int data_to_send, time_to_send;
   int numbytes = 0;

   my_pid = getpid();
   cerr << "\tLand Warrior send process started, pid = "
        << my_pid << "." << endl;

   my_lw_pos = lw_tcpip::next_pos++;
   lw_tcpip::sock_info[my_lw_pos].process_id = my_pid; 
   lw_tcpip::sock_info[my_lw_pos].socket_id = -1;

   cerr << "\tSend process " << my_pid
        << " added to socket array position " << my_lw_pos
        << endl;

   prctl(PR_TERMCHILD);
   if ((signal ( SIGTERM, (void (*)(int))send_signal_handler )) == SIG_ERR)
            perror("lw_tcpip:\tError setting SIGTERM handler -\n   ");
   if ((signal ( SIGHUP, (void (*)(int))send_signal_handler )) == SIG_ERR)
            perror("lw_tcpip:\tError setting SIGHUP handler -\n   ");
   if ( (temp_signal = signal ( SIGINT, (void (*)(int))send_signal_handler ))
      != SIG_DFL ) {
      if ((signal ( SIGINT, SIG_IGN )) == SIG_ERR)
         perror("lw_tcpip:\tError setting SIGINT ignore -\n   ");
   }

   while (1) {

   if ((signal ( SIGPIPE, (void (*)(int))send_signal_handler )) == SIG_ERR)
            perror("lw_tcpip:\tError setting SIGTERM handler -\n   ");
   cerr << "     Waiting for connect to LW system for writing..." << endl;
   // Open the TCP/IP connection-oriented stream socket to be used to
   // send data from NPSNET to the Land Warrior System
   socket_send =
      new IP_CO_SockCommClass(L_lw_tcpip->sender_port,
                              L_lw_tcpip->sender_interface,
                              L_lw_tcpip->sender_ip );
   if ( !socket_send->open_communication(SC_SOCKET_WRITE) ) {
      cerr << "Land Warrior TCP/IP Socket for writing failed to open."
           << endl;
      delete socket_send;
      socket_send = NULL;
      *L_lw_tcpip->sender_running = FALSE;
   }
   else {
      *L_lw_tcpip->sender_running = TRUE;
      lw_tcpip::sock_info[my_lw_pos].socket_id = socket_send->get_sock_id();
      cerr << "\tLand Warrior send process connected via socket "
           << socket_send->get_sock_id() << endl;
   }

   while ( *L_lw_tcpip->sender_running ) {
      current_time = get_time();
      data_to_send = FALSE;
      time_to_send = ( (current_time - last_trans_time) >= LW_HEARTBEAT );
      if ( ustestlock(L_lw_tcpip->data_to_send) ) {
         if ( uspsema(L_lw_tcpip->send_sema) == -1 ) {
            perror("ERROR:\tlw_tcpip psema error -\n   ");
         }
         else {
            if ( usunsetlock(L_lw_tcpip->data_to_send) == -1 ) {
               perror("ERROR:\tlw_tcpip unable to unset lock -\n   ");
            }
            bcopy ( (char *)L_lw_tcpip->send_packet,
                    (char *)&(outgoing.data), sizeof(AER_rec) );
            data_to_send = TRUE;
            if ( usvsema(L_lw_tcpip->send_sema) == -1 ) {
               perror("ERROR:\tlw_tcpip vsema error -\n   ");
            }
         } 
      }
      if ( time_to_send ) {
         outgoing.packet_size = 0;
         outgoing.data.padding = 0;
         outgoing.data.host_id = 0;
         outgoing.data.entity_id = 0;
         outgoing.data.site_id = 0;
         outgoing.data.elevation.whole = 0;
         outgoing.data.elevation.decimal = 0;
         outgoing.data.azimuth.whole = 0;
         outgoing.data.azimuth.decimal = 0;
         outgoing.data.range.whole = 0;
         outgoing.data.range.decimal = 0;
         numbytes = sizeof(outgoing.packet_size);
      }
      if ( data_to_send ) {
         outgoing.packet_size = sizeof(AER_rec);
         outgoing.data.padding = 0;
         outgoing.data.host_id = *L_lw_tcpip->host;
         outgoing.data.entity_id = *L_lw_tcpip->entity;
         outgoing.data.site_id = *L_lw_tcpip->site;
         numbytes = sizeof(outgoing);
      }
      if ( data_to_send || time_to_send ) {
         if ( !socket_send->write_buffer ( (char *)&(outgoing),
                                           numbytes) ) {
            *L_lw_tcpip->sender_running = FALSE;
            cerr << "LW system has broken connection - "
                 << "waiting for reconnect..." << endl;
            delete socket_send;
            socket_send = NULL;
            lw_tcpip::sock_info[my_lw_pos].socket_id = -1;
         }
         else {
            last_trans_time = current_time;
         }
      }

      my_timer.sleep(0,100000); /* sleep for 1/10 of a second */

   }

   } // while 1

}


void
receive_process ( void *tempthis )
{
/* Server to receive trigger, laze and video requests to LW system */

   IP_CO_SockCommClass *socket_recv;
   lw_tcpip *L_lw_tcpip = (lw_tcpip *)tempthis;
   LW_TO_NPS_PACKET incoming;
   char sender[255];
   static void (*temp_signal)(int);
   pid_t my_pid;
   int my_lw_pos;

   my_pid = getpid();

   cerr << "\tLand Warrior receive process started, pid = "
        << my_pid << "." << endl;

   my_lw_pos = lw_tcpip::next_pos++;
   lw_tcpip::sock_info[my_lw_pos].process_id = my_pid;         
   lw_tcpip::sock_info[my_lw_pos].socket_id = -1;

   cerr << "\tReceive process " << my_pid
        << " added to socket array position " << my_lw_pos
        << endl;

   prctl(PR_TERMCHILD);
   if ((signal ( SIGTERM, (void (*)(int))recv_signal_handler )) == SIG_ERR)
            perror("lw_tcpip:\tError setting SIGTERM handler -\n   ");
   if ((signal ( SIGHUP, (void (*)(int))recv_signal_handler )) == SIG_ERR)
            perror("lw_tcpip:\tError setting SIGHUP handler -\n   ");
   if ( (temp_signal = (void (*)(int))
                        signal ( SIGINT, (void (*)(int))recv_signal_handler ))
      != (void (*)(int))SIG_DFL ) {
      if ((signal ( SIGINT, SIG_IGN )) == SIG_ERR)
         perror("lw_tcpip:\tError setting SIGINT ignore -\n   ");
   }

   while ( 1 ) {

   if ((signal ( SIGPIPE, (void (*)(int))recv_signal_handler )) == SIG_ERR)
            perror("lw_tcpip:\tError setting SIGPIPE handler -\n   ");


   // Open the TCP/IP connection-oriented stream socket to be used to
   // receive data from the Land Warrior System into NPSNET
   socket_recv =
      new IP_CO_SockCommClass(L_lw_tcpip->receiver_port,
                              L_lw_tcpip->receiver_interface,
                              L_lw_tcpip->receiver_ip);
   if ( !socket_recv->open_communication(SC_SOCKET_READ) ) {
      cerr << "Land Warrior TCP/IP Socket for reading failed to open."
           << endl;
      delete socket_recv;
      socket_recv = NULL;
      *L_lw_tcpip->receiver_running = FALSE;
   }
   else {
      *L_lw_tcpip->receiver_running = TRUE;
      lw_tcpip::sock_info[my_lw_pos].socket_id = socket_recv->get_sock_id();
      cerr << "\tLand Warrior receive process connected via socket "
           << socket_recv->get_sock_id() << endl;
   }

   while (*L_lw_tcpip->receiver_running) {
      if ( socket_recv->read_buffer( (char *)&(incoming), sizeof(incoming),
                                     sender) ) {
         if ( uspsema(L_lw_tcpip->recv_sema) == -1 ) {
            perror("ERROR:\tlw_tcpip psema error -\n   ");
         }
         else {
            L_lw_tcpip->recv_packet->laser =
               L_lw_tcpip->recv_packet->laser | incoming.data.laser;
            L_lw_tcpip->recv_packet->trigger =
               L_lw_tcpip->recv_packet->trigger | incoming.data.trigger;
            L_lw_tcpip->recv_packet->video =
               L_lw_tcpip->recv_packet->video | incoming.data.video;
            if ( usvsema(L_lw_tcpip->recv_sema) == -1 ) {
               perror("ERROR:\tlw_tcpip vsema error -\n   ");
            }
         }
      }
      else {
         *L_lw_tcpip->receiver_running = FALSE;
         cerr << "LW system has broken connection - "
              << "waiting for reconnect..." << endl;
         delete socket_recv;
         socket_recv = NULL;
         lw_tcpip::sock_info[my_lw_pos].socket_id = -1;
      }
   }

   } // while 1

}

int
lw_tcpip::set_laze_info ( float elevation, float azimuth, float range )
{
   int temp_whole, temp_decimal;
   int success = FALSE;

   if ( valid ) {
      if ( uspsema(send_sema) == -1 ) {
         perror("ERROR:\tlw_tcpip psema error -\n   ");
      }
      else {
         temp_whole = (int)floor(elevation);
         temp_decimal = (int)floor((elevation - (float)temp_whole)*10000000.0f);
         send_packet->elevation.whole = temp_whole;
         send_packet->elevation.decimal = temp_decimal;
         temp_whole = (int)floor(azimuth);
         temp_decimal = (int)floor((azimuth - (float)temp_whole)*10000000.0f);
         send_packet->azimuth.whole = temp_whole;
         send_packet->azimuth.decimal = temp_decimal;
         temp_whole = (int)floor(range);
         temp_decimal = (int)floor((range - (float)temp_whole)*10000000.0f);
         send_packet->range.whole = temp_whole;
         send_packet->range.decimal = temp_decimal;
         success = TRUE;
         if ( ussetlock(data_to_send) == -1 ) {
            perror("ERROR:\tlw_tcpip unable to unset lock -\n   ");
            success = FALSE;
         }
         if ( usvsema(send_sema) == -1 ) {
            perror("ERROR:\tlw_tcpip vsema error -\n   ");
            success = FALSE;
         }
      } 
   }
 
   return success;
}

int
lw_tcpip::set_entity_info ( short mysite, short myhost, short myentity )
{
   int success = FALSE;
   
   if ( valid ) {
      *site = mysite;
      *host = myhost;
      *entity = myentity;
      success = TRUE;
   }

   return success;
}


int
lw_tcpip::trigger ()
{
   int pulled = 0;

   if ( valid ) {
      if ( uspsema(recv_sema) == -1 ) {
         perror("ERROR:\tlw_tcpip psema error -\n   ");
      }
      else {
         pulled = (recv_packet->trigger != 0);
         recv_packet->trigger = 0;
         if ( usvsema(recv_sema) == -1 ) {
            perror("ERROR:\tlw_tcpip vsema error -\n   ");
         }
      }
   }

   return(pulled);
}


int
lw_tcpip::laser ()
{
   int pulled = 0;

   if ( valid ) {
      if ( uspsema(recv_sema) == -1 ) {
         perror("ERROR:\tlw_tcpip psema error -\n   ");
      }
      else {
         pulled = (recv_packet->laser != 0);
         recv_packet->laser = 0;
         if ( usvsema(recv_sema) == -1 ) {
            perror("ERROR:\tlw_tcpip vsema error -\n   ");
         }
      }
   }

   return(pulled);
}


int
lw_tcpip::video ()
{
   int pulled = 0;

   if ( valid ) {
      if ( uspsema(recv_sema) == -1 ) {
         perror("ERROR:\tlw_tcpip psema error -\n   ");
      }
      else {
         pulled = (recv_packet->video != 0);
         recv_packet->video = 0;
         if ( usvsema(recv_sema) == -1 ) {
            perror("ERROR:\tlw_tcpip vsema error -\n   ");
         }
      }
   }

   return(pulled);
}


void
lw_tcpip::get_all_inputs ( pfChannel * )
{
}

int
lw_tcpip::button_pressed ( const NPS_BUTTON button_num, int &num_presses )
{
   int success = FALSE;
   num_presses = 0;

   if ( valid ) {
      success = TRUE;
      switch ( button_num ) {
         case NPS_LW_TRIGGER:
            num_presses = this->trigger();
            break;
         case NPS_LW_LASER:
            num_presses = this->laser();
            break;
         case NPS_LW_VIDEO:
            num_presses = this->video();
            break;
      }
   }
   
   return success;
}

int
lw_tcpip::set_multival ( const NPS_MULTIVAL multi_val, void *data )
{
   int success = FALSE;
   float *laze_values;
   short *entity_info;

   if ( valid ) {
      if (multi_val == NPS_LW_LAZE) {
         laze_values = (float *)data;
         if ( data != NULL ) {
            /* Laze info should be elevation, heading and range in that order */
            success = set_laze_info ( laze_values[0], laze_values[1],
                                      laze_values[2] );
         }
      }
      else if (multi_val == NPS_LW_ENTITY) {
         entity_info = (short *)data;
         if ( data != NULL ) {
            /* Entity info should be site, host, entity in that order */
            success = set_entity_info ( entity_info[0], entity_info[1],
                                        entity_info[2] );
         }
      }
   }


   return success;
}


int
lw_tcpip::exists ()
{
   return valid;
}
// End file - lw_tcpip.cc
