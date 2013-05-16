
#include <iostream.h>
#include <fstream.h>
#include <pfutil.h>
#include "common_defs.h"
#include "screen_dump.h"

extern volatile DYNAMIC_DATA *G_dynamic_data;


void
get_unique_numbered_filename ( const char *directoryname,
                               const char *basefilename,
                               const char *extension,
                               char *uniquefilename )
{
   static ifstream input_test_name;
   int file_exists = FALSE;
   int local_cnt = 1;

   do {
      sprintf ( uniquefilename, "%s/%s.%d.%s",
                directoryname, basefilename, local_cnt++,extension ); 
      input_test_name.open ( uniquefilename, ios::in );
      file_exists = input_test_name.good();
      if ( file_exists ) {
         input_test_name.close();
      }
      input_test_name.clear();
   } while ( file_exists );


}


int
print_chan_image_to_file ( pfChannel *chan, 
                           char *directoryname,
                           char *basefilename,
                           int verbose )
{
   char image_filename[255];
   int minX, minY, sizeX, sizeY;
   long success = FALSE;

   // Compute the channel's screen coordinate boundaries
   pfGetChanOrigin ( chan, &minX, &minY );
   pfGetChanSize ( chan, &sizeX, &sizeY );
 
   // Compute a uniquely numbered filename for the .rgb image 
   get_unique_numbered_filename ( directoryname, basefilename,
                                  "rgb", image_filename );

   // Save the image
   if ( verbose ) {
      cerr << "*** Saving RGB image of channel to " 
           << image_filename << endl;
   }
   success = pfuSaveImage(image_filename,minX,minY,sizeX,sizeY,1);
   return int(success);
}


int
print_pipe_image_to_file ( pfPipeWindow *pipe,
                           char *directoryname,
                           char *basefilename,
                           int verbose )
{
   char image_filename[255];
   int minX, minY, sizeX, sizeY;
   long success = FALSE;

   // Compute the pipe's screen coordinate boundaries
   pfGetPWinOrigin ( pipe, &minX, &minY );
   pfGetPWinSize ( pipe, &sizeX, &sizeY );
   
   // Compute a uniquely numbered filename for the .rgb image
   get_unique_numbered_filename ( directoryname, basefilename,
                                  "rgb", image_filename );

   // Save the image
   if ( verbose ) {
      cerr << "*** Saving RGB image of pipe to " 
           << image_filename << endl;
   }
   success = pfuSaveImage(image_filename,minX,minY,sizeX,sizeY,1);
   return int(success);
}


int
print_gl_window_to_file ( char *directoryname,
                          char *basefilename,
                          int verbose )

{
   char image_filename[255];
   char save_command[512];
#ifdef IRISGL
   long minX, minY, sizeX, sizeY;
#else
   int minX, minY, sizeX, sizeY;
#endif
   long success = FALSE;

   // Compute the pipe's screen coordinate boundaries
#ifdef IRISGL
   getorigin ( &minX, &minY );
   getsize ( &sizeX, &sizeY );
#else
   //pfGetPWinOrigin (G_dynamic_data->pw, &minX, &minY );
   //pfGetPWinSize (G_dynamic_data->pw, &sizeX, &sizeY);
   minX = minY = 0;
   sizeX=G_dynamic_data->pipe[0].window.right;
   sizeY=G_dynamic_data->pipe[0].window.top;
#endif

   // Compute a uniquely numbered filename for the .rgb image
   get_unique_numbered_filename ( directoryname, basefilename,
                                  "rgb", image_filename );

   // Save the image
   if ( verbose ) {
      cerr << "*** Saving RGB image of pipe to "
           << image_filename << endl;
   }

   sprintf ( save_command, "/usr/sbin/scrsave %s %d %d %d %d",
             image_filename,
             minX, minX+sizeX, minY, minY+sizeY );

   if ( (system(save_command)) == -1 ) {
      perror(save_command);
      success = FALSE;
   }
   else {
      success = TRUE;
   }

   return int(success);
}

