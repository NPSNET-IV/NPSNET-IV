
#include <string.h>
#include <math.h>

#include "unmanned_veh.h"
#include "common_defs2.h"
#include "terrain.h"
#include "macros.h"
#include "entity.h"
#include "conversion_const.h"
#include "interface_const.h"
#ifndef IRISGL
#include "fontdef.h"
#endif

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;
extern          VEH_LIST     *G_vehlist;

// Locals used by this library
static pfVec3 ZeroVec = {0.0f,0.0f,0.0f};

#ifndef XMAXSCREEN
#define XMAXSCREEN 1279
#endif

//************************************************************************//
//**************** class UNMANNED_VEH ************************************//
//************************************************************************//

UNMANNED_VEH::UNMANNED_VEH(int id,int atype, ForceID the_force)
:AIRCRAFT_VEH(id,atype,the_force)
{

#ifdef DATALOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
   G_dynamic_data->hud_callback = drawhud_UAV;
#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

#ifdef TRACE
   cerr << "Unmanned vehicle constructor\n";
#endif
}


vtype UNMANNED_VEH::gettype()
{
   return (UNMANNED);
}


movestat UNMANNED_VEH::move()
// move the vehicle for the next frame
{
   pfMatrix orient;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 up = { 0.0f, 0.0f, 1.0f };
   pfVec3 tempeye;
   float speed_setting = cdata.thrust * MAX_UAV_SPEED;
   //float thrust_setting = cdata.thrust * MAX_THRUST;
   float ground, pitch_factor, altitude;
   pfVec3 oldpos;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tUNMANNED_VEH::move" << endl;
#endif

   update_time ();
   collect_inputs ();

   pfCopyVec3(oldpos,posture.xyz);

#ifdef TRACE
   cerr << "Moving an Unmanned vehicle \n";
#endif

   if (!(paused || tethered || cdata.rigid_body))
      {

      // Move to the location we should be at after the last time interval
      posture.xyz[X] += velocity[X]*cdata.delta_time;
      posture.xyz[Y] += velocity[Y]*cdata.delta_time;
      posture.xyz[Z] += velocity[Z]*cdata.delta_time;
      tempeye[X] = eyepoint[X] + (velocity[X] * cdata.delta_time);
      tempeye[Y] = eyepoint[Y] + (velocity[Y] * cdata.delta_time);
      tempeye[Z] = eyepoint[Z] + (velocity[Z] * cdata.delta_time);

      ground = gnd_level(posture.xyz);
      altitude = posture.xyz[Z] - ground + bounding_box.min[PF_Z];

      // Now see if we hit something in this time interval
      if (!G_static_data->stealth)
         {
         if ( check_collide( oldpos, posture.xyz,
                             posture.hpr,
                             speed, altitude) )
            return(HITSOMETHING);
         }


      // Set the speed for the next frame, pitch_factor adds or
      // subtracts from speed at level flight
      pitch_factor = 0.5f * (sinf(posture.hpr[PITCH] * DEG_TO_RAD));

      if ((input_control == NPS_FCS) || (input_control == NPS_KAFLIGHT) || 
          (input_control == NPS_BASIC_PANEL))
         {
         if ( (speed < speed_setting) && IS_SMALL(pitch_factor) )
            speed += FCS_SPEED_CHANGE - pitch_factor;
         if ( (speed > speed_setting) && IS_SMALL(pitch_factor) )
            speed -= FCS_SPEED_CHANGE - pitch_factor;
         }
      else
         {
         speed += cdata.thrust;
         cdata.thrust = 0.0f;
         }

      speed -= pitch_factor;
      if ( (speed < UAV_STALL_SPEED) &&
           (posture.xyz[Z] > ground) )
         {
         posture.xyz[Z] -=
            ((UAV_STALL_SPEED-speed)*10.0f)/UAV_STALL_SPEED;
         posture.hpr[PITCH] -= 2.0f;
         }
      if (posture.xyz[Z] < ground)
         posture.xyz[Z] = ground;

      if (speed > MAX_UAV_SPEED)
         speed = MAX_UAV_SPEED;


      // set pitch and roll when in the air
      if ( G_static_data->stealth || (altitude > 0.0f))
         {
         posture.hpr[ROLL] =
                  fmod ( cdata.roll*4.0f + posture.hpr[ROLL], 360.0f);
         if (posture.hpr[ROLL] > 180.0f)
            posture.hpr[ROLL] -= 360.0f;
         if (posture.hpr[ROLL] < -180.0f)
            posture.hpr[ROLL] += 360.0f;

         posture.hpr[PITCH] =
            fmod ( cosf(posture.hpr[ROLL]*DEG_TO_RAD) * cdata.pitch
               * 2.0f + posture.hpr[PITCH], 180.0f);

         if (posture.hpr[PITCH] > 90.0f)
            {
            posture.hpr[PITCH] = 180.0f - posture.hpr[PITCH];
            posture.hpr[ROLL] -= 180.0f;
            posture.hpr[HEADING] -= 180.0f;
            if ( posture.hpr[HEADING] < 0.0f)
                posture.hpr[HEADING] += 360.0f;
            }
         if (posture.hpr[PITCH] < -90.0f)
            {
            posture.hpr[PITCH] = -180.0f - posture.hpr[PITCH];
            posture.hpr[ROLL] -= 180.0f;
            posture.hpr[HEADING] -= 180.0f;
            if ( posture.hpr[HEADING] < 0.0f)
                posture.hpr[HEADING] += 360.0f;
            }
          }
      else  // not in the air
          {
          // only allow the vehicle to go up when on the ground
          if (cdata.pitch < 0 )
             {
             cdata.pitch = 0.0f;
             }
         // still on the ground, so steer with stick
         posture.hpr[HEADING] -= cdata.roll;
         grnd_orient(posture,&(bounding_box));
         posture.hpr[PITCH] += cdata.pitch;
         }


      // Calculate the heading
      posture.hpr[HEADING] = fmod (( posture.hpr[HEADING] -
              ((cdata.pitch*2.0f+2.0f) * sinf(posture.hpr[ROLL] *
                DEG_TO_RAD) * speed/MAX_UAV_SPEED)),360.0f);

      if (posture.hpr[HEADING] < 0.0f)
         posture.hpr[HEADING] += 360.0f;

      // Zero control data for the next frame
      cdata.pitch = cdata.roll = 0.0f;

      // Make sure we are still in bounds, if not, stop it at the boundary.
      if (at_a_boundary(posture.xyz))
         {
         posture.hpr[HEADING] =
              fmod(posture.hpr[HEADING] + 180.0f, 360.0f);
         pfNegateVec3(velocity,ZeroVec);
         }
      else  // Still in bounds
         {
         // Calculate the new velocity vector
         pfMakeEulerMat ( orient, posture.hpr[HEADING],
                                  posture.hpr[PITCH],
                                  posture.hpr[ROLL]);

         pfXformPt3 ( direction, direction, orient );
         pfNormalizeVec3 ( direction );
         pfScaleVec3 ( velocity, speed, direction);
         }

      } // End !paused !tethered !rigid_body

   // If tethered, let the other vehicle calculate our position for us
   if (tethered)
      {
      if ((cdata.vid > -1) && G_vehlist[cdata.vid].vehptr)
         {
         if ( tethered == 1)
            G_vehlist[cdata.vid].vehptr->teleport(posture,speed);
         else if ( tethered == 2)
            G_vehlist[cdata.vid].vehptr->attach(posture,speed,
                                                G_dynamic_data->attach_dis);
         }
      else
         {
#ifdef DATALOCK
         pfDPoolLock ( (void *) G_dynamic_data );
#endif
         tethered = FALSE;
#ifdef DATALOCK
         pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         }
      }

/* // This segment of code is a hook for the implementing the
   //  rigid_body class for vehicle updates
   if (cdata.rigidbody)
      {
      if (cdata.reset)
         // make call to set rigid_body parameters to the pdata values.
      // do the normal updates to pdata
      }
*/

   pfMakeEulerMat ( orient, posture.hpr[HEADING],
                            posture.hpr[PITCH],
                            posture.hpr[ROLL]);

   pfXformPt3 ( up, up, orient );
   pfNormalizeVec3 ( up );
   pfScaleVec3 ( up, EYE_OFFSET, up);
   pfAddVec3 ( eyepoint, posture.xyz, up );
   pfCopyVec3 ( firepoint, eyepoint );

   altitude = ::get_altitude(posture.xyz) + bounding_box.min[PF_Z];
   if ( (!tethered) && (!G_static_data->stealth) && (altitude < -0.1) )
      {
#ifdef DATALOCK
      pfDPoolLock ( (void *) G_dynamic_data );
#endif
//      G_dynamic_data->cause_of_death = UNDER_GROUND_DEATH;
//      G_dynamic_data->dead = DYING;
      posture.xyz[PF_Z] -= altitude;
#ifdef DATALOCK
      pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
      }

   check_targets ( posture.xyz, posture.hpr, 
                   look.hpr );

   //put the vehicle in the correct location in space
   pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
   pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);

#ifdef DEBUG_SCALE
   pfDCSScale(hull,2.0f);
#endif

   thrust = cdata.thrust;
   return(FINEWITHME);

}


void drawhud_UAV ( pfChannel *, void *data )
{
  PASS_DATA *pdata = (PASS_DATA *)data;
  float real_heading = (360.0f - pdata->posture.hpr[HEADING]);
  float view_heading;
  static float view_right;
  static float view_left;
  static float view_top;
  static float view_bottom;
  static float width;
  static float height;
  static int halfX;
  static int halfY;
  static int compsize;
  static float completter;
  static float fieldofview;
  static float tempdir;
  static char valstr[80];
  static float base;
  static float top;
  static float mytop;
  static float ypos;
  static float step;
  static float speed;
  static float alt;
  static int camera;
  static int camlock;

#ifdef DATALOCK
  pfDPoolLock ( (void *) G_dynamic_data );
#endif
  view_right = G_dynamic_data->view.right;
  view_left = G_dynamic_data->view.left;
  view_top = G_dynamic_data->view.top;
  view_bottom = G_dynamic_data->view.bottom;
  fieldofview = G_dynamic_data->fovx;
  camera = G_dynamic_data->camera;
  camlock = G_dynamic_data->camlock;
#ifdef DATALOCK
  pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

  while ( real_heading >= 360.0f )
     real_heading -= 360.0f;
  while ( real_heading < 0.0f )
     real_heading += 360.0f;
  view_heading = real_heading - pdata->look.hpr[HEADING];
  while ( view_heading >= 360.0f )
     view_heading -= 360.0f;
  while ( view_heading < 0.0f )
     view_heading += 360.0f;

  width = view_right - view_left;
  height = view_top - view_bottom;
  halfX = int(width / 2.0f);
  halfY = int(height / 2.0f);
#ifdef IRISGL
  ortho2 ( 0.0f, width, 0.0f, height );
#else //OPENGL
  // This is here because OpenGL handles the MODELVIEW & PROJECTION matrixes
  // differently then does IRISGL
  // OpenGL multiplys the matrices using gluOrtho, but GL does not.
  // Therefore must load identity on PROJECTION matrix before gluOrtho
  int gl_matrix_mode;
  glGetIntegerv (GL_MATRIX_MODE, &gl_matrix_mode);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D( 0.0f, width, 0.0f, height );
  pfVec2 v; //vertex for drawing lines
  npsFont(DEFFONT);
#endif

  /*draw a white aiming point*/
#ifdef IRISGL
  RGBcolor(255,255,255);
  move2i(halfX, halfY + int(0.1953125*height));
  draw2i(halfX, halfY + int(0.048828125*height));
  move2i(halfX, halfY - int(0.1953125*height));
  draw2i(halfX, halfY - int(0.048828125*height));

  move2i(halfX + int(0.1953125*height), halfY);
  draw2i(halfX + int(0.048828125*height), halfY);
  move2i(halfX - int(0.1953125*height), halfY);
  draw2i(halfX - int(0.048828125*height), halfY);
#else //OPENGL
  glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
  glBegin(GL_LINES);
     PFSET_VEC2(v, halfX, halfY + int(0.1953125*height)); glVertex2fv(v);
     PFSET_VEC2(v, halfX, halfY + int(0.048828125*height)); glVertex2fv(v);
     PFSET_VEC2(v, halfX, halfY - int(0.1953125*height)); glVertex2fv(v);
     PFSET_VEC2(v, halfX, halfY - int(0.048828125*height)); glVertex2fv(v);
     PFSET_VEC2(v, halfX + int(0.1953125*height), halfY); glVertex2fv(v);
     PFSET_VEC2(v, halfX + int(0.048828125*height), halfY); glVertex2fv(v);
     PFSET_VEC2(v, halfX - int(0.1953125*height), halfY); glVertex2fv(v);
     PFSET_VEC2(v, halfX - int(0.048828125*height), halfY); glVertex2fv(v);
  glEnd();
#endif


  /*View Direction*/
  sprintf(valstr,"Camera Dir:%6.1f", view_heading );
#ifdef IRISGL
  cmov2(XMAXSCREEN - (0.15625*width),(0.029296875*height));
  charstr(valstr);
#else //OPENGL
  npsDrawStringPos(valstr,XMAXSCREEN-(0.15625*width),(0.029296875*height),0.0f);
#endif

  sprintf(valstr,"FoV(deg):  %6.1f", fieldofview);
#ifdef IRISGL
  /*Field of view*/
  cmov2(XMAXSCREEN - (0.15625*width),(0.048828125*height));
  charstr(valstr);
#else //OPENGL
  npsDrawStringPos(valstr,XMAXSCREEN-(0.15625*width),(0.048828125*height),0.0f);
#endif

  /* Which way are we looking - foward or out the camera? */
  strcpy ( valstr, "View:" );
  if (camera)
     strcat ( valstr, "      Camera");
  else
     strcat ( valstr, "     Forward");
#ifdef IRISGL
  cmov2(XMAXSCREEN - (0.15625*width),(0.087890625*height));
  charstr(valstr);
#else
  npsDrawStringPos(valstr,XMAXSCREEN-(0.15625*width),(0.087890625*height),0.0f);
#endif

  /* Is the camera fixed on terrain (locked) or not? */
  strcpy ( valstr, "Camera:" );
  if (camlock)
     strcat ( valstr, "    Locked");
  else
     strcat ( valstr, "  Unlocked");
#ifdef IRISGL
  cmov2(XMAXSCREEN - (0.15625*width),(0.10742188*height));
  charstr(valstr);
#else
  npsDrawStringPos(valstr,XMAXSCREEN-(0.15625*width),(0.10742188*height),0.0f);
#endif


  compsize = int(0.073242188*height);
  completter = compsize + (0.01 * height);

  /*end of text, now we start in on the fun stuff*/
  /*draw the compass */
#ifdef IRISGL
  pushmatrix();
  translate ( float(halfX), (0.09765625*height), 0.0f );

  linewidth(2);

  RGBcolor(255,0,0);
  move2i(0,0);
  draw2i(0, compsize+20);
  cmov2i(-10,compsize+20);
  tempdir = real_heading;
  sprintf(valstr, "%3.0f",tempdir);
  charstr(valstr);

  RGBcolor(255,255,255);
  circi(0,0,compsize);
  /*figure out where to put the letters*/
  tempdir = real_heading * DEG_TO_RAD - HALFPI;
  cmov2((fcos(tempdir)*completter),(fsin(tempdir)*completter));
  charstr("S");
  tempdir = tempdir + HALFPI;
  cmov2((fcos(tempdir)*completter),(fsin(tempdir)*completter));
  charstr("E");
  tempdir = tempdir + HALFPI;
  cmov2((fcos(tempdir)*completter),(fsin(tempdir)*completter));
  charstr("N");
  tempdir = tempdir + HALFPI;
  cmov2((fcos(tempdir)*completter),(fsin(tempdir)*completter));
  charstr("W");

  /*now the lookangle*/
  RGBcolor(255,0,255);
  tempdir = (pdata->look.hpr[HEADING]*DEG_TO_RAD) + HALFPI;
  move2i(0, 0);
  draw2((fcos(tempdir)*compsize),(fsin(tempdir)*compsize));

  /*don't forget the field of view*/
  RGBcolor(0,255,255);
  tempdir += fieldofview* 0.5 *DEG_TO_RAD;
  move2i(0, 0);
  draw2((fcos(tempdir)*compsize),(fsin(tempdir)*compsize));
  tempdir -= fieldofview * DEG_TO_RAD;
  move2i(0, 0);
  draw2((fcos(tempdir)*compsize),(fsin(tempdir)*compsize));
  /*end of compass*/

  linewidth(1);

  popmatrix();
#else //OPENGL
     pfPushMatrix();
     glTranslatef ( completter+12.0f, completter+12.0f, 0.0f );
     glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
     glLineWidth(2);
     glBegin(GL_LINES);
        PFSET_VEC2(v, 0.0f, 0.0f); glVertex2fv(v);
        PFSET_VEC2(v, 0.0f, (float)(compsize+20)); glVertex2fv(v);
     glEnd();

     tempdir = real_heading;
     sprintf(valstr, "%3.0f",tempdir);
     npsDrawStringPos(valstr,-10.0f,(float)(compsize+20),0.0f);

     glColor4f(1.0f, 1.0f, 1.0f, 0.0f);

     // Draw The circle
     {
     GLUquadricObj *qobj = gluNewQuadric();
     gluDisk(qobj,(float)compsize-1.0f,(float)compsize,100,1);
     gluDeleteQuadric(qobj);
     }

     /*figure out where to put the letters*/
     tempdir = real_heading * DEG_TO_RAD - HALFPI;
     npsDrawStringPos("S",(fcos(tempdir)*completter),(fsin(tempdir)*completter),0.0f);
     tempdir = tempdir + HALFPI;
     npsDrawStringPos("E",(fcos(tempdir)*completter),(fsin(tempdir)*completter),0.0f);
     tempdir = tempdir + HALFPI;
     npsDrawStringPos("N",(fcos(tempdir)*completter),(fsin(tempdir)*completter),0.0f);
     tempdir = tempdir + HALFPI;
     npsDrawStringPos("W",(fcos(tempdir)*completter),(fsin(tempdir)*completter),0.0f);

     //Look Heading
     glColor4f(1.0f, 0.0f, 1.0f, 0.0f);
     tempdir = (pdata->look.hpr[HEADING]*DEG_TO_RAD) + HALFPI;
     glBegin(GL_LINES);
        PFSET_VEC2(v, 0.0f, 0.0f); glVertex2fv(v);
        PFSET_VEC2(v, (fcos(tempdir)*compsize),(fsin(tempdir)*compsize));
        glVertex2fv(v);
     glEnd();

     //Field of View
     glColor4f(0.0f, 1.0f, 1.0f, 0.0f);
     tempdir += fieldofview* 0.5 *DEG_TO_RAD;
     glBegin(GL_LINES);
        PFSET_VEC2(v, 0.0f, 0.0f); glVertex2fv(v);
        PFSET_VEC2(v, (fcos(tempdir)*compsize),(fsin(tempdir)*compsize));
        glVertex2fv(v);
     glEnd();
     tempdir -= fieldofview * DEG_TO_RAD;
     glBegin(GL_LINES);
        PFSET_VEC2(v, 0.0f, 0.0f); glVertex2fv(v);
        PFSET_VEC2(v, (fcos(tempdir)*compsize),(fsin(tempdir)*compsize));
        glVertex2fv(v);
     glEnd();

     glLineWidth(1);
     pfPopMatrix();
#endif

  base = (0.1953125 * height);
  top = (0.78125 * height);

#define STOPPED 0.0
#define SLOW 150.0
#define FAST 1000.0
#define VERYFAST 2000.0
#define TOOFAST 10000.0

#ifdef IRISGL
  RGBcolor(255,255,0);
#else //OPENGL
  glColor4f(1.0f, 1.0f, 0.0f, 0.0f);
#endif

  step = (top - base) / 10.0;
  for(ypos = base;ypos<= top; ypos += step){
#ifdef IRISGL
    move2((0.058594*width),ypos);
    draw2((0.078125*width),ypos);
#else //OPENGL
    glBegin(GL_LINES);
       PFSET_VEC2(v, (0.058594*width),ypos); glVertex2fv(v);
       PFSET_VEC2(v, (0.078125*width),ypos); glVertex2fv(v);
    glEnd();
#endif
  }
  ypos += step/2.0f;
#ifdef IRISGL
  cmov2 ( (0.058594*width),ypos);
  charstr("Speed");
#else //OPENGL
  npsDrawStringPos("Speed",(0.058594*width),ypos,0.0f);
#endif

  /* Your speed gauge*/
   speed = pdata->speed * MILE_TO_KILOMETER;
   if(speed < SLOW){
     mytop = SLOW;
   }
   else{ if(speed <FAST){
     mytop = FAST;
   }
   else{ if(speed <VERYFAST){
     mytop = VERYFAST;
   }
   else{
     mytop = TOOFAST;
   }}}

#ifdef IRISGL
  cmov2((0.015625*width),base);
  sprintf(valstr,"%6.0f",STOPPED);
  charstr(valstr);
  cmov2((0.015625*width),top);
  sprintf(valstr,"%6.0f",mytop);
  charstr(valstr);
#else //OPENGL
  sprintf(valstr,"%6.0f",STOPPED);
  npsDrawStringPos(valstr,(0.015625*width),base,0.0f);
  sprintf(valstr,"%6.0f",mytop);
  npsDrawStringPos(valstr,(0.015625*width),top,0.0f);
#endif

  /*figure out where out where to put the writing*/
#ifdef IRISGL
  RGBcolor(255,0,0);
#else //OPENGL
  glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
#endif
  if(speed < 0.0)
    ypos = base;
  else
    ypos = base + (speed/mytop)*(top-base);
  if(ypos > top) ypos = top;
  sprintf(valstr,"%5.0fkph",speed);
#ifdef IRISGL
  move2((0.058594*width),ypos);
  draw2((0.11382125*width),ypos);
  cmov2((0.078125*width),ypos+3.0);
  charstr(valstr);
#else //OPENGL
  glBegin(GL_LINES);
     PFSET_VEC2(v, (0.058594*width),ypos); glVertex2fv(v);
     PFSET_VEC2(v, (0.11382125*width),ypos); glVertex2fv(v);
  glEnd();
  npsDrawStringPos(valstr,(0.078125*width),ypos+3.0,0.0f);
#endif

  /*now let's do the altitute*/

#define MSL 0.0
#define VERYLOW 1000.0
#define ALTLOW 5000.0
#define ALTMEDIUM 10000.0
#define ALTHIGH  50000.0

#ifdef IRISGL
  RGBcolor(255,255,0);
#else //OPENGL
  glColor4f(1.0f, 1.0f, 0.0f, 0.0f);
#endif

  step = (top - base) / 10.0;
  for(ypos = base;ypos<= top; ypos += step){
#ifdef IRISGL
    move2((0.91796875*width),ypos);
    draw2((0.9375*width),ypos);
#else //OPENGL
    glBegin(GL_LINES);
       PFSET_VEC2(v, (0.91796875*width),ypos); glVertex2fv(v);
       PFSET_VEC2(v, (0.9375*width),ypos); glVertex2fv(v);
    glEnd();
#endif
  }
  ypos += step/2.0f;
#ifdef IRISGL
  cmov2((0.91796875*width),ypos);
  charstr("Altitude");
#else //OPENGL
  npsDrawStringPos("Altitude",(0.91796875*width),ypos,0.0f);
#endif

   alt = pdata->posture.xyz[Z];
   if(alt < VERYLOW){
     mytop = VERYLOW;
   }
   else{ if(alt < ALTLOW){
     mytop = ALTLOW;
   }
   else{ if(alt < ALTMEDIUM){
     mytop = ALTMEDIUM;
   }
   else{
     mytop = ALTHIGH;
   }}}

#ifdef IRISGL
  cmov2((0.9375*width),base);
  sprintf(valstr,"%6.0f",STOPPED);
  charstr(valstr);
  cmov2((0.9375*width),top);
  sprintf(valstr,"%6.0f",mytop);
  charstr(valstr);
  RGBcolor(255,0,0);
#else //OPENGL
  sprintf(valstr,"%.0f",STOPPED);
  npsDrawStringPos(valstr,(0.9375*width),base,0.0f);
  sprintf(valstr,"%.0f",mytop);
  npsDrawStringPos(valstr,(0.9375*width),top,0.0f);
  glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
#endif

  /*figure out where out where to put the writing*/
  if(alt < 0.0)
    ypos = base;
  else
    ypos = base + (alt/mytop)*(top-base);
  if(ypos > top) ypos = top;
#ifdef IRISGL
  move2((0.859375*width),ypos);
  draw2((0.9375*width),ypos);
  cmov2((0.859375*width),ypos+(0.00293*height));
  sprintf(valstr,"%5.0fm",alt);
  charstr(valstr);
#else //OPENGL
  glBegin(GL_LINES);
     PFSET_VEC2(v, (0.859375*width),ypos); glVertex2fv(v);
     PFSET_VEC2(v, (0.9375*width),ypos); glVertex2fv(v);
  glEnd();
  sprintf(valstr,"%5.0fm",alt);
  npsDrawStringPos(valstr,(0.859375*width),ypos+(0.00293*height),0.0f);
#endif
#ifdef OPENGL
  // This is here because OpenGL handles the MODELVIEW & PROJECTION
  // matrixes differently then does IRISGL
  glMatrixMode(gl_matrix_mode);
#endif
}
