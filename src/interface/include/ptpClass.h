#ifndef __PTP_CLASS_DOT_H__
#define __PTP_CLASS_DOT_H__

#include <iostream.h>  // for iostream definition
#include <bstring.h>   // for bzero and bcopy definitions
#include <pf.h>        // for pfVec3 definition

const float PTP_OFFSET = 15.0f;
const float PTP_MAX_SPEED = 0.447049f * 1100.0f;

typedef struct _aPoint aPoint;

struct _aPoint {
   pfVec3  xyz;
   int     status;
   aPoint *next;
};

class ptpClass {

   public:

      ptpClass (const char *);
      ~ptpClass ();

      int exists ();
      
      void set_position (pfVec3, int = 0);
      void get_velocity (pfVec3 &);
      void get_heading (float &);
      void get_status (int &);

   private:

      aPoint *head;
      aPoint *current;
      pfVec3  c_position;
      pfVec3  c_velocity;
      float   c_heading;
      int     c_flags;
      int     success;
      int     node_count;

      int in_range ();
      void calculate_velocity ();
      void calculate_heading ();
      void next_point ();
      void create_list(const char *);
      void delete_list();
      void print_list();


}; // ptpClass

#endif // __PTP_CLASS_DOT_H__
