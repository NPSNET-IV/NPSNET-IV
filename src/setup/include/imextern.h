// imextern.h

extern "C" {
   void parser();
// void perror(char*);
//   void bzero(void*, int);
// void bcopy(void*, void*, int);
// void sscanf(char*, char*, ...);
   char *strcpy(char*, const char*);
// void strcat(void*, void*);
// long unsigned int	strlen(const char*);
// char *strdup(void*);
// void strncpy(void*, void*, int);
// int  strcasecmp(void*, void*);
// int  strncasecmp(void*, void*, int);
   void free(void*);
   void	exit(int);
};

// EOF
