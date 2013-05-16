

#ifndef __NPS_SCREEN_DUMP_H__
#define __NPS_SCREEN_DUMP_H__


#include <pf.h>


void get_unique_numbered_filename ( const char * /*directoryname*/,
                                    const char * /*basefilename*/,
                                    const char * /*extension*/,
                                    char * /*uniquefilename*/ );

int print_chan_image_to_file ( pfChannel * /*chan*/, 
                               char * /*directoryname*/,
                               char * /*basefilename*/,
                               int verbose = TRUE );

int print_pipe_image_to_file ( pfPipe * /*pipe*/,
                               char * /*directoryname*/,
                               char * /*basefilename*/,
                               int verbose = TRUE );

int print_gl_window_to_file ( char * /*directoryname*/,
                              char * /*basefilename*/,
                              int verbose = TRUE );


#endif

