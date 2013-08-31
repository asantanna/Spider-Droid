
//
// Failsafe/heartbeat thread
//
// This loop runs on a separate thread and is responsible for:
//
// 1) Polling all devices and updating a snapshot of the current state
// 2) Heartbeat on status LED
// 3) Failsafe checking for conditions that might damage the robot
// 

#include "phi.h"

//
// DEFINES
//

#define LOOPS_PER_SEC       100
#define LOOP_PERIOD_USEC    ((INT32)(1e6 / LOOPS_PER_SEC))

//
// VARS
//

PHI_MUTEX_DECL(mtxState);

PHI_STATE_PACKET phiState;

//
// CODE
//

void* failsafeLoop(void* arg);
void updateState();

void startFailsafeThread() {

  // spawn thread to run the loop
  // Note2: thread is started detached and we don't keep track
  // of the pthread_t, thread is "fire and forget"

  pthread_t thread;
  pthread_attr_t threadAttr;

  // init default attrs
  PHI_ZERO(threadAttr);
  pthread_attr_init(&threadAttr);

  // request detached thread
  pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

  /*
  // alloc args
  
  PHILINK_ARGS* pArgs = PHI_ALLOC(....);
  pArgs -> a = a;
  pArgs -> b = b;
  */

  // create thread
  pthread_create(&thread, &threadAttr,  &failsafeLoop, NULL);

  // release thread attr because we don't use it
  pthread_attr_destroy(&threadAttr);

  // increase priority to make loop more even

  if (phi_setRealtimePrio(thread) == FALSE) {
    // not fatal
    LOG_ERR("set_realtime_priority() failed!");
  }
}

void* failsafeLoop(void* arg)
{
  UINT64 usec_loopStart;
  UINT64 usec_workEnd;
  UINT64 usec_loopEnd;
  INT32 usec_error = 0;

  LOG_INFO("Failsafe thread started");
  
  usec_loopEnd = phi_upTime();

  while (TRUE) {

    // time at start of loop
    usec_loopStart = usec_loopEnd;

    //
    // Do loop work
    //

    // update state variables
    updateState();

    //
    // Adjust loop duration
    //

    // time at end of work
    usec_workEnd = phi_upTime();

    // sleep any leftover time
    
    INT32 usec_workTime = (INT32) (usec_workEnd - usec_loopStart);
    INT32 usec_sleepTime = (INT32) (LOOP_PERIOD_USEC - usec_workTime + usec_error);

    if (usec_sleepTime < 0) {
      // no time left over
      usec_sleepTime = 0;
    }
    
    usleep(usec_sleepTime);
    usec_loopEnd = phi_upTime();

    // compute error in loop time
    //
    // NOTE: this is disabled because it doesn't improve things much
    // but it DOES increase the standard deviation
    //
    // usec_error += (INT32) (LOOP_PERIOD_USEC - (usec_loopEnd - usec_loopStart));

    // DEBUG
    // printf("sleep time = %ld uS, error = %ld uS\n", usec_sleepTime, usec_error);
    // sleep(1);
    // usec_loopEnd = phi_upTime();

    // HACK HACK
    // usec_error = 0;
  }
}

//
// PHI state related functions
//

void updateState() {

  int i;

  // sign

  PHI_MUTEX_GET(&mtxState);
  memcpy(phiState.sign, STAP_SIGN, sizeof(phiState.sign));
  PHI_MUTEX_RELEASE(&mtxState);

  // image

  PHI_MUTEX_GET(&mtxState);
  memset(phiState.image, 0, sizeof(phiState.image));
  PHI_MUTEX_RELEASE(&mtxState);

  // gyro (return +/- percent of max reading)

  PHI_MUTEX_GET(&mtxState);

  float pitchDelta, yawDelta, rollDelta;
  HAL_gyroGetDeltas(&pitchDelta, &yawDelta, &rollDelta);

  phiState.gyro[0] = pitchDelta;
  phiState.gyro[1] = yawDelta;
  phiState.gyro[2] = rollDelta;

  PHI_MUTEX_RELEASE(&mtxState);

  // joint (motor) positions

  for (i = 0 ; i < COUNTOF(phiState.joint) ; i++) {
    PHI_MUTEX_GET(&mtxState);
    phiState.joint[i] = HAL_getMotorPosition(i);
    PHI_MUTEX_RELEASE(&mtxState);
  }

  // temp

  PHI_MUTEX_GET(&mtxState);
  TODO("Temperature not implemented");
  PHI_MUTEX_RELEASE(&mtxState);
}

void prepStatePacket(PHI_STATE_PACKET *p) {
  PHI_MUTEX_GET(&mtxState);
  memcpy(p, &phiState, sizeof(phiState));
  PHI_MUTEX_RELEASE(&mtxState);
}




