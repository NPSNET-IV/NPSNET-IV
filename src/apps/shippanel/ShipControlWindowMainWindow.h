
//////////////////////////////////////////////////////////////
//
// Header file for ShipControlWindowMainWindow
//
//    This class is a ViewKit VkSimpleWindow subclass
//
// Normally, very little in this file should need to be changed.
// Create/add/modify menus using the builder.
//
// Try to restrict any changes to adding members below the
// "//---- End generated code section" markers
// Doing so will allow you to make chnages using the builder
// without losing any changes you may have made manually
//
//////////////////////////////////////////////////////////////
#ifndef SHIPCONTROLWINDOWMAINWINDOW_H
#define SHIPCONTROLWINDOWMAINWINDOW_H
#include <Vk/VkSimpleWindow.h>
#include <Vk/VkPeriodic.h>  //COMMS
#include "global.h"


//---- ShipControlWindowMainWindow class declaration

class ShipControlWindowMainWindow: public VkSimpleWindow { 

  public:

    ShipControlWindowMainWindow( const char * name, 
                                 ArgList args = NULL,
                                 Cardinal argCount = 0 );
    ~ShipControlWindowMainWindow();
    const char *className();
    virtual Boolean okToQuit();
    //---- End generated code section

    void functionloop();
    VkPeriodic *_timer;              //COMMS

  protected:



    // Classes created by this class

    // class DeckTabbedDeck *_deck;
    class Deck *_deck;


    //---- End generated code section

  private:


    static String      _defaultShipControlWindowMainWindowResources[];

    //---- End generated code section

};
#endif
