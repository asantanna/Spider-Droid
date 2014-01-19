//
// Routines for recording data for statistics
//

#define STANDALONE

#ifdef STANDALONE

#define UINT32          unsigned long
#define UINT64          unsigned long long

#define LOG_FATAL printf

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

  UINT64 phi_uptime() {
    static UINT64 time = 0;
    return time++;
  }

#else

#include "phi.h"

#endif

#define WRAP(x) ((x) >= 0) ? (x) % pLog -> numElem : (x) + pLog -> numElem
#define DLOG_SIGN 'DLOG'

typedef struct {
  UINT64 time;
  UINT32 data;

} PHI_DLOG_ELEM;

typedef struct {
  UINT32 sign;
  int currIdx;
  int numElem;
  PHI_DLOG_ELEM elem[];
  
} PHI_DLOG;


PHI_DLOG* dlog_create(int numElem) {

  int size = sizeof(PHI_DLOG) + numElem * sizeof(PHI_DLOG_ELEM);
  PHI_DLOG* pLog = malloc(size);

  if (pLog == NULL) {
    LOG_FATAL("Not enough memory to allocate log in datalog.c");
  }

  memset(pLog, 0, size);
  pLog -> sign = DLOG_SIGN;
  pLog -> currIdx = 0;
  pLog -> numElem = numElem;

  return pLog;
}

void dlog_free(PHI_DLOG* pLog) {

  if (pLog -> sign != DLOG_SIGN) {
    LOG_FATAL("bad pointer passed to freeDlog in datalog.c");
  }

  free(pLog);
}

void dlog_addElem(PHI_DLOG* pLog, UINT32 data) {
  // add element to current position of array
  pLog -> elem[pLog -> currIdx].time = phi_uptime();
  pLog -> elem[pLog -> currIdx].data = data;
  pLog -> currIdx = WRAP(pLog -> currIdx + 1);

  // advance current position and wrap to beginning if past end
  if (pLog -> currIdx > pLog -> numElem)  pLog -> currIdx = 0;
}

double dlog_avg(PHI_DLOG* pLog, int depth) {
  // average the last "depth" entries stored in the array
  if (depth < 0 || depth > pLog -> numElem) {
    LOG_FATAL("depth invalid");
    return (double)1;

  } else {
    int i;
    UINT32 sum = 0;
    int _currIdx = pLog -> currIdx;
    _currIdx = WRAP(_currIdx - depth);

    for (i=0 ; i<depth ; i++) {
      sum += pLog -> elem[_currIdx].data;
      _currIdx = WRAP(_currIdx + 1);
    }

    return ((double)sum) / depth;
  }
}

UINT32 dlog_predict(PHI_DLOG*pLog, int depth) {
  // look at last "depth" entries and guess the next one
}

void dlog_test()  {
  double avg;
  PHI_DLOG* pLog = dlog_create(3);

  printf("//\n// Running Datalog Test Suite\n//\n\nTesting dlog_avg() function: ");

  dlog_addElem(pLog, 6);
  dlog_addElem(pLog, 8);
  //array has 6 and 8
  avg = dlog_avg(pLog, 2);

  (avg == 7) ? printf("PASS\n") : printf("FAIL\n");

  printf("\nTesting wrapping: ");

  dlog_addElem(pLog, 2);
  dlog_addElem(pLog, 5);
  //array has 5, 8, and 2
  avg = dlog_avg(pLog, 3);

  (avg == 5) ? printf("PASS\n") : printf("FAIL\n");

  printf("\nTesting different depths: ");

  if ((avg = dlog_avg(pLog, 2)) == 3.5) {
    if (dlog_avg(pLog, 1) == 5) {
      printf("PASS\n");

    } else {
      printf("FAIL DEPTH 1\n");
    }

  } else {
    printf("FAIL DEPTH 2 - got %f, expected 3.5\n", avg);
  }

  printf("\nTesting invalid depth: ");

  avg = dlog_avg(pLog, -2);
  (avg == 1) ? printf(": PASS\n") : printf("FAIL\n");

  printf("\nTesting invalid depth: ");

  avg = dlog_avg(pLog, 7);
  (avg == 1) ? printf(": PASS\n") : printf("FAIL\n");
}

#ifdef STANDALONE

int main()  {
  dlog_test();
  return 0;
}

#endif