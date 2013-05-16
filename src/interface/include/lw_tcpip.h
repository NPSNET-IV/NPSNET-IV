// File: <lw_tcpip.h>

#ifndef __NPS_LW_TCPIP_H__
#define __NPS_LW_TCPIP_H__

#include "ip_co_sockcomm.h"
#include "input_device.h"
#include "lw-nps.h"


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#define MAX_LW 10

typedef struct _lwStruct {
   int r_port;
   int s_port;
   char name[128];
} lwStruct;

typedef struct {
   pid_t process_id;
   int socket_id;
} sock_info_rec;

/* *********************** Class Definition ******************************* */

class lw_tcpip : virtual public input_device {

/* ********************** Private Declarations **************************** */

private:
   unsigned short sender_port;
   char sender_interface[255];
   char sender_ip[255];
   unsigned short receiver_port;
   char receiver_interface[255];
   char receiver_ip[255];

   int valid;
   short *site;
   short *host;
   short *entity;

   char arena_name[255];
   usptr_t *arena;
   volatile int *receiver_running;
   volatile int *sender_running;
   volatile int *receiver_pid;
   volatile int *sender_pid;

   volatile AER_rec *send_packet;
   volatile DIS_WSIF_rec *recv_packet;
   usema_t *recv_sema;
   usema_t *send_sema;
   ulock_t data_to_send;

   friend void send_process ( void *tempthis );
   friend void receive_process ( void *tempthis );


/* *********************** Public Declarations **************************** */

public:

   static int next_pos;
   static sock_info_rec sock_info[MAX_LW];
   lw_tcpip ( const unsigned short /*recvport*/,
              const char * /*recvinterface*/,
              const char * /*recvfromip*/,
              const unsigned short /*sendport*/,
              const char * /*sendinterface*/,
              const char * /*sendtoip*/ );
   ~lw_tcpip ();
   int communications_up ();
   int sender_up ();
   int receiver_up ();

   int init_shared_vars ();
   int set_laze_info ( float, float, float );
   int set_entity_info ( short, short, short );

   int trigger();
   int laser();
   int video();

   void get_all_inputs ( pfChannel *chan );
   int button_pressed ( const NPS_BUTTON, int & );
   int button_pressed ( const char, int &num_presses )
      { num_presses = 0; return FALSE; }
   int set_multival (const NPS_MULTIVAL, void *);
   int exists ();


};

#endif
