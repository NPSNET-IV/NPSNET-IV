// File: <sound.cc>

/*
 * Copyright (c) 1995,
 *    Naval Postgraduate School
 *    Computer Graphics and Video Laboratory
 *    NPSNET Research Group
 *    npsnet@cs.nps.navy.mil
 *
 *
 *    Permission to use, copy, and modify this software and its
 * documentation for any non-commercial purpose is hereby granted
 * without fee, provided that (i) the above copyright notices and the
 * following permission notices appear in ALL copies of the software
 * and related documentation, and (ii) The Naval Postgraduate School
 * Computer Graphics and Video Laboratory and the NPSNET Research Group
 * be given written credit in your software's written documentation and
 * be given graphical credit on any start-up/credit screen your 
 * software generates.
 *    This restriction helps justify our research efforts to the
 * sponsors who fund our research.
 *
 *    Do not redistribute this code without the express written
 * consent of the NPSNET Research Group. (E-mail communication and our
 * confirmation qualifies as written permission.) As stated above, this 
 * restriction helps justify our research efforts to the sponsors who
 * fund our research.
 *
 *    This software was designed and implemented at U.S. Government
 * expense and by employees of the U.S. Government.  It is illegal
 * to charge any U.S. Government agency for its partial or full use.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * E-Mail addresses:
 *    npsnet@cs.nps.navy.mil
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *    npsnet-info@cs.nps.navy.mil
 *       Contact principle investigators.
 *       Overall research project information and funding.
 *       Requests for demonstations.
 *
 * Thank you to our sponsors:  ARL, STRICOM, TRAC, ARPA and DMSO.
 */
// NPSMONO Version 4.0
// Written by:  Paul T. Barham and John T. Roesli
// Naval Postgraduate School, Computer Science Department
// Code CS, Monterey, CA 93940-5100 USA


#include "sound.h"

#ifdef AUDIO

/* For the audio types */
#include <audio.h>

#endif


/* For semaphores, locks and arenas */
#include <ulocks.h>

/* For standard Unix open and file operations */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* For standard Unix close and other stuff */
#include <unistd.h>

/* For conversions, atoi and others */
#include <stdlib.h>

/* For string operations */
#include <string.h>

/* For math operations */
#include <math.h>

/* For signal operations */
#include <signal.h>

/* For C++ file and terminal I/O */
#include <iostream.h>
#include <fstream.h>

/* InitManager */
#include "imstructs.h"
#include "imclass.h"

extern InitManager *initman;

#ifdef AUDIO

#include "fileio.h"

/*
 * some of the audio parameters in an AIFF file
 */
typedef struct
{
    long samprate;
    long nchannels;
    long sampwidth;
} audio_params_t;


/*
 * all chunks consist of a chunk header followed by some data
 *
 * WARNING: the spec says that every chunk must contain an even number
 * of bytes. A chunk which contains an add number of bytes is padded with
 * a trailing zero byte which is NOT counted in the chunk header's size
 * field.
 */
typedef struct
{
    char id[4];
    long size;
} chunk_header_t;

#define CHUNK_ID     4
#define CHUNK_HEADER 8

typedef struct
{
    chunk_header_t header;
    int file_position;  /* not in AIFF file */
    char type[4]; /* should contain 'AIFF' for any audio IFF file */
} form_chunk_t;

#define FORM_CHUNK      12  /* including the header */
#define FORM_CHUNK_DATA 4

#define COMM_CHUNK      26   /* including the header */
#define COMM_CHUNK_DATA 18

typedef struct
{
    chunk_header_t header;
    short nchannels;
    unsigned long nsampframes;
    short sampwidth;
    long samprate;              /* not in AIFF file */
} comm_chunk_t;


#define SSND_CHUNK      16   /* including the header */
#define SSND_CHUNK_DATA 8

typedef struct
{
    chunk_header_t header;
    long samplecount; /* the number of samples in the sound*/
    long samprate;
    long numchan;
    long sampwidth;
    float length; /*sound length in seconds*/
    long sample_length_bytes; /* not in AIFF file */
    char *sample; /*the pointer to the buffer containing the sounds*/
    float range;
    int valid;
} SOUNDTYPE;

typedef struct
   {
   ulock_t datalock;
   usema_t *sema1;
   usema_t *sema2;
   usema_t *quitsema;
   int noise;
   int lod;
   int pid;
   int killflag;
   } port_data_rec;
/*
 * we're going to nail down the value of infinity for the IEEE conversion
 * routine below: math.h yields to local definition of HUGE_VAL
 */

typedef union
{
    unsigned char b[2];
    short         s;
} align_short_t;

typedef union
{
    unsigned char b[4];
    long         l;
} align_long_t;

union
{
    unsigned long n[2];   /* initialized in main */
    double        d;
} _inf_union;

#define A_HUGE_VAL _inf_union.d;

static int fd; /* the file handle for the sound*/
static usptr_t *arena = NULL;
static int L_num_arena = 0;

static ALport audioport[NUMAPORTS]; /*the array of audio ports*/
static SOUNDTYPE soundfiles[NUMSOUNDS][MAXSNDLOD]; /* the array of sounds*/
static int soundLODS[NUMSOUNDS];
volatile static port_data_rec portdata[NUMAPORTS];


/* Local function prototypes */

inline double sqr(const double);

static int read_sound_file( char * );

static int init_audio(ALport &, SOUNDTYPE *);

static void port_deamon(void *);

static int setupaudioports();

static int readsoundfile(char *,int,float);

static int insert_sound_for_LOD ( int, SOUNDTYPE * );

static int read_chunk_header(chunk_header_t *);

static int read_form_chunk(chunk_header_t *, form_chunk_t *);

static int read_comm_chunk(chunk_header_t *, comm_chunk_t *,SOUNDTYPE *);

static void read_ssnd_chunk(chunk_header_t *, SOUNDTYPE *);

static void skip_chunk(chunk_header_t *);

static double ConvertFromIeeeExtended(char *);


inline double sqr ( const double x )
   {
   return (x*x);
   }

#endif


int sound_capable ()
   {
   int n;

   if ((n = open("/dev/hdsp/hdsp0master", O_RDWR)) < 0)
      {
      return FALSE;
      }
   else
      {
      close(n);
      return TRUE;
      }
   }


#ifdef AUDIO

static int
read_sound_file ( char *pathname )
{
   char local_pathname[NPS_MAXPATH];
   /*
   int quit = FALSE;
   */

   struct Sound	im_sounds[MAX_SOUND_EVENTS];
   int		found;
   ifstream	testfile;

   initman->get_sounds((struct Sound**) im_sounds);

   for (int i = 0; i < MAX_SOUND_EVENTS; i++) {
      if (im_sounds[i].num_events <= 0) {
         // No events for this sound index
         continue;
      }
      for (int j = 0; j < im_sounds[i].num_events; j++) {
         if ( pathname != NULL ) {
            strcpy ( local_pathname, pathname );
         }
         else {
            strcpy ( local_pathname, "" );
         }
         found = npsFindFile(im_sounds[i].Events[j].filename,local_pathname);
         /*
         testfile.open(im_sounds[i].Events[j].filename, ios::in);
         found = testfile.good();
         testfile.close();
         */

         if (found) {
            readsoundfile(local_pathname, i,
                          im_sounds[i].Events[j].range);
         } else {
            cerr << "\tUnable to locate sound file: "
                 << im_sounds[i].Events[j].filename << endl;
         }
      }
   }

   return (TRUE);
 
/*
   if ( pathname != NULL ) { 
      strcpy ( local_pathname, pathname );
   }
   else {
      strcpy ( local_pathname, "" );
   }

   if ( !npsFindFile(filename,local_pathname) ) {
      return FALSE;
   }

   ifstream sound_file ( local_pathname );
   if (!sound_file ) {
      return FALSE;
   }

   while ( !quit ) {
      static int sound_index = 0;
      static char sound_filename[NPS_MAXFILENAME];
      static float sound_range = MAXSNDDIS;
      static int read_valid = TRUE;
      
      read_valid = npsfscanf ( sound_file, "%i%s%f",
                               &sound_index, sound_filename, &sound_range); 
      switch ( read_valid ) {
         case NPSFSCANF_EOF:
            quit = TRUE;
            break;
         case NPSFSCANF_UNDERFLOW:
            read_valid = npsfscanf ( sound_file, "%i%s",
                                     &sound_index, sound_filename );
            if ( read_valid != TRUE ) {
               print_npsfscanf_error ( read_valid, "In read_sound_file." );
            }
            else {
               sound_range = MAXSNDDIS;            
            }
            break;
         case TRUE:
            break;
         default:
            print_npsfscanf_error ( read_valid, "In read_sound_file." );
            break;
      }
      
      if ( read_valid == TRUE ) {
         if ( pathname != NULL ) {
            strcpy ( local_pathname, pathname );
         }
         else {
            strcpy ( local_pathname, "" );
         }
         if ( !npsFindFile(sound_filename,local_pathname) ) {
            cerr << "ERROR: Loading sounds - " << endl;
            cerr << "\tUnable to locate sound file: " << sound_filename << endl;
         }
         else {
            readsoundfile ( local_pathname, sound_index, sound_range );
         }
      }   

   }

   sound_file.close();
   return TRUE;
*/
}


static int
determine_lod_index ( int sound_index, float distance )
{
   int count = 0;
   while ( (count < MAXSNDLOD) && (soundfiles[sound_index][count].valid) &&
           (distance > soundfiles[sound_index][count].range) ) {
      count++;
   }
   return count;
}


int playsound(LocationRec *ourloc, 
              LocationRec *sndloc, int noise, int priority)
/*the the designated noise*/
{
  int ix;
  long lefttoplay[NUMAPORTS];
  int minleft=-1;
  float distance;
  int lod = 0;
  static int sndpriority[NUMAPORTS];
  int playsound = FALSE;

  if ( (noise < 0) || (noise >= NUMSOUNDS) )
     return FALSE;

  distance = sqrt ( sqr(ourloc->x - sndloc->x) +
                    sqr(ourloc->y - sndloc->y) +
                    sqr(ourloc->z - sndloc->z) );

  lod = determine_lod_index ( noise, distance );

  if ( lod >= MAXSNDLOD )
      return FALSE;
  if ( !soundfiles[noise][lod].valid )
     return FALSE;

  if ( priority == 0 )
     {
#ifdef SOUND_DEBUG
     cerr << "SOUND:\tComputing priority based on distance for " << 
              noise << endl;
     cerr << "\tDistance is " << distance << " meters." << endl;
#endif
     if (distance >= MAXSNDDIS )
        return FALSE;
     priority = int(ffloor(( distance * MAXPRIORITY)
                        /MAXSNDDIS)) + 1;
#ifdef SOUND_DEBUG
     cerr << "\tComputed priority is " << priority << endl;
#endif 
     }
  else if ( priority < 0 )
     {
     priority = -1;
#ifdef SOUND_DEBUG
     cerr << "SOUND:\tPriority for sound < 0 ... using -1." << endl;
#endif
     }
  else if ( priority > MAXPRIORITY )
     {
     priority = MAXPRIORITY;
#ifdef SOUND_DEBUG
     cerr << "SOUND:\tPriority > " << MAXPRIORITY << " ... using "
          << MAXPRIORITY << endl;
#endif
     }
#ifdef SOUND_DEBUG
  else
     cerr << "SOUND:\tPriority supplied is " << priority << endl;
#endif
 
      
  /*try to find an empty port*/
  for(ix=0;ix<NUMAPORTS;ix++)
    {
    if ( ( audioport[ix] != NULL ) && ( !ustestlock(portdata[ix].datalock) ) )
       {
       if((lefttoplay[ix] = ALgetfilled(audioport[ix])) == 0)
         {
         /*empty port*/
#ifdef SOUND_DEBUG
         cerr << "\tPort " << ix << " is empty." << endl;
         cerr << "\tPlaying sound, " << noise << ", on channel " <<
              ix << endl;
#endif
         sndpriority[ix] = priority;
         portdata[ix].noise = noise;
         portdata[ix].lod = lod;
         uspsema ( portdata[ix].sema2 );
         usvsema ( portdata[ix].sema1 );
         uspsema ( portdata[ix].sema1 );
         usvsema ( portdata[ix].sema2 );

         return TRUE;
         }
       else 
         {
         if ( minleft < 0 )
            minleft = ix;
         if ( priority < sndpriority[ix] )
            {
            if ( sndpriority[ix] > sndpriority[minleft] )
               {
               minleft = ix;
               playsound = TRUE;
               }
            else if ( sndpriority[ix] == sndpriority[minleft] )
               {
               /*find out which port has the least to go*/
               if(lefttoplay[ix] <= lefttoplay[minleft])
                  {
                  minleft = ix;
                  playsound = TRUE;
                  }
               }
            }
          }
       }
   }
  /*we got here so all sounds are playing*/
  /*be kind of draconian and kill the port with the least #of sounds and 
    then reopen it and send the sound to it*/
  if ( playsound )
      {
#ifdef SOUND_DEBUG
      cerr << "\tEVICT: Playing sound, " << noise << ", on channel " <<
           minleft << endl;
#endif
      sndpriority[minleft] = priority;
      portdata[minleft].noise = noise;
      portdata[minleft].lod = lod;
      uspsema ( portdata[minleft].sema2 );
      usvsema ( portdata[minleft].sema1 );
      uspsema ( portdata[minleft].sema1 );
      usvsema ( portdata[minleft].sema2 );
      return TRUE;
      }
   else
      return FALSE;

}


static void port_deamon ( void *portnum )
   {
   long theportLong = long(portnum);
   int theport = int (theportLong);
   int soundnum, lodnum;

   signal ( SIGTERM, SIG_DFL );
   uspsema ( portdata[theport].quitsema );

   while ( !portdata[theport].killflag )
      {

      uspsema ( portdata[theport].sema1 );
      ussetlock ( portdata[theport].datalock );
      usvsema ( portdata[theport].sema1 );

      soundnum = portdata[theport].noise;
      lodnum = portdata[theport].lod;

      if ( !portdata[theport].killflag )
         {

         if(!(init_audio(audioport[theport],
                         &soundfiles[soundnum][lodnum])))
            cerr << "Error opening the audio port " << theport << endl;
         else
            ALwritesamps(audioport[theport],
                         soundfiles[soundnum][lodnum].sample,
                         soundfiles[soundnum][lodnum].samplecount);
   
         }

      uspsema ( portdata[theport].sema2 );
      usunsetlock ( portdata[theport].datalock );
      usvsema ( portdata[theport].sema2 );
     }

   usvsema ( portdata[theport].quitsema );
   portdata[theport].killflag = FALSE;
   exit(0);

   }


void init_sounds ( char *pathname )
   {
   for ( int i = 0; i < NUMSOUNDS; i++ ) {
      for ( int j = 0; j < MAXSNDLOD; j++ ) {
         soundfiles[i][j].valid = FALSE;
         soundfiles[i][j].range = MAXSNDDIS;
      }
      soundLODS[i] = 0;
   }
   if (!read_sound_file (pathname ))
      {
      //cerr << "Sound file not found:  " << sound_file << endl;
      cerr << "Sound file not found:  " << "obsolete" << endl;
      exit(0);
      }
   setupaudioports();
   }


static int init_audio (ALport &theport, SOUNDTYPE *thesound )
   {
   long pvbuf[2];
   ALconfig audio_port_config;

   pvbuf[0] = AL_OUTPUT_RATE;
   pvbuf[1] = thesound->samprate;
   ALsetparams(AL_DEFAULT_DEVICE,pvbuf,2);

   audio_port_config = ALnewconfig();
   if ( audio_port_config == 0 )
      {
      cerr << "Error allocating ALconfig structure.\n";
      return FALSE;
      }
   ALsetwidth(audio_port_config,thesound->sampwidth);
   ALsetchannels(audio_port_config,thesound->numchan);
//   if ( thesound->numchan == AL_MONO )
//      ALsetqueuesize(audio_port_config, 262139);
//   else
//      ALsetqueuesize(audio_port_config, 131069);

   ALcloseport(theport);
   theport = ALopenport("Sound", "w", audio_port_config);
   ALfreeconfig(audio_port_config);
   if(theport == NULL)
      return FALSE;
   else
      return TRUE;

   }

 
static int setupaudioports()
/*open the audio ports in the default mode*/
{
  int ix;
  char arena_name[255];

  L_num_arena++;
  sprintf ( arena_name, "/tmp/NPSNET.audio.arena.%d.%d",
                        getpid(), L_num_arena );
  usconfig(CONF_ARENATYPE, US_SHAREDONLY);
  arena = usinit ( arena_name );
  usconfig ( CONF_CHMOD, arena, 0666 );
  signal(SIGCLD,SIG_IGN);

  for(ix=0;ix<NUMAPORTS;ix++)
     {
     portdata[ix].pid = -1;
     portdata[ix].sema1 = NULL;
     portdata[ix].sema2 = NULL;
     portdata[ix].datalock = NULL;
     portdata[ix].killflag = FALSE;
     audioport[ix] = ALopenport("Sound", "w", NULL);
     if(audioport[ix] == NULL)
        {
        cout << "Error opening the audio port " << ix << endl;
        return(FALSE);
        }
     else
        {
        portdata[ix].sema1 = usnewsema ( arena, 1 );
        uspsema ( portdata[ix].sema1 );
        portdata[ix].sema2 = usnewsema ( arena, 1);
        portdata[ix].quitsema = usnewsema ( arena, 1);
        portdata[ix].datalock = usnewlock ( arena );
        usinitlock ( portdata[ix].datalock );
        portdata[ix].noise = 0;
        portdata[ix].pid = sproc ( port_deamon, PR_SALL, ix ); 
        }
     }
  return(TRUE);
}


void closeaudioports()
/*close the audio ports*/
{
  int ix;
   for(ix=0;ix<NUMAPORTS;ix++){

      portdata[ix].killflag = TRUE;
      uspsema ( portdata[ix].sema2 );
      usvsema ( portdata[ix].sema1 );
      uspsema ( portdata[ix].sema1 );
      usvsema ( portdata[ix].sema2 );
      uspsema ( portdata[ix].quitsema );


      if ( audioport[ix] != NULL )
         ALcloseport(audioport[ix]);

      
      if ( portdata[ix].killflag )
         kill ( portdata[ix].pid, SIGTERM );
      if ( portdata[ix].sema1 != NULL )
         usfreesema ( portdata[ix].sema1, arena );
      if ( portdata[ix].sema2 != NULL )
         usfreesema ( portdata[ix].sema2, arena );
      if ( portdata[ix].datalock != NULL )
         usfreelock ( portdata[ix].datalock, arena );
      if ( portdata[ix].quitsema != NULL )
         usfreesema ( portdata[ix].quitsema, arena );
   }

   usdetach(arena);
}



static int readsoundfile(char *filename,int sound,float soundrange)
/*read the file in from the disk and store it in the buffer*/
{

    int n;  /*a dummy place holder*/
    chunk_header_t chunk_header;
    form_chunk_t form_data;
    comm_chunk_t comm_data;
    SOUNDTYPE ssnd_data;

    if(NUMSOUNDS <= sound){
      cerr << "Only 0 to " << NUMSOUNDS-1 << " sound definitions are allowed\n";
      return FALSE; 
    }
    if ((fd = open(filename, O_RDONLY)) < 0) {
        cerr << "Can't open sound file: " <<  filename << endl;
        return FALSE; 
    }


    if ((n = read_chunk_header(&chunk_header)) != CHUNK_HEADER) {
        cerr << "Failed to read FORM chunk header\n";
        return FALSE;
    }
    if (strncmp(chunk_header.id, "FORM", 4)){       /* form container */
        cerr << "Couldn't find FORM chunk id\n";
        return FALSE; 
    }

    if ( !read_form_chunk(&chunk_header, &form_data) )
       {
       cerr << "Bad form chunk for file:  " << filename << endl;
       return FALSE;
       }

    /*loop on the local chunks */
    while ((n = read_chunk_header(&chunk_header)) != 0) {
        if (n != CHUNK_HEADER) {
            cerr << "Failed to read a chunk header\n";
            return FALSE; 
        }
        if ( chunk_header.size <= 0 ) 
           skip_chunk(&chunk_header);
        else if (!strncmp(chunk_header.id, "COMM", 4)){    /* common set up data*/
            read_comm_chunk(&chunk_header, &comm_data, &ssnd_data);
        }
        else if (!strncmp(chunk_header.id, "SSND", 4)){  /* sound data */
            read_ssnd_chunk(&chunk_header, &ssnd_data);
        }
        else if ((!strncmp(chunk_header.id, "MARK", 4))  /* marker */
              || (!strncmp(chunk_header.id, "INST", 4))  /* instrument */
              || (!strncmp(chunk_header.id, "APPL", 4))   /* appl specific */
              || (!strncmp(chunk_header.id, "MIDI", 4))  /* midi data */
              || (!strncmp(chunk_header.id, "AESD", 4))   /* audio  rec */
              || (!strncmp(chunk_header.id, "COMT", 4))   /* comments */
              || (!strncmp(chunk_header.id, "NAME", 4))   /* text */
              || (!strncmp(chunk_header.id, "AUTH", 4))   /* text */
              || (!strncmp(chunk_header.id, "(c) ", 4))   /* text */
              || (!strncmp(chunk_header.id, "ANNO", 4)))   /* text */
        {
            skip_chunk(&chunk_header);
        }
        else {
            cout << "Bad chunk id " << hex 
                 << int(chunk_header.id[0]) << " "
                 << int(chunk_header.id[1]) << " "
                 << int(chunk_header.id[2]) << " "
                 << int(chunk_header.id[3]) << dec << endl;
        }
    } /* while */

    ssnd_data.range = soundrange;
   
    if (!insert_sound_for_LOD ( sound, &ssnd_data )) {
       cerr << "Too many sound LODs for entry # " << sound << endl;
       cerr << "\t" << filename << " not loaded as an LOD." << endl;
       return FALSE;
    }
 
    return TRUE;
}


static int
insert_sound_for_LOD ( int sound, SOUNDTYPE *ssnd_data )
{
   if ( soundLODS[sound] >= MAXSNDLOD ) {
      return FALSE;
   }
   else {
      int index = 0;
      while ( (index < soundLODS[sound]) &&
              (ssnd_data->range > soundfiles[sound][index].range) ) {
         index++;
      }
      for ( int count = soundLODS[sound]; count > index; count-- ) {
         soundfiles[sound][count] = soundfiles[sound][count-1];
      }
      soundfiles[sound][index] = *ssnd_data;
      soundfiles[sound][index].valid = TRUE;
      soundLODS[sound] += 1;
   }

   return TRUE;
}



/*
 * R E A D _ C H U N K _ H E A D E R
 */
static int read_chunk_header(chunk_header_t *chunk_header)
{
    align_long_t align_long;
    char buf[CHUNK_HEADER];
    int i;
    int n;

    if ((n = read(fd, buf, CHUNK_HEADER)) != CHUNK_HEADER) {
        return(n);
    }
    for (i=0; i<4; i++) {
        chunk_header->id[i] = buf[i];
    }
    for (i=0; i<4; i++) {
        align_long.b[i] = buf[i+4];
    } 
    chunk_header->size = align_long.l;
    return(CHUNK_HEADER);
}


/*
 * R E A D _ form_chunk
 */
static int read_form_chunk(chunk_header_t *chunk_header, form_chunk_t *form_data)
{
    int n;
    char buf[FORM_CHUNK_DATA];
    form_chunk_t *temp;
 
    temp = form_data;

    if (chunk_header->size < 0) {
        cerr << "Invalid FORM chunk data size " << chunk_header->size << endl;
        return FALSE; 
    }
    else if (chunk_header->size == 0) {
        cerr << "FORM chunk data size = 0\n" << endl;
        return FALSE; 
    } 
    if ((n = read(fd, buf, FORM_CHUNK_DATA)) != FORM_CHUNK_DATA) {
        cerr << "Couldn't read AIFF identifier\n";
        return FALSE; 
    }
    if (strncmp(buf, "AIFF", 4)) {
       cerr << "The file does not have an AIFF identifier\n";
       cerr << "Identifier is " << buf[0] << buf[1] << buf[2]
            << buf[3] << endl;
       return FALSE; 
    }
   return TRUE;
}


/*
 * R E A D _ C O M M _ C H U N K
 */
static int read_comm_chunk(chunk_header_t *chunk_header, 
                    comm_chunk_t *comm_data,SOUNDTYPE *ssnd_data)
{
    int n;
    char *buf, *bufp;
    int i;
    align_short_t align_short;
    align_long_t  align_long;
    chunk_header_t *temp;

    temp = chunk_header;
    buf = (char *)malloc(COMM_CHUNK_DATA + 1);

    if ((n = read(fd,  buf, COMM_CHUNK_DATA)) != COMM_CHUNK_DATA) {
        cerr << "Failed to read COMM chunk data. Expected "
             << COMM_CHUNK_DATA << " bytes, got " << n << endl;
        return FALSE; 
    }
    bufp = buf;
    for (i=0; i<2; i++) {
       align_short.b[i] = *bufp++;
    }
    comm_data->nchannels = align_short.s;
    for (i=0; i<4; i++) { 
       align_long.b[i]  = *bufp++;
    }
    ssnd_data->samplecount = comm_data->nsampframes = align_long.l;
    for (i=0; i<2; i++) {
       align_short.b[i] = *bufp++;
    }
    comm_data->sampwidth = align_short.s; 

    /*
     * the sample rate value from the common chunk is an 80-bit IEEE extended
     * floating point number:
     * [s bit] [15 exp bits (bias=16383)] [64 mant bits (leading 1 not hidden)]
     *
     */
    comm_data->samprate = (long)ConvertFromIeeeExtended(bufp);
    bufp+=10;
    /*let go of the buffer*/
    free(buf);

    switch (comm_data->samprate)
       {
       case 48000:
          ssnd_data->samprate = AL_RATE_48000;
          break;
       case 44100:
          ssnd_data->samprate = AL_RATE_44100;
          break;
       case 32000:
          ssnd_data->samprate = AL_RATE_32000;
          break;
       case 22050:
          ssnd_data->samprate = AL_RATE_22050;
          break;
       case 16000:
          ssnd_data->samprate = AL_RATE_16000;
          break;
       case 11025:
          ssnd_data->samprate = AL_RATE_11025;
          break;
       case 8000:
          ssnd_data->samprate = AL_RATE_8000;
          break;
       default:
          cerr << "Unknown sample rate: " << comm_data->samprate << endl;
          ssnd_data->samprate = AL_RATE_48000;
          break;
       }
    switch ( comm_data->nchannels )
       {
       case 1:
          ssnd_data->numchan = AL_MONO;
          break;
       case 2:
          ssnd_data->numchan = AL_STEREO;
          break;
       default:
          ssnd_data->numchan = AL_STEREO;
          cerr << "Unknown number of channels: " << comm_data->nchannels
               << endl;
          break;
       }

    if ( ssnd_data->numchan == AL_STEREO )
       ssnd_data->samplecount *= 2;

    switch ( comm_data->sampwidth )
       {
       case 8:
          ssnd_data->sampwidth = AL_SAMPLE_8;
          break;
       case 16:
          ssnd_data->sampwidth = AL_SAMPLE_16;
          break;
       case 24:
          ssnd_data->sampwidth = AL_SAMPLE_24;
          break;
       default:
          ssnd_data->sampwidth = AL_SAMPLE_16;
          cerr << "Unknown sample width:  " << comm_data->sampwidth
               << endl;
          break;
       }

 
    return TRUE;
}


/*
 * read_ssnd_chunk
 */
static void read_ssnd_chunk(chunk_header_t *chunk_header, SOUNDTYPE *ssnd_data)
{
    char buf[SSND_CHUNK_DATA];

    /*read the chunk header*/
    read(fd, buf, SSND_CHUNK_DATA);

    /*figure out how big the buffer should be*/
    ssnd_data->sample_length_bytes = chunk_header->size - 2*sizeof(long);

    if ( ssnd_data->numchan == AL_STEREO )
        ssnd_data->sample_length_bytes *= 2;

    if (chunk_header->size %2 == 1){ /*must be even size*/
        ssnd_data->sample_length_bytes++;
    }
    
    /*allocate the memory*/
    if((ssnd_data->sample = 
           (char *)malloc((unsigned int)(ssnd_data->sample_length_bytes)))
               == NULL) {
      printf("Unable to malloc memory for the sound\n");
    }
    
    /*read the puppy*/
    read(fd, ssnd_data->sample, (unsigned int)ssnd_data->sample_length_bytes);
}


/*
 * S K I P _ C H U N K
 */
static void
skip_chunk(chunk_header_t *chunk_header) {
    char id[5];
    int s;
    char *buf;

    strncpy(id, chunk_header->id, 4);
    id[4] = '\0';

    /* skip the pad byte, if necessary */
    s = int(((chunk_header->size % 2) == 1) ? 
              chunk_header->size + 1 : chunk_header->size);
    if ( (s > 0) && ((buf = (char *)malloc(size_t(s))) == NULL ) )
       {
       cerr << "Unable to malloc " << s << " bytes for skip chunk!\n";
       }
    else if (s > 0)
       {
       read(fd,buf,s);
       free(buf);
       }
}


/*
 * C O N V E R T   F R O M   I E E E   E X T E N D E D  
 */

/* 
 * Copyright (C) 1988-1991 Apple Computer, Inc.
 * All rights reserved.
 *
 * Warranty Information
 *  Even though Apple has reviewed this software, Apple makes no warranty
 *  or representation, either express or implied, with respect to this
 *  software, its quality, accuracy, merchantability, or fitness for a
 *  particular purpose.  As a result, this software is provided "as is,"
 *  and you, its user, are assuming the entire risk as to its quality
 *  and accuracy.
 *
 * This code may be used and freely distributed as long as it includes
 * this copyright notice and the above warranty information.

 * Machine-independent I/O routines for IEEE floating-point numbers.
 *
 * NaN's and infinities are converted to HUGE_VAL or HUGE, which
 * happens to be infinity on IEEE machines.  Unfortunately, it is
 * impossible to preserve NaN's in a machine-independent way.
 * Infinities are, however, preserved on IEEE machines.
 *
 * These routines have been tested on the following machines:
 *    Apple Macintosh, MPW 3.1 C compiler
 *    Apple Macintosh, THINK C compiler
 *    Silicon Graphics IRIS, MIPS compiler
 *    Cray X/MP and Y/MP
 *    Digital Equipment VAX
 *
 *
 * Implemented by Malcolm Slaney and Ken Turkowski.
 *
 * Malcolm Slaney contributions during 1988-1990 include big- and little-
 * endian file I/O, conversion to and from Motorola's extended 80-bit
 * floating-point format, and conversions to and from IEEE single-
 * precision floating-point format.
 *
 * In 1991, Ken Turkowski implemented the conversions to and from
 * IEEE double-precision format, added more precision to the extended
 * conversions, and accommodated conversions involving +/- infinity,
 * NaN's, and denormalized numbers.
 */


# define UnsignedToFloat(u)    \
     (((double)((long)(u - 2147483647L - 1))) + 2147483648.0)

/****************************************************************
 * Extended precision IEEE floating-point conversion routine.
 ****************************************************************/

static double ConvertFromIeeeExtended(char *bytes)
{
    double    f;
    long    expon;
    unsigned long hiMant, loMant;
    
    expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);
    hiMant    =    ((unsigned long)(bytes[2] & 0xFF) << 24)
            |    ((unsigned long)(bytes[3] & 0xFF) << 16)
            |    ((unsigned long)(bytes[4] & 0xFF) << 8)
            |    ((unsigned long)(bytes[5] & 0xFF));
    loMant    =    ((unsigned long)(bytes[6] & 0xFF) << 24)
            |    ((unsigned long)(bytes[7] & 0xFF) << 16)
            |    ((unsigned long)(bytes[8] & 0xFF) << 8)
            |    ((unsigned long)(bytes[9] & 0xFF));

    if (expon == 0 && hiMant == 0 && loMant == 0) {
        f = 0;
    }
    else {
        if (expon == 0x7FFF) {    /* Infinity or NaN */
            f = A_HUGE_VAL;
        }
        else {
            expon -= 16383;
            f  = ldexp(UnsignedToFloat(hiMant), int(expon-=31));
            f += ldexp(UnsignedToFloat(loMant), int(expon-=32));
        }
    }

    if (bytes[0] & 0x80)
        return -f;
    else
        return f;
}


#else
/* ifndef AUDIO */

int playsound(LocationRec *, LocationRec *, int, int )
   {
   return TRUE;
   }

void init_sounds ( char *pathname )
   {
   cerr << "\t!WARNING!:\tThe program was compiled on a machine that did not"
        << endl;
   cerr << "\t\thave the appropriate sound libraries and header files."
        << endl;
   cerr << "\t\tTherefore, no sounds will be played." << endl;
   }

void closeaudioports()
   {
   }

#endif

