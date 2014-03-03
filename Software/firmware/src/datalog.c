//
// Routines for recording data for statistics
//

// uncomment this line to build this file standalone
// #define STANDALONE

#ifndef STANDALONE

    #include "phi.h"

#else

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #define UINT32          unsigned long
    #define UINT64          unsigned long long
    #define LOG_FATAL       printf

    UINT64 phiUpTime() {
      static UINT64 time = 0;
      return time++;
    }

    // preprocessor

    #define BASH_BLACK      "30"
    #define BASH_RED        "31"
    #define BASH_GREEN      "32"
    #define BASH_YELLOW     "33"
    #define BASH_BLUE       "34"
    #define BASH_MAGENTA    "35"
    #define BASH_CYAN       "36"
    #define BASH_WHITE      "37"

    #define BASH_START_COLOR(c)   "\e[1;" c "m"
    #define BASH_STOP_COLOR       "\e[0m"

#endif

#define DLOG_SIGN 'DLOG'

#define PASS BASH_START_COLOR(BASH_GREEN) "PASS" BASH_STOP_COLOR
#define FAIL BASH_START_COLOR(BASH_RED) "FAIL" BASH_STOP_COLOR

#define WRAP(x) ((x) >= 0) ? ((x) % pLog -> numElem) : ((x) + pLog -> numElem)
#define CLOSE_ENOUGH(a, b) (abs((a) - (b)) < 1e-3)

#define TO_TIME64(d)  ((UINT64) (((d) + 100) * 1e6))

typedef struct {
  UINT64 time;
  double data;

} DATALOG_ELEM;

typedef struct {
  UINT32 sign;
  int currIdx;
  int numElem;
  DATALOG_ELEM elem[];
  
} DATALOG;


DATALOG* dlog_create(int numElem) {

  int size = sizeof(DATALOG) + numElem * sizeof(DATALOG_ELEM);
  DATALOG* pLog = malloc(size);

  if (pLog == NULL) {
    LOG_FATAL("Not enough memory to allocate log in datalog.c");
  }

  memset(pLog, 0, size);
  pLog -> sign = DLOG_SIGN;
  pLog -> currIdx = 0;
  pLog -> numElem = numElem;

  return pLog;
}

void dlog_free(DATALOG* pLog) {

  if (pLog -> sign != DLOG_SIGN) {
    LOG_FATAL("bad pointer passed to freeDlog in datalog.c");
  }

  free(pLog);
}

void dlog_addElem_withTime(DATALOG* pLog, UINT64 time, double data) {
  // add element to current position of array
  pLog -> elem[pLog -> currIdx].time = time;
  pLog -> elem[pLog -> currIdx].data = data;
  pLog -> currIdx = WRAP(pLog -> currIdx + 1);

  // advance current position and wrap to beginning if past end
  if (pLog -> currIdx > pLog -> numElem)  pLog -> currIdx = 0;
}

void dlog_addElem(DATALOG* pLog, double data) {
  dlog_addElem_withTime(pLog, phiUpTime(), data);
}

double dlog_avg(DATALOG* pLog, int depth) {
  // average the last "depth" entries stored in the array
  if (depth < 0 || depth > pLog -> numElem) {
    LOG_FATAL("depth invalid");
    return 1;

  } else {
    int i;
    double sum = 0;
    int _currIdx = pLog -> currIdx;
    _currIdx = WRAP(_currIdx - depth);

    for (i=0 ; i<depth ; i++) {
      sum += pLog -> elem[_currIdx].data;
      _currIdx = WRAP(_currIdx + 1);
    }

    return ((double)sum) / depth;
  }
}

double cubic(
  double x1,
  double y1,
  double x2,
  double y2,
  double x3,
  double y3,
  double x4,
  double y4,
  double x5);

double dlog_predict(DATALOG* pLog, UINT64 time) {
  // look at last "depth" entries and guess the next one
  int _currIdx = WRAP(pLog -> currIdx - 4);

  return cubic(
    pLog -> elem[_currIdx].time,
    pLog -> elem[_currIdx].data,
    pLog -> elem[WRAP(_currIdx + 1)].time,
    pLog -> elem[WRAP(_currIdx + 1)].data,
    pLog -> elem[WRAP(_currIdx + 2)].time,
    pLog -> elem[WRAP(_currIdx + 2)].data,
    pLog -> elem[WRAP(_currIdx + 3)].time,
    pLog -> elem[WRAP(_currIdx + 3)].data,
    time);
  
}

void dlog_test()  {
  double avg;
  double guess;
  int i;
  DATALOG* pLog = dlog_create(3);

  printf("//\n// Running Datalog Test Suite\n//\n\n");

  printf("Testing dlog_avg() function: ");

  dlog_addElem(pLog, 6);
  dlog_addElem(pLog, 8);
  //array has 6 and 8
  avg = dlog_avg(pLog, 2);

  (avg == 7) ? printf(PASS "\n") : printf(FAIL "\n");

  printf("Testing wrapping: ");

  dlog_addElem(pLog, 2);
  dlog_addElem(pLog, 5);
  //array has 5, 8, and 2
  avg = dlog_avg(pLog, 3);

  (avg == 5) ? printf(PASS "\n") : printf(FAIL "\n");

  printf("Testing different depths: ");

  if ((avg = dlog_avg(pLog, 2)) == 3.5) {
    if (dlog_avg(pLog, 1) == 5) {
      printf(PASS "\n");

    } else {
      printf(FAIL " DEPTH 1 - got %f, expected 5\n", avg);
    }

  } else {
    printf(FAIL " DEPTH 2 - got %f, expected 3.5\n", avg);
  }

  printf("Testing invalid depth: ");

  avg = dlog_avg(pLog, -2);
  (avg == 1) ? printf(": "  PASS "\n") : printf(FAIL "\n");

  printf("Testing invalid depth: ");

  avg = dlog_avg(pLog, 7);
  (avg == 1) ? printf(": " PASS "\n") : printf(FAIL "\n");

  printf("Testing dlog_delete(): ");

  dlog_free(pLog);

  printf(PASS "\n");
  
  printf("Testing Cubic Extrapolator (direct): ");

  pLog = dlog_create(4);

  double x[] = { -3.8280, -2.5065, 1.3328, 4.1670 };
  double y[] = { 2.5192, 3.2189, 4.5950, 7.8150 };

  double x_guess = 5.7876;
  double y_guess = 11.6189;
  
  guess = cubic(
    x[0], y[0],
    x[1], y[1],
    x[2], y[2],
    x[3], y[3],
    x_guess);

  CLOSE_ENOUGH(guess, y_guess) ? printf(PASS "\n") : printf(FAIL " - got %g, expected %g\n", guess, y_guess);

  printf("Testing Cubic Extrapolator (direct + scale): ");

  guess = cubic(
    TO_TIME64(x[0]), y[0],
    TO_TIME64(x[1]), y[1],
    TO_TIME64(x[2]), y[2],
    TO_TIME64(x[3]), y[3],
    TO_TIME64(x_guess));

  CLOSE_ENOUGH(guess, y_guess) ? printf(PASS "\n") : printf(FAIL " - got %g, expected %g\n", guess, y_guess);
  
  printf("Testing Cubic Extrapolator (using dlog_predict): ");

  for (i=0 ; i<4 ; i++) {
    dlog_addElem_withTime(pLog, TO_TIME64(x[i]), y[i]);
  }

  guess = dlog_predict(pLog, TO_TIME64(x_guess));

  CLOSE_ENOUGH(guess, y_guess) ? printf(PASS "\n") : printf(FAIL " - got %g, expected %g\n", guess, y_guess);

  // done
  printf("\n");
}

#ifdef STANDALONE

int main()  {
  dlog_test();
  return 0;
}

#endif