
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#include "fileio.h"

void
main () 
{
   char pathname[NPS_MAXPATH];
   char filename[NPS_MAXFILENAME];
   int  found = FALSE;
   
   strcpy ( pathname, "" );
   strcpy ( filename, "" );
   
   cerr << "Enter pathname (or quit): ";
   cin.getline ( pathname, NPS_MAXPATH );
   cerr << "Enter filename: ";
   cin.getline ( filename, NPS_MAXFILENAME );
   if ( npsFindFile(filename,pathname) )
      found = 1;
   else
      found = 0;
   
   if ( found ) {
      ifstream input_file;
      int retval = 0;
      int quit = FALSE;

      input_file.open ( pathname, ios::in );
      while ( !quit ) {
         static int i;
         static char string[255];
         static float f;
         retval = npsfscanf(input_file,"%i%s%f", &i, string, &f);
         switch ( retval ) {
            case NPSFSCANF_EOF:
               cerr << "End of File." << endl;
               quit = TRUE;
               break;
            case NPSFSCANF_FORMAT_ERROR:
               cerr << "Format line in error." << endl;
               quit = TRUE;
               break;
            case NPSFSCANF_UNDERFLOW:
               cerr << "Data underflow." << endl;
               retval = npsfscanf(input_file,"%i%s", &i, string);
               if ( retval != TRUE ) {
                  cerr << "Secondary read failed: " << retval << endl;
                  quit = !skipline(input_file);
               }
               else {
                  cerr << "Secondary good values: "
                       << i << ", " << string << ", " << 5000.0 <<  endl;
               }
               break;
            case NPSFSCANF_OVERFLOW:
               cerr << "Data overflow." << endl;
               cerr << "Valid values were: "
                    << i << ", " << string << ", " << f << endl;
               break;
            case NPSFSCANF_TYPE_CONFLICT:
               cerr << "Type conflict for data." << endl;
               quit = !skipline(input_file);
               break;
            case NPSFSCANF_RANGE_ERROR:
               cerr << "Range (to big/small) conflict for data." << endl;
               quit = !skipline(input_file);
               break;
            case TRUE:
               cerr << "Good values: "
                    << i << ", " << string << ", " << f << endl;
               break; 
            default:
               cerr << "Unknown return value from npsfscanf: " 
                    << retval << endl;
               quit = TRUE;
               break;
         }
      }
      input_file.close();
   }
   else {
      cerr << "File not found." << endl;
   } 

exit(0);
}
