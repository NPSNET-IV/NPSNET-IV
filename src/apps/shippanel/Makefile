# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre
include ../../Makefile.exe_pre

#MAKEFILE	=	Makefile.shipcontrols

# You can change these flags while testing for optimizations
OPT	=	-g
CFLAGS	=	$(OPT) -quickstart_info -nostdlib -nostdinc \
                       -xansi -woff 3262
#OBJLIST	=

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=	-I../../communication/include \
                        -I/usr/include/Vk       \
                        -I/usr/include          \
                        -I/usr/include/CC

# You can define your own standard include files, if necessary
STDINCS =	../../communication/include/disnetlib.h \
		../../communication/include/disconvert.h

# You can define include directories your code needs here --
APPLIBDIRS	=	-L../../communication/lib \
	                -L../../utilities/lib \
                        -L/lib \
                        -L/usr/lib 
                       
# You can include your own library flags (i.e. -llib) here --
APPLIBFLAGS	= -ldisnet -lLoral_round -lm -lmpc -lvkEZ -lvk\
                  -lvkhelp -lSgm -lXpm -lXm -lXt -lX11 -lgen -lidunet

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

TARGETS =       $(BINDIR)shipcontrol
OBJECTS	=	main.o Deck.o DeckUI.o \
                ShipControlWindowMainWindow.o unimplemented.o 
                

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.exe_post

# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

main.o:	/usr/include/X11/Xatom.h /usr/include/X11/Intrinsic.h 
main.o:	/usr/include/Vk/VkCallbackObject.h global.h 
main.o:	/usr/include/stddef.h /usr/include/X11/Xresource.h 
main.o:	/usr/include/sys/endian.h /usr/include/sys/ioctl.h 
main.o:	/usr/include/sys/termio.h /usr/include/net/soioctl.h 
main.o:	/usr/include/X11/Composite.h /usr/include/sys/mac_label.h 
main.o:	/usr/include/sys/bsd_types.h /usr/include/Vk/VkApp.h 
main.o:	/usr/include/sys/select.h /usr/include/sys/ioccom.h 
main.o:	/usr/include/sys/errno.h /usr/include/netinet/in.h 
main.o:	/usr/include/X11/Constraint.h /usr/include/X11/SM/SMlib.h 
main.o:	/usr/include/sys/ttydev.h /usr/include/Xm/Xm.h 
main.o:	/usr/include/Xm/VirtKeys.h ./ShipControlWindowMainWindow.h 
main.o:	/usr/include/Vk/VkSimpleWindow.h /usr/include/sys/ipc.h
main.o:	/usr/include/net/if_arp.h /usr/include/sys/types.h 
main.o:	/usr/include/sys/termios.h /usr/include/sys/siginfo.h 
main.o:	/usr/include/Xm/XmStrDefs.h /usr/include/X11/Object.h 
main.o:	/usr/include/sys/procset.h /usr/include/Vk/VkComponent.h 
main.o:	/usr/include/X11/StringDefs.h /usr/include/Vk/VkComponentList.h 
main.o:	/usr/include/X11/Core.h /usr/include/stdio.h 
main.o:	/usr/include/sys/prctl.h /usr/include/X11/SM/SM.h 
main.o:	/usr/include/Vk/VkPeriodic.h ../../communication/include/envirvalues.h 
main.o:	/usr/include/Xm/TextF.h /usr/include/errno.h 
main.o:	/usr/include/sgidefs.h ../../communication/include/pdu.h 
main.o:	/usr/include/net/if.h /usr/include/Xm/VendorS.h 
main.o:	/usr/include/task.h /usr/include/X11/Xutil.h 
main.o:	/usr/include/X11/Xlib.h /usr/include/ulocks.h 
main.o:	/usr/include/getopt.h /usr/include/stdlib.h 
main.o:	/usr/include/X11/ICE/ICElib.h /usr/include/X11/Xfuncproto.h 
main.o:	/usr/include/string.h /usr/include/X11/ICE/ICE.h 
main.o:	/usr/include/sys/signal.h /usr/include/time.h 
main.o:	/usr/include/assert.h /usr/include/X11/Xosdefs.h 
main.o:	/usr/include/sys/socket.h /usr/include/sys/time.h 
main.o:	/usr/include/limits.h /usr/include/X11/X.h 
main.o:	/usr/include/CC/iostream.h /usr/include/signal.h 
main.o:	/usr/include/memory.h /usr/include/X11/Shell.h 
main.o:	/usr/include/X11/RectObj.h #:edgc
DeckUI.o: /usr/include/X11/Xatom.h /usr/include/X11/Intrinsic.h 
DeckUI.o: /usr/include/Vk/VkCallbackObject.h /usr/include/Xm/Label.h 
DeckUI.o: /usr/include/stddef.h /usr/include/X11/Xresource.h 
DeckUI.o: /usr/include/X11/Composite.h /usr/include/sys/bsd_types.h 
DeckUI.o: /usr/include/sys/select.h /usr/include/Vk/VkResource.h 
DeckUI.o: /usr/include/X11/Constraint.h /usr/include/X11/SM/SMlib.h 
DeckUI.o: /usr/include/Xm/Xm.h /usr/include/Xm/VirtKeys.h 
DeckUI.o: /usr/include/Xm/Frame.h /usr/include/Xm/Form.h 
DeckUI.o: /usr/include/Xm/ArrowB.h /usr/include/Xm/RowColumn.h 
DeckUI.o: /usr/include/sys/types.h /usr/include/Xm/XmStrDefs.h 
DeckUI.o: /usr/include/X11/Object.h /usr/include/Xm/Separator.h 
DeckUI.o: /usr/include/Vk/VkComponent.h /usr/include/X11/StringDefs.h 
DeckUI.o: /usr/include/X11/Core.h /usr/include/Sgm/Dial.h 
DeckUI.o: /usr/include/X11/SM/SM.h /usr/include/Xm/TextF.h DeckUI.h 
DeckUI.o: /usr/include/sgidefs.h /usr/include/Xm/ToggleB.h 
DeckUI.o: /usr/include/Xm/VendorS.h /usr/include/Xm/BulletinB.h 
DeckUI.o: /usr/include/X11/Xutil.h /usr/include/X11/Xlib.h 
DeckUI.o: /usr/include/X11/ICE/ICElib.h /usr/include/X11/Xfuncproto.h 
DeckUI.o: /usr/include/string.h /usr/include/X11/ICE/ICE.h 
DeckUI.o: /usr/include/X11/Xosdefs.h /usr/include/X11/X.h 
DeckUI.o: /usr/include/X11/Shell.h /usr/include/X11/RectObj.h #:edgc
ShipControlWindowMainWindow.o: /usr/include/Vk/VkMenuItem.h 
ShipControlWindowMainWindow.o: /usr/include/X11/Xatom.h 
ShipControlWindowMainWindow.o: /usr/include/Vk/VkDialogManager.h 
ShipControlWindowMainWindow.o: /usr/include/X11/Intrinsic.h 
ShipControlWindowMainWindow.o: /usr/include/Vk/VkCallbackObject.h ./global.h 
ShipControlWindowMainWindow.o: /usr/include/stddef.h 
ShipControlWindowMainWindow.o: /usr/include/X11/Xresource.h 
ShipControlWindowMainWindow.o: /usr/include/sys/endian.h 
ShipControlWindowMainWindow.o: /usr/include/sys/ioctl.h 
ShipControlWindowMainWindow.o: /usr/include/sys/termio.h 
ShipControlWindowMainWindow.o: /usr/include/net/soioctl.h 
ShipControlWindowMainWindow.o: /usr/include/X11/Composite.h 
ShipControlWindowMainWindow.o: /usr/include/sys/mac_label.h 
ShipControlWindowMainWindow.o: /usr/include/sys/bsd_types.h 
ShipControlWindowMainWindow.o: /usr/include/Vk/VkApp.h 
ShipControlWindowMainWindow.o: /usr/include/sys/select.h 
ShipControlWindowMainWindow.o: /usr/include/Vk/VkRadioSubMenu.h 
ShipControlWindowMainWindow.o: /usr/include/sys/ioccom.h 
ShipControlWindowMainWindow.o: /usr/include/sys/errno.h 
ShipControlWindowMainWindow.o: /usr/include/Vk/VkSubMenu.h 
ShipControlWindowMainWindow.o: /usr/include/netinet/in.h 
ShipControlWindowMainWindow.o: /usr/include/X11/Constraint.h 
ShipControlWindowMainWindow.o: /usr/include/X11/SM/SMlib.h 
ShipControlWindowMainWindow.o: /usr/include/sys/ttydev.h 
ShipControlWindowMainWindow.o: /usr/include/Xm/Xm.h 
ShipControlWindowMainWindow.o: /usr/include/Vk/VkMenu.h 
ShipControlWindowMainWindow.o: /usr/include/Xm/VirtKeys.h ./Deck.h 
ShipControlWindowMainWindow.o: ./ShipControlWindowMainWindow.h 
ShipControlWindowMainWindow.o: /usr/include/Vk/VkSimpleWindow.h 
ShipControlWindowMainWindow.o: /usr/include/sys/ipc.h 
ShipControlWindowMainWindow.o: ../../communication/include/idu.h 
ShipControlWindowMainWindow.o: ../../communication/include/idunetlib.h 
ShipControlWindowMainWindow.o: /usr/include/net/if_arp.h 
ShipControlWindowMainWindow.o: /usr/include/sys/types.h 
ShipControlWindowMainWindow.o: /usr/include/sys/termios.h 
ShipControlWindowMainWindow.o: /usr/include/sys/siginfo.h 
ShipControlWindowMainWindow.o: /usr/include/Xm/XmStrDefs.h 
ShipControlWindowMainWindow.o: /usr/include/X11/Object.h 
ShipControlWindowMainWindow.o: /usr/include/sys/procset.h 
ShipControlWindowMainWindow.o: /usr/include/Vk/VkComponent.h 
ShipControlWindowMainWindow.o: /usr/include/X11/StringDefs.h 
ShipControlWindowMainWindow.o: /usr/include/Vk/VkComponentList.h 
ShipControlWindowMainWindow.o: /usr/include/X11/Core.h 
ShipControlWindowMainWindow.o: /usr/include/Vk/VkWidgetList.h 
ShipControlWindowMainWindow.o: /usr/include/stdio.h 
ShipControlWindowMainWindow.o: /usr/include/sys/prctl.h 
ShipControlWindowMainWindow.o: /usr/include/X11/SM/SM.h 
ShipControlWindowMainWindow.o: /usr/include/Vk/VkPeriodic.h 
ShipControlWindowMainWindow.o: ../../communication/include/envirvalues.h 
ShipControlWindowMainWindow.o: /usr/include/errno.h ./DeckUI.h 
ShipControlWindowMainWindow.o: /usr/include/sgidefs.h 
ShipControlWindowMainWindow.o: ../../communication/include/pdu.h 
ShipControlWindowMainWindow.o: /usr/include/Vk/VkFileSelectionDialog.h 
ShipControlWindowMainWindow.o: /usr/include/net/if.h 
ShipControlWindowMainWindow.o: /usr/include/Xm/VendorS.h 
ShipControlWindowMainWindow.o: /usr/include/task.h 
ShipControlWindowMainWindow.o: /usr/include/X11/Xutil.h 
ShipControlWindowMainWindow.o: /usr/include/X11/Xlib.h 
ShipControlWindowMainWindow.o: /usr/include/ulocks.h 
ShipControlWindowMainWindow.o: /usr/include/getopt.h 
ShipControlWindowMainWindow.o: /usr/include/stdlib.h 
ShipControlWindowMainWindow.o: /usr/include/X11/ICE/ICElib.h 
ShipControlWindowMainWindow.o: /usr/include/X11/Xfuncproto.h 
ShipControlWindowMainWindow.o: /usr/include/string.h 
ShipControlWindowMainWindow.o: /usr/include/X11/ICE/ICE.h 
ShipControlWindowMainWindow.o: /usr/include/sys/signal.h 
ShipControlWindowMainWindow.o: /usr/include/time.h 
ShipControlWindowMainWindow.o: /usr/include/assert.h 
ShipControlWindowMainWindow.o: /usr/include/X11/Xosdefs.h 
ShipControlWindowMainWindow.o: /usr/include/sys/socket.h 
ShipControlWindowMainWindow.o: /usr/include/sys/time.h 
ShipControlWindowMainWindow.o: /usr/include/limits.h 
ShipControlWindowMainWindow.o: /usr/include/X11/X.h 
ShipControlWindowMainWindow.o: /usr/include/CC/iostream.h 
ShipControlWindowMainWindow.o: /usr/include/signal.h 
ShipControlWindowMainWindow.o: /usr/include/memory.h 
ShipControlWindowMainWindow.o: /usr/include/X11/Shell.h 
ShipControlWindowMainWindow.o: /usr/include/X11/RectObj.h #:edgc
Deck.o: /usr/include/X11/Xatom.h /usr/include/X11/Intrinsic.h 
Deck.o: /usr/include/svr4_math.h /usr/include/Vk/VkCallbackObject.h 
Deck.o: ./global.h /usr/include/Xm/Label.h 
Deck.o: /usr/include/stddef.h /usr/include/sys/endian.h 
Deck.o: /usr/include/sys/ioctl.h /usr/include/X11/Xresource.h 
Deck.o: /usr/include/sys/termio.h /usr/include/net/soioctl.h 
Deck.o: /usr/include/X11/Composite.h /usr/include/sys/mac_label.h 
Deck.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h 
Deck.o: /usr/include/sys/ioccom.h /usr/include/Vk/VkResource.h 
Deck.o: /usr/include/sys/errno.h /usr/include/Vk/VkCompletionField.h 
Deck.o: /usr/include/netinet/in.h /usr/include/X11/Constraint.h 
Deck.o: /usr/include/X11/SM/SMlib.h /usr/include/sys/ttydev.h 
Deck.o: /usr/include/Vk/VkEZ.h /usr/include/Xm/Xm.h 
Deck.o: /usr/include/Xm/VirtKeys.h /usr/include/Xm/Frame.h 
Deck.o: ./Deck.h /usr/include/Xm/Form.h 
Deck.o: ./ShipControlWindowMainWindow.h /usr/include/Vk/VkSimpleWindow.h 
Deck.o: /usr/include/sys/ipc.h /usr/include/Xm/ArrowB.h 
Deck.o: /usr/include/sys/ipc.h /usr/include/Xm/ArrowB.h 
Deck.o: /usr/include/Vk/VkFormat.h /usr/include/Xm/RowColumn.h 
Deck.o: /usr/include/net/if_arp.h /usr/include/sys/types.h 
Deck.o: /usr/include/sys/termios.h /usr/include/sys/siginfo.h 
Deck.o: /usr/include/Xm/XmStrDefs.h /usr/include/X11/Object.h 
Deck.o: /usr/include/Xm/Separator.h /usr/include/sys/procset.h 
Deck.o: /usr/include/Vk/VkComponent.h /usr/include/X11/StringDefs.h 
Deck.o: /usr/include/X11/Core.h /usr/include/stdio.h 
Deck.o: /usr/include/Sgm/Dial.h /usr/include/sys/prctl.h 
Deck.o: /usr/include/X11/SM/SM.h /usr/include/Vk/VkPeriodic.h 
Deck.o: /usr/include/ctype.h ../../communication/include/envirvalues.h 
Deck.o: /usr/include/Xm/TextF.h /usr/include/errno.h 
Deck.o: ./DeckUI.h /usr/include/sgidefs.h 
Deck.o: ../../communication/include/pdu.h /usr/include/net/if.h 
Deck.o: /usr/include/Xm/ToggleB.h /usr/include/Xm/VendorS.h 
Deck.o: /usr/include/task.h /usr/include/Xm/BulletinB.h 
Deck.o: /usr/include/X11/Xutil.h /usr/include/X11/Xlib.h 
Deck.o: /usr/include/ulocks.h /usr/include/getopt.h 
Deck.o: /usr/include/stdlib.h /usr/include/X11/ICE/ICElib.h 
Deck.o: /usr/include/X11/Xfuncproto.h /usr/include/string.h 
Deck.o: /usr/include/sys/signal.h /usr/include/X11/ICE/ICE.h 
Deck.o: /usr/include/time.h /usr/include/math.h 
Deck.o: /usr/include/assert.h /usr/include/sys/socket.h 
Deck.o: /usr/include/X11/Xosdefs.h /usr/include/sys/time.h 
Deck.o: /usr/include/limits.h /usr/include/X11/X.h 
Deck.o: /usr/include/CC/iostream.h /usr/include/signal.h 
Deck.o: /usr/include/memory.h /usr/include/X11/Shell.h 
Deck.o: /usr/include/X11/RectObj.h #:edgc
