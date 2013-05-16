#include <time.h>

#define FB_NOBLOCK 1
#define FB_BLOCK 2

#define BAUD12  0x10
#define BAUD24  0x20
#define BAUD48  0x30
#define BAUD96  0x40
#define BAUD192 0x50
#define BAUD384 0x60
#define BAUD576 0x70

typedef struct rs_struct
{
   int wrt;
   int rd;
   int len;
   int nl;
   int cycles;
   int thou;
} RS_ERR;

typedef struct chan_struct
{
   int StrLen;
   int n_AChan;
   int AChan;
   int n_DChan;
   int DChan;
   int Baud;
   char mode[2];
   time_t tag;
   int running;
   long count;
   float hz;
   int dio;
   float ach[8];
   int Port;
   long num_points;
   int update_rate;
   int precision;
} Chans;
