
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

  // truncate log files that are > 1000 lines (or 500 if CRLF is used)
  phi_logfp = fopen(filename, "r");

  int lines = 0;
  if (phi_logfp != NULL) {
    int c;
    //count end of lines, both of them... just in case
    while ( (c = fgetc(phi_logfp)) != EOF ) {
      if (c== '\n' || c == '\r') {
        lines++;
      }
    }
    fclose(phi_logfp);
  }

  // reopen file handle
  printf("log file has %d lines\n", lines);

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
  fprintf(phi_logfp, "T=%.2f mS : ", ((double)phi_upTime()) / 1000);
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

UINT32 getHostIP() {
  
  struct ifaddrs *ifaddr, *ifa;
  int family, s;
  char host[NI_MAXHOST];
  UINT32 ipAddr = 0;

  if (getifaddrs(&ifaddr) == -1) {
    LOG_ERR("getifaddrs() failed in getHostIP()");
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
    LOG_ERR("interace IP address not found in getHostIP()");
  } else {
    LOG_INFO("returning interface IP=%lu.%lu.%lu.%lu",
             ipAddr & 0xff, (ipAddr >> 8) & 0xff,
             (ipAddr >> 16) & 0xff,(ipAddr >> 24) & 0xff);
  }

  return ipAddr;
}


void phi_setLED(PHI_LED_COLOR color) {
  
  TODO("Implement phi_setLED()")

  switch (color) {
    case OFF:
      break;
    case RED:
      break;
    case FLASH_RED:
      break;
    case YELLOW:
      break;
    case FLASH_YELLOW:
      break;
    case GREEN:
      break;
    case FLASH_GREEN:
      break;

      }
}

double phi_rand() {
  return ((double) rand()) / RAND_MAX;
}

//
// To change curr thread, use set_realtime_priority(pthread_self());
//

BOOL phi_setRealtimePrio(pthread_t thread) {

  // struct sched_param is used to store the scheduling priority
  struct sched_param params;
  
  // Set the priority to the maximum
  params.sched_priority = sched_get_priority_max(SCHED_FIFO);

  // Attempt to set thread real-time priority to the SCHED_FIFO policy
  // (return TRUE if success)
  return (pthread_setschedparam(thread, SCHED_FIFO, &params) == 0);
}

//
// I/O control helpers
//

//
// Setting a file descriptor to non-blocking causes write and read calls
// to return an error if the call would have blocked.  Typically a block
// will occur with a write if the the output buffer is full.  With a read,
// it typically occurs if there is no data available.
//
// The actual error returned is EAGAIN for anything but sockets.  In the
// case of a socket, EAGAIN or EWOULDBLOCK might be returned (equivalently)
// due to lack of clarity in the POSIX doc.
//

int setNonBlocking(int fd)
{
  int flags;

#if defined(O_NONBLOCK)

  INFO("Using POSIX O_NONBLOCK in setNonBlocking()");
  
  // O_NONBLOCK exists, use the Posix way to do it
  if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
    // get failed - assume flags = 0
    flags = 0;
  }
  
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  
#else
  
  INFO("Using old school method in setNonBlocking()");
  
  // O_NONBLOCK does not exist, use old method
  flags = 1;
  return ioctl(fd, FIOBIO, &flags);
  
#endif
}   