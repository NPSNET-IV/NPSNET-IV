#include <fstream.h>   // for iostream definitions
#include <bstring.h>   // for bzero and bcopy definitions
#include <math.h>      // for fsqrt definition
#include <pf.h>        // for pfVec3 definition

#include "ptpClass.h"  // for ptpClass definition
#include "jointmods.h" // for joint definitions
#include "view.h"      // for compute_hpr
#include "DI_comm.h"   // for FORCE_STOP_FL definition

ptpClass::ptpClass (const char *ptp_file)
{
   success = FALSE;

   head = current = (aPoint *) NULL;

   node_count = 0;
   create_list (ptp_file);

   if ( head != (aPoint *) NULL ) {
      success = TRUE;
      current = head;
   }
   
   bzero ( (void *) c_position, sizeof (pfVec3) );
   bzero ( (void *) c_velocity, sizeof (pfVec3) );
   c_flags = 0;
   c_heading = 0.0f;
}

ptpClass::~ptpClass ()
{
   delete_list ();
}

int
ptpClass::exists ()
{
   return success;
}

void
ptpClass::set_position (pfVec3 new_position, int new_flags)
{
   c_flags = new_flags;
#ifdef NOPERFORMER
   bcopy ( (void *) new_position, (void *) c_position, sizeof (pfVec3) );
#else
   pfCopyVec3 (c_position, new_position);
#endif // NOPERFORMER
   if (in_range () ) {
      cerr << "In range, moving to next point." << endl;
      next_point ();
   }
   
   calculate_velocity ();
   calculate_heading ();
}

void
ptpClass::get_velocity (pfVec3 &new_velocity)
{
#ifdef NOPERMORMER
   bcopy ( (void *) c_velocity, (void *) new_velocity, sizeof (pfVec3) );
#else
   pfCopyVec3 (new_velocity, c_velocity);
#endif // NOPERMORMER
}

void
ptpClass::get_heading (float &new_heading)
{
   new_heading = c_heading;
}

void
ptpClass::get_status (int &new_status)
{
   new_status = current->status;
}

int
ptpClass::in_range ()
{
   int in_range = TRUE;
   
   if ( (c_position[0] > (current->xyz[0] + PTP_OFFSET) ) ||
        (c_position[0] < (current->xyz[0] - PTP_OFFSET) ) ) {
      in_range = FALSE;
   }

   if ( (c_position[1] > (current->xyz[1] + PTP_OFFSET) ) ||
        (c_position[1] < (current->xyz[1] - PTP_OFFSET) ) ) {
      in_range = FALSE;    
   }

   return in_range;
}

void
ptpClass::calculate_velocity ()
{
   float x_dist;
   float y_dist;
   float length;

   x_dist = current->xyz[0] - c_position[0];
   y_dist = current->xyz[1] - c_position[1];

   length = fsqrt ( (x_dist * x_dist) + (y_dist * y_dist) );

   if (length != 0.0f) {
      c_velocity[0] = PTP_MAX_SPEED * x_dist / length;
      c_velocity[1] = PTP_MAX_SPEED * y_dist / length;
   }
   else {
      c_velocity[0] = 0.0f;
      c_velocity[1] = 0.0f;
   }

//   cerr << "Velocity X: " << c_velocity[0] << " Y: " << c_velocity[1]
//        << endl;

}

void
ptpClass::calculate_heading ()
{
   pfVec3 hpr_bin;

   compute_hpr (c_position, current->xyz, hpr_bin);
   c_heading = hpr_bin[0];
}

void
ptpClass::next_point ()
{
   if (current->next == (aPoint *) NULL) {
      current = head;
   }
   else {
      current = current->next;
   }
}

void
ptpClass::create_list (const char *ptp_file)
{
   float    temp_x;
   float    temp_y;
   int      temp_status;
   ifstream inFile (ptp_file);

   if (!inFile) {
      cerr << "Error: Could not open file \"" << ptp_file
           << "\" for ptpClass initialization." << endl;
      return;
   }

   if (inFile >> temp_x >> temp_y >> temp_status) {
      head = current = new aPoint; node_count++;
      current->xyz[0] = temp_x;
      current->xyz[1] = temp_y;
      current->xyz[2] = 0.0f;
      current->status = temp_status;

      while (inFile >> temp_x >> temp_y >> temp_status) {
         current->next = new aPoint; node_count++;
         current = current->next;
         current->xyz[0] = temp_x;
         current->xyz[1] = temp_y;
         current->xyz[2] = 0.0f;
         current->status = temp_status;
      }

      current->next = (aPoint *) NULL;
      success = TRUE;
      print_list ();
   }
}

void
ptpClass::delete_list ()
{
   aPoint *previous_node;

   current = head;

   while (current != (aPoint *) NULL) {
      previous_node = current;
      current = current->next;
      delete previous_node; node_count--;
   }

   if (node_count != 0) {
      cerr << "Warning: " << node_count << " undeleted nodes detected."
           << endl;
   }
}

void
ptpClass::print_list ()
{
   aPoint *temp_place = head;
   int     the_count = 0;

   cerr << "Point to point file list:" << endl;

   while (temp_place) {
      the_count++;
      cerr << "Point: " << the_count << "  "
           << "X: " << temp_place->xyz[0] << "  "
           << "Y: " << temp_place->xyz[1] << "  "
           << "Status: ";
      switch (temp_place->status) {
         case NOSIGNAL:
            cerr << "NOSIGNAL";
            break;
         case SALUTE:
            cerr << "SALUTE";
            break;
         case CROUCH:
            cerr << "CROUCH";
            break;
         default:
            break;
      }
      cerr << endl;
      temp_place = temp_place->next;
   }
}
 
