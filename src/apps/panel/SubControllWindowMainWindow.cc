
//////////////////////////////////////////////////////////////
//
// Source file for SubControllWindowMainWindow
//
//    This class is a ViewKit VkSimpleWindow subclass
//
//
// Normally, very little in this file should need to be changed.
// Create/add/modify menus using the builder.
//
// Try to restrict any changes to the bodies of functions
// corresponding to menu items, the constructor and destructor.
//
// Add any new functions below the "//--- End generated code"
// markers
//
// Doing so will allow you to make changes using the builder
// without losing any changes you may have made manually
//
// Avoid gratuitous reformatting and other changes that might
// make it difficult to integrate changes made using the builder
//////////////////////////////////////////////////////////////
#include "SubControllWindowMainWindow.h"
#include <Vk/VkApp.h>
#include <Vk/VkFileSelectionDialog.h>
#include <Vk/VkSubMenu.h>
#include <Vk/VkRadioSubMenu.h>
#include <Vk/VkMenuItem.h>
#include "DeckTabbedDeck.h"
//---- End generated headers

//---- Added includes
#include <iostream.h>

IDUType type;
char *idu;
TestIDU *testidu;
extern NPSNETToSubIDU       *updatesubidu;
extern HelmToNPSNETIDU      helmidu;
extern SubOodToNPSNETIDU    oodidu;
extern WeapsToNPSNETIDU     weapsidu;
sender_info idu_sender_info;
int swap_bufs;
extern IDU_net_manager     *net;

// These are default resources for widgets in objects of this class
// All resources will be prepended by *<name> at instantiation,
// where <name> is the name of the specific instance, as well as the
// name of the baseWidget. These are only defaults, and may be overriden
// in a resource file by providing a more specific resource name

String  SubControllWindowMainWindow::_defaultSubControllWindowMainWindowResources[] = {
        (char*)NULL
};


//---- Class declaration

SubControllWindowMainWindow::SubControllWindowMainWindow( const char *name,
                                                          ArgList args,
                                                          Cardinal argCount) : 
                                                    VkSimpleWindow (name, args, argCount) 
{
    // Create the view component contained by this window

    _deck= new DeckTabbedDeck("deck",mainWindowWidget());


    XtVaSetValues ( _deck->baseWidget(),
                    XmNwidth, 1264, 
                    XmNheight, 319, 
                    (XtPointer) NULL );

    // Add the component as the main view

    addView (_deck);
    _deck->setParent(this);

    //---- End generated code section
    // set up the timer loop to read the network and DIS at the interval  //COMMS
    // (in milliseconds) specified by the VkPeriodic argument.            //COMMS
    _timer = new VkPeriodic(1000);                                         //COMMS
                           
    VkAddCallbackMethod(VkPeriodic::timerCallback,  
			_timer, this, &SubControllWindowMainWindow::functionloop, NULL);      //COMMS
    _timer->start();                                                      //COMMS
    
    
} // End Constructor


SubControllWindowMainWindow::~SubControllWindowMainWindow()
{
    delete _deck;
} // End destructor


const char *SubControllWindowMainWindow::className()
{
    return ("SubControllWindowMainWindow");
} // End className()


Boolean SubControllWindowMainWindow::okToQuit()
{

    // This member function is called when the user quits by calling
    // theApplication->terminate() or uses the window manager close protocol
    // This function can abort the operation by returning FALSE, or do some.
    // cleanup before returning TRUE. The actual decision is normally passed on
    // to the view object


    // Query the view object, and give it a chance to cleanup

    return ( _deck->okToQuit() );
} // End okToQuit()

void SubControllWindowMainWindow::functionloop()
{
  int number_read = 0;
   
   while ( (number_read < 5) &&
           (net->read_idu(&idu, &type, idu_sender_info, swap_bufs) != NULL)  )
       {
        switch(type)
           {
           case (Test_Type):
             testidu = (TestIDU *)idu;
             cerr<<"Got a Test_type idu"<<endl;
             _deck->update();
             break;
           case (NPSNET_To_SUB_Type):
             updatesubidu = (NPSNETToSubIDU *)idu;
             _deck->update();
             break;
           default:
             cerr << "\nUnknown type received: " << int(type)
                  << endl << endl;
             break;
            }
        number_read++;
       }
     
} // End functionloop()
//--- End generated member functions

