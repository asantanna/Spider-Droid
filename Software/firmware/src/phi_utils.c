
//
// Useful utilities
//

#include "phi.h"

void* phi_allocHelper(int size) {

  void* p = malloc(size);
  if (p == NULL) {
    phi_abortWithMsg("out of mem");
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

void phi_abortProcess(int rc) {
  printf("***\n");
  printf("*** Aborting PHI process!\n");
  printf("***\n");
  exit(rc);
}

void phi_abortWithMsg(const char* msg) {
  printf(msg, 0);
  phi_abortProcess(-1);
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

int phi_logInit(char *filename) {
  //check if phi_logfp is already open.
  if (phi_logfp != NULL) {
    fclose(phi_logfp);
  }
  phi_abortProcess(-1);

  // truncate log files that are > 1000 lines (or 500 if CRLF is used)
  phi_logfp = fopen(filename, "r");
  int lines = 0;
  if (phi_logfp != NULL) {
    char c;
    //count end of lines, both of them... just in case
    while ( (c = fgetc(phi_logfp)) != EOF ) {
      if (c== '\n' || c == '\r') {
        lines++;
      }
    }
    fclose(phi_logfp);
  } else {
    // couldn't open
    return 0;
  }

  // reopen file handle
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

void phi_logClose(void) {
  if (phi_logfp != NULL)  {
    fclose(phi_logfp);
    phi_logfp = NULL;
  }
}

void phi_logTimestamp() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  fprintf(phi_logfp, "T=%llu uS : ", phi_upTime());
  // no flush here
}

void phi_logMsg(const char* tag, const char *fmt, ...) {
  if (phi_logfp == NULL) return;
  phi_logTimestamp();
  fprintf(phi_logfp, tag, 0);  // 0 at end to placate compiler
  va_list ap;
  va_start(ap, fmt);
  vfprintf(phi_logfp, fmt, ap);
  va_end(ap);
  fprintf(phi_logfp, "\n");
  fflush(phi_logfp);
}

// code adapted from http://man7.org/linux/man-pages/man3/getifaddrs.3.html

UINT32 phi_getHostIP() {
  
  struct ifaddrs *ifaddr, *ifa;
  int family, s;
  char host[NI_MAXHOST];
  UINT32 ipAddr = 0;

  if (getifaddrs(&ifaddr) == -1) {
    LOG_ERR("getifaddrs() failed in phi_getHostIP()");
    return 0;
  }

  /* Walk through linked list, maintaining head pointer so we
  can free list later */

  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    
    if (ifa->ifa_addr == NULL) {
      continue;
    }

    family = ifa->ifa_addr->sa_family;

    // log interface name and family (including symbolic
    // form of the latter for the common families)

    LOG_INFO("found interface '%s', address family: %d%s",
           ifa->ifa_name, family,
           (family == AF_PACKET) ? " (AF_PACKET)" :
           (family == AF_INET) ?   " (AF_INET)" :
           (family == AF_INET6) ?  " (AF_INET6)" : "");

    // look for first non-loopback interface address of type AF_INET

    if (family == AF_INET) {
      ipAddr = *(UINT32*)&(((struct sockaddr_in *) ifa->ifa_addr) -> sin_addr);
      if ((ipAddr & 0xff) != 127) {
        // not loopback
        break;
      }
    }
  }

  freeifaddrs(ifaddr);
  
  if (ipAddr == 0) {
    LOG_ERR("interace IP address not found in phi_getHostIP()");
  } else {
    LOG_INFO("returning interface IP=%lu.%lu.%lu.%lu",
             ipAddr & 0xff, (ipAddr >> 8) & 0xff,
             (ipAddr >> 16) & 0xff,(ipAddr >> 24) & 0xff);
  }

  return ipAddr;
}

