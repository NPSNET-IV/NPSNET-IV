#include <stdio.h>
#include "joystick.h"
#include "fblib.h"
#include "fb.h"

#define FALSE 0
#define TRUE 1

static int joystick_active = FALSE;

int joystick_init (JoystickInit *init_data)
{
	if (joystick_active) {
		fprintf(stderr, "Warning: joystick already initialized -- joystick_init ignored.\n");
	}
	else if ((open_flybox(init_data->port, FB_NOBLOCK) == -1) || (init_flybox() == -1)) {
		fprintf(stderr, "Error: joystick_init failed.\n");
		joystick_active = FALSE;
	}
	else {
		joystick_active = TRUE;
	}
	return joystick_active;
}

int joystick_reset (JoystickInit *init_data)
{
	if (joystick_active) {
		joystick_exit ();
	}
	return joystick_init (init_data);
}

int joystick_acquire (JoystickData *data)
{
	if (!joystick_active) {
		fprintf(stderr, "Error: joystick not active in joystick_acquire.\n");
		return FALSE;
	}
	else {
		w_fb ("o");
		if (r_fb(&(data->digital), data->analog) == -1) {
			fprintf(stderr, "Error: joystick_acquire failed.\n");
			return FALSE;
		}
		else {
			return TRUE;
		}
	}
}

int joystick_exit (void)
{
	close_flybox ();
	joystick_active = FALSE;
	return TRUE;
}
