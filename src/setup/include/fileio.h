
#ifndef __NPS_FILEIO__
#define __NPS_FILEIO__

#include <fstream.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define NPS_MAXPATH	1024
#define NPS_MAXFILENAME	512
#define DIRECTORY_CHAR	"/"
#define NEXT_PATH_CHAR	":"
#define MAX_ENTRIES_PER_LINE 50

#define NPSFSCANF_COMMENT_CHAR '#'

#define NPSFSCANF_EOF -1
#define NPSFSCANF_FORMAT_ERROR -2
#define NPSFSCANF_UNDERFLOW -3
#define NPSFSCANF_OVERFLOW -4
#define NPSFSCANF_TYPE_CONFLICT -5
#define NPSFSCANF_RANGE_ERROR -6

void
print_npsfscanf_error ( int, char * );

int
npsFindFile ( const char *filename,
              char searchpath[NPS_MAXPATH] );

void
upcase ( char *data );

void
stripblanks ( char *data );

int
read_int ( ifstream &infile, int &temp_int );

int
read_float ( ifstream &infile, float &temp_float );

int
read_string ( ifstream &infile, char *temp_str );

int
read_string_to_EOL ( ifstream &infile, char *temp_str );

int
read_filename ( ifstream &infile, char *filename );

void
remove_excessive_spaces ( char *data );

int
read_and_parse_line ( ifstream &infile, char *strings[MAX_ENTRIES_PER_LINE] );

int
clean_line ( char *strings[MAX_ENTRIES_PER_LINE] );

int
skipline ( ifstream &infile );

int
npsfscanf ( ifstream &infile, char *fmt, ... );

#endif

