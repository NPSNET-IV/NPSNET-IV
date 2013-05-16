/* File:	client_lib.c
 * Description:	Client communications library - DIS package
 * Revision:	3.0 - 28Jul94
 */

#ifndef NOC2MEASURES

char PRINT_TYPE = 0;
char PRINT_FLAG = 0;
#include "po_net.h"
#include <sys/types.h>
#include <unistd.h>

/*@  Networking globals*/
int			po_sock_send;
static int		po_sock_recv;
struct sockaddr_in	po_dest;
static char		my_in_addr[IN_ADDR_SIZE];
unsigned short		po_host_id;
SP_PersistentObjectPDUKind	pdulist[MAX_NUM_PDUS]; /*@  PDU types to receive*/

/*@  Arena globals*/
usptr_t			*po_netarena = NULL;	/*@  arena handle*/
#define BUF1		0
#define BUF2		1
typedef struct {
   int			receiver_pid;
   int			application_pid;
   /*@ */
   int			current_net_buf;
			  /*@  PDUs from net go into current buffer; the*/
			  /*@    application reads PDUs from the other buf*/
   usema_t		*swap_buf_sema;	/*@  control access to swap_buf_flag*/
   int			swap_buf_flag;
   /*@ */
   int			buf1_oldest;
   int			buf1_newest;
   int			buf1_wasted;	/*@  overwritten PDUs*/
   int			nodes_in_buf1;
   STORE		*pdu_buf1;	/*@  PDU array; size selected through*/
					/*@    arg to net_open()*/
   /*@ */
   int			buf2_oldest;
   int			buf2_newest;
   int			buf2_wasted;	/*@  overwritten PDUs*/
   int			nodes_in_buf2;
   STORE		*pdu_buf2;
} PDUarena;
PDUarena		*po_PDUarena_ptr;
int			po_pdus_in_buffer;


int netOpen(char *interf, int buf_len,int port)
{
   int			i, on = 1;
   int			valid_interface = FALSE;
   char			buf[BUFSIZ];
   struct sockaddr_in	sin_send, sin_recv, *in_addr;
   struct ifconf	ifc;
   struct ifreq		*ifr;


   /*@  Set up arena*/
   po_pdus_in_buffer = buf_len;
   if (init_po_arena() == FALSE)
      return (FALSE);

   /*@  Create sockets*/
   if ((po_sock_send = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      perror("socket(po_sock_send)");
      return (FALSE);
   }
   if ((po_sock_recv = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      perror("socket(po_sock_recv)");
      return (FALSE);
   }

   /*@  Mark send socket for broadcasting*/
   if (setsockopt(po_sock_send, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) == -1) {
      perror("setsockopt(SO_BROADCAST)");
      return (FALSE);
   }

   /*@  Bind port numbers to sockets*/
   sin_send.sin_family = AF_INET;
   sin_send.sin_addr.s_addr = htonl(INADDR_ANY);
   sin_send.sin_port = htons((port-1));
   if (bind(po_sock_send, (struct sockaddr *) &sin_send, sizeof(sin_send)) == -1) {
      perror("bind(po_sock_send)");
      return (FALSE);
   }
   sin_recv.sin_family = AF_INET;
   sin_recv.sin_addr.s_addr = htonl(INADDR_ANY);
   sin_recv.sin_port = htons(port);
   if (bind(po_sock_recv, (struct sockaddr *) &sin_recv, sizeof(sin_recv)) == -1) {
      perror("bind(po_sock_recv)");
      return (FALSE);
   }

   /*@  Get InterFace CONFig*/
   ifc.ifc_len = sizeof(buf);
   ifc.ifc_buf = buf;
   if (ioctl(po_sock_send, SIOCGIFCONF, (char *) &ifc) == -1) {
      perror("ioctl(SIOCGIFCONF)");
      return (FALSE);
   }

   /*@  Check interfaces (assumes same interface for sender and receiver)*/
   ifr = ifc.ifc_req;		/*@  ptr to interface structure*/
   for (i = 0; i < (ifc.ifc_len / sizeof(struct ifreq)); i++, ifr++) {

      if (ifr->ifr_addr.sa_family != AF_INET)	/*@  Internet only*/
         continue;

      /*@  Get InterFace FLAGS*/
      if (ioctl(po_sock_send, SIOCGIFFLAGS, (char *) ifr) == -1) {
         perror("ioctl(SIOCGIFFLAGS)");
         return (FALSE);
      }

      /*@  Skip boring cases*/
      if ((ifr->ifr_flags & IFF_UP) == 0 ||		/*@  interface down?*/
          (ifr->ifr_flags & IFF_LOOPBACK) ||		/*@  local loopback?*/
          (ifr->ifr_flags & IFF_BROADCAST) == 0) {	/*@  no broadcast?*/
         continue;
      }

      /*@  Get and save InterFace ADDRess*/
      if (ioctl(po_sock_send, SIOCGIFADDR, (char *) ifr) == -1) {
         perror("ioctl(SIOCGIFADDR)");
         return (FALSE);
      }
      in_addr = (struct sockaddr_in *) &(ifr->ifr_addr);
      bcopy((char *) &(in_addr->sin_addr), my_in_addr, IN_ADDR_SIZE);
      po_assign_host_id();

      /*@  Get InterFace BRoaDcast ADDRess*/
      if (ioctl(po_sock_send, SIOCGIFBRDADDR, (char *) ifr) == -1) {
         perror("ioctl(SIOCGIFBRDADDR)");
         return (FALSE);
      }
      bcopy((char *) &(ifr->ifr_broadaddr), (char *) &po_dest,
                                             sizeof(ifr->ifr_broadaddr));

      /*@  Assign destination port*/
      po_dest.sin_port = htons(port);

      /*@  Interface we want?*/
      if (strcmp(interf, ifr->ifr_name) == 0) {
         valid_interface = TRUE;
      }
   }

   if (valid_interface == FALSE) {
      printf("netOpen(): No valid interface found");
      return (FALSE);
   }

   /*@  Save PID*/
   po_PDUarena_ptr->application_pid = (int)getpid();

   /*@  Spawn the receiving process*/
   if ((po_PDUarena_ptr->receiver_pid = sproc(po_receiver_process, PR_SALL)) == -1)  {
      perror("sproc()");
      return (FALSE);
   }

   signal(SIGCLD,SIG_IGN);
   return (TRUE);
}


int init_po_arena()
{
   int		arenasize, fudge;
   char		arenaname[64];

#ifdef DEBUG2
   printf("Enter init_po_arena()\n");
   fflush(stdout);
#endif

   /*@  Set arena size*/
   fudge = (1024 * 30);	/*@  fudge factor*/
   arenasize =
         (2 * po_pdus_in_buffer * sizeof(STORE)) + 
          sizeof(PDUarena) + fudge;
   if (usconfig(CONF_INITSIZE, arenasize) == -1)  {
      perror("usconfig(CONF_INITSIZE)");
      return (FALSE);
   }

   /*@  Set arena to be shared by parent and child only*/
   if (usconfig(CONF_ARENATYPE, US_SHAREDONLY) == -1)  {
      perror("usconfig(CONF_ARENATYPE)");
      return (FALSE);
   }

   /*@  Name and open arena*/
   
   while(po_netarena == NULL){
     int ix = 0;
     char arenaname[255];
    
     sprintf(arenaname,"/usr/tmp/po_reader.arena.%1d",ix);
     po_netarena = usinit(arenaname);
     ix++;
   }

   /*@  malloc root data structure*/
   if ((po_PDUarena_ptr =
              (PDUarena *) usmalloc(sizeof(PDUarena), po_netarena)) == NULL) {
      perror("usmalloc(PDUarena)");
      return (FALSE);
   }

   /*@  malloc PDU buffers*/
   if ((po_PDUarena_ptr->pdu_buf1 = (STORE *)
           usmalloc((sizeof(STORE)*po_pdus_in_buffer), po_netarena)) == NULL) {
      perror("usmalloc(buf1)");
      return (FALSE);
   }
   if ((po_PDUarena_ptr->pdu_buf2 = (STORE *)
           usmalloc((sizeof(STORE)*po_pdus_in_buffer), po_netarena)) == NULL) {
      perror("usmalloc(buf2)");
      return (FALSE);
   }

   /*@  Init semaphore*/
   if ((po_PDUarena_ptr->swap_buf_sema = usnewsema(po_netarena, 1)) == NULL) {
      perror("usnewsema(swap_buf_sema)");
      return (FALSE);
   }

   /*@  Data initializations*/
   po_PDUarena_ptr->current_net_buf = BUF1;
   po_PDUarena_ptr->swap_buf_flag = FALSE;
   po_PDUarena_ptr->buf1_oldest = 0;
   po_PDUarena_ptr->buf1_newest = 0;
   po_PDUarena_ptr->nodes_in_buf1 = 0;
   po_PDUarena_ptr->buf2_oldest = 0;
   po_PDUarena_ptr->buf2_newest = 0;
   po_PDUarena_ptr->nodes_in_buf2 = 0;

   return (TRUE);

} /*@  init_po_arena()*/


void po_swap_net_buffers()
{
   /*@  Simply switch current buffer var*/
   if (uspsema(po_PDUarena_ptr->swap_buf_sema) == 1) { /*@  acquire semaphore*/
#ifdef DEBUG2
      printf("Swapping buffers\n");
      fflush(stdout);
#endif
      if (po_PDUarena_ptr->current_net_buf == BUF1) {
         /*@  switch to buf2*/
         po_PDUarena_ptr->current_net_buf = BUF2;
      } else {
         /*@  switch to buf1*/
         po_PDUarena_ptr->current_net_buf = BUF1;
      }
      po_PDUarena_ptr->swap_buf_flag = TRUE;
   }
   usvsema(po_PDUarena_ptr->swap_buf_sema); /*@  release semaphore*/
}


STORE *netRead(struct read_po_stat *rstat)
   /*@  Return pointer to oldest currently available PDU; fill rstat with*/
   /*@    status on individual PDU and buffer state; request buffer swap*/
   /*@    when buffer is emptied or for other reasons*/
{
   STORE	*tmp_ptr;
   SP_PersistentObjectPDU	*header;

   /*@  Force buffer swap*/
   if (rstat->rs_swap_buffers_flag == TRUE) {
      po_swap_net_buffers();
      rstat->rs_swap_buffers_flag = FALSE;
   }

   /*@  Use the buffer not in use by po_receiver_process()*/
   if (po_PDUarena_ptr->current_net_buf == BUF1) {
      /*@  Any PDUs available?*/
      if (po_PDUarena_ptr->nodes_in_buf2 == 0) {
         /*@  BUF2 empty*/
         po_swap_net_buffers();
         if (po_PDUarena_ptr->nodes_in_buf1 == 0) {
            /*@  both buffers empty*/
            return (NULL);
         } else {
            /*@  PDUs available in BUF1*/
            tmp_ptr = &(po_PDUarena_ptr->pdu_buf1[po_PDUarena_ptr->buf1_oldest]);
         }
      } else {
         /*@  PDUs available in BUF2*/
         tmp_ptr = &(po_PDUarena_ptr->pdu_buf2[po_PDUarena_ptr->buf2_oldest]);
      }
   } else { /*@  (po_PDUarena_ptr->current_net_buf == BUF2)*/
      /*@  Any PDUs available?*/
      if (po_PDUarena_ptr->nodes_in_buf1 == 0) {
         /*@  BUF1 empty*/
         po_swap_net_buffers();
         if (po_PDUarena_ptr->nodes_in_buf2 == 0) {
            /*@  both buffers empty*/
            return (NULL);
         } else {
            /*@  PDUs available in BUF2*/
            tmp_ptr = &(po_PDUarena_ptr->pdu_buf2[po_PDUarena_ptr->buf2_oldest]);
         }
      } else {
         /*@  PDUs available in BUF1*/
         tmp_ptr = &(po_PDUarena_ptr->pdu_buf1[po_PDUarena_ptr->buf1_oldest]);
      }
   }

   /*@  Now we have a ptr to buffer with nodes; update buffer state*/
   if (po_PDUarena_ptr->current_net_buf == BUF1) {
      rstat->rs_wasted = po_PDUarena_ptr->buf2_wasted;
      po_PDUarena_ptr->nodes_in_buf2--;
      if (po_PDUarena_ptr->nodes_in_buf2 == 0) {
         /*@  plan ahead for next net_read()*/
         po_swap_net_buffers();
      } else {
         /*@  update buffer state*/
         po_PDUarena_ptr->buf2_oldest++;
         if (po_PDUarena_ptr->buf2_oldest > (po_pdus_in_buffer-1))
            /*@  circle the buffer*/
            po_PDUarena_ptr->buf2_oldest = 0;
      }
   } else { /*@  (po_PDUarena_ptr->current_net_buf == BUF2)*/
      rstat->rs_wasted = po_PDUarena_ptr->buf1_wasted;
      po_PDUarena_ptr->nodes_in_buf1--;
      if (po_PDUarena_ptr->nodes_in_buf1 == 0) {
         /*@  plan ahead for next net_read()*/
         po_swap_net_buffers();
      } else {
         /*@  update buffer state*/
         po_PDUarena_ptr->buf1_oldest++;
         if (po_PDUarena_ptr->buf1_oldest > (po_pdus_in_buffer-1))
            /*@  circle the buffer*/
            po_PDUarena_ptr->buf1_oldest = 0;
      }
   }
   
   /*@  Fill read status structure*/
   header = (SP_PersistentObjectPDU *)&(tmp_ptr->v.po);
   rstat->rs_type = header->kind;

   return (tmp_ptr);
}


void po_receiver_process(void *dummy)
{
   int			i, j = 0, bytes_read;
   register int		buf_dimension;
   struct sockaddr_in	from;
   int			len = sizeof(from);
   int			oldest = 0, newest = 0;	/*@  array indexes*/
   int			circled = FALSE;
   STORE		input_buf, *curr_buf;
   SP_PersistentObjectPDU		*header;
   int size = 0;

   /*short hand to get the the pdu*/
   header = (SP_PersistentObjectPDU *)&input_buf.v.po;

   signal ( SIGTERM, SIG_DFL );

   /*@  Initialize local vars*/
   buf_dimension = (po_pdus_in_buffer-1);
   if (uspsema(po_PDUarena_ptr->swap_buf_sema) == 1) { /*@  acquire semaphore*/
      switch (po_PDUarena_ptr->current_net_buf) {
         /*@  get pointer to the correct buffer*/
         case (BUF1):
            /*@  pointer to current buffer; must be handled*/
            /*@    as local var (i.e. not in arena) in case application*/
            /*@    swaps buffers between reading the packet*/
            curr_buf = (STORE *) po_PDUarena_ptr->pdu_buf1;
            break;
         case (BUF2):
            curr_buf = (STORE *) po_PDUarena_ptr->pdu_buf2;
            break;
         default:
            /*@  shouldn't get here*/
            printf("Invalid net buffer.\n");
            fflush(stdout);
      }
   }
   usvsema(po_PDUarena_ptr->swap_buf_sema); /*@  release semaphore*/

   /* printf("Read Network is set up\n"); */

   /*@  NETWORK READ LOOP*/
   while (TRUE) {
top_of_loop:
if(PRINT_FLAG)printf("CHKPT  1\n");
      /*@  Read packet off network into temp buffer*/
      /*@    Net read must be done while semaphore is released to prevent*/
      /*@    hanging application while no packets are incoming*/
      if ((bytes_read = recvfrom(po_sock_recv, (void *) &(input_buf.v.po),
              2048, 0, (struct sockaddr *) &from, &len)) == -1) {
         perror("recvfrom()");
      }
if(PRINT_FLAG)printf("read %d bytes len = %d\n",bytes_read,len);
if(PRINT_FLAG)printf("CHKPT  2\n");
if(PRINT_FLAG)printf("Got a packet\n");

      /*@  Reject our own broadcasts*/
      if (bcmp((void *) &(from.sin_addr), (void *) my_in_addr,
                                                       IN_ADDR_SIZE) == 0) {
         continue;	/*@  don't process packet further*/
      }

      /*@  Copy new packet to open slot in current buffer*/
      /*they couldn't just store the length in the header*/
      switch(header->kind){

        /*Ones we care about*/
        case SP_describeObjectPDUKind:
          {
          /*figures what we want is the most complex of them all*/
          SP_PersistentObjectPDU *p= &(input_buf.v.po);
          SP_PersistentObjectClass  type = p->variant.describeObject.class;
          int objcnt;
              
              fflush(stdout);
              /*the simple part*/
              size = PRO_PO_DESC_OBJ_SIZE(p);
              /*now the fun part*/
              switch(type){
                case SP_objectClassPoint:
                  /* printf("Got a SP_describeObjectPDUKind"); */
                  /* printf(" of type SP_objectClassPoint\n"); */
                  size += PRO_PO_POINT_CLASS_SIZE;
                  break;
                case SP_objectClassLine:
                  /* printf("Got a SP_describeObjectPDUKind"); */
                  /* printf(" of type SP_objectClassLine\n"); */
                  size += PRO_PO_LINE_CLASS_SIZE(
                              p->variant.describeObject.variant.line.pointCount);
                  break;
                case SP_objectClassMinefield:
                  /* printf("Got a SP_describeObjectPDUKind"); */
                  /* printf(" of type SP_objectClassMinefield\n"); */
                  size += PRO_PO_MINEFIELD_CLASS_SIZE(
                          &(p->variant.describeObject.variant.minefield));
                  break;

                case SP_objectClassText:
                  /* printf("Got a SP_describeObjectPDUKind"); */
                  /* printf(" of type SP_objectClassText\n"); */
                  size += PRO_PO_TEXT_CLASS_SIZE(
                               p->variant.describeObject.variant.text.length);
                  break;

                case SP_objectClassOverlay:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                   printf(" of type SP_objectClassOverlay *** ignored\n");
                   size += PRO_PO_OVERLAY_CLASS_SIZE;
                   break;

                /*we don't care about these guys*/
                case SP_objectClassWorldState:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                  printf(" of type SP_objectClassWorldState *** ignored\n");
                  size = 0;
                  break;
                
                case SP_objectClassSector:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                   printf(" of type SP_objectClassOverlay *** ignored\n");
                   size = 0;
                   break;
                case SP_objectClassUnit:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                   printf(" of type SP_objectClassUnit *** ignored\n");
                   size = 0;
/*Not sure */                   /*size = header->length;        SAK change made 18 May !!!*/
                   break;
                case SP_objectClassStealthController:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                   printf(" of type SP_objectClassStealthController *** ignored\n");
                   size = 0;
                   break;
                case SP_objectClassHHour:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                   printf(" of type SP_objectClassHHour *** ignored\n");
                   size = 0;
                   break;
                case SP_objectClassTask:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                   printf(" of type SP_objectClassTask *** ignored\n");
                   size = 0;
                   break;
                case SP_objectClassTaskState:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                   printf(" of type SP_objectClassTaskState *** ignored\n");
                   size = 0;
                   break;
                case SP_objectClassTaskFrame:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                   printf(" of type SP_objectClassTaskFrame *** ignored\n");
                   size = 0;
                   break;
                case SP_objectClassTaskAuthorization:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                   printf(" of type SP_objectClassTaskAuthorization *** ignored\n");
                   size = 0;
                   break;
                case SP_objectClassParametricInput:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                   printf(" of type SP_objectClassParametricInput *** ignored\n");
                   size = 0;
                   break;
                case SP_objectClassParametricInputHolder:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                   printf(" of type SP_objectClassParametricInputHolder *** ignored\n");
                   size = 0;
                   break;
                case SP_objectClassExerciseInitializer:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                   printf(" of type SP_objectClassExerciseInitializer *** ignored\n");
                   size = 0;
                   break;
                case SP_objectClassFireParameters:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                   printf(" of type SP_objectClassFireParameters *** ignored\n");
                   size = 0;
                   break;
                default:
                  if(PRINT_TYPE) printf("Got a SP_describeObjectPDUKind");
                  if(PRINT_TYPE)
                  printf("of unknown type %3d\n",type);
                   size = 0;
                  break;
              }
              }
              break;
        case SP_deleteObjectsPDUKind:
              {
             /* SP_PersistentObjectPDU *tmp=(SP_PersistentObjectPDU *)&input_buf;*/
              SP_PersistentObjectPDU *tmp=(SP_PersistentObjectPDU *)&(input_buf.v.po);
              int objcnt = tmp->variant.deleteObjects.objectCount;
          
              /* printf("Got a SP_deleteObjectsPDUKind\n"); */
              /* printf("object count %d \n", objcnt); */

              /*size =  PRO_PO_DELETE_OBJS_SIZE(&input_buf.v.po,objcnt);*/
              size =  PRO_PO_DELETE_OBJS_SIZE(tmp,objcnt);   

              /* printf("size %d \n", size); */
              } 
              break;

        /*Ones we don't*/
        case SP_simulatorPresentPDUKind:
            if(PRINT_FLAG)
              printf("Got a SP_simulatorPresentPDUKind *** Ignored\n");
              size += sizeof(SP_SimulatorPresentVariant);
              break;
        case SP_objectsPresentPDUKind:
            if(PRINT_TYPE)
              printf("Got a SP_objectsPresentPDUKind *** Ignored\n");
              break;
        case SP_objectRequestPDUKind:
            if(PRINT_TYPE)
              printf("Got a SP_objectRequestPDUKind *** Ignored\n");
              break;
        case SP_setWorldStatePDUKind:
            if(PRINT_TYPE)
              printf("Got a SP_setWorldStatePDUKind *** Ignored\n");
              break;
        case SP_nominationPDUKind:
            if(PRINT_TYPE)
              printf("Got a SP_nominationPDUKind *** Ignored\n");
              break;
        default:
            if(PRINT_TYPE)
             printf("Unknown type of PDU\n");
             break;
      }
 
      /*if the size is zero, we are suppose the skip this one*/

if(PRINT_FLAG)

printf("Size == %d length == %d \n",size,header->length);
      if(size == 0) continue;


if(PRINT_FLAG)printf("CHKPT  3\n");
      /*@  Save packet, update buffer indexes*/
      /*@    (this must be done after acquiring semaphore to block*/
      /*@     buffer swap by application)*/
      if (uspsema(po_PDUarena_ptr->swap_buf_sema) == 1) { /*@  acquire semaphore*/
         /*@  Swap buffers?*/
         if (po_PDUarena_ptr->swap_buf_flag == TRUE) {
            switch (po_PDUarena_ptr->current_net_buf) {	/*@  BUF1 or BUF2*/
               /*@  get pointer to the correct buffer*/
               case (BUF1):
                  curr_buf = (STORE *) po_PDUarena_ptr->pdu_buf1;
                  po_PDUarena_ptr->nodes_in_buf1 = 0;
                  po_PDUarena_ptr->buf1_wasted = 0;
                  break;
               case (BUF2):
                  curr_buf = (STORE *) po_PDUarena_ptr->pdu_buf2;
                  po_PDUarena_ptr->nodes_in_buf2 = 0;
                  po_PDUarena_ptr->buf2_wasted = 0;
                  break;
               default:
                  /*@  booboo time*/
                  printf("Invalid net buffer.\n");
                  fflush(stdout);
            }
            /*@  Buffers swapped so start at beginning of buf*/
            oldest = newest = 0;
            circled = FALSE;
            /*@ */
            po_PDUarena_ptr->swap_buf_flag = FALSE;
         }

         /*copy it over from the buffer*/
         bcopy((void *) &input_buf.v.po, (void *) &(curr_buf[newest].v.po),size);
         curr_buf[newest].length = size;
         /*reset the size*/
         size = 0;


if(PRINT_FLAG)printf("CHKPT  5\n");
if(PRINT_FLAG)printf("CHKPT  6\n");
#ifdef DEBUG2
         printPDU((char *) &(curr_buf[newest]));
         fflush(stdout);
#endif
#ifdef DEBUG2
         j++;
         printf("%d:oldest,newest = %d,%d\n", j, oldest, newest);
         fflush(stdout);
#endif

         /*@  Save buffer state in case swap occurs*/
         if (po_PDUarena_ptr->current_net_buf == BUF1) {
            po_PDUarena_ptr->buf1_oldest = oldest;
            po_PDUarena_ptr->buf1_newest = newest;
         } else {
            /*@  BUF2*/
            po_PDUarena_ptr->buf2_oldest = oldest;
            po_PDUarena_ptr->buf2_newest = newest;
         }

         /*@  Update oldest and newest node values of circular buffer for next*/
         /*@    packet*/
         newest++;
if(PRINT_FLAG)printf("CHKPT  7\n");
         if (!circled) {	/*@  first pass through buf is the special case*/
            if (po_PDUarena_ptr->current_net_buf == BUF1) {
               po_PDUarena_ptr->nodes_in_buf1++;
            } else {
               /*@  BUF2*/
               po_PDUarena_ptr->nodes_in_buf2++;
            }
            if (newest > buf_dimension) { /*@  back to beginning*/
               newest = 0, oldest = 1;
               circled = TRUE;
            }
         } else {	/*@  buf is full; all but first pass*/
            oldest = newest + 1;	/*@  chasing our tail now*/
            if (oldest > buf_dimension)
               oldest = 0;
            if (newest > buf_dimension)
               newest = 0, oldest = 1;
            /*@  track overwritten PDUs*/
            if (po_PDUarena_ptr->current_net_buf == BUF1) {
               po_PDUarena_ptr->buf1_wasted++;
            } else {
               po_PDUarena_ptr->buf2_wasted++;
            }
         }

if(PRINT_FLAG)printf("CHKPT  8\n");
#ifdef DEBUG2
         if (po_PDUarena_ptr->current_net_buf == BUF1)
            printf("nodes_in_buf1 = %d\n", po_PDUarena_ptr->nodes_in_buf1);
         else
            printf("nodes_in_buf2 = %d\n", po_PDUarena_ptr->nodes_in_buf2);
         fflush(stdout);
#endif
      } /*@  end processing of single packet*/
      usvsema(po_PDUarena_ptr->swap_buf_sema); /*@  release semaphore*/

if(PRINT_FLAG)printf("CHKPT  9\n");
   } /*@  end while(TRUE)*/

if(PRINT_FLAG)printf("CHKPT  A\n");
} /*@  po_receiver_process()*/


void po_assign_host_id()
{
   po_host_id = (unsigned short) my_in_addr[3];    /*@  unique on subnet*/
}


void netClose()
{
   kill(po_PDUarena_ptr->receiver_pid, SIGTERM);
   sginap(5);
   close(po_sock_send);
   close(po_sock_recv);

}

int net_write(char *pdu, int length)
{
   int            bytes_sent;

   /* Actually send it*/
   if ((bytes_sent = sendto(po_sock_send, pdu, length, 0,
                             (struct sockaddr *) &po_dest, sizeof(po_dest))) == -1) {
      printf("Error Sending out packet\n");
      return (FALSE);
   }

   return (TRUE);
}
/*@  EOF*/

#endif
