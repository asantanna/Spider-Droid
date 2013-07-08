
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

useconds_t phi_getMicros() {
  // get time in uS
  struct timeval t;
  gettimeofday (&t, NULL) ;
  return (useconds_t) t.tv_usec;
}

useconds_t phi_upTime() {
  return (useconds_t) (phi_getMicros() - g_startupTime.tv_usec);
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


