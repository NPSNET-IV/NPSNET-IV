/*
  Title: tcpip

  Author: Jim Guyton, Jed Marti

  Description: TCP/IP interface library. Establish connections, poll,
    timeout and the like. All functions are prefixed with net_???.
    This code is System V compliant and has been tested on:

       HP 300
       HP 800
       SUN Sparc/SUN 4
       SUN 3
       VAX 780 (well sort of, it compiles and executes, this is subject
                to some byte swapping problems that I don't really care
		about).


  Entry Points:
    net_connect_to(portname, portnum) -- Client connect to server.
    net_listen(portnum, timeout) -- Server listen for client.
    net_poll(fds, timeout) -- Poll for I/O.
    net_read_poll(fds, timeout) -- Input poll.
    net_write_poll(fds, timeout) -- Output poll.
    net_except_poll(fds, timeout) -- Exception poll.

  Requires:
    #include "tcpip.h"


  Revision History: (Created Wed Feb 12 13:56:01 1992 from other stuff).
    Tue Feb 18 09:18:42 1992 - JBM Use tcpip.h instead of old netstruct.h,
       fix the poll functions.
    Wed Feb 19 16:17:03 1992 - JBM Move error messages here.
    Fri Feb 28 11:39:35 1992 - JBM Fix socket busy problem with suggestions
       by JDG.
    Wed Mar 11 09:48:32 1992 - JBM have a time-out on listen. Remove
       MAX_CLIENTS variable and adjust comments about VAX - curtesy review
       by JDG.
    Wed Mar 18 15:52:04 1992 - JBM fix up net_connect_to to not use up
       all available ports on retries.
    Sat Jun 13 13:17:45 1992 - JBM Fix some things pointed out by JDG.
*/

#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <setjmp.h>
#include <signal.h>
#include <errno.h>
#include "tcpip.h"

/* Stream file handles for those that need them. */
FILE *LASTFOUT, *LASTFIN;

/* Error names (see tcpip.h for numbers). */
char *tcperrlist[] = {
"Invalid portname, not a known name",                         /* EBPORT */
"The socket cannot be opened",                                /* ENSOCKET */
"Bind failed, probably busy",                                 /* EBIND */
"Connect failed",                                             /* ECONN */
"No available sockets",                                       /* ENASOCK */
"Listen failed",                                              /* ELISTEN */
"Accept failed",                                              /* EACCEPT */
"Net select failed",                                          /* ESEL */
"Set Socket reuse-address failed",                            /* EREUSE */
"Timeout on LISTEN"                                           /* ETIMEOUT */
                     };


/* Here's a jump address for listen problems. */
jmp_buf listen_err;


/* This is used during initialization. */
int on = 1;


/*
   net_connect_to(portname:char *, portnum:int):int - Actively connect to
  machine "portname" on port number "portnum". This function returns:

     EBPORT    Invalid portname, not a known name.
     ENSOCKET  The socket cannot be opened.
     EBIND     Bind failed, probably busy.
     ECONN     Connect failed.
     >0        File handle of open r+w port.
 
  This function is normally used on client side to connect with a lurking
  program (a server). Note that the stream file handle can be found in
  LASTFOUT.
*/
int net_connect_to(portname, portnum)
char *portname; int portnum;
{
  char *t;
  struct hostent *host;
  int sock;
  struct protoent *getprotobyname();
  struct  sockaddr_in my_addr;
  struct  sockaddr_in his_addr;


  host = gethostbyname(portname);
  if (host == NULL) return(EBPORT);

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) return(ENSOCKET);

   /* don't care about local port number */
  memset(&my_addr, 0, sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = INADDR_ANY;
  my_addr.sin_port = htons(0);           /* any portnum */

  /* The bind shouldn't fail since we're asking for any available port */
  if (bind(sock, &my_addr, sizeof (my_addr)) < 0)
    { close(sock); return(EBIND); }

  his_addr.sin_family = host->h_addrtype;

  /* set ip # */
  memcpy(&his_addr.sin_addr, host->h_addr_list[0], host->h_length);
  his_addr.sin_port = portnum;

  if (connect(sock, &his_addr, sizeof (his_addr)) < 0)
    { close(sock); return(ECONN); }

  LASTFOUT = (FILE *)fdopen(sock, "r+w");     /* turn it into a FILE */
  return (fileno(LASTFOUT));
}





/*
  listencatcher() -- This is called when a listen times out. 
*/
void listencatcher(int junk)
{
  longjmp(listen_err, 1);
}




/*
  net_listen(portnum:int, timeout:int):int -- Listen on port "portnum" but 
 for only "timeout" seconds.  Return a file handle for open connection.
 Call more than once with different ports to accept more than one connection.
 There is a definite limit on the number accepted (30 or so unless you do
 the extended file handle stuff). Error returns are < 0:

   ENASOCK    No available sockets.
   EREUSE     Can't reuse address.
   EBIND      Bind failed (probably busy).
   ELISTEN    Listen failed.
   EACCEPT    Accept failed.
   ETIMEOUT   Timeout.

*/
int net_listen(portnum, timeout)
int portnum;                    /* local socket to listen on */
int timeout;                    /* time in seconds to wait. */
{
  static int lastport= -1;          /* make it re-useable */
  static struct sockaddr_in my_addr;
  static struct sockaddr_in his_addr;
  static int sock;                  /* socket handle */
  int fd;                /* returned by accept */
  int from_len;
  
  if (portnum != lastport) {
    lastport = portnum;
    
    /* get a socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return(ENASOCK);
    
    /* setup portnum */
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(portnum);
    
    /* Get the port */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)))
      return(EREUSE);
    if (bind(sock, &my_addr, sizeof(my_addr)) < 0) return(EBIND);

    /* do the listen */
    if (listen(sock, 1) < 0) return(ELISTEN); }
 
  from_len = sizeof(his_addr);

  /* Now setup for a timeout on the accept. */
  signal(SIGALRM, listencatcher);
  alarm((unsigned long)timeout);
  if (setjmp(listen_err) == 0)
    { fd = accept(sock, &his_addr, &from_len);
      close(sock); }
  else fd = -99;

  /* Turn off alarm. */
  alarm(0);
  signal(SIGALRM, SIG_IGN);

  /* Return appropriate error code if necessary. */
  close(sock);     /* This helps free up the busy problem. */
  if (fd == -99) return(ETIMEOUT);
  else if ( fd <  0) return(EACCEPT);

  LASTFIN = (FILE *)fdopen(fd, "r+w");
  return (fileno(LASTFIN));
}


/*
   net_poll(int:fds, timeout:int):int - Poll the set of file descriptors.
  Return the first one with something to do on it. Priority is given to
  exception, then read, then write, events. The functions
  net_read_poll, net_write_poll and net_except_poll perform the same
  functions, but for the appropriate functions. fds is a bit string with
  a bit on for each active file descriptor. This is created by:

     (1 << fd[0]) || (1 << fd[1]) ...

  For a "timeout" value of 0, this is a poll, otherwise, timeout is the
  number of seconds to wait. Returns:
    <0      Select failed.
    =0      Timeout.
    >0      First active event.
*/
int net_poll(fds, timeout)
int fds, timeout;
{
  int readfds, writefds, exceptfds, stat;
  struct timeval tout;
  register int i;

  /* Set the timeout interval. */
  tout.tv_sec = timeout;
  tout.tv_usec = 0;

  /* Set the selections. */
  readfds = fds;
  writefds = fds;
  exceptfds = fds;
  stat = select(32, &readfds, &writefds, &exceptfds, &tout);

  /* Determine what to return. */
  if (stat <= 0) return(ESEL);
  for (i=0; i<32; i++) if (((1 << i) & fds) & readfds) break;
  if (i < 32) return(i);
  for (i=0; i<32; i++) if (((1 << i) & fds) & writefds) break;
  if (i < 32) return(i);
  for (i=0; i<32; i++) if (((1 << i) & fds) & exceptfds) break;
  if (i < 32) return(i);
  return(-99);
}


/*
   net_read_poll(int:fds, timeout:int):int - Poll the set of file
  descriptors "fds". Return the first one with some read activity
  to do on it. fds is a bit string with a bit on for each active
  file descriptor. This is created by:

     (1 << fd[0]) || (1 << fd[1]) ...

  For a "timeout" value of 0, this is a poll, otherwise, timeout is the
  number of seconds to wait. Returns:
    <0      Select failed.
    =0      Timeout.
    >0      First active event.
*/
int net_read_poll(fds, timeout)
int fds, timeout;
{
  int readfds, stat;
  struct timeval tout;
  register int i;

  /* Set the timeout interval. */
  tout.tv_sec = timeout;
  tout.tv_usec = 0;

  /* Set the selections. */
  readfds = fds;
  stat = select(32, &readfds, 0, 0, &tout);

  /* Determine what to return. */
  if (stat <= 0) { perror("net_read_poll"); return(ESEL); }
  for (i=0; i<32; i++) if (((1 << i) & fds) & readfds) break;
  if (i < 32) return(i);
  return(-99);
}



/*
   net_write_poll(int:fds, timeout:int):int - Poll the set of file
  descriptors "fds". Return the first one with some write activity
  to do on it. fds is a bit string with a bit on for each active
  file descriptor. This is created by:

     (1 << fd[0]) || (1 << fd[1]) ...

  For a "timeout" value of 0, this is a poll, otherwise, timeout is the
  number of seconds to wait. Returns:
    <0      Select failed.
    =0      Timeout.
    >0      First active event.
*/
int net_write_poll(fds, timeout)
int fds, timeout;
{
  int writefds, stat;
  struct timeval tout;
  register int i;

  /* Set the timeout interval. */
  tout.tv_sec = timeout;
  tout.tv_usec = 0;

  /* Set the selections. */
  writefds = fds;
  stat = select(32, 0, &writefds, 0, &tout);

  /* Determine what to return. */
  if (stat <= 0) return(ESEL);
  for (i=0; i<32; i++) if (((1 << i) & fds) & writefds) break;
  if (i < 32) return(i);
  return(-99);
}



/*
   net_except_poll(int:fds, timeout:int):int - Poll the set of file
  descriptors "fds". Return the first one with some write activity
  to do on it. fds is a bit string with a bit on for each active
  file descriptor. This is created by:

     (1 << fd[0]) || (1 << fd[1]) ...

  For a "timeout" value of 0, this is a poll, otherwise, timeout is the
  number of seconds to wait. Returns:
    <0      Select failed.
    =0      Timeout.
    >0      First active event.
*/
int net_except_poll(fds, timeout)
int fds, timeout;
{
  int exceptfds, stat;
  struct timeval tout;
  register int i;

  /* Set the timeout interval. */
  tout.tv_sec = timeout;
  tout.tv_usec = 0;

  /* Set the selections. */
  exceptfds = fds;
  stat = select(32, 0, &exceptfds, 0, &tout);

  /* Determine what to return. */
  if (stat <= 0) return(ESEL);
  for (i=0; i<32; i++) if (((1 << i) & fds) & exceptfds) break;
  if (i < 32) return(i);
  return(-99);
}

