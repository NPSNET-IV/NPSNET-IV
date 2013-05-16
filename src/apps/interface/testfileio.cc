
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
      char *tokens[MAX_ENTRIES_PER_LINE]; 
      int counter;

      input_file.open ( pathname, ios::in );
      while ( (counter = read_and_parse_line(input_file, tokens)) != -1 ) {

          cerr << "NEW LINE:" << endl << "\t";
          for ( int i = 0; i < counter; i++ ) {
             cerr << tokens[i] << "*"; 
          }
          cerr << endl;
          if ( (clean_line(tokens)) != counter ) {
             cerr << endl << "ERROR CLEANING LINES." << endl;
          }
       }
       input_file.close();
    }
    else {
       cerr << "File not found." << endl;
    } 

exit(0);
}
