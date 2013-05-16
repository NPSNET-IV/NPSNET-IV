
#include "lw_tcpip.h"
#include <iostream.h>

int
main ( int argc, char **argv )
{
   lw_tcpip *nps_to_lw;
   int counts[] = { 0, 0, 0 };
   float laze[] = { 10.2000f, 30.4000f, 50.6000f };
   short entity[] = { 36, 36, 1001 };

   if ( argc < 2 ) {
      cerr << "usage: " << argv[0] << " <machine>" << endl;
      exit(0);
   }

   nps_to_lw = new lw_tcpip ( 5000, "", argv[1],
                              5001, "", argv[1] );

   while ( !nps_to_lw->communications_up() );

   if ( !nps_to_lw->set_multival(NPS_LW_ENTITY, (void *)entity ) ) {
      cerr << "ERROR setting site, host and entity information." << endl;
   }


   while ( (counts[0] < 5) || (counts[1] < 5) || (counts[2] < 5) ) {
      if ( nps_to_lw->trigger() ) {
         cerr << "\tTRIGGER press." << endl;
         counts[0]++;
      }
      if ( nps_to_lw->laser() ) {
         cerr << "\tLASER press." << endl;
         counts[1]++;
         if ( !nps_to_lw->set_multival(NPS_LW_LAZE, (void *)laze) ) {
            cerr << "\t\tERROR sending laser response." << endl;
         }
         else {
            cerr << "\t\tLaser response sent." << endl;
         }
      }
      if ( nps_to_lw->video() ) {
         cerr << "\tVIDEO press." << endl;
         counts[2]++;
      }
   }


delete nps_to_lw;
exit(0);
}


