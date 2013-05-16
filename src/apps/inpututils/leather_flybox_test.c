#include <stdio.h>
#include "joystick.h"

#define FALSE  0
#define TRUE   1

JoystickInit *j_init;
JoystickData *j_data;

void
main ()
{
	int done=FALSE;
        int pass; 
	char crap=' ';
	
        j_data = (JoystickData *) malloc (sizeof (JoystickData));
        j_init = (JoystickInit *) malloc (sizeof (JoystickInit));
        j_init->port = "/dev/ttyd1";

        if (!joystick_init (j_init))
		fprintf(stderr, "Error opening the joystick.\n");
        else 
		fprintf(stderr, "Joystick open succeeded.\n");

	while ((!done) && (crap != 'q'))
	{
		if (!joystick_acquire (j_data))
			fprintf (stderr,"Error reading joystick\n");
		else
		{
			fprintf (stderr,"Analog[0] = %f\n", j_data->analog[0]);
			fprintf (stderr,"Analog[1] = %f\n", j_data->analog[1]);
			fprintf (stderr,"Analog[2] = %f\n", j_data->analog[2]);
			fprintf (stderr,"Analog[3] = %f\n", j_data->analog[3]);
			fprintf (stderr,"Analog[4] = %f\n", j_data->analog[4]);
			fprintf (stderr,"Analog[5] = %f\n", j_data->analog[5]);
			fprintf (stderr,"Analog[6] = %f\n", j_data->analog[6]);
			fprintf (stderr,"Analog[7] = %f\n", j_data->analog[7]);
			fprintf (stderr,"Digital = %d\n\n", j_data->digital);
		}

		if (j_data->digital > 63800)
			done = TRUE;
		scanf ("%c",&crap);
	}
}
