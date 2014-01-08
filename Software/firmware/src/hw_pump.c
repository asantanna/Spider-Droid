
//
// Hardware pump thread
//
// This loop runs on a separate thread and is responsible for:
//
// 1) Pumping data from all devices and updating a snapshot of the current state.
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

// snapshot of phi state (sensors and cmds)

typedef struct {

  PHI_CMD_PACKET cmds;
  PHI_STATE_PACKET sensors;

} PHI_STATE;

static PHI_STATE phiState;

// mutex to sync access to phiStateSnapshot
static PHI_MUTEX_DECL(mtxState);

// packet ID to check for missed packets
static UINT32 packetId = 0;

// internal

void initState();
void updateState();

void* hwPump_UART_thread(void* arg);
void* hwPump_SPI_thread(void* arg);
void* hwPump_I2C_thread(void* arg);

//
// CODE
//

/*

  The main purpose of startHwPump is to start helper threads to continually and efficiently keep synchronized the true
  hardware state and its snapshot in phiState.  This involves continually polling the hardware and updating phiState.

  To be efficient, we must take into account that:

  1) We have three different interfaces to poll so they can be done in parallel (UART, SPI and I2C). For simplicity,
     we start a separate "pump" thread for each interface.

  2) There are also other threads in the system that may require access to the interfaces so we must synchronize this.
  
  3) Access to phiState must also be synchronized between threads.
  
*/

void startHwPump() {

  // init phi state
  initState();

  // spawn threads to pump each interface
  //
  // Note: threads are started detached and we don't keep track
  // of the pthread_t, threads are "fire and forget"

  int iface;

  for (iface = 0 ; iface < 3 ; iface++) {


    pthread_t thread;
    pthread_attr_t threadAttr;

    // init default attrs
    PHI_ZERO(threadAttr);
    pthread_attr_init(&threadAttr);

    // request detached thread
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

    /* NO ARGUMENTS 

    // alloc args

    PHILINK_ARGS* pArgs = PHI_ALLOC(....);
    pArgs -> a = a;
    pArgs -> b = b;

    */

    // create interface thread

    switch(iface) {
      case 0:
        pthread_create(&thread, &threadAttr,  &hwPump_UART_thread, NULL);
        break;
      case 1:
        pthread_create(&thread, &threadAttr,  &hwPump_SPI_thread, NULL);
        break;
      case 2:
        pthread_create(&thread, &threadAttr,  &hwPump_I2C_thread, NULL);
        break;
    }

    // release thread attr because we don't use it
    pthread_attr_destroy(&threadAttr);

    /*
    // increase priority to make loop more even

    if (phi_setRealtimePrio(thread) == FALSE) {
      // not fatal
      LOG_ERR("set_realtime_priority() failed!");
    }
    */
    
  } // for
  
}

void* hwPump_UART_thread(void* arg)
{
  LOG_INFO("hwPump_UART_thread started");
  printf("hwPump_UART_thread started");

  while (TRUE) {
  }
}
 
void* hwPump_SPI_thread(void* arg)
{
  LOG_INFO("hwPump_SPI_thread started");
  printf("hwPump_SPI_thread started");

  int i;

  while (TRUE) {

    // joint (motor) positions

    for (i = 0 ; i < COUNTOF(phiState.sensors.joint) ; i++) {
      float j = HAL_getJointPosition(i);
      lock_state();
      phiState.sensors.joint[i] = j;
      unlock_state();
    }
  }
}

void* hwPump_I2C_thread(void* arg)
{
  LOG_INFO("hwPump_I2C_thread started");
  printf("hwPump_I2C_thread started");

  while (TRUE) {
  }
}

//
// PHI state init/pump functions
//

void initState() {

  int i;

  // grab mutex
  lock_state();
  
  // sign
  memcpy(phiState.sensors.sign, STAP_SIGN, sizeof(phiState.sensors.sign));

  // packet ID
  phiState.sensors.id = 0;

  // image
  memset(phiState.sensors.image, 0, sizeof(phiState.sensors.image));

  // gyro
  phiState.sensors.gyro[0] = 0;
  phiState.sensors.gyro[1] = 0;
  phiState.sensors.gyro[2] = 0;

  // joint (motor) positions
  for (i = 0 ; i < COUNTOF(phiState.sensors.joint) ; i++) {
    phiState.sensors.joint[i] = 0;
  }

  // temp
  phiState.sensors.temp = 0;

  // free state mutex
  unlock_state();
}

void updateState() {

  int i;

  // sign

  lock_state();
  memcpy(phiState.sensors.sign, STAP_SIGN, sizeof(phiState.sensors.sign));
  unlock_state();
  
  // packet ID (increment)

  lock_state();
  phiState.sensors.id = packetId++;
  unlock_state();

  // image

  lock_state();
  memset(phiState.sensors.image, 0, sizeof(phiState.sensors.image));
  unlock_state();

  // gyro (return +/- percent of max reading)
  //
  // note: we actually accumulate the change in the state
  // instead of of the change because this runs at a faster
  // rate than the transmission so data would be lost

  float pitchDelta, yawDelta, rollDelta;
  HAL_gyroGetDeltas(&pitchDelta, &yawDelta, &rollDelta);

  lock_state();

  phiState.sensors.gyro[0] += pitchDelta;
  phiState.sensors.gyro[1] += yawDelta;
  phiState.sensors.gyro[2] += rollDelta;

  unlock_state();

  // temperature
  float t = HAL_gyroGetTemp();
  lock_state();
  phiState.sensors.temp = t;
  unlock_state();
}

void phi_getStateSnapshot(PHI_STATE_PACKET *p) {

  //
  // DEBUG - update every request
  // updateState();
  //
  
  lock_state();
  memcpy(p, &phiState.sensors, sizeof(*p));
  unlock_state();
}

void lock_state() {
  lock_state();
}

void unlock_state() {
  unlock_state();
}


/* TMP JUNK SAVE

-----

  usec_loopEnd = phi_upTime();
  
  while (true) {

    // time at start of loop
    usec_loopStart = usec_loopEnd;

    bla bla
  }

-----  

  UINT64 usec_loopStart;
  UINT64 usec_workEnd;
  UINT64 usec_loopEnd;
  INT32 usec_error = 0;


-----

    //
    // Adjust loop sleep
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

    // DEBUG - uncomment to disable loop adaptation
    // usec_error = 0;
*/ 
