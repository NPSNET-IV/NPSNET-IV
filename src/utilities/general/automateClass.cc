#include <fstream.h>   // for iostream definitions
#include <string.h>
#include <bstring.h>   // for bzero and bcopy definitions
#include <math.h>      // for fsqrt definition
#include <pf.h>        // for pfVec3 definition

#include "npsIOStream.h"
#include "automateClass.h"
#include "infoClass.h"
#include "transportClass.h"

npsAutomate::npsAutomate (const char *autoFile)
{
//   info = (npsInfo **)NULL;
   infoNumber = 0;
   infoSize = 50;
   create_list (autoFile);

}

npsInfo*
npsAutomate::find_info (pfVec3 point)
{
   npsInfo *foundInfo = (npsInfo *) NULL;
   int done = FALSE;
   int place = 0;

   while (!(done) && (place < infoNumber)) {
      if (info[place]->in_zone (point)) {
         foundInfo = info[place];
         done = TRUE;
      }
      else {
         place++;
      }
   }

   return foundInfo;
}
         

npsAutomate::~npsAutomate ()
{
}

void
npsAutomate::create_list (const char *autoFile)
{
   float min_x, min_y, min_z;
   float max_x, max_y, max_z;
   float theX, theY, theZ;
   float theH, theP, theR;
   int mode;

   char tag[255];
   char junk[255];

   ifstream inFile (autoFile);

   if (!inFile) {
     cerr << "Unable to open: " << autoFile << endl;
   }
   else {

//      info = (npsInfo **) new (npsInfo *)[infoSize];

      tag[0] = '\0';

      inFile >> tag;

      do {
         if (strcmp (tag, "TRANSPORT_ZONE") == 0) {
            if (inFile >> min_x >> min_y >> min_z
                       >> max_x >> max_y >> max_z
                       >> theX  >> theY  >> theZ
                       >> theH  >> theP  >> theR
                       >> mode) {
               info[infoNumber] = new npsTransportInfo (min_x, min_y, min_z,
                                                        max_x, max_y, max_z,
                                                        theX, theY, theZ,
                                                        theH, theP, theR,
                                                        mode);
               infoNumber++;

               cerr << min_x << "  " << min_y << "  " << min_z  << "  "
                    << max_x << "  " << max_y << "  " << max_z << "  "
                    << theX << "  "  << theY << "  "  << theZ << "  "
                    << theH << "  "  << theP << "  "  << theR << "  "
                    << mode << endl;
            
            }
            else {
               cerr << "Syntax Error:" << endl
                    << "\tTRANSPORT_ZONE min_x min_y min_z" << endl
                    << "\t               max_x max_y max_z" << endl
                    << "\t               theX  theY  theZ" << endl
                    << "\t               theH  theP  theR mode" << endl;
            }
         }
         else if (tag[0] != '#') {
            cerr << "Syntax Error: Unknow tag " << tag << endl;
         }

         inFile.getline (junk, 255);
  
        inFile >> tag;
      } while (!inFile.eof () && (infoSize >= infoNumber));
   }


   inFile.close ();
} 
