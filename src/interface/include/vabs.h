
/**********************************************************************
 *                                                                    *
 *      File:         vabs.h                                          *
 *      Date:         5/28/94                                         *
 *	Written by:   Jose Vazquez                                    *
 *                    NCCOSC RDTE DIV                                 *
 *                    Code 842                                        *
 *                    San Diego, CA  92152-5000                       *
 *                                                                    *
 **********************************************************************/

#ifndef VABS_H
#define VABS_H

/*
 * Include files.
 */
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/MainW.h>
#include <Xm/Frame.h>
#include <Xm/ToggleB.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/FileSB.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#include <Xm/DialogS.h>
#include <Xm/SelectioB.h>
#include <Xm/Label.h>
#include <Xm/Separator.h>
#include <Xm/MwmUtil.h>
 
/*
 * Function declarations.
 */
void create_icon(Widget), create_system(), create_nps(), create_message_nps(), create_npsmenu();
int set_properties(char *, Dimension, Dimension), set_properties2();
void default_callback(Widget, int, XmAnyCallbackStruct *);
void create_fsb(), create_info(), create_text(), create_prev(), create_next();
void create_message_box(), create_resize_window(), create_npsmessage();
void kill_application(), npsfunction();
void load_file();
void pushb_callback(Widget, Widget, XmAnyCallbackStruct *);
void ok_handler(Widget, XtPointer, XEvent *);
void ok_handler2(Widget, XtPointer, XEvent *);
void file_cb(Widget, int);
void edit_cb(Widget, int);
void help_cb();
void file_focus(Widget, caddr_t, XmAnyCallbackStruct *);
void info_focus(Widget, caddr_t, XmAnyCallbackStruct *);
void text_focus(Widget, caddr_t, XmAnyCallbackStruct *);
void default_focus(Widget, caddr_t, XmAnyCallbackStruct *);
void number_focus(Widget, Widget, XmAnyCallbackStruct *);
void default_nps(Widget, int, XmAnyCallbackStruct *);
void default_labels(Widget, int, XmAnyCallbackStruct *);
void quit(CompositeWidget, XEvent *, String *, int *);
void do_up(CompositeWidget, XEvent *, String *, int *);
void do_right(CompositeWidget, XEvent *, String *, int *);
void do_middle(CompositeWidget, XEvent *, String *, int *);
void do_left(CompositeWidget, XEvent *, String *, int *);
void do_down(CompositeWidget, XEvent *, String *, int *);
void do_message(CompositeWidget, XEvent *, String *, int *);
void do_info(CompositeWidget, XEvent *, String *, int *);
void do_file(CompositeWidget, XEvent *, String *, int *);
void do_divide(CompositeWidget, XEvent *, String *, int *);
void do_prev(CompositeWidget, XEvent *, String *, int *);
void do_next(CompositeWidget, XEvent *, String *, int *);
void do_npshdds(CompositeWidget, XEvent *, String *, int *);
void do_default(CompositeWidget, XEvent *, String *, int *);
void do_number(CompositeWidget, XEvent *, String *, int *);
void add_pulldown(), resize_window();
void done_callback(Widget, Widget, XmAnyCallbackStruct *);
void rbutton_callback();
void pulldown_callback(Widget, int, XmPushButtonCallbackStruct *);

void init_vabs(int argc, char *argv[]);
Widget cascade_button();
Atom register_atom(Widget, char *);

/*
 * Global variables.
 */

/* 
 * Action record for the OSV window.
 */
static XtActionsRec actionsTable[] = {
        {"file", (XtActionProc) do_file},
        {"divide", (XtActionProc) do_divide},
        {"prev", (XtActionProc) do_prev},
        {"next", (XtActionProc) do_next},
        {"message",(XtActionProc)  do_message},
        {"up", (XtActionProc) do_up},
        {"info", (XtActionProc) do_info},
        {"right", (XtActionProc) do_right},
        {"middle", (XtActionProc) do_middle},
        {"left", (XtActionProc) do_left},
        {"down", (XtActionProc) do_down},
        {"bye", (XtActionProc) quit},
        {"default", (XtActionProc) do_default},
        {"npshdds", (XtActionProc) do_npshdds},
        {"number", (XtActionProc) do_number},
};
/*
 * Mapping of the OSV keys with their actions.
 */
static char defaultTranslations[] =
        "<Key>KP_Divide: divide() \n\
         Ctrl<Key>t: default(1) \n\
         <Key>KP_Multiply: prev() \n\
         <Key>KP_Subtract: next() \n\
         <Key>KP_Add: default(6) \n\
         <Key>KP_Enter: default(7) \n\
         <Key>KP_Decimal: default(4) \n\
         <Key>KP_9: message() \n\
         <Key>KP_8: up() \n\
         <Key>KP_7: info()\n\
         <Key>KP_6: right()\n\
         <Key>KP_5: middle() \n\
         <Key>KP_4: left() \n\
         <Key>KP_3: npshdds() \n\
         <Key>KP_2: down() \n\
         <Key>KP_1: bye()\n\
         <Key>KP_0: default(0)";
/*
 * Mapping of the OSV keys on numeric mode.
 */
static char numericTranslations[] =
        "<Key>F29: number(9)\n\
         <Key>Up: number(8)\n\
         <Key>F27: number(7)\n\
         <Key>Right: number(6)\n\
	 <Key>F31: number(5)\n\
	 <Key>Left: number(4)\n\
	 <Key>F35: number(3)\n\
	 <Key>Down: number(2)\n\
	 <Key>F33: number(1)\n\
	 <Key>Insert: number(0)\n\
     Ctrl<Key>t: number(20)";

/*
 * Shell and button widgets.
 */
#ifdef GLOBAL
Widget  	toplevel, form, formnps;
Widget  	widget[17], popup4;

XtTranslations  trans_table, /* Translation tables */
                trans_table3;
Arg     	wargs[10];                 /* Arguments          */
Display	       *display;                   /* Display            */
int		mode;			   /* 0 - Standard Mode  */
					   /* 1 - Numerical Mode */
Widget 		receive;		   /* Last numeric field */
					   /* that was accessed  */
#else

extern Widget          toplevel, form;
extern Widget          widget[], popup4;

extern XtTranslations  trans_table, /* Translation tables */
                trans_table3;
extern Arg             wargs[];                 /* Arguments          */
extern Display        *display;                   /* Display            */
extern int             mode;                      /* 0 - Standard Mode  */
                                           /* 1 - Numerical Mode */
extern Widget          receive;                   /* Last numeric field */
                                           /* that was accessed  */



#endif

#endif
