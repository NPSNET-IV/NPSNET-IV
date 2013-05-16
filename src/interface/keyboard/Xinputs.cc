#include <Performer/pf.h>
#include <X11/X.h>
#include <X11/keysym.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "common_defs.h"
#include "nps_device.h"
#include "keyboard.h"

extern volatile DYNAMIC_DATA *G_dynamic_data;

inline void keybump (int &);

inline void keybump (int &bumpNum)
{
   bumpNum = (bumpNum + 1) % ROLL_OVER_VALUE;
}

void keyboard::collect_X_input () 
{

// cerr << "     pid for X Input process is: " << getpid () << endl;

   uspsema (fork_sema);


   Display *theDisplay = pfGetCurWSConnection ();

//   Atom WMProtocols = XInternAtom(theDisplay, "WM_PROTOCOLS", 1);
//   Atom WMDeleteWindow = XInternAtom(theDisplay, "WM_DELETE_WINDOW", 1);

   Window theWindow;

   prctl (PR_TERMCHILD);
   sigset (SIGHUP, SIG_DFL);

   while (!G_dynamic_data->pw);
   
   while (!pfIsPWinOpen(G_dynamic_data->pw));

   while (!(theWindow = pfGetPWinWSWindow(G_dynamic_data->pw)));

//   XSetWMProtocols (theDisplay, theWindow, &WMDeleteWindow, 1);

   G_dynamic_data->XCursor = new XCursorUtil (char (NULL),
                                              theDisplay,
                                              theWindow);


   XSelectInput (theDisplay, theWindow, KeyPressMask| KeyReleaseMask|
                                        PointerMotionMask|
                                        ButtonPressMask|ButtonReleaseMask|
                                        FocusChangeMask| ExposureMask|
                                        VisibilityChangeMask);

   XMapWindow (theDisplay, theWindow);
   XFlush (theDisplay);

   int quitX = FALSE;

   char theBuffer[100];
   int isChar;
   KeySym keyType;
 
   while (!quitX) {

      XEvent theEvent;

      XPeekEvent (theDisplay, &theEvent);

      while (XEventsQueued(theDisplay, QueuedAfterReading)) {

         XNextEvent (theDisplay, &theEvent);

         switch (theEvent.type) {

            case ClientMessage:
               cerr << "ClientMessage: ";
//               if ((theEvent.xclient.message_type == WMProtocols) &&
//                   (theEvent.xclient.data.l[0] == WMDeleteWindow)) {
//                   cerr << "Quiting." << endl;
//                   keyPressCounter[int (NPS_ESCKEY)] 
//                               = (keyPressCounter[int (NPS_ESCKEY)] + 1)
//                                  % ROLL_OVER_VALUE;
//               }
               break;
            case MotionNotify:
               theMouse[0] = theEvent.xmotion.x;
               theMouse[1] = theEvent.xmotion.y;
//               cerr << "The X: " << theEvent.xmotion.x << endl
//                    << "The Y: " << theEvent.xmotion.y << endl
//                    << endl;
               break;
            case ButtonPress:
               switch (theEvent.xbutton.button) {
                  case 1:
                     mouseButtonState |= LEFT_MOUSE_BUTTON_MASK;
                     break;
                  case 2:
                     mouseButtonState |= MIDDLE_MOUSE_BUTTON_MASK;
                     break;
                  case 3:
                     mouseButtonState |= RIGHT_MOUSE_BUTTON_MASK;
                     break;
                  default:
                     break;
               }
//               cerr << "Button Number Pressed: "
//                   << theEvent.xbutton.button << endl
//                    << endl;
               break;
            case ButtonRelease:
               switch (theEvent.xbutton.button) {
                  case 1:
                     mouseButtonState &= ~(LEFT_MOUSE_BUTTON_MASK);
                     break;
                  case 2:
                     mouseButtonState &= ~(MIDDLE_MOUSE_BUTTON_MASK);
                     break;
                  case 3:
                     mouseButtonState &= ~(RIGHT_MOUSE_BUTTON_MASK);
                     break;
                  default:
                     break;
               }
//               cerr << "Button Number Released: "
//                    << theEvent.xbutton.button << endl
//                    << endl;
               break;
            case DestroyNotify:
               cerr << "DestroyNotify" << endl;
               break;
            case KeyRelease:
               isChar = XLookupString (&theEvent.xkey, theBuffer,
                                       sizeof (theBuffer),
                                       &keyType, 0);
                switch (keyType) {
                   case XK_Shift_L:
                     keyState &= ~(LEFT_SHIFT_KEY_MASK);
                     break;
                  case XK_Shift_R:
                     keyState &= ~(RIGHT_SHIFT_KEY_MASK);
                     break;
                  case XK_Control_L:
                     keyState &= ~(LEFT_CONTROL_KEY_MASK);
                     break;
                  case XK_Control_R:
                     keyState &= ~(RIGHT_CONTROL_KEY_MASK);
                     break;
                  case XK_Caps_Lock:
                     break;
                  case XK_Alt_L:
                     keyState &= ~(LEFT_ALT_KEY_MASK);
                     break;
                  case XK_Alt_R:
                     keyState &= ~(RIGHT_ALT_KEY_MASK);
                     break;
                  default:
                     break;
                }
                break;
            case KeyPress:
               isChar = XLookupString (&theEvent.xkey, theBuffer, 
                                       sizeof (theBuffer),
                                       &keyType, 0);

//cerr << "Key Press: " << (hex) << keyType << (dec) << endl;

               if (isChar == 1) {
                  if (theBuffer[0] == NPS_ESCKEY) {
                     quitX = TRUE;
                  }
                  int thePlace = int (theBuffer[0]);
                  keyPressCounter[thePlace] = (keyPressCounter[thePlace] + 1)
                                              % ROLL_OVER_VALUE;

/*if ((thePlace == int('/')) || (thePlace == int('?'))) {
cerr << char(thePlace) << ": " << keyPressCounter[thePlace] << ","
                               << lastKeyPressCount[thePlace] << endl;
}*/

               }
               else {
                  switch (keyType) {
                     case XK_BackSpace:
                        break;
                     case XK_Tab:
                        break;
                     case XK_Linefeed:
                        break;
                     case XK_Clear:
                        break;
                     case XK_Return:
                        break;
                     case XK_Pause:
                        keybump(keyPressCounter[NPS_PAUSEKEY
                                + NPS_KEYOFFSET]);
                        break;
                     case XK_Scroll_Lock:
                        break;
                     case XK_Sys_Req:
                        break;
                     case XK_Delete:
                        break;
                     case XK_Home:
                        keybump(keyPressCounter[NPS_HOMEKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_Left:
                        keybump(keyPressCounter[NPS_LEFTARROWKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_Up:
                        keybump(keyPressCounter[NPS_UPARROWKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_Right:
                        keybump(keyPressCounter[NPS_RIGHTARROWKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_Down:
                        keybump(keyPressCounter[NPS_DOWNARROWKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_Page_Up:
                        keybump(keyPressCounter[NPS_PAGEUPKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_Page_Down:
                        keybump(keyPressCounter[NPS_PAGEDOWNKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_End:
                        keybump(keyPressCounter[NPS_ENDKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_Begin:
                        break;
                     case XK_Select:
                        break;
                     case XK_Print:
                        keybump(keyPressCounter[NPS_PRINTSCREENKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_Execute:
                        break;
                     case XK_Insert:
                        keybump(keyPressCounter[NPS_INSERTKEY 
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_Undo:
                        break;
                     case XK_Redo:
                        break;
                     case XK_Menu:
                        break;
                     case XK_Find:
                        break;
                     case XK_Cancel:
                        break;
                     case XK_Help:
                        break;
                     case XK_Break:
                        break;
                     case XK_script_switch:
                        break;
                     case XK_Num_Lock:
                        break;
                     case XK_KP_Space:
                        break;
                     case XK_KP_Tab:
                        break;
                     case XK_KP_Enter:
                        break;
                     case XK_KP_F1:
                        break;
                     case XK_KP_F2:
                        break;
                     case XK_KP_F3:
                        break;
                     case XK_KP_F4:
                        break;
                     case XK_KP_Home:
                        keybump(keyPressCounter[NPS_PADHOMEKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_KP_Left:
                        keybump(keyPressCounter[NPS_PADLEFTARROWKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_KP_Up:
                        keybump(keyPressCounter[NPS_PADUPARROWKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_KP_Right:
                        keybump(keyPressCounter[NPS_PADRIGHTARROWKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_KP_Down:
                        keybump(keyPressCounter[NPS_PADDOWNARROWKEY 
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_KP_Page_Up:
                        keybump(keyPressCounter[NPS_PADPGUPKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_KP_Page_Down:
                        keybump(keyPressCounter[NPS_PADPGDNKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_KP_End:
                        keybump(keyPressCounter[NPS_PADENDKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_KP_Begin:
                        keybump(keyPressCounter[NPS_PADCENTERKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_KP_Insert:
                        keybump(keyPressCounter[NPS_PADINSKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_KP_Delete:
                        keybump(keyPressCounter[NPS_PADDELKEY
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_KP_Equal:
                        break;
                     case XK_KP_Multiply:
                        break;
                     case XK_KP_Add:
                        break;
                     case XK_KP_Separator:
                        break;
                     case XK_KP_Subtract:
                        break;
                     case XK_KP_Decimal:
                        break;
                     case XK_KP_Divide:
                        break;
                     case XK_KP_0:
                        break;
                     case XK_KP_1:
                        break;
                     case XK_KP_2:
                        break;
                     case XK_KP_3:
                        break;
                     case XK_KP_4:
                        break;
                     case XK_KP_5:
                        break;
                     case XK_KP_6:
                        break;
                     case XK_KP_7:
                        break;
                     case XK_KP_8:
                        break;
                     case XK_KP_9:
                        break;
                     case XK_F1:
                        keybump(keyPressCounter[NPS_F1KEY + NPS_KEYOFFSET]);
                        break;
                     case XK_F2:
                        keybump(keyPressCounter[NPS_F2KEY + NPS_KEYOFFSET]);
                        break;
                     case XK_F3:
                        keybump(keyPressCounter[NPS_F3KEY + NPS_KEYOFFSET]);
                        break;
                     case XK_F4:
                        keybump(keyPressCounter[NPS_F4KEY + NPS_KEYOFFSET]);
                        break;
                     case XK_F5:
                        keybump(keyPressCounter[NPS_F5KEY + NPS_KEYOFFSET]);
                        break;
                     case XK_F6:
                        keybump(keyPressCounter[NPS_F6KEY + NPS_KEYOFFSET]);
                        break;
                     case XK_F7:
                        keybump(keyPressCounter[NPS_F7KEY + NPS_KEYOFFSET]);
                        break;
                     case XK_F8:
                        keybump(keyPressCounter[NPS_F8KEY + NPS_KEYOFFSET]);
                        break;
                     case XK_F9:
                        keybump(keyPressCounter[NPS_F9KEY + NPS_KEYOFFSET]);
                        break;
                     case XK_F10:
                        keybump(keyPressCounter[NPS_F10KEY + NPS_KEYOFFSET]);
                        break;
                     case XK_F11:
                        keybump(keyPressCounter[NPS_F11KEY + NPS_KEYOFFSET]);
                        break;
                     case XK_F12:
                        keybump(keyPressCounter[NPS_F12KEY 
                                                + NPS_KEYOFFSET]);
                        break;
                     case XK_Shift_L:
                        keyState |= LEFT_SHIFT_KEY_MASK;
                        break;
                     case XK_Shift_R:
                        keyState |= RIGHT_SHIFT_KEY_MASK;
                        break;
                     case XK_Control_L:
                        keyState |= LEFT_CONTROL_KEY_MASK;
                        break;
                     case XK_Control_R:
                        keyState |= RIGHT_CONTROL_KEY_MASK;
                        break;
                     case XK_Caps_Lock:
                        break;
                     case XK_Shift_Lock:
                        break;
                     case XK_Alt_L:
                        keyState |= LEFT_ALT_KEY_MASK;
                        break;
                     case XK_Alt_R:
                        keyState |= RIGHT_ALT_KEY_MASK;
                        break;
                     default:
                        break;
                  }
               }
               break;
            default:
               break;
         }
      }
   }

   exit (0);
}
