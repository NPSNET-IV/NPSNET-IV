
//////////////////////////////////////////////////////////////
//
// Source file for VkTabbedDeck
//
// This class is a ViewKit component, which should eventually
// be added to the ViewKit library.
// Normally, nothing in this file should need to be changed.
//////////////////////////////////////////////////////////////
#include "VkTabbedDeck.h"
#include <Vk/VkDeck.h>
#include <Vk/VkTabPanel.h>
#include <Vk/VkResource.h>
#include <Xm/Form.h>


VkTabbedDeck::VkTabbedDeck(const char *name, Widget parent) : VkComponent(name)
{
    _baseWidget = XmCreateForm ( parent,  _name, NULL, 0);

    installDestroyHandler();
    
    _vkdeck = new VkDeck("deck", _baseWidget);
    _vkdeck->show();
    _tabs = new VkTabPanel("tabs", _baseWidget);    
    _tabs->show();
    VkAddCallbackMethod ( VkTabPanel::tabSelectCallback, _tabs, this,
                          &VkTabbedDeck::pop, NULL);

    XtVaSetValues(_vkdeck->baseWidget(), 
                  XmNtopAttachment, XmATTACH_FORM,
                  XmNleftAttachment, XmATTACH_FORM,
                  XmNrightAttachment, XmATTACH_FORM,
                  XmNbottomAttachment, XmATTACH_WIDGET,
                  XmNbottomWidget, _tabs->baseWidget(),
                  NULL);

    XtVaSetValues(_tabs->baseWidget(), 
                  XmNtopAttachment, XmATTACH_NONE,
                  XmNleftAttachment, XmATTACH_FORM,
                  XmNrightAttachment, XmATTACH_FORM,
                  XmNbottomAttachment, XmATTACH_FORM,
                  NULL);
}


VkTabbedDeck::~VkTabbedDeck()
{
    delete _vkdeck;
    delete _tabs;
}


const char *VkTabbedDeck::className()
{
    return "VkTabbedDeck";
}


void VkTabbedDeck::registerChild(VkComponent *child, const char *name)
{
    char *label = (char *)VkGetResource(child->baseWidget(), "tabLabel", "TabLabel", XmRString, name );
    _vkdeck->addView(child);
    _tabs->addTab((char *)label, child->baseWidget());    
}



void VkTabbedDeck::registerChild(Widget child, const char *name)
{
    char *label = (char *) VkGetResource(child, "tabLabel", "TabLabel", XmRString, name );
    _vkdeck->addView(child);
    _tabs->addTab((char *) label, child);
}


void VkTabbedDeck::pop(VkCallbackObject *, void *, void *callData)
{
    VkTabCallbackStruct *cbs = (VkTabCallbackStruct*) callData;

    if ( cbs && cbs->clientData )
        _vkdeck->pop((Widget) cbs->clientData);
}

