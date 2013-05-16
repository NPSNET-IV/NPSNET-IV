#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdlib.h>
#include <string.h>

#ifndef X_CURSOR_UTIL_DOT_H
#define X_CURSOR_UTIL_DOT_H

#ifndef TRUE
#define TRUE int (1);
#endif

#ifndef FALSE
#define FALSE int (0);
#endif

const int MAX_NUM_CUR = 20;

enum X_CUR_MODE {
   X_CUR_DEFAULT, X_CUR_ANIMATE, X_CUR_PICK, X_CUR_ERROR
};

enum X_PICK_MODE {
   X_CUR_TWO_D_RADAR_INFO, X_CUR_TWO_D_RADAR_PICK,
   X_CUR_SHIP_PICK,
   X_CUR_TWO_D_AHA_INFO, X_CUR_TWO_D_AHA_PICK,
   X_CUR_THREE_D_AHA_INFO, X_CUR_THREE_D_AHA_PICK
};


class XCursorUtil {

   private:
      double xcuLastTime;
      X_CUR_MODE mode;
      Display *xcuDisplay;
      Window xcuWindow;
      Cursor *xcuCursor;
      int xcuNumCursors;
      int xcuCurrentCursor;

      int parseFile (char *);

   public:
      XCursorUtil (char *, Display *, Window);
     ~XCursorUtil ();

      int updateTime (double);
      int updateMode (X_CUR_MODE);
      int updatePick (X_PICK_MODE);

};

#endif X_CURSOR_UTIL_DOT_H
