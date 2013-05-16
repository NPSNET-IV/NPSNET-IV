/*
                                \          /
                                 +--------+
                                 |        |
                                 |        |
                                 |        |
                                 | SEMINT |
                                 |        |
                                 |        |
                                 |        |
                                 +--------+
                                /          \

  Title: tcpip.h
  Author: Jed Marti
  Description: TCP/IP error bindings and friends.
  Revision History: (Created Tue Feb 18 09:17:51 1992)
    Wed Feb 19 16:17:47 1992 - JBM Remove error list.
    Wed Mar 11 09:52:47 1992 - JBM Add timeout on listen, remove MAX_CLIENTS.

*/

#ifndef __TCPIP_H__
#define __TCPIP_H__

/* net_connect_to fail codes */
#define EBPORT      -1       /* Invalid portname, not a known name */
#define ENSOCKET    -2       /* The socket cannot be opened */
#define EBIND       -3       /* Bind failed, probably busy */
#define ECONN       -4       /* Connect failed */
#define ENASOCK     -5       /* No available sockets */
#define ELISTEN     -6       /* Listen failed */
#define EACCEPT     -7       /* Accept failed */
#define ESEL        -8       /* Net select failed */
#define EREUSE      -9       /* Reuse socket failed. */
#define ETIMEOUT   -10       /* Timeout on listen. */

/*Function prototypes*/
#ifdef __cplusplus
extern "C" {
#endif

int net_connect_to(char *, int); 
int net_listen(int, int); 
int net_poll(int, int) ;
int net_read_poll(int, int);
int net_write_poll(int, int);
int net_except_poll(int, int);
void listencatcher(int);

#ifdef __cplusplus
}
#endif

#endif
