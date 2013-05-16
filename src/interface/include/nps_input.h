
#ifndef __NPS_INPUT_H__
#define __NPS_INPUT_H__

#include <pf.h>
#include "display_types.h"

void process_input_states ( pfPipeWindow *, INFO_PDATA * );

void check_for_query ( const char *,
                       const u_short,
                       const int,
                       const char *,
                       const u_char );

void check_for_reset ();

void update_range_filter ( float & );

void open_input_devices ( int &, const char * );

#endif

