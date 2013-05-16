// File: <combo.cc>
// Creators: Joel Brand and Andrzej Kapolka
// June 1996
// Description: This program will generate detonations around an entity
// running in NPSNET. The detonations occur at equadistant points around
// a circle of specified radius. The program can be set to detonate a
// certain amount of points, a delay between detonations, and a certain
// munition type. The master computer can be changed on the fly. The 
// site and host variables should alter automatically. When the program
// is running, variable values are changes using the numeric keypad
// to highlight a field. Press enter once, enter a new value, and then
// press enter again to salidify the value. The program will instantly
// accomadate the new value.

/*
 * Copyright (c) 1995,
 *    Naval Postgraduate School
 *    Computer Graphics and Video Laboratory
 *    NPSNET Research Group
 *    npsnet@cs.nps.navy.mil
 *
 *
 *    Permission to use, copy, and modify this software and its
 * documentation for any non-commercial purpose is hereby granted
 * without fee, provided that (i) the above copyright notices and the
 * following permission notices appear in ALL copies of the software
 * and related documentation, and (ii) The Naval Postgraduate School
 * Computer Graphics and Video Laboratory and the NPSNET Research Group
 * be given written credit in your software's written documentation and
 * be given graphical credit on any start-up/credit screen your 
 * software generates.
 *    This restriction helps justify our research efforts to the
 * sponsors who fund our research.
 *
 *    Do not redistribute this code without the express written
 * consent of the NPSNET Research Group. (E-mail communication and our
 * confirmation qualifies as written permission.) As stated above, this 
 * restriction helps justify our research efforts to the sponsors who
 * fund our research.
 *
 *    This software was designed and implemented at U.S. Government
 * expense and by employees of the U.S. Government.  It is illegal
 * to charge any U.S. Government agency for its partial or full use.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * E-Mail addresses:
 *    npsnet@cs.nps.navy.mil
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *    npsnet-info@cs.nps.navy.mil
 *       Contact principle investigators.
 *       Overall research project information and funding.
 *       Requests for demonstations.
 *
 * Thank you to our sponsors:  ARL, STRICOM, TRAC, ARPA and DMSO.
 */

#define VERSION "Combo v1.01 Alpha by Joel Brand and Andrzej Kapolka"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream.h>
#include <fstream.h>
#include <strings.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>
#include "disnetlib.h"
#include "im.tab.h"
#include "imstructs.h"
#include "imdefaults.h"
#include "imclass.h"

#include <stream.h>
#include <strstream.h>
#include <iomanip.h>
#include <getopt.h>
#include <string.h>
#include <bstring.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <curses.h>
#include <ctype.h>
#ifdef __cplusplus
}
#endif

#define DEG_TO_RAD 0.0174532925
#define RAD_TO_DEG 57.2957795786

// Prototypes //
void senddetonation(double, double, double, DIS_net_manager *);

// Declare and initialize Curses variables //
static WINDOW *main_win=NULL, *scroll_win=NULL;
int menu_x=0, menu_y=0, do_fire=FALSE, pre_char=ERR;
long countdown=0;

// Global Variables //
EntityID address;
InitManager *initman;
DIS_net_manager *net;
int G_exit_flag = FALSE;
int dis_exercise, entityGlob, hostGlob, siteGlob, cur_point=0,
    entity_kindGlob, domainGlob, countryGlob, categoryGlob, subcategoryGlob,
    specificGlob, amount_of_points, site, host, entity, is_there=FALSE;         
double entityx, entityy, entityz, munition_id=0, event_id=0, r,
       xcenter, ycenter, delay_seed; 

double pitch, roll, rotation;


char master[256];

FILE *err_file;

struct {
     double x;
     double y;
     double z;
} circle_loc[500];

// Function: init_circle_engine
// Description: This function initializes the coordinate arrays
// with values that depend on the amount of points specified and
// the radius of the circle. 
void init_circle_engine(void) {
     int i;
     double increment;
     double oldx, oldy, oldz;

     increment=6.2831853/amount_of_points;
     for(i=0;i<amount_of_points;i++) {
       
        circle_loc[i].x=(cos(increment*i)*r);
	circle_loc[i].y=(sin(increment*i)*r);
	//Accomidate pitch
	circle_loc[i].y=circle_loc[i].y * cos(pitch);
	circle_loc[i].z=circle_loc[i].y * tan(pitch);       
	//Accomidate roll
	oldx = circle_loc[i].x;
	oldz = circle_loc[i].z;	
	circle_loc[i].z=oldz*cos(roll)-oldx*sin(roll);
	circle_loc[i].x=oldz*sin(roll)+oldx*cos(roll);
	//Accomidate rotation
	oldx = circle_loc[i].x;
	oldy = circle_loc[i].y;
	circle_loc[i].x=oldx*cos(rotation)-oldy*sin(rotation);
	circle_loc[i].y=oldx*sin(rotation)+oldy*cos(rotation);
     }
     cur_point=0;
}

// Function: circle_engine
// Description: This function calculates the next point at which
// a detonation should occur. It then calls the senddetonation
// function and a confirmation message to the screen that includes 
// the coordinates.
void circle_engine(void)
{
     char buf[80];
    
     if (cur_point==amount_of_points) cur_point=0;

     senddetonation(circle_loc[cur_point].x+entityx, 
                    circle_loc[cur_point].y+entityy,
		    circle_loc[cur_point].z+entityz,
		    net);
     sprintf(buf,"(%f,%f,%f) is where a Detonation PDU just went out.\n",
                  circle_loc[cur_point].x+entityx, 
                  circle_loc[cur_point].y+entityy,
	          circle_loc[cur_point].z+entityz);
     cerr << buf;
     cur_point++;
}

// Function: handler
// Description: This function is used during the initialization,
// specifically during the SIGNAL function calls.
void handler(int sig)
   {
   if ( !G_exit_flag )
      {
      signal ( sig, SIG_IGN );
      switch ( sig )
         {
         case SIGTERM:
         case SIGHUP:
         case SIGINT:
         case SIGQUIT:
            break;
         default:
            cerr << "DEATH NOTICE:\tpdudisplay exitting due to signal:  "
                 << sig << endl;
            break;
         }
      G_exit_flag = TRUE;
      signal ( sig, (void (*)(int))handler );
      }
   }

// Function: init_window
// Description: This function initializes a window to be used for
// display and variable manipulation. The window is set with 
// the following fields: Points, Delay, Site, Host, Entity,
// EntityKind, Domain, Country, Category, Subcategory, Specific,
// and Master. Also initialized in this function is an on/off
// switch for detonations. This is the <Commence Firing> and
// <Cease Firing> toggle. It is activated/deactivated using the
// space bar while the program is running.
void init_window(void) {
   // Initializes main window/menu
   int temp_x, temp_y, temp_pos, temp_fd[2];
   char buf[256];
   
   main_win=initscr();
   cbreak();
   noecho();
   intrflush(main_win,FALSE);
   keypad(main_win,TRUE);
   nodelay(main_win,TRUE);
   curs_set(0);
   clear();
   scroll_win=subwin(main_win,0,0,15,0);
   idlok(scroll_win,TRUE); scrollok(scroll_win,TRUE);
   getmaxyx(main_win,temp_y,temp_x);
   attron(A_UNDERLINE); attron(A_BOLD);
   mvaddstr(0,((temp_x+1)-strlen(VERSION))/2,VERSION);
   attroff(A_UNDERLINE); attron(A_REVERSE);
   mvaddstr(3,15,"Radius:             ");
   sprintf(buf,"%g",r); mvaddstr(3,23,buf);
   attroff(A_REVERSE); attroff(A_BOLD);
   for(temp_pos=0;temp_pos<temp_x+1;temp_pos++) mvaddch(14,temp_pos,'_');
   temp_x-=35;
   sprintf(buf,"Points: %d",amount_of_points); mvaddstr(4,15,buf);
   sprintf(buf,"Delay: %g",delay_seed); mvaddstr(5,15,buf);
   sprintf(buf,"Pitch: %g",pitch*RAD_TO_DEG); mvaddstr(6,15,buf);
   sprintf(buf,"Roll: %g",roll*RAD_TO_DEG); mvaddstr(7,15,buf);
   sprintf(buf,"Rotation: %g",rotation*RAD_TO_DEG); mvaddstr(8,15,buf);
   sprintf(buf,"Site: %d",site); mvaddstr(9,15,buf); 
   sprintf(buf,"Host: %d",host); mvaddstr(10,15,buf);
   sprintf(buf,"Entity: %d",entity); mvaddstr(11,15,buf);
   sprintf(buf,"EntityKind: %d",entity_kindGlob); mvaddstr(3,temp_x,buf);
   sprintf(buf,"Domain: %d",domainGlob); mvaddstr(4,temp_x,buf);
   sprintf(buf,"Country: %d",countryGlob); mvaddstr(5,temp_x,buf);
   sprintf(buf,"Category: %d",categoryGlob); mvaddstr(6,temp_x,buf);
   sprintf(buf,"Subcategory: %d",subcategoryGlob); mvaddstr(7,temp_x,buf);
   sprintf(buf,"Specific: %d",specificGlob); mvaddstr(8,temp_x,buf);
   sprintf(buf,"Master: %s",master); mvaddstr(13,15,buf);
   mvaddstr(13,temp_x,"<Commence Firing>");
   refresh();
   pipe(temp_fd);
   fcntl(temp_fd[0],F_SETFL,fcntl(temp_fd[0],F_GETFL,0)|FNDELAY|FNONBLOCK);
   sprintf(buf,"/dev/fd/%d",temp_fd[1]);
   freopen(buf,"w",stderr);
   err_file=fdopen(temp_fd[0],"r");
}                

// Function: get_string
// Description: This function allows the user to edit a string at 
// x and y with a maximum length. The field is editted while the 
// program is running with the key sequence:  (enter)(new value)(enter)
void get_string(int y, int x, char *string, int length, int firstchar) {
   int str_pos=0, old_x, old_y, inp_char, temp, procfirst=FALSE;
   char buf[256];
   
   getyx(main_win,old_y,old_x);
   
   for(temp=0;temp<length;temp++) mvaddch(y,x+temp,' ');   
   move(y,x+str_pos);
   while(!G_exit_flag) {
       
      if((temp=getc(err_file))!=EOF) {
         ungetc(temp,err_file);
         attroff(A_REVERSE); attroff(A_BOLD);
         while(fgets(buf,255,err_file)!=NULL) {
            waddstr(scroll_win,buf);
            touchwin(main_win); wrefresh(scroll_win);
         }
         fflush(err_file);
	 attron(A_REVERSE); attron(A_BOLD);
      }  
   
      if(countdown-clock()<=0) {
	 if(is_there&&do_fire) circle_engine();
	 countdown=clock()+(delay_seed*CLOCKS_PER_SEC);
      }
   
      if(procfirst==FALSE) { inp_char=firstchar; procfirst=TRUE; }
      else inp_char=getch();
      
      switch(inp_char) {
         case KEY_ENTER:
            string[str_pos]=0;
            move(old_y,old_x);
            return;
         case KEY_DL:
         case KEY_BACKSPACE:
         case KEY_DC:
         case '\b':
            if(str_pos>0) {
               str_pos--; 
               mvaddch(y,x+str_pos,' ');
               move(y,x+str_pos);
            }
            break;
	 case ERR: break;
         case KEY_UP: case KEY_DOWN: case KEY_RIGHT: case KEY_LEFT:
	    string[str_pos]=0;
	    move(old_y,old_x);
	    pre_char=inp_char;
	    return;
         default:
	    if(isprint(inp_char)) {
               if(str_pos<length) {
                  string[str_pos]=inp_char;
                  mvaddch(y,x+str_pos,inp_char);
                  str_pos++;
               }
	    }
            break;
      }
   }
   string[str_pos]=0;
   move(old_y,old_x);
}

// Function: reverse_loc
// Description: This function reverses a menu location on the screen.
void reverse_loc(int x_pos, int y_pos) {
   // Reverses a menu location on the screen
   int xloc, yloc, len;
   char buf[40]; 
   
   if(x_pos==0) { 
      xloc=15; len=20; 
      if(y_pos==9) yloc=13;
      else yloc=y_pos+3; 
   }
   else {
      getmaxyx(main_win,yloc,xloc);
      xloc-=35; len=20;
      if(y_pos==6) yloc=13;
      else yloc=y_pos+3;
   }
   
   mvinnstr(yloc,xloc,buf,len); buf[20]=0;
   if(mvinch(yloc,xloc)&A_REVERSE) { attroff(A_REVERSE); attroff(A_BOLD); }
   else { attron(A_REVERSE); attron(A_BOLD); }
   mvaddstr(yloc,xloc,buf);
}

// Function: adj_len
// Description: This function adjusts the length of a stored string
// variable.
void adj_len(char *str, int len) {
   if(strlen(str)<len) {
      while(strlen(str)<len) strcat(str," ");
   }
   else if(strlen(str)>len) str[len]=0;
}

// Function: findmaster
// Description: This function reads the host.dat file and 
// searches for a values that correlate to the specified
// master. The correct values are then stored in the 
// site/host variables.
int findmaster(char *mname) {
  char temp[80];
  int dummy;
  int success=FALSE;

  ifstream hostfile ("./datafiles/hosts.dat");
  if (!hostfile) {
    cerr << "Unable to open ./datafiles/hosts.dat.\n";
    return FALSE;
  }
  else {
    while(!success&&(hostfile >> "HOST" >> temp)) {
      if(!strcmp(temp,mname)) {
        hostfile >> site >> host; 
        success = TRUE;
      }
      if (!success)
         hostfile >> dummy >> dummy >> temp;
    }
    if (!success) 
       cerr << mname << 
       " was not found in the host table, ./datafiles/hosts.dat.\n";
  }
  return success;
}

// Function: proc_option
// Description: This function processes the menu option that the menu
// cursor has currently highlighted.
void proc_option(int firstchar) {
   int temp_x, temp_y, temp_i;
   char buf[256], *temp_ptr;
   double temp_d;
   
   getmaxyx(main_win,temp_y,temp_x);
   temp_x-=35;
   
   attron(A_REVERSE); attron(A_BOLD);
   if(menu_x==0) {
      switch(menu_y) {
         case 0:
	    sprintf(buf,"%g",r);
	    get_string(3,23,buf,12,firstchar);
	    temp_d=strtod(buf,&temp_ptr);
	    if(temp_ptr!=buf && temp_d > 0) r=temp_d;
	    else cerr << "Invalid radius value.\n";
	    sprintf(buf,"%g",r); adj_len(buf,12);
	    mvaddstr(3,23,buf);
	    init_circle_engine();
	    break;
	 case 1:
	    sprintf(buf,"%d",amount_of_points);
	    get_string(4,23,buf,12,firstchar);
	    temp_i=strtol(buf,&temp_ptr,10);
	    if(temp_ptr!=buf && temp_i > 0) amount_of_points=temp_i;
	    else cerr << "Invalid number of points.\n";
	    sprintf(buf,"%d",amount_of_points); adj_len(buf,12);
	    mvaddstr(4,23,buf);
	    init_circle_engine();
	    break;
	 case 2:
	    sprintf(buf,"%g",delay_seed);
	    get_string(5,22,buf,13,firstchar);
	    temp_d=strtod(buf,&temp_ptr);
	    if(temp_ptr!=buf && temp_d > 0) delay_seed=temp_d;
	    else cerr << "Invalid delay seed value.\n";
	    sprintf(buf,"%g",delay_seed); adj_len(buf,13);
	    mvaddstr(5,22,buf);
	    break;
	 case 3:
	    sprintf(buf,"%g",pitch*RAD_TO_DEG);
	    get_string(6,22,buf,13,firstchar);
	    temp_d=strtod(buf,&temp_ptr);
	    if(temp_ptr!=buf && (temp_d>=-360) && (temp_d<=360)) 
	       pitch=temp_d*DEG_TO_RAD;
	    else cerr << "Invalid pitch value.\n";
	    sprintf(buf,"%g",pitch*RAD_TO_DEG); adj_len(buf,13);
	    mvaddstr(6,22,buf);
	    init_circle_engine();
	    break;
	 case 4:
	    sprintf(buf,"%g",roll*RAD_TO_DEG);
	    get_string(7,21,buf,14,firstchar);
	    temp_d=strtod(buf,&temp_ptr);
	    if(temp_ptr!=buf && (temp_d>=-360) && (temp_d<=360)) 
	       roll=temp_d*DEG_TO_RAD;
	    else cerr << "Invalid roll value.\n";
	    sprintf(buf,"%g",roll*RAD_TO_DEG); adj_len(buf,14);
	    mvaddstr(7,21,buf);
	    init_circle_engine();
	    break;
	 case 5:
	    sprintf(buf,"%g",rotation*RAD_TO_DEG);
	    get_string(8,25,buf,10,firstchar);
	    temp_d=strtod(buf,&temp_ptr);
	    if(temp_ptr!=buf && (temp_d>=-360) && (temp_d<=360)) 
	       rotation=temp_d*DEG_TO_RAD;
	    else cerr << "Invalid rotation value.\n";
	    sprintf(buf,"%g",rotation*RAD_TO_DEG); adj_len(buf,10);
	    mvaddstr(8,25,buf);
	    init_circle_engine();
	    break;
	 case 6:
	    sprintf(buf,"%d",site);
	    get_string(9,21,buf,14,firstchar);
	    temp_i=strtol(buf,&temp_ptr,10);
	    if(temp_ptr!=buf && temp_i > 0) { site=temp_i; is_there=FALSE; }
	    else cerr << "Invalid site number.\n";
	    sprintf(buf,"%d",site); adj_len(buf,14);
	    mvaddstr(9,21,buf);
	    break;
	 case 7:
	    sprintf(buf,"%d",host);
	    get_string(10,21,buf,14,firstchar);
	    temp_i=strtol(buf,&temp_ptr,10);
	    if(temp_ptr!=buf && temp_i>0) { host=temp_i; is_there=FALSE; }
	    else cerr << "Invalid host number.\n";
	    sprintf(buf,"%d",host); adj_len(buf,14);
	    mvaddstr(10,21,buf);
	    break;
	 case 8:
	    sprintf(buf,"%d",entity);
	    get_string(11,23,buf,12,firstchar);
	    temp_i=strtol(buf,&temp_ptr,10);
	    if(temp_ptr!=buf && temp_i>0) { entity=temp_i; is_there=FALSE; }
	    else cerr << "Invalid entity number.\n";
	    sprintf(buf,"%d",entity); adj_len(buf,12);
	    mvaddstr(11,23,buf);
	    break;
	 case 9:
	    strcpy(buf,master);
	    get_string(13,23,buf,12,firstchar);
	    if(findmaster(buf)) {
	         attroff(A_REVERSE); attroff(A_BOLD);
	         strcpy(master,buf);
	         sprintf(buf,"%d",site); adj_len(buf,14);
		 mvaddstr(9,21,buf);
		 sprintf(buf,"%d",host); adj_len(buf,14);
		 mvaddstr(10,21,buf);
		 attron(A_REVERSE); attron(A_BOLD);
		 is_there=FALSE;
	    }
	    else {
	       strcpy(buf,master);
	       adj_len(buf,12);
	       mvaddstr(13,23,buf);
	    }
	    break;
      }
   }
   else {
      switch(menu_y) {
         case 0:
	    sprintf(buf,"%d",entity_kindGlob);
	    get_string(3,temp_x+12,buf,8,firstchar);
	    temp_i=strtol(buf,&temp_ptr,10);
	    if(temp_ptr!=buf && (7>=temp_i) && (temp_i>=0)) 
	       entity_kindGlob=temp_i;
	    else cerr << "Invalid entity kind code.\n";
	    sprintf(buf,"%d",entity_kindGlob); adj_len(buf,8);
	    mvaddstr(3,temp_x+12,buf);
	    break;
	 case 1:
	    sprintf(buf,"%d",domainGlob);
	    get_string(4,temp_x+8,buf,12,firstchar);
	    temp_i=strtol(buf,&temp_ptr,10);
	    if(temp_ptr!=buf&&(5>=temp_i) && (temp_i>=0)) domainGlob=temp_i;
	    else cerr << "Invalid domain code.\n";
	    sprintf(buf,"%d",domainGlob); adj_len(buf,12);
	    mvaddstr(4,temp_x+8,buf);
	    break;
	 case 2:
	    sprintf(buf,"%d",countryGlob);
	    get_string(5,temp_x+9,buf,11,firstchar);
	    temp_i=strtol(buf,&temp_ptr,10);
	    if(temp_ptr!=buf&&(266>=temp_i) && (temp_i>=0)) countryGlob=temp_i;
	    else cerr << "Invalid country code.\n";
	    sprintf(buf,"%d",countryGlob); adj_len(buf,11);
	    mvaddstr(5,temp_x+9,buf);
	    break;
	 case 3:
	    sprintf(buf,"%d",categoryGlob);
	    get_string(6,temp_x+10,buf,10,firstchar);
	    temp_i=strtol(buf,&temp_ptr,10);
	    if(temp_ptr!=buf&&(60>=temp_i) && (temp_i>=0)) categoryGlob=temp_i;
	    else cerr << "Invalid category code.\n";
	    sprintf(buf,"%d",categoryGlob); adj_len(buf,10);
	    mvaddstr(6,temp_x+10,buf);
	    break;
	 case 4:
	    sprintf(buf,"%d",subcategoryGlob);
	    get_string(7,temp_x+13,buf,7,firstchar);
	    temp_i=strtol(buf,&temp_ptr,10);
	    if(temp_ptr!=buf&&(99>=temp_i) && (temp_i>=0)) subcategoryGlob=temp_i;
	    else cerr << "Invalid subcategory code.\n";
	    sprintf(buf,"%d",subcategoryGlob); adj_len(buf,7);
	    mvaddstr(7,temp_x+13,buf);
	    break;
	 case 5:
	    sprintf(buf,"%d",specificGlob);
	    get_string(8,temp_x+10,buf,10,firstchar);
	    temp_i=strtol(buf,&temp_ptr,10);
	    if(temp_ptr!=buf&&(99>=temp_i) && (temp_i>=0)) specificGlob=temp_i;
	    else cerr << "Invalid specific code.\n";
	    sprintf(buf,"%d",specificGlob); adj_len(buf,10);
	    mvaddstr(8,temp_x+10,buf);
	    break;
	 case 6:
	    if(do_fire==FALSE) {
	       do_fire=TRUE;
	       mvaddstr(13,temp_x,"<Cease Firing>   ");
	    }
	    else {
	       do_fire=FALSE;
	       mvaddstr(13,temp_x,"<Commence Firing>");
	    }
	    break;
      }
   }
   refresh();
}

// Function: update_window
// Description: This function checks for a new user input
// such as an arrow key, enter key, or 'q'. Depending on
// the input the function responds accordingly.
void update_window(void) {
   int temp;
   int temp_x=93, temp_y=93;
   char buf[256];
   
   if((temp=getc(err_file))!=EOF) {
      ungetc(temp,err_file);
      attroff(A_REVERSE); attroff(A_BOLD);
      while(fgets(buf,255,err_file)!=NULL) {
         waddstr(scroll_win,buf);
         touchwin(main_win); wrefresh(scroll_win);
      }
      fflush(err_file);
   }
   
   if(pre_char!=ERR) { temp=pre_char; pre_char=ERR; }
   else temp=getch();
   
   if(temp!=ERR) {
      switch(temp) {
	 case KEY_UP:
	    if(menu_y>0) { temp_y=menu_y-1; temp_x=menu_x; }
	    break;
	    
	 case KEY_DOWN:
	    if((menu_x==0&&menu_y<9)||(menu_x==1&&menu_y<6)) 
	       { temp_y=menu_y+1; temp_x=menu_x; }
	    break;
	    
	 case KEY_LEFT:
	    if(menu_x==1&&menu_y<6) { temp_x=0; temp_y=menu_y; }
	    else if(menu_x==1) { temp_x=0; temp_y=9; }
	    break;
	    
	 case KEY_RIGHT:
	    if(menu_x==0&&menu_y<6) { temp_x=1; temp_y=menu_y; }
	    else if(menu_x==0&&menu_y==9) { temp_x=1; temp_y=6; }
	    else if(menu_x==0) { temp_x=1; temp_y=5; }
	    break;
	    
	 case ' ':
	 case KEY_ENTER:
	    proc_option(ERR);
	    break;
	    
	 default:
	    proc_option(temp);
	    break;
      }
      if(temp_x!=93 || temp_y!=93) {
         reverse_loc(menu_x,menu_y);
	 menu_x=temp_x; menu_y=temp_y;
	 reverse_loc(menu_x,menu_y);
	 refresh();
      }
   }
}

// Main //
void main ( int argc, char **argv )
   {
 
   // Declare and Set Multicast Defaults
   int multicast = 0;
   u_short port;
   char group[33];
   u_char ttl = DEFAULT_MC_TTL;
 
   char net_interface[20];
   int exercise = 0;
   int round = 0;
   char rfile[512];

   extern char *optarg;
   extern int optind, operr;
   EntityID filterID;

   PDUType type;
   char *pdu;   
   EntityStatePDU *entitystatepdu;
   sender_info pdu_sender_info;
   
   int swap_bufs;
   struct FIL im_filters[MAX_PDU_FILTERS];
   int num_filters;
   
   signal ( SIGTERM, (void (*)(int))handler );
   signal ( SIGHUP, (void (*)(int))handler );
   signal ( SIGINT, (void (*)(int))handler );
   signal ( SIGQUIT, (void (*)(int))handler );
   
   port = 0;

   // START Initialization Manager routines
   union query_data local;
   initman = new InitManager();         // defaults assigned
   initman->process_Master_Configs(argc, argv);
   initman->process_environment_vars();
   initman->process_cl_args(argc, argv);
   initman->parse_input_files();
   initman->validate_data();
   initman->dump_inits();       // let's keep a record of what we've got
   cout.flush();
   // END Initialization Manager routines

   initman->query(IM_PORT, &local, GET);
   port = (u_short)local.ints[0];

   initman->query(MULTICAST, &local, GET);
   multicast = local.ints[0];
   initman->query(MULTICAST_TTL, &local, GET);
   ttl = (u_char) local.ints[0];
   initman->query(MULTICAST_GROUP, &local, GET);
   strncpy ( group, local.string, STRING32 );

   initman->query(DIS_EXERCISE, &local, GET);
   exercise = (u_char) local.ints[0];
   dis_exercise = (u_char) local.ints[0];

   initman->query(ROUND_WORLD_FILE, &local, GET);
   strcpy ( rfile, local.string );
   round = (strcmp(rfile,"NONE") != 0);

   initman->query(ETHER_INTERF, &local, GET);
   strncpy ( net_interface, local.string, 10 );

   roundWorldStruct *rw = NULL;

   if (round) {
      rw = new roundWorldStruct;
      struct RW initManRW;
      initman->get_round_world_data(&initManRW);
      rw->northing = initManRW.northing;
      rw->easting = initManRW.easting;
      rw->o_zone_num = initManRW.zone_num;
      rw->o_zone_letter = initManRW.zone_letter;
      rw->map_datum = initManRW.map_datum;
      rw->width = initManRW.width;
      rw->height = initManRW.height;
   }

   initman->query(IM_PORT, &local, GET);
   port = (u_short)local.ints[0];

   initman->query(MULTICAST, &local, GET);
   multicast = local.ints[0];
   initman->query(MULTICAST_TTL, &local, GET);
   ttl = (u_char) local.ints[0];
   initman->query(MULTICAST_GROUP, &local, GET);
   strncpy ( group, local.string, STRING32 );

   initman->query(DIS_EXERCISE, &local, GET);
   exercise = (u_char) local.ints[0];

   initman->query(ROUND_WORLD_FILE, &local, GET);
   strcpy ( rfile, local.string );
   round = (strcmp(rfile,"NONE") != 0);

   initman->query(ETHER_INTERF, &local, GET);
   strncpy ( net_interface, local.string, 10 );

   if (round) {
      rw = new roundWorldStruct;
      struct RW initManRW;
      initman->get_round_world_data(&initManRW);
      rw->northing = initManRW.northing;
      rw->easting = initManRW.easting;
      rw->o_zone_num = initManRW.zone_num;
      rw->o_zone_letter = initManRW.zone_letter;
      rw->map_datum = initManRW.map_datum;
      rw->width = initManRW.width;
      rw->height = initManRW.height;
   }

   if ( multicast )
      {
      if ( port == (u_short)0 )
         port = DEFAULT_MC_PORT;
      net = new DIS_net_manager ( group, ttl, port, exercise, round, rw,
                                  DEFAULT_BUF_SIZE, net_interface );
      }
   else
      {
      if ( port == (u_short)0 )
         port = DEFAULT_BC_PORT;
      net = new DIS_net_manager ( exercise, round, rw, DEFAULT_BUF_SIZE,
                                  net_interface, port );
      }

   if ( !net->net_open() )
      {
      cerr << "Could not open network." << endl;
      exit(0);
      }
   else {
      net->add_to_receive_list(EntityStatePDU_Type);
      initman->get_filters((struct FIL**) im_filters, &num_filters);
      for (int i = 0; i < num_filters; i++) {
         if (im_filters[i].filter_type == TYPE_BLOCK) {
            filterID.address.site = (u_short)im_filters[i].Type.Block.site;
            filterID.address.host = (u_short)im_filters[i].Type.Block.host;
            filterID.entity = (u_short)im_filters[i].Type.Block.entity;
            net->add_to_filter_list(filterID);
         }
      }
   }

   r=20.0; amount_of_points=22; delay_seed=0.01; site=0; host=0; entity=1001;
   entity_kindGlob=2; domainGlob=1; countryGlob=225; categoryGlob=1;
   subcategoryGlob=1; specificGlob=0; pitch=0; roll=0; rotation=0;

   // Query the initialization manager for the computer running
   // NPSNET (the MASTER)
   initman->query(MASTER, &local, GET);
   strcpy(master,local.string);
   if(!findmaster(master)) strcpy(master,"");

   // Initialize the coordinate arrays
   init_circle_engine();

   if(!G_exit_flag) init_window();

   while(!G_exit_flag) {

      update_window();
      // Read a pdu
      net->read_pdu(&pdu, &type, pdu_sender_info, swap_bufs);
      // Make sure it is a PDU and is an entity state PDU
      if (pdu!=NULL && type==EntityStatePDU_Type) {
         entitystatepdu = (EntityStatePDU *)pdu;
	 if(entitystatepdu->entity_id.entity==entity &&
	    entitystatepdu->entity_id.address.host==host &&
	    entitystatepdu->entity_id.address.site==site) {
	    // The above if statement checks for three things.
	    // 1) the pdu that has been read is an entity
	    //	state pdu.
	    // 2) the host controling the pdu correlates to
            //  the one specified by the user.
	    // 3) the site of which the pdu came is the
	    //  same as the one specified by the user.
	    is_there=TRUE;
	    entityx=entitystatepdu->entity_location.x;
	    entityy=entitystatepdu->entity_location.y;
	    entityz=entitystatepdu->entity_location.z;
	    entityGlob = entitystatepdu->entity_id.entity;
            hostGlob = entitystatepdu->entity_id.address.host;
            siteGlob = entitystatepdu->entity_id.address.site;
	 }  
      }
      // If the time has gone past the amount of delay specified
      // by the user, print out the last retrieved coordinates.
      if(countdown-clock()<=0) {
	 if(is_there&&do_fire) circle_engine();
	 countdown=clock()+(delay_seed*CLOCKS_PER_SEC);
      }  
   }
   
   net->net_close();
   clear();
   refresh();
   endwin();  
}

// Function: senddetonation
// Description: This function sends out a detonation PDU that
// contains coordinates generated with circle_engine. The type
// of detonation is sent out with the entity_kind, domain, country,
// category, subcategory, and specific fields. This information
// can be changed to make different types of explosions.
void senddetonation(double x, double y, double z, DIS_net_manager *net )
//send a detonation PDU
{
  static DetonationPDU dpdu;

  dpdu.target_entity_id.entity = 0;
  dpdu.target_entity_id.address.site = 0;
  dpdu.target_entity_id.address.host = 0;
  dpdu.velocity.x = 0;
  dpdu.velocity.y = 0;
  dpdu.velocity.z = 0;
  dpdu.location_in_world.x = x;
  dpdu.location_in_world.y = y;
  dpdu.location_in_world.z = z;
  dpdu.burst_descriptor.munition.entity_kind = entity_kindGlob;
  dpdu.burst_descriptor.munition.domain = domainGlob;
  dpdu.burst_descriptor.munition.country = countryGlob;
  dpdu.burst_descriptor.munition.category = categoryGlob;
  dpdu.burst_descriptor.munition.subcategory = subcategoryGlob;
  dpdu.burst_descriptor.munition.specific = specificGlob; 
  dpdu.detonation_result = DetResult_Detonation;
  dpdu.firing_entity_id.address.site = 1;
  dpdu.firing_entity_id.address.host = 1;
  dpdu.firing_entity_id.entity = 1;
  dpdu.munition_id.address.site = 99;
  dpdu.munition_id.address.host = 1;
  dpdu.munition_id.entity = 1;

  if ( net->write_pdu((char *)&dpdu,DetonationPDU_Type) == FALSE)
  	cerr << "net_write() failed" << endl;	
}


