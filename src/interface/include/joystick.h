typedef struct {
	char *port;			/* NULL for default port, given in fblib.h (/dev/ttyd2) */
} JoystickInit;

typedef struct {
	int digital;		/* One bit per digital signal */
	float analog[8];	/* Values range from -1.0 through 1.0 */
} JoystickData;

#ifdef __cplusplus
extern "C" {
#endif

int joystick_init (JoystickInit *);
int joystick_reset (JoystickInit *);
int joystick_acquire (JoystickData *);
int joystick_exit (void);

#ifdef __cplusplus
}
#endif
