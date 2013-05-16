#define PORT "/dev/ttyd1"

int open_flybox (char *p, int flag);
int init_flybox (void);
int set_baud (int b);
int r_fb (int *dio, float *ach);
int w_fb (char *mode);
void close_flybox (void);

