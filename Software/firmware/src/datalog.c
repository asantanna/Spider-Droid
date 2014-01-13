//
// Routines for recording data for statistics
//

#include "phi.h"

#define DLOG_SIGN 'DLOG'

typedef struct {
  UINT32 time;
  UINT32 elem;

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

void dlog_addElem(PHI_DLOG* pLog, UINT32 elem) {

  // add element to current position of array
  // advance current position and wrap to beginning if past end
}

UINT32 dlog_avg(PHI_DLOG* pLog, int depth) {
  // average the last "depth" entries stored in the array
}

UINT32 dlog_predict(PHI_DLOG*pLog, int depth) {
  // look at last "depth" entries and guess the next one
}


