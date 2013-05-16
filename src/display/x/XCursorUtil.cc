#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream.h>
#include <string.h>


#include "XCursorUtil.h"

static int setXcolor (XColor *, char *);

int setXcolor (XColor *theColor, char *theName)
{
   int theResult = TRUE;

   if (!strcmp (theName, "red")) {
      theColor->red = 65535; theColor->green = 0; theColor->blue = 0;
      theColor->flags = DoRed | DoGreen | DoBlue;
   }
   else if (!strcmp (theName, "yellow")) {
      theColor->red = 65535; theColor->green = 65535; theColor->blue = 0;
      theColor->flags = DoRed | DoGreen | DoBlue;
   }
   else if (!strcmp (theName, "green")) {
      theColor->red = 0; theColor->green = 65535; theColor->blue = 0;
      theColor->flags = DoRed | DoGreen | DoBlue;
   }
   else if (!strcmp (theName, "gumbygreen")) {
      theColor->red = 0; theColor->green = 35000; theColor->blue = 25000;
      theColor->flags = DoRed | DoGreen | DoBlue;
   }
   else if (!strcmp (theName, "blue")) {
      theColor->red = 0; theColor->green = 0; theColor->blue = 65535;
      theColor->flags = DoRed | DoGreen | DoBlue;
   }
   else if (!strcmp (theName, "black")) {
      theColor->red = 0; theColor->green = 0; theColor->blue = 0;
      theColor->flags = DoRed | DoGreen | DoBlue;
   }
   else if (!strcmp (theName, "white")) {
      theColor->red = 65535; theColor->green = 65535; theColor->blue = 65535;
      theColor->flags = DoRed | DoGreen | DoBlue;
   }
   else {
      theResult = FALSE;
      theColor->red = 65535; theColor->green = 65535; theColor->blue = 65535;
      theColor->flags = DoRed | DoGreen | DoBlue;
   }

   return theResult;

}

int
XCursorUtil::parseFile (char *fileName)
{

   Pixmap pixmap, mask;
   int x, y, status;
   unsigned int h, w;

   XColor maskColor, curColor;

   ifstream inFile (fileName);

   if (!inFile) {
      cerr << "Error: Unable to open cursor config file: " << fileName
           << endl;
   }
   else {

      char curNameString[50];
      char curNameBM[100];
      char curNameMask[100];
      char curColorString[50];
      char maskColorString[50];
      char junkBuffer[1000]; 

      do {

         if (!(inFile >> junkBuffer >> curNameString 
                      >> curColorString >> maskColorString)) {
            inFile.getline (junkBuffer, 1000);
         }
         else {
            int loadOkay = TRUE;
            sprintf (curNameBM, "%s.bm", curNameString);
            sprintf (curNameMask, "%s.mask.bm", curNameString);

            status = XReadBitmapFile (xcuDisplay, xcuWindow,
                                      curNameMask,
                                      &w, &h, &mask, &x, &y);

            if (status != BitmapSuccess) {
               loadOkay = FALSE;
               cerr << "Unable to load: " << curNameMask << endl;
            }

            status = XReadBitmapFile (xcuDisplay, xcuWindow,
                                      curNameBM,
                                      &w, &h, &pixmap, &x, &y);

            if (status != BitmapSuccess) {
               loadOkay = FALSE;
               cerr << "Unable to load: " << curNameBM << endl;
            }

            setXcolor (&curColor, curColorString);
            setXcolor (&maskColor, maskColorString);

            if (loadOkay) {
               xcuCursor[xcuNumCursors] = XCreatePixmapCursor (xcuDisplay, 
                                                               pixmap, mask, 
                                                               &curColor,
                                                               &maskColor,
                                                               x, y);

               xcuNumCursors++;
            }
            else {
               cerr << "Error: Load Failed for: " << curNameString << endl;
            }
         }

      } while (!inFile.eof());
   }

   return TRUE;
}

XCursorUtil::XCursorUtil (char *configFile, 
                          Display *theDisplay, 
                          Window theWindow)
{
   xcuNumCursors = 0;
   xcuDisplay = theDisplay; 
   xcuWindow = theWindow;
   xcuLastTime = 0.0;
   mode = X_CUR_DEFAULT;

   xcuCursor = new Cursor[MAX_NUM_CUR];

   xcuCursor[0] = XCreateFontCursor (xcuDisplay, XC_X_cursor);
   XDefineCursor (xcuDisplay, xcuWindow, xcuCursor[0]);

   if (configFile != char (NULL)) {

      if (strcmp (configFile, "NONE")) {
        parseFile (configFile);
      }
   }

}

XCursorUtil::~XCursorUtil ()
{

   delete []xcuCursor;

}

int
XCursorUtil::updateTime (double)
{
   return TRUE;
}

int
XCursorUtil::updateMode (X_CUR_MODE theMode)
{
   mode = theMode;
   return TRUE;
}

int
XCursorUtil::updatePick (X_PICK_MODE thePick)
{
   (thePick);
   return TRUE;
}
