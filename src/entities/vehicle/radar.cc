
#include <fstream.h>
#include <iostream.h>
#include "radar.h"
#include "entity_const.h"
#include "appearance.h"

#ifdef OPENGL
#include "fontdef.h"
#include "display_const.h"
#define RING_SIZE 2
#endif

#include "imstructs.h"
#include "imclass.h"

extern InitManager *initman;

// Globals used by this library
extern volatile STATIC_DATA     *G_static_data;
extern volatile DYNAMIC_DATA    *G_dynamic_data;

// Local types
enum MATRIX_ACTION { PUSH_MATRIX, POP_MATRIX };

enum CONTAINMENT_ACTION { RESET_BOUNDS, CHECK_FOR_CONTAINMENT };

typedef struct
   {
   char ch;
   float Xpos;
   float Ypos;
   } LANDMARK_ENTRY;

// Local prototypes

static int
bounding_sphere ( CONTAINMENT_ACTION, pfVec2,
                  float radius = 0.0f );

static radar_screen_info *
radar_setup ( pfChannel *, float, float, float,
              int, float, float,
              float, float, float, float );

static void
radar_cleanup( int );

static void
perspective_matrix_action ( MATRIX_ACTION );

static void
draw_radar_rings ( float, float, float, unsigned int alpha = 255 );

static int
read_landmarks ( LANDMARK_ENTRY * );

static void
draw_landmarks ( int forward_looking = TRUE,
                 unsigned int alpha = 255,
                 int reload = FALSE );

static void
draw_bounding_box ( float, float, float, float,
                    int forward_looking = TRUE, 
                    unsigned int alpha = 255 );

static void
draw_veh_direction_indicator ( float, float, float, float,
                               int forward_looking = TRUE,
                               unsigned int alpha = 255 );

static char
lookup_icon_char_from_type ( int );


static void
draw_vehicle_icons ( HUD_VEH_REC *, int,
                     int forward_looking = TRUE,
                     int use_veh_icons = TRUE,
                     int elevation_coloring = FALSE,
                     float ref_elevation = 0.0f,
                     unsigned int alpha = 255 );


#ifdef IRISGL
// Local global variables
static pfMatrix L_identity = { {1.0f, 0.0f, 0.0f, 0.0f},
                               {0.0f, 1.0f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 1.0f, 0.0f},
                               {0.0f, 0.0f, 0.0f, 1.0f} };
#endif


int
bounding_sphere ( CONTAINMENT_ACTION action, pfVec2 point,
                  float radius )
{
   static pfSphere b_sphere;
   int success = FALSE;
   pfVec3 threeDpoint;

   pfSetVec3 ( threeDpoint, point[PF_X], point[PF_Y], 0.0f );
   switch ( action ) {
      case RESET_BOUNDS:
         pfCopyVec3 ( b_sphere.center, threeDpoint );
         b_sphere.radius = radius;
         success = TRUE;
         break;
      case CHECK_FOR_CONTAINMENT:
         success = pfSphereContainsPt(&b_sphere, threeDpoint);
         break;
      default:
         break;
   }

   return success;
}


radar_screen_info *
radar_setup ( pfChannel *chan, float world_centerX, float world_centerY,
              float range_meters, int forward_looking, float heading,
              float gaze_offset,
              float viewportl, float viewportr,
              float viewportb, float viewportt )
{

   static radar_screen_info my_screen_info;
   pfPipeWindow *pipe;
   int pipe_minX, pipe_minY, pipe_maxX, pipe_maxY, pipe_sizeX, pipe_sizeY;
   int chan_minX, chan_minY, chan_maxX, chan_maxY, chan_sizeX, chan_sizeY;
   float radar_vp_minX, radar_vp_maxX, radar_vp_minY, radar_vp_maxY;
   float radar_sizeX, radar_sizeY;
   float ortho_minX, ortho_minY, ortho_maxX, ortho_maxY;
   float aspect, corrected_range;
   pfVec2 bsphere_center;

   pipe = pfGetChanPWin(chan);
   pfGetPWinOrigin(pipe, &pipe_minX, &pipe_minY);
   pfGetPWinSize(pipe, &pipe_sizeX, &pipe_sizeY);
   pipe_maxX = pipe_minX + pipe_sizeX - 1;
   pipe_maxY = pipe_minY + pipe_sizeY - 1;

   pfGetChanOrigin(chan, &chan_minX, &chan_minY);
   pfGetChanSize(chan, &chan_sizeX, &chan_sizeY);
   chan_maxX = chan_minX + chan_sizeX - 1;
   chan_maxY = chan_minY + chan_sizeY - 1;

   radar_vp_minX = (viewportl * (chan_sizeX-1)) + chan_minX;
   radar_vp_maxX = (viewportr * (chan_sizeX-1)) + chan_minX;
   radar_sizeX = radar_vp_maxX - radar_vp_minX + 1;
   radar_vp_minY = (viewportb * (chan_sizeY-1)) + chan_minY;
   radar_vp_maxY = (viewportt * (chan_sizeY-1)) + chan_minY;
   radar_sizeY = radar_vp_maxY - radar_vp_minY + 1;
   my_screen_info.radar_centerX = (radar_vp_minX + radar_vp_maxX) / 2.0f;
   my_screen_info.radar_centerY = (radar_vp_minY + radar_vp_maxY) / 2.0f;
/*
cerr << "RADAR:" << endl;
cerr << "\tRadar pixel dimensions: " << radar_vp_minX << "x" << radar_vp_maxX
     << ", " <<  radar_vp_minY << "x" << radar_vp_maxY << endl;
cerr << "\tRadar pixel size: " << radar_sizeX << "x" 
     << radar_sizeY << endl;
cerr << "\tRadar pixel center: " << my_screen_info.radar_centerX << "x"
     << my_screen_info.radar_centerY << endl;
*/
   if ( (radar_sizeX > radar_sizeY) && (radar_sizeY > 0.0f) ) {
      aspect = radar_sizeX / radar_sizeY;
      corrected_range = range_meters * aspect;   
   }
   else {
      corrected_range = range_meters;
   }

   my_screen_info.meters_per_pixelX = (2.0f * corrected_range)/radar_sizeX;

   ortho_minX = world_centerX - ((corrected_range*radar_sizeX)/chan_sizeX);
   ortho_maxX = world_centerX + ((corrected_range*radar_sizeX)/chan_sizeX);

   if ( (radar_sizeY > radar_sizeX) && (radar_sizeX > 0.0f) ) {
      aspect = radar_sizeY / radar_sizeX;
      corrected_range = range_meters * aspect;
   }
   else {
      corrected_range = range_meters;
   }

   my_screen_info.meters_per_pixelY = (2.0f * corrected_range)/radar_sizeY;

   ortho_minY = world_centerY - ((corrected_range*radar_sizeY)/chan_sizeY);
   ortho_maxY = world_centerY + ((corrected_range*radar_sizeY)/chan_sizeY);
/*
cerr << "\tOrtho dimensions: " << ortho_minX << "x" << ortho_maxX
     << ", " << ortho_minY << "x" << ortho_maxY << endl;
*/
#ifdef IRISGL
   pushmatrix();
#else // OPENGL
   pfPushMatrix ();
#endif

   if ( forward_looking ) {
#ifdef IRISGL
      loadmatrix(L_identity);
      translate(world_centerX, world_centerY, 0.0f);
      rot(heading-gaze_offset,'z');
#else // OPENGL
      glLoadIdentity();
      glTranslatef(world_centerX, world_centerY, 0.0f );
      glRotatef(heading-gaze_offset,0.0f,0.0f,1.0f);
#endif
      my_screen_info.rot_amount = heading - gaze_offset;
#ifdef IRISGL
      translate(-world_centerX, -world_centerY, 0.0f);
      pushmatrix();
#else // OPENGL
      glTranslatef(-world_centerX, -world_centerY, 0.0f );
      pfPushMatrix ();
#endif
   }
#ifdef IRISGL
   loadmatrix(L_identity);
#else // OPENGL
   glLoadIdentity();
#endif 
   if ( !forward_looking ) {
      my_screen_info.rot_amount = 0.0f;
   }
   perspective_matrix_action(PUSH_MATRIX);
#ifdef IRISGL
   ortho2 ( ortho_minX, ortho_maxX, ortho_minY, ortho_maxY );
#else //OPENGL
   int gl_matrix_mode;
   glGetIntegerv (GL_MATRIX_MODE, &gl_matrix_mode);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D ( ortho_minX, ortho_maxX, ortho_minY, ortho_maxY );
   glMatrixMode(gl_matrix_mode);
#endif
   pfSetVec2 ( bsphere_center, world_centerX, world_centerY );
   bounding_sphere ( RESET_BOUNDS, bsphere_center, range_meters );

   return (&my_screen_info);
}


void
radar_cleanup ( int forward_looking )
{
   if ( forward_looking ) {
#ifdef IRISGL
      popmatrix();
#else // OPENGL
      pfPopMatrix ();
#endif
   }
#ifdef IRISGL
   popmatrix();
#else // OPENGL
    pfPopMatrix ();
#endif
   perspective_matrix_action(POP_MATRIX);
}


void
perspective_matrix_action ( MATRIX_ACTION action )
{
#ifdef IRISGL
   static Matrix restore_matrix;
   long gl_matrix_mode;
#else // OPENGL
   static float restore_matrix[16];
   int gl_matrix_mode;
#endif
   
   switch ( action ) {
      case PUSH_MATRIX:
#ifdef IRISGL
         gl_matrix_mode = getmmode();
         mmode(MPROJECTION);
         getmatrix(restore_matrix);
         mmode(gl_matrix_mode);
#else // OPENGL
         glGetFloatv (GL_PROJECTION_MATRIX,restore_matrix);
#endif
         break;
      case POP_MATRIX:
#ifdef IRISGL
         gl_matrix_mode = getmmode();
         mmode(MPROJECTION);
         loadmatrix(restore_matrix);
         mmode(gl_matrix_mode);
#else // OPENGL
         glGetIntegerv (GL_MATRIX_MODE, &gl_matrix_mode);
         glMatrixMode (GL_PROJECTION);
         glLoadMatrixf (restore_matrix);
         glMatrixMode(gl_matrix_mode);
#endif
         break;
      default:
         break;
   }

}


void
draw_radar_rings ( float world_centerX, float world_centerY,
                   float range_meters, unsigned int alpha )
{
#ifdef IRISGL
   long old_linewidth = getlwidth();
#else //OPENGL
   int old_linewidth;
   glGetIntegerv (GL_LINE_WIDTH, &old_linewidth);
   npsFont(DEFFONT);
   float range_ratio = RING_SIZE / MIN_RADAR_RANGE;
#endif 
   float half_range = range_meters/2.0f;
   char text[8];
#ifdef IRISGL
   linewidth(2);

   cpack(0x000000ff|alpha);
   circ(world_centerX,world_centerY,half_range);
   cpack(0x0000ffff|alpha);
   circ(world_centerX,world_centerY,range_meters);
   cpack(0x0000ff00|alpha);
   cmov2 ( world_centerX-1, world_centerY-half_range+1 );
   sprintf ( text, "%d", (int)half_range );
   charstr ( text );
   cmov2 ( world_centerX-1, world_centerY-range_meters+1 );
   sprintf ( text, "%d", (int)range_meters );
   charstr ( text );


   linewidth(old_linewidth);
#else // OPENGL
   glLineWidth(2);

   GLUquadricObj *qobj = gluNewQuadric();
   pfPushMatrix();
   glTranslatef ( world_centerX, world_centerY, 0.0f );

   glColor4f(1.0f, 0.0f, 0.0f, alpha/255.0f);
   gluDisk(qobj,half_range-(range_meters*range_ratio),half_range,100,1);
   glColor4f(1.0f, 1.0f, 0.0f, alpha/255.0f);
   gluDisk(qobj,range_meters-(range_meters*range_ratio),range_meters,100,1);

   pfPopMatrix();
   gluDeleteQuadric(qobj);
 
   glColor4f(0.0f, 1.0f, 0.0f, alpha/255.0f);
   sprintf ( text, "%d", (int)half_range );
   npsDrawStringPos(text,world_centerX-1.0f, world_centerY-half_range+1.0f,0.0f);
   sprintf ( text, "%d", (int)range_meters );
   npsDrawStringPos(text,world_centerX-1.0f, world_centerY-range_meters+1.0f,0.0f);
   glLineWidth(old_linewidth);
#endif

}


int
read_landmarks ( LANDMARK_ENTRY *lm_data )
{
/*
   int lm_count = -1;
   char tempstr[80];
   char lm_char;
   float lm_X, lm_Y;
   ifstream lm_file ( (char *)G_static_data->landmark_file );
*/
   struct LAND im_landmarks[MAX_LANDMARKS];
   int num_landmarks;

   initman->get_landmarks((struct LAND**) im_landmarks, &num_landmarks);

   for (int i = 0; i < num_landmarks; i++) {
      lm_data[i].ch = im_landmarks[i].icon;
      lm_data[i].Xpos = im_landmarks[i].x;
      lm_data[i].Ypos = im_landmarks[i].y;
      /*
      cout << "Landmark=" << im_landmarks[i].icon << ","
                          << im_landmarks[i].x << ","
                          << im_landmarks[i].y << endl;
      */
   }
   return (num_landmarks);


/*
   if ( !lm_file )
      return lm_count;
   else
      {
      lm_count = 0;
      while ( !lm_file.eof() && (lm_count < MAX_LANDMARKS) )
         {
         lm_file >> lm_char >> lm_X >> lm_Y;
         if (!lm_file)
            {
            if (!lm_file.eof())
               cerr << "Error reading from landmarks file.\n";
            }
         else
            {
            lm_file.getline ( tempstr, 80 );
            lm_data[lm_count].ch = lm_char;
            lm_data[lm_count].Xpos = lm_X;
            lm_data[lm_count].Ypos = lm_Y;
            lm_count++;
            }
         }
      return lm_count;
      }
*/
}


void
draw_landmarks ( int forward_looking, unsigned int alpha, int reload )
{
   static int first_time = TRUE;
   static LANDMARK_ENTRY landmark[MAX_LANDMARKS];
   static int num_landmarks = 0;
#ifdef IRISGL
   static Matrix temp_matrix;
#else //OPENGL
   static float temp_matrix[16];
#endif
   static char text[5];
   pfVec2 landmark_pos;

   if ( first_time || reload ) {
      first_time = FALSE;
      num_landmarks = read_landmarks( landmark );
   }
      
   if ( forward_looking ) {
#ifdef IRISGL
      getmatrix(temp_matrix);
      popmatrix();
#else //OPENGL
      glGetFloatv (GL_MODELVIEW_MATRIX,temp_matrix);
      //glGetFloatv (GL_PROJECTION_MATRIX,temp_matrix);
      pfPopMatrix();
#endif
   }

#ifdef IRISGL
   font(1);
   cpack (0x00ffffff|alpha);
#else //OPENGL
   npsFont(ICONFONT);
   glColor4f(1.0f, 1.0f, 1.0f, alpha/255.0f);
#endif
   if ( num_landmarks > 0 ) {
     for ( int i = 0; i < num_landmarks; i++ ) {
        pfSetVec2 ( landmark_pos, landmark[i].Xpos, landmark[i].Ypos );
        if ( bounding_sphere(CHECK_FOR_CONTAINMENT,landmark_pos) ) {
#ifdef IRISGL
           cmov2 ( landmark[i].Xpos, landmark[i].Ypos );
           sprintf ( text, "%c", landmark[i].ch );
           charstr ( text );
#else //OPENGL
           sprintf ( text, "%c", landmark[i].ch );
           npsDrawStringPos(text,landmark[i].Xpos, landmark[i].Ypos,0.0f);
#endif
        }
     }
   }
#ifdef IRISGL
   font(0);
#else //OPENGL
   npsFont(DEFFONT);
#endif

   if ( forward_looking ) {
#ifdef IRISGL
      pushmatrix();
      loadmatrix(temp_matrix);
#else //OPENGL
      pfPushMatrix();
      glLoadMatrixf (temp_matrix);
#endif
   }

}


void
draw_bounding_box ( float bbox_xmin, float bbox_xmax,
                    float bbox_ymin, float bbox_ymax,
                    int forward_looking, unsigned int alpha )
{
#ifdef IRISGL
   static Matrix temp_matrix;
   long old_linewidth = getlwidth();
#else //OPENGL
   static float temp_matrix[16];
   int old_linewidth;
   glGetIntegerv (GL_LINE_WIDTH, &old_linewidth);
   pfVec2 v; //vertex for drawing lines
#endif

   if ( forward_looking ) {
#ifdef IRISGL
      getmatrix(temp_matrix);
      popmatrix();
#else //OPENGL
      glGetFloatv (GL_MODELVIEW_MATRIX,temp_matrix);
      //glGetFloatv (GL_PROJECTION_MATRIX,temp_matrix);
      pfPopMatrix();
#endif
   }
#ifdef IRISGL
   linewidth(1);
   cpack(0x00ffffff|alpha);
   move2 ( bbox_xmin, bbox_ymin );
   draw2 ( bbox_xmax, bbox_ymin );
   draw2 ( bbox_xmax, bbox_ymax );
   draw2 ( bbox_xmin, bbox_ymax );
   draw2 ( bbox_xmin, bbox_ymin );
   linewidth(old_linewidth);
#else //OPENGL
   glColor4f(1.0f, 1.0f, 1.0f, alpha/255.0f);
   glLineWidth(1);
   glBegin(GL_LINE_STRIP);
      PFSET_VEC2(v, bbox_xmin, bbox_ymin); glVertex2fv(v);
      PFSET_VEC2(v, bbox_xmax, bbox_ymin); glVertex2fv(v);
      PFSET_VEC2(v, bbox_xmax, bbox_ymax); glVertex2fv(v);
      PFSET_VEC2(v, bbox_xmin, bbox_ymax); glVertex2fv(v);
      PFSET_VEC2(v, bbox_xmin, bbox_ymin); glVertex2fv(v);
   glEnd();
   glLineWidth(old_linewidth);
#endif

   if ( forward_looking ) {
#ifdef IRISGL
      pushmatrix();
      loadmatrix(temp_matrix);
#else //OPENGL
      pfPushMatrix();
      glLoadMatrixf (temp_matrix);
#endif
   }


}


void
draw_veh_direction_indicator ( float world_centerX, float world_centerY,
                               float range_meters, float heading,
                               int forward_looking, unsigned int alpha )
{
#ifdef IRISGL
   static Matrix temp_matrix;
   long old_linewidth = getlwidth();
#else //OPENGL
   static float temp_matrix[16];
   int old_linewidth;
//   int gl_matrix_mode;
   glGetIntegerv (GL_LINE_WIDTH, &old_linewidth);
   pfVec2 v; //vertex for drawing lines
#endif
   float reflength, leadery, leaderx;

   if ( forward_looking ) {
#ifdef IRISGL
      getmatrix(temp_matrix);
      popmatrix();
#else //OPENGL
      glGetFloatv (GL_MODELVIEW_MATRIX,temp_matrix);
      pfPopMatrix();
#endif
   }

#ifdef IRISGL
   cpack (0x00ff00ff|alpha);
   move2 (world_centerX, world_centerY );
   linewidth(2);
#else //OPENGL
   glColor4f(1.0f, 0.0f, 1.0f, alpha/255.0f);
   glLineWidth(2);
#endif
   reflength = range_meters * 0.1f;
   pfSinCos (90.0f-heading, &leadery, &leaderx);
#ifdef IRISGL
   draw2 (world_centerX + (reflength * leaderx),
          world_centerY + (reflength * leadery));
   linewidth(old_linewidth);
#else //OPENGL
   glBegin(GL_LINES);
      PFSET_VEC2(v, world_centerX, world_centerY); glVertex2fv(v);
      PFSET_VEC2(v, world_centerX + (reflength * leaderx),
                    world_centerY + (reflength * leadery)); glVertex2fv(v);
   glEnd();
   glLineWidth(old_linewidth);
#endif

   if ( forward_looking ) {
#ifdef IRISGL
      pushmatrix();
      loadmatrix(temp_matrix);
#else //OPENGL
      pfPushMatrix();
      glLoadMatrixf (temp_matrix);
#endif
   }

}


char
lookup_icon_char_from_type ( int type )
{
   int found = FALSE;
   int i = 0;
   char ret_val = '\0';

   while ( !found && (i < G_static_data->num_radar_divs) ) {
      if ( (type >= G_static_data->radar[i].min) &&
           (type <= G_static_data->radar[i].max) ) {
         found = TRUE;
         ret_val = G_static_data->radar[i].icon_char;
      }
      else {
         i++;
      }
   }
   if (!found ) {
      ret_val = D_ICON_CHAR;
   }

   return ret_val;
}


void
draw_vehicle_icons ( HUD_VEH_REC *veh_list, 
                     int highlight_vid,
                     int forward_looking,
                     int use_veh_icons,
                     int elevation_coloring,
                     float ref_elevation,
                     unsigned int alpha )
{
   int count = 0;
   HUD_VEH_REC *next_veh = veh_list;
   char text[5];
   float elev_change = 0.0f;
#ifdef IRISGL
   long old_font;
   static Matrix temp_matrix;
#else //OPENGL
   static float temp_matrix[16];
#endif
   pfVec2 twoDposition;

   if ( forward_looking ) {
#ifdef IRISGL
      getmatrix(temp_matrix);
      popmatrix();
#else //OPENGL
      glGetFloatv (GL_MODELVIEW_MATRIX,temp_matrix);
      //glGetFloatv (GL_PROJECTION_MATRIX,temp_matrix);
      pfPopMatrix();
#endif
   }

   if ( use_veh_icons ) {
#ifdef IRISGL
      old_font = getfont();
      font(1);
#else //OPENGL
      npsFont(ICONFONT);
#endif
   }

   while ( (count < HUD_MAX_VEH) && (next_veh->type != NOSHOW) ) {

      if ( use_veh_icons ) {
         text[0] = lookup_icon_char_from_type(next_veh->type);
         text[1] = '\0';
      }
      else {
         sprintf(text,"%d",next_veh->vid);
      }

      if (elevation_coloring) {
         elev_change = ref_elevation - next_veh->elevation;
         if (elev_change < -2000.0f) {
#ifdef IRISGL
            cpack(0x00ffffff|alpha);
#else //OPENGL
            glColor4f(1.0f, 1.0f, 1.0f, alpha/255.0f);
#endif
         }
         else if (elev_change <= -150.0f) {
#ifdef IRISGL
            cpack(0x0000ffff|alpha);
#else //OPENGL
            glColor4f(1.0f, 1.0f, 0.0f, alpha/255.0f);
#endif
         }
         else if (elev_change > 2000.0f) {
#ifdef IRISGL
            cpack(0x00111177|alpha);
#else //OPENGL
            glColor4f(0.46f, 0.07f, 0.07f, alpha/255.0f);
#endif
         }
         else if (elev_change >= 150.0f) {
#ifdef IRISGL
            cpack(0x00212188|alpha);
#else //OPENGL
            glColor4f(0.53f, 0.12f, 0.12f, alpha/255.0f);
#endif
         }
         else {
#ifdef IRISGL
            cpack(0x002288ff|alpha);
#else //OPENGL
            glColor4f(1.0f, 0.53f, 0.13f, alpha/255.0f);
#endif
         }
      }
      else {
         if (next_veh->force == ForceID_Blue) {
#ifdef IRISGL
            cpack(0x00ff8f00|alpha);
#else //OPENGL
            glColor4f(0.0f, 0.56f, 1.0f, alpha/255.0f);
#endif
         }
         else if (next_veh->force == ForceID_Red) {
#ifdef IRISGL
            cpack(0x000000ff|alpha);
#else //OPENGL
            glColor4f(1.0f, 0.0f, 0.0f, alpha/255.0f);
#endif
         }
         else if (next_veh->force == ForceID_White) {
#ifdef IRISGL
            cpack(0x00ffffff|alpha);
#else //OPENGL
            glColor4f(1.0f, 1.0f, 1.0f, alpha/255.0f);
#endif
         }
         else {
#ifdef IRISGL
            cpack(0x0000ff00|alpha);
#else //OPENGL
            glColor4f(0.0f, 1.0f, 0.0f, alpha/255.0f);
#endif
         }
      }

      pfSetVec2 ( twoDposition, next_veh->location[PF_X],
                                next_veh->location[PF_Y] );
      if ( bounding_sphere(CHECK_FOR_CONTAINMENT, twoDposition) ) {
#ifdef IRISGL
         cmov2(next_veh->location[PF_X],next_veh->location[PF_Y]);
         charstr(text);
#else //OPENGL
         npsDrawStringPos(text,next_veh->location[PF_X],
                               next_veh->location[PF_Y], 0.0f);
#endif
         if ( next_veh->vid == highlight_vid ) {
#ifdef IRISGL
            cpack(0x00FFFFFF|alpha);
            cmov2(next_veh->location[PF_X],next_veh->location[PF_Y]);
            sprintf(text,"%c",'f');
            charstr(text);
#else //OPENGL
            glColor4f(1.0f, 1.0f, 1.0f, alpha/255.0f);
            sprintf(text,"%c",'f');
            npsDrawStringPos(text,next_veh->location[PF_X],
                                  next_veh->location[PF_Y],0.0f);
#endif
         }
         if ( next_veh->status & Appearance_DamageDestroyed ) {
#ifdef IRISGL
            cpack(0x00000000|alpha);
            cmov2(next_veh->location[PF_X],next_veh->location[PF_Y]);
            sprintf(text,"%c",'/');
            charstr(text);
#else //OPENGL
            glColor4f(0.0f, 0.0f, 0.0f, alpha/255.0f);
            sprintf(text,"%c",'/');
            npsDrawStringPos(text,next_veh->location[PF_X],
                                  next_veh->location[PF_Y],0.0f);
#endif
         }
      }
      count++;
      next_veh++;
   }

   if ( use_veh_icons ) {
#ifdef IRISGL
      font(old_font);
#else //OPENGL
      npsFont(DEFFONT);
#endif
   }

   if ( forward_looking ) {
#ifdef IRISGL
      pushmatrix();
      loadmatrix(temp_matrix);
#else //OPENGL
      pfPushMatrix();
      glLoadMatrixf (temp_matrix);
#endif
   }


}


radar_screen_info *
draw_radar ( pfChannel *chan, pfVec3 center_position, 
             float range_meters, float heading, float gaze_offset,
             HUD_VEH_REC *veh_list,
             int highlight_vid,
             float bound_xmin, float bound_xmax,
             float bound_ymin, float bound_ymax,
             int draw_rings, int draw_lmarks, int draw_bbox,
             int draw_veh_dir_ind, int draw_veh_icons,
             int use_veh_icons, int elevation_coloring,
             int forward_looking, unsigned int alpha,
             float viewportl, float viewportr,
             float viewportb, float viewportt )
{
   long original_trans_mode;
   unsigned int trans_amount = 
#ifdef IRISGL
                                alpha<<24;
#else //OPENGL
                                alpha;
#endif

   radar_screen_info *screen_info;
   if ( alpha < 255 ) {
      original_trans_mode = pfGetTransparency();
      pfOverride(PFSTATE_TRANSPARENCY, PF_OFF );
      pfTransparency(PFTR_ON);
      pfOverride(PFSTATE_TRANSPARENCY, PF_ON );
   } 

   screen_info = 
      radar_setup ( chan, center_position[PF_X], center_position[PF_Y],
                    range_meters, forward_looking, heading, gaze_offset,
                    viewportl, viewportr, viewportb, viewportt );

   if ( draw_rings ) {
      draw_radar_rings ( center_position[PF_X], center_position[PF_Y],
                         range_meters, trans_amount );
   }
   if ( draw_lmarks ) {
      draw_landmarks ( forward_looking, trans_amount );
   }
   if ( draw_bbox ) {
      draw_bounding_box ( bound_xmin, bound_xmax, bound_ymin, bound_ymax,
                          forward_looking, trans_amount );
   }
   if ( draw_veh_dir_ind ) {
      draw_veh_direction_indicator ( center_position[PF_X],
                                     center_position[PF_Y],
                                     range_meters, heading,
                                     forward_looking, trans_amount );
   }
   if ( draw_veh_icons ) {
      draw_vehicle_icons ( veh_list, highlight_vid, forward_looking,
                           use_veh_icons, elevation_coloring,
                           center_position[PF_Z], trans_amount );
   }
   radar_cleanup( forward_looking );

   if ( alpha < 255 ) {
      pfOverride(PFSTATE_TRANSPARENCY, PF_OFF );
      pfTransparency(original_trans_mode);
      pfOverride(PFSTATE_TRANSPARENCY, PF_ON );
   }

   return screen_info;
}

