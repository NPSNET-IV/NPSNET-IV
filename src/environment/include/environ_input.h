

#ifndef __NPS_ENVIRON_INPUT__
#define __NPS_ENVIRON_INPUT__

#include "common_defs.h"

void modify_clouds ( CHANNEL_DATA * );
void modify_fog ( CHANNEL_DATA * );
void update_environment ( CHANNEL_DATA * );
void getDVWTexturePtrs();
void DVWExit();
void setDVWEnvPaths(char **);

#endif

