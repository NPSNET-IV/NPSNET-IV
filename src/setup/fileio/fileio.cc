
#include <iostream.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <float.h>
#include <limits.h>
#include <sys/bsd_types.h>
#include <errno.h>

extern int errno;

#include "fileio.h"


// Local prototypes

static int
process_double ( va_list &next_param, 
                 char *tokens[MAX_ENTRIES_PER_LINE],
                 int &counter, int &num_tokens_used );
static int
process_float ( va_list &next_param,
                char *tokens[MAX_ENTRIES_PER_LINE],
                int &counter, int &num_tokens_used );
static int
process_long ( va_list &next_param,
               char *tokens[MAX_ENTRIES_PER_LINE],
               int &counter, int &num_tokens_used );
static int
process_int ( va_list &next_param,
              char *tokens[MAX_ENTRIES_PER_LINE],
              int &counter, int &num_tokens_used );

static int
process_uint ( va_list &next_param,
              char *tokens[MAX_ENTRIES_PER_LINE],
              int &counter, int &num_tokens_used );

static int
process_string ( va_list &next_param,
                 char *tokens[MAX_ENTRIES_PER_LINE],
                 int &counter, int &num_tokens_used );


void
print_npsfscanf_error ( int error_num, char *extra_message )
{
   switch ( error_num ) {
      case NPSFSCANF_EOF:
         cerr << "NPSFSCANF -- End of File." << endl;
         break;
      case NPSFSCANF_FORMAT_ERROR:
         cerr << "NPSFSCANF -- Format line in error." << endl;
         break;
      case NPSFSCANF_UNDERFLOW:
         cerr << "NPSFSCANF -- Data underflow." << endl;
         break;
      case NPSFSCANF_OVERFLOW:
         cerr << "NPSFSCANF -- Data overflow." << endl;
         break;
      case NPSFSCANF_TYPE_CONFLICT:
         cerr << "NPSFSCANF -- Type conflict for data." << endl;
         break;
      case NPSFSCANF_RANGE_ERROR:
         cerr << "NPSFSCANF -- Range (to big/small) conflict for data." << endl;
         break;
      default:
         cerr << "NPSFSCANF -- Unknown error." << endl;
         break;
   }
   if ( extra_message != NULL ) {
      cerr << "\t" << extra_message << endl;
   }
}


int
npsFindFile ( const char *filename,
              char searchpath[NPS_MAXPATH] )
{
   char temp_path[NPS_MAXPATH];
   char filename_only[NPS_MAXFILENAME];
   char current_path[NPS_MAXPATH];
   char *current_token, *previous_token;
   ifstream testfile;
   int found, failed;

#ifdef SEARCH_PATH_TRACE
   cerr << "npsFindFile:\tSearching for " << filename << " -> ";
#endif

   strcpy ( current_path, filename );
   testfile.open ( current_path, ios::in );
   found = testfile.good();

   if ( !found ) {
#ifdef SEARCH_PATH_TRACE
      cerr << "Not found." << endl;
#endif
      testfile.clear();
      strcpy ( temp_path, filename );
      current_token = strtok ( temp_path, DIRECTORY_CHAR );
      previous_token = NULL;
      while ( current_token != NULL ) {
         previous_token = current_token;
         current_token = strtok ( NULL, DIRECTORY_CHAR );
      }

      if ( previous_token == NULL ) {
         failed = TRUE;
      }
      else {
         strncpy ( filename_only, previous_token, NPS_MAXFILENAME-1 );
         filename_only[NPS_MAXFILENAME-1] = '\0';
#ifdef SEARCH_PATH_TRACE
         cerr << "\t\tFilename extracted from original path is: "
              << filename_only << endl;
#endif
         strcpy ( temp_path, searchpath );
         current_token = strtok ( temp_path, NEXT_PATH_CHAR );
         failed = (current_token == NULL);
      }

      while ( !failed && !found ) {
         strcpy ( current_path, current_token );
         if ( current_path[strlen(current_path)-1] != DIRECTORY_CHAR[0] ) {
            strcat ( current_path, DIRECTORY_CHAR );
         }
#ifdef SEARCH_PATH_TRACE
         cerr << "\t\tChecking path: " << current_path << " -> ";
#endif
         strcat ( current_path, filename_only );
         testfile.open ( current_path, ios::in );
         found = testfile.good();
         if ( !found ) {
#ifdef SEARCH_PATH_TRACE
            cerr << "Not Found." << endl;
#endif
            testfile.clear();
            current_token = strtok ( NULL, NEXT_PATH_CHAR );
            failed = (current_token == NULL);
         }
      }
   }

   if ( found ) {
#ifdef SEARCH_PATH_TRACE
      cerr << "Found!" << endl;
#endif
      testfile.close();
      strcpy ( searchpath, current_path );
#ifdef SEARCH_PATH_TRACE
      cerr << "\t\tFinal path is: " << searchpath << endl;
#endif
   }

   return found;
}


void
upcase ( char *data )
{
   static char minch = 'a';
   static char maxch = 'z';
   static int offset = ('A' - 'a');

   if ( data != NULL ) {
      char *lastchar = data + strlen ( data );
      for (char *curr1 = data; curr1 < lastchar; curr1++) {
         if ( ( *curr1 >= minch ) && ( *curr1 <= maxch ) ) {
            *curr1 += offset;
         }
      }
   }
}


void
stripblanks ( char *data )
{
   char *current = data;
   char *next = data;
   char *last = data + strlen(data);

   if ( data != NULL ) {
      while ( next < last ) {
         if ( (*next != ' ') && (*next != '\t' ) ) {
            *current++ = *next;
         }
         next++;
      }
      *current = '\0';
   }
}


int
read_int ( ifstream &infile, int &temp_int )
{
   infile >> temp_int;
   if (!infile) {
      infile.clear();
      return FALSE;
   }
   else {
      return TRUE;
   }
}


int
read_float ( ifstream &infile, float &temp_float )
{
   infile >> temp_float;
   if (!infile) {
      infile.clear();
      return FALSE;
   }
   else {
      return TRUE;
   }
}


int
read_string ( ifstream &infile, char *temp_str )
{
   if ( infile >> temp_str ) {
      upcase ( temp_str );
      stripblanks ( temp_str );
      if (strlen(temp_str) == 0) {
         return FALSE;
      }
      else {
         return TRUE;
      }
   }
   else {
      return FALSE;
   }
}


int
read_string_to_EOL ( ifstream &infile, char *temp_str )
{
   if ( infile.getline ( temp_str, NPS_MAXFILENAME) ) {
      upcase ( temp_str );
      stripblanks ( temp_str );
      if (strlen(temp_str) == 0) {
         return FALSE;
      }
      else {
         return TRUE;
      }
   }
   else {
      return FALSE;
   }
}


int
read_filename ( ifstream &infile, char *filename )
{
   if ( infile.getline ( filename, NPS_MAXFILENAME) ) {
      stripblanks ( filename );
      if (strlen(filename) == 0) {
         return FALSE;
      }
      else {
         return TRUE;
      }
   }
   else {
      return FALSE;
   }
}


void
remove_excessive_spaces ( char *data )
{
   char *current = data;
   char *next = data;
   char *last = data + strlen(data);
   char *previous = NULL;

   if ( data != NULL ) {
      while ( next < last ) {
         if ( (*next != ' ') && (*next != '\t' ) ) {
            *current++ = *next;
            previous = next;
         }
         else {
            if ( *next == '\t' ) {
               *next = ' ';
            }
            if ( (previous != NULL) && (*previous != ' ') ) {
                  *current++ = *next;
                  previous = next;
            }
         }
         next++;
      }
      *current = '\0';
   }
}


int
read_and_parse_line ( ifstream &infile, char *strings[MAX_ENTRIES_PER_LINE] )
{
   char unparsed_line[255];   
   char *current_token;
   int counter = 0;

   if ( infile.getline(unparsed_line, 254) ) {
      remove_excessive_spaces ( unparsed_line );
      current_token = strtok ( unparsed_line, " " );
      while ( current_token != NULL ) {
         if ( *current_token != NPSFSCANF_COMMENT_CHAR ) {
            strings[counter] = new char[strlen(current_token)];
            strcpy ( strings[counter], current_token );
            counter++;
            current_token = strtok ( NULL, " " );
         }
         else {
            current_token = NULL;
         }
      }
   strings[counter] = NULL;
   return counter;
   }
   else {
      return -1;
   }
}


int
clean_line ( char *strings[MAX_ENTRIES_PER_LINE] )
{
   int counter = 0;

   while ( strings[counter] != NULL ) {
      delete strings[counter];
      counter++;
   }

   strings[0] = NULL;
   return counter;
}


int
skipline ( ifstream &infile )
{
   char junk_line[255];
   if ( infile.getline(junk_line,254) ) {
      return TRUE;
   }
   else {
      return FALSE;
   }

}

int
npsfscanf ( ifstream &infile, char *fmt, ... )
{
   va_list next_param;
   char *next_format;
   int counter = 0;
   int num_tokens = 0;
   int num_tokens_used = 0;
   streampos marker;
   char *tokens[MAX_ENTRIES_PER_LINE];
   int return_condition = TRUE;
   int have_line = FALSE;

   do {
      marker = infile.tellg();
      num_tokens = read_and_parse_line ( infile, tokens );
      if ( num_tokens == NPSFSCANF_EOF ) {
         return NPSFSCANF_EOF;
      }
      else if ( (num_tokens > 0) && (tokens[0] != NULL) ) {
         have_line = ( tokens[0][0] != NPSFSCANF_COMMENT_CHAR );
      }
   } while (!have_line);
  
   va_start ( next_param, fmt );
   next_format = fmt;

   while ( (return_condition == TRUE) && *next_format ) {
      if (*next_format != '%') {
         return_condition = NPSFSCANF_FORMAT_ERROR;
      }
      else {
         switch (*++next_format) {
            case 'd':
            case 'D':
               return_condition = process_double ( next_param, tokens, 
                                                   counter, num_tokens_used );
               break;
            case 'f':
            case 'F':
               return_condition = process_float ( next_param, tokens,
                                                  counter, num_tokens_used );
               break;
            case 'l':
            case 'L':
               return_condition = process_long ( next_param, tokens,
                                                 counter, num_tokens_used );
               break;
            case 'i':
            case 'I':
               return_condition = process_int ( next_param, tokens,
                                                counter, num_tokens_used );
               break;
            case 'u':
            case 'U':
                return_condition = process_uint ( next_param, tokens,
                                                  counter, num_tokens_used );
               break;
            case 's':
            case 'S':
               return_condition = process_string ( next_param, tokens,
                                                   counter, num_tokens_used );
               break;
            default:
               return_condition = NPSFSCANF_FORMAT_ERROR;
               break;
         }
      counter++;
      }
   next_format++;
   }

   va_end ( next_param );
   clean_line ( tokens );

   if ( return_condition != TRUE ) {
      infile.seekg(marker);
   }
   else if ( (num_tokens > num_tokens_used) &&
             (tokens[num_tokens_used] != NULL) &&
             (tokens[num_tokens_used][0] != NPSFSCANF_COMMENT_CHAR) ) {
      return_condition = NPSFSCANF_OVERFLOW;
   }

   return (return_condition);

}


int
process_double ( va_list &next_param, 
                 char *tokens[MAX_ENTRIES_PER_LINE],
                 int &counter, int &num_tokens_used )
{
   int return_condition = TRUE;
   char *last_valid;
   double *temp_double = va_arg ( next_param, double *);

   if ( tokens[counter] ) {
      *temp_double = strtod ( tokens[counter], &last_valid );
      if ( errno == ERANGE ) {
         return_condition = NPSFSCANF_RANGE_ERROR;
      }
      else if ( last_valid != (tokens[counter]+strlen(tokens[counter]))){
         return_condition = NPSFSCANF_TYPE_CONFLICT;
      }
      num_tokens_used++;
   }
   else {
      return_condition = NPSFSCANF_UNDERFLOW;
   }

   return (return_condition);
}


int
process_float ( va_list &next_param,
                char *tokens[MAX_ENTRIES_PER_LINE],
                int &counter, int &num_tokens_used )
{
   int return_condition = TRUE;
   char *last_valid;
   double temp_double = 0.0;
   float *temp_float = va_arg ( next_param, float *);

   if ( tokens[counter] ) {
      temp_double = strtod ( tokens[counter], &last_valid );
      if ( errno == ERANGE ) {
         return_condition = NPSFSCANF_RANGE_ERROR;
      }
      else if ( last_valid != (tokens[counter]+strlen(tokens[counter]))){
         return_condition = NPSFSCANF_TYPE_CONFLICT;
      }
      else if ( (temp_double > FLT_MAX) || (temp_double < FLT_MIN) ) {
         return_condition = NPSFSCANF_RANGE_ERROR;
      }
      num_tokens_used++;
      *temp_float = (float)temp_double;

   }
   else {
      return_condition = NPSFSCANF_UNDERFLOW;
   }

   return (return_condition);
}


int
process_long ( va_list &next_param,
                 char *tokens[MAX_ENTRIES_PER_LINE],
                 int &counter, int &num_tokens_used )
{
   int return_condition = TRUE;
   char *last_valid;
   long *temp_long = va_arg ( next_param, long *);

   if ( tokens[counter] ) {
      *temp_long = strtol ( tokens[counter], &last_valid, 0 );
      if ( errno == ERANGE ) {
         return_condition = NPSFSCANF_RANGE_ERROR;
      }
      else if ( last_valid != (tokens[counter]+strlen(tokens[counter]))){
         return_condition = NPSFSCANF_TYPE_CONFLICT;
      }
      num_tokens_used++;
   }
   else {
      return_condition = NPSFSCANF_UNDERFLOW;
   }

   return (return_condition);
}


int
process_int ( va_list &next_param,
              char *tokens[MAX_ENTRIES_PER_LINE],
              int &counter, int &num_tokens_used )
{
   int return_condition = TRUE;
   char *last_valid;
   long temp_long;
   int *temp_int = va_arg ( next_param, int *);

   if ( tokens[counter] ) {
      temp_long = strtol ( tokens[counter], &last_valid, 0 );
      if ( errno == ERANGE ) {
         return_condition = NPSFSCANF_RANGE_ERROR;
      }
      else if ( last_valid != (tokens[counter]+strlen(tokens[counter]))){
         return_condition = NPSFSCANF_TYPE_CONFLICT;
      }
      else if ( (temp_long > INT_MAX) || (temp_long < INT_MIN) ) {
         return_condition = NPSFSCANF_RANGE_ERROR;
      }
      num_tokens_used++;
      *temp_int = (int)temp_long;
   }
   else {
      return_condition = NPSFSCANF_UNDERFLOW;
   }

   return (return_condition);
}


int
process_uint ( va_list &next_param,
              char *tokens[MAX_ENTRIES_PER_LINE],
              int &counter, int &num_tokens_used )
{
   int return_condition = TRUE;
   char *last_valid;
   u_long temp_long;
   u_int *temp_int = va_arg ( next_param, u_int *);

   if ( tokens[counter] ) {
      temp_long = strtoul ( tokens[counter], &last_valid, 0 );
      if ( errno == ERANGE ) {
         return_condition = NPSFSCANF_RANGE_ERROR;
      }
      else if ( last_valid != (tokens[counter]+strlen(tokens[counter]))){
         return_condition = NPSFSCANF_TYPE_CONFLICT;
      }
      else if (temp_long > UINT_MAX) {
         return_condition = NPSFSCANF_RANGE_ERROR;
      }
      num_tokens_used++;
      *temp_int = (u_int)temp_long;
   }
   else {
      return_condition = NPSFSCANF_UNDERFLOW;
   }

   return (return_condition);
}


int
process_string ( va_list &next_param,
                 char *tokens[MAX_ENTRIES_PER_LINE],
                 int &counter, int &num_tokens_used )
{
   int return_condition = TRUE;
   char *temp_str = va_arg ( next_param, char *);

   if ( tokens[counter] ) {
      strcpy ( temp_str, tokens[counter] );
      num_tokens_used++;
   }
   else {
      return_condition = NPSFSCANF_UNDERFLOW;
   }

   return (return_condition);
}

