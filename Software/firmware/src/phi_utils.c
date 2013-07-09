
//
// Useful utilities
//

#include "phi.h"

void* phi_allocHelper(int size) {

  void* p = malloc(size);
  if (p == NULL) {
    phi_abort_with_msg("out of mem");
  }
  memset(p, 0, size);
  return p;
}

UINT64 phi_upTime() {
  // up time in uSecs
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return TV_TO_USEC(tv) - g_startupTime;
}

void phi_abort_process(int rc) {
  printf("***\n");
  printf("*** Aborting PHI process!\n");
  printf("***\n");
  exit(rc);
}

void phi_abort_with_msg(const char* msg) {
  printf(msg, 0);
  phi_abort_process(-1);
}

/*
 C++ version 0.4 char* style "itoa":
 Written by Lukás Chmela
 Released under GPLv3.
*/

char* __itoa(int value, char* result, int base) {

  // make sure we don't get a null ptr
  if (result == 0) {
    return 0;
  }
  
  // check that the base if valid
  if (base < 2 || base > 36) {
    *result = '\0';
    return result;
  }

  char* ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
  } while (value);

  // Apply negative sign
  if (tmp_value < 0) {
    *ptr++ = '-';
  }
  
  *ptr-- = '\0';
  
  while(ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr-- = *ptr1;
    *ptr1++ = tmp_char;
  }
  
  return result;
}

//
// Logging functions
//

/*
 * Originally based on 
 *
 * File   :	dm8log.c
 * Author :	DM
 * License: BSD 3-clause license http://gotfu.wordpress.com/bsd-3-clause-license/
 * Copyright (c) 2011 Got-fu? http://gotfu.wordpress.com/
 *
 */

FILE *phi_logfp = NULL;

int phi_log_init(char *filename) {
  //check if phi_logfp is already open.
  if (phi_logfp != NULL) {
    fclose(phi_logfp);
  }

  //truncate log files that are > 1000 lines ( or if \r\n EOL is used, 500.)
  phi_logfp = fopen(filename, "r"); //open file in read mode.
  int lines = 0;
  if (phi_logfp != NULL) {
    char c;
    //count end of lines, both of them... just in case
    while ( (c = fgetc(phi_logfp)) != EOF )
      if (c== '\n' || c == '\r')
        lines++;
    fclose(phi_logfp);
  }

  //open file handle
  if (lines > 1000)
  {
    phi_logfp = fopen(filename, "w");	// w will truncate the file, and then open it for writing.
    if (phi_logfp == NULL) {
      return 0;
    }
    else
      return 1;
  }
  else
  {
    phi_logfp = fopen(filename, "a"); // append, or make new file.
    if (phi_logfp == NULL) {
      return 0;
    }
    else
      return 2;
  }
  return 0;
}

void phi_log_close(void) {
  if (phi_logfp != NULL)  {
    fclose(phi_logfp);
    phi_logfp = NULL;
  }
}

void phi_log_timestamp() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  fprintf(phi_logfp, "Time %llu : ", phi_upTime());
  // no flush here
}

void phi_log_msg(const char* tag, const char *fmt, ...) {
  if (phi_logfp == NULL) return;
  phi_log_timestamp();
  fprintf(phi_logfp, tag, 0);  // 0 at end to placate compiler
  va_list ap;
  va_start(ap, fmt);
  vfprintf(phi_logfp, fmt, ap);
  va_end(ap);
  fprintf(phi_logfp, "\n");
  fflush(phi_logfp);
}

/* example usage of the above.
int main(void) {
phi_log_init("phi.log");
int number = 123;
char *string = "hello, world!";
char ch = 'A';
int curtime = (int) time(NULL);
srand(curtime);
int foo = rand();
phi_log_printf("log entry%d: number=%d, string=%s, string\'s address=%p, ch=%c, foo=%d\n", curtime, number, string, string, ch, foo);
phi_log_printf("plain string, no params\n");
phi_log_close();
}
*/

