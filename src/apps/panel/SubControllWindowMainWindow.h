
//////////////////////////////////////////////////////////////
//
// Header file for SubControllWindowMainWindow
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
#ifndef SUBCONTROLLWINDOWMAINWINDOW_H
#define SUBCONTROLLWINDOWMAINWINDOW_H
#include <Vk/VkSimpleWindow.h>
#include "global.h"

//---- End generated headers
#include <Vk/VkPeriodic.h>  //COMMS

//---- SubControllWindowMainWindow class declaration

class SubControllWindowMainWindow: public VkSimpleWindow { 

  public:

    SubControllWindowMainWindow( const char * name, 
                                 ArgList args = NULL,
                                 Cardinal argCount = 0 );
    ~SubControllWindowMainWindow();
    const char *className();
    virtual Boolean okToQuit();
    
    void functionloop();
    //---- End generated code section
    VkPeriodic *_timer;              //COMMS
  protected:



    // Classes created by this class

    class DeckTabbedDeck *_deck;


    //---- End generated code section

  private:


    static String      _defaultSubControllWindowMainWindowResources[];

    //---- End generated code section

};

#endif
