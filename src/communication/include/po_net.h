/* File:	disdefs.h
 * Description:	Common info
 */

#ifndef __DISDEFS_H
#define __DISDEFS_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/soioctl.h>
#include <ulocks.h>
#include <sys/prctl.h>
#include <signal.h>		

#ifdef __cplusplus
extern "C" {
#endif

extern int errno;

#ifndef FALSE
#   define FALSE		0
#endif
#ifndef TRUE
#   define TRUE			!FALSE
#endif

#undef SYSLOG
#ifdef SYSLOG
#   include <syslog.h>
#   define SYSLOG_FACILITY	LOG_LOCAL7
#   define DIS_APP_NAME		"DIS Lib"
#endif

/*
#define UDP_SEND_PORT	2099
#define UDP_RECV_PORT	3000
*/
#define UDP_SEND_PORT	3001
#define UDP_RECV_PORT	(UDP_SEND_PORT+1)

#define MAX_NUM_PDUS                    32

/* Check for interfaces with "netstat -rn"*/
#define IN_ADDR_SIZE	4
#define MAX_INTERF	6
#define BCAST_INTERF	"ec0"	/*@ local for indigos*/


/*@ DIS*/
#ifdef COMMENT
#include <pdu.h>	/*@ list this first*/
#include <appearance.h>
#include <articulat.h>
#include <category.h>
#include <country.h>
#include <datum.h>
#include <radar.h>
#include <repair.h>
#include <sites.h>
#endif

/*@ PO network stuff*/
#include "basic.h"
#include "p_po.h"
#include "p_size.h"

/*@ net_read() status return struct*/
struct read_po_stat {
   SP_PersistentObjectPDUKind	rs_type;
   int		rs_wasted;	/*@ wasted (overwritten) PDUs*/
   int		rs_swap_buffers_flag;
};

/*the storage format*/
typedef struct {
  int length;
  union{
    SP_PersistentObjectPDU po;
    char filler[2048];
  }v;
} STORE;

/*@ Prototypes*/
void	po_assign_host_id();
int	init_po_arena();
void	netClose();
int netOpen(char *interf, int buf_len,int port);
STORE *netRead(struct read_po_stat *rstat);
void	po_receiver_process(void *);
void	po_swap_net_buffers();
int net_write(char *pdu, int length);

/*
void	bcopy(const void *src, void *dst, int length);
int	bcmp(const void *b1, const void *b2, int length);
void	bzero(void *b, int length);
*/
#include <bstring.h>
int	ioctl(int fildes, int request, ...);
unsigned sleep (unsigned seconds);
char	*strcat (char *s1, const char *s2);
char	*strncat (char *s1, const char *s2, size_t n);
int	strcmp (const char *s1, const char *s2);
int	strncmp (const char *s1, const char *s2, size_t n);
char	*strcpy (char *s1, const char *s2);
size_t	strlen (const char *s);
void	syslog(int priority, char *message, ...);


/*@ EOF */
#ifdef __cplusplus
}
#endif

#endif
