
//
// Hardware pump threads
//
// Create separate threads responsible for:
//
// 1) Pumping data from all devices and updating a snapshot of the current state.
// 2) Heartbeat on status LED
// 3) Failsafe checking for conditions that might damage the robot
// 

#include "phi.h"

//
// DEFINES
//

//
// VARS
//

// snapshot of phi (cmds and sensor states)

typedef struct {

  PHI_CMD_PACKET cmds;
  PHI_STATE_PACKET state;

} PHI_SNAPSHOT;

static PHI_SNAPSHOT phiSnapshot;

// mutex to sync access to phiSnapshot
static PHI_MUTEX_DECL_INIT(mtxSnapshot);

// event gates
static PHI_EVENT_GATE egMotorWrite;;

// debug count of egMotorWrite timeouts
static DWORD numMotorTimeouts = 0;

//
// PROTOS
//

void initSnapshot();

void* hwPump_UART_thread(void* arg);
void* hwPump_SPI_thread(void* arg);
void* hwPump_I2C_thread(void* arg);

//
// CODE
//

/*

  The main purpose of startHwPump is to start helper threads to continually and efficiently keep synchronized the true
  hardware state and its snapshot in phiSnapshot.  This involves continually polling the hardware and updating phiSnapshot.

  To be efficient, we must take into account that:

  1) We have three different interfaces to poll so they can be done in parallel (UART, SPI and I2C). For simplicity,
     we start a separate "pump" thread for each interface.
  
  2) Access to phiSnapshot must also be synchronized between threads.
  
*/

void startHwPump() {

  // DEBUG
  //
  // WARN("pump disabled");
  // printf("*** Hw pump disabled ***\n");
  // return;

  // init phi snapshot
  initSnapshot();

  // init the event gate that synchronizes writing out to the motors (init to CLOSED)
  eventGate_init(&egMotorWrite, FALSE);
  
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

    // increase priority to make pumps more even
// Note: not sure if this is a good idea
//
//    if (setRealtimePrio(thread) == FALSE) {
//      // not fatal
//      LOG_ERR("pump set_realtime_priority() failed!");
//    }
    
  } // for
  
}

/////////////////////////////////////////////////
//
// UART pump thread
//
// Devices serviced by UART:
//
//     motor controllers (TX only)
//
/////////////////////////////////////////////////

static UINT64 UART_loopStart_save = 0;

void* hwPump_UART_thread(void* arg)
{
  LOG_INFO("hwPump_UART_thread started");
  printf("UART pump thread started\n");

  // update motor controllers from snapshot whenever we
  // are told to do it by egMotorWrite being pulsed.
  //
  // note: if the event gate times out, the motors are set to coast
  //       to prevent damage.
  //

  // time at start of loop
  UINT64 usec_loopStart = phiUpTime();

  while (TRUE) {

    int ctrlID;
    int selIdx;
    int motorIdx = 0;

    //
    // Send motor power commands to all controllers
    //

    // go through each controller
    for (ctrlID = 0 ; ctrlID <= 5 ; ctrlID ++) {
      // go through each motor of this controller
      for (selIdx = 0 ; selIdx <= 1 ; selIdx ++) {
        // send out motor power command for this motor

        lock_snapshot();
        INT8 power = phiSnapshot.cmds.motors[motorIdx++];
        unlock_snapshot();
        
        BYTE absPower = abs(power);
        BOOL bFwd = ( power >= ((INT8)0) ) ? TRUE : FALSE;
        HAL_setMotorPower(ctrlID, selIdx, absPower, bFwd);
      }
    }

    // time at end of work
    UINT64 usec_workEnd = phiUpTime();
    INT32 usec_workTime = (INT32) (usec_workEnd - usec_loopStart);

    //
    // Wait on event gate to be told data has been updated
    //
    // The purpose of the event gate is to allow commands to be written to the
    // UARTs as quickly as possible after they are received.  It is basically an
    // interruptible sleep. We wait on the gate but timeout if it takes too long.
    // When a timeout occurs, it means we have not received commands from the
    // server for a while and something is seriously wrong.  In this case, we
    // stop all motors for safety.

    #define MOTOR_TIMEOUT_MS   500      // 1/2 second

    int waitRc = eventGate_wait(&egMotorWrite, MOTOR_TIMEOUT_MS);

    if (waitRc == ETIMEDOUT) {

      // gate timed out - only bad if link connected

      if (g_phiLinkStatus == LINK_CONNECTED) {

        // link is connected - stop all motors
        stopAllMotors();

        // count it
        numMotorTimeouts ++;

        // DEBUG
        LOG_INFO("eventGate timeout in UART pump - count=%lu", numMotorTimeouts);
      }
    }
        
    // new loop start
    usec_loopStart = phiUpTime();

    // log loop period
    
    if (UART_loopStart_save != 0) {
      dlog_addElem(g_pDlog_hwPump_UART_period, usec_loopStart-UART_loopStart_save);
    }
    
    UART_loopStart_save = usec_loopStart;

    // log loop work time
    dlog_addElem(g_pDlog_hwPump_UART_workTime, usec_workTime);
    
    
  } // while
  
} // hwPump_UART_thread

/////////////////////////////////////////////////
//
// SPI pump thread
//
// Devices serviced by SPI:
//
//     Gyro (SPI index 0)
//     ADCs (SPI index 1)
//
/////////////////////////////////////////////////

static UINT64 SPI_loopStart_save = 0;
 
void* hwPump_SPI_thread(void* arg)
{
  LOG_INFO("hwPump_SPI_thread started");
  printf("SPI pump thread started\n");

  //
  // Continually read all SPI devices
  //

  // time at start of loop
  UINT64 usec_loopStart = phiUpTime();

  while (TRUE) {

    //
    // Read joint positions from ADCs
    //

    int jointIdx;

    for (jointIdx = 0 ; jointIdx < COUNTOF(phiSnapshot.state.joints) ; jointIdx++) {
      float pos = HAL_getJointPos(jointIdx);
      lock_snapshot();
      phiSnapshot.state.joints[jointIdx] = pos;
      unlock_snapshot();
    }

    //
    // Read gyro (delta degrees)
    //
    // note: we accumulate the change in the snapshot because this loop
    // runs at a different rate than the transmission

    float pitchDelta, yawDelta, rollDelta;
    HAL_gyroGetDeltas(&pitchDelta, &yawDelta, &rollDelta);

    lock_snapshot();

    phiSnapshot.state.gyro[0] += pitchDelta;
    phiSnapshot.state.gyro[1] += yawDelta;
    phiSnapshot.state.gyro[2] += rollDelta;

    unlock_snapshot();

    // read temperature from gyro
    
    float t = HAL_gyroGetTemp();
    lock_snapshot();
    phiSnapshot.state.temp = t;
    unlock_snapshot();

    //
    // Sleep enough to update at desired speed
    //

    // time at end of work
    UINT64 usec_workEnd = phiUpTime();

    // sleep any leftover time

    INT32 usec_workTime = (INT32) (usec_workEnd - usec_loopStart);
    INT32 usec_sleepTime = (INT32) (HW_PUMP_LOOP_PERIOD_USEC - usec_workTime);

    if (usec_sleepTime < 0) {
      // no time left over
      usec_sleepTime = 0;
    }

    // always sleep (even if zero) so we at least give up our time slice
    usleep(usec_sleepTime);

    // new loop start
    usec_loopStart = phiUpTime();

    // log loop period

    if (SPI_loopStart_save != 0) {
      dlog_addElem(g_pDlog_hwPump_SPI_period, usec_loopStart-SPI_loopStart_save);
    }

    SPI_loopStart_save = usec_loopStart;

    // log loop work time
    dlog_addElem(g_pDlog_hwPump_SPI_workTime, usec_workTime);
    
  } // while
  
} // hwPump_SPI_thread

/////////////////////////////////////////////////
//
// I2C pump thread
//
// Devices serviced by I2C:
//
//     Accel (addr ???)
//
/////////////////////////////////////////////////

static UINT64 I2C_loopStart_save = 0;

void* hwPump_I2C_thread(void* arg)
{
  LOG_INFO("hwPump_I2C_thread started");
  printf("I2C pump thread started\n");

  //
  // Continually read all I2C devices
  //

  // time at start of loop
  UINT64 usec_loopStart = phiUpTime();

  while (TRUE) {

    // READ I2C DEVICES HERE - TODO - HACK

    //
    // Sleep enough to update at desired speed
    //

    // time at end of work
    UINT64 usec_workEnd = phiUpTime();

    // sleep any leftover time

    INT32 usec_workTime = (INT32) (usec_workEnd - usec_loopStart);
    INT32 usec_sleepTime = (INT32) (HW_PUMP_LOOP_PERIOD_USEC - usec_workTime);

    if (usec_sleepTime < 0) {
      // no time left over
      usec_sleepTime = 0;
    }

    // always sleep (even if zero) so we at least give up our time slice
    usleep(usec_sleepTime);

    // new loop start
    usec_loopStart = phiUpTime();

    // log loop period

    if (I2C_loopStart_save != 0) {
      dlog_addElem(g_pDlog_hwPump_I2C_period, usec_loopStart-I2C_loopStart_save);
    }

    I2C_loopStart_save = usec_loopStart;

    // log loop work time
    dlog_addElem(g_pDlog_hwPump_I2C_workTime, usec_workTime);

  } // while

} // hwPump_I2C_thread

//
// PHI snapshot functions
//

void lock_snapshot() {
  PHI_MUTEX_GET(&mtxSnapshot);
}

void unlock_snapshot() {
  PHI_MUTEX_RELEASE(&mtxSnapshot);
}

// set the snapshot to its initial state

void initSnapshot() {

  int i;

  // grab mutex
  lock_snapshot();

  //
  // Commands
  //

  // sign
  memcpy(phiSnapshot.cmds.sign, CMDP_SIGN, sizeof(phiSnapshot.cmds.sign));
  
  // motor commands
  for (i = 0 ; i < COUNTOF(phiSnapshot.cmds.motors) ; i++) {
    phiSnapshot.cmds.motors[i] = 0;
  }

  //
  // State
  //
  
  // sign
  memcpy(phiSnapshot.state.sign, STAP_SIGN, sizeof(phiSnapshot.state.sign));

  // packet ID
  phiSnapshot.state.packetID = 0;

  // image
  memset(phiSnapshot.state.image, 0, sizeof(phiSnapshot.state.image));

  // joint (motor) positions
  for (i = 0 ; i < COUNTOF(phiSnapshot.state.joints) ; i++) {
    phiSnapshot.state.joints[i] = 0;
  }

  // gyro
  phiSnapshot.state.gyro[0] = 0;
  phiSnapshot.state.gyro[1] = 0;
  phiSnapshot.state.gyro[2] = 0;

  // accel
  phiSnapshot.state.accel[0] = 0;
  phiSnapshot.state.accel[1] = 0;
  phiSnapshot.state.accel[2] = 0;

  // temp
  phiSnapshot.state.temp = 0;

  // free state mutex
  unlock_snapshot();
}

// get state portion of PHI snapshot

void getStateSnapshot(PHI_STATE_PACKET* p) {

  // lock state updates
  lock_snapshot();

  // increment packet ID
  phiSnapshot.state.packetID ++;

  // copy state back to caller
  memcpy(p, &phiSnapshot.state, sizeof(*p));

  // unlock state updates
  unlock_snapshot();
}

// write to cmd portion of the PHI snapshot

void writeToCmdSnapshot(PHI_CMD_PACKET* p) {

  // lock snapshot updates
  lock_snapshot();

  // write to cmd snapshot
  memcpy(&phiSnapshot.cmds, p, sizeof(*p));

  // unlock state updates
  unlock_snapshot();

  // pulse the event gate to signal new data
  eventGate_pulse(&egMotorWrite);
}

void setSnapshotMotorVal(char* motorName, INT8 powerVal) {
  
  // lock snapshot updates
  lock_snapshot();
  
  phiSnapshot.cmds.motors[
    MOTOR_ID_TO_ARRAY_IDX(
      MOTOR_NAME_TO_CTRL_ID(motorName),
      MOTOR_NAME_TO_SEL_IDX(motorName))] = powerVal;

  // unlock state updates
  unlock_snapshot();

  // pulse the event gate to signal new data
  eventGate_pulse(&egMotorWrite);
}

/*
 * TMP JUNK SAVE
 *

-----

// read hardware and update the state structure
//
// note: we lock the state structure in a fine grained
// fashion so that PHI can reply quickly to state requests
// without being blocked here
//

void updateState() {

  int i;

  // sign not set here because it is not necessary and serves as a sentinel
  // to detect a bug that might over-write it

  // packet ID is not incremented here because this routine runs
  // at a higher rate than packets are sent

  // image (not implemented)
  TODO("image sensor not implemented");

  //
  // Get joint positions
  //

  char ctrlID;
  char selIdx;
  int jointIdx = 0;

  // go through each controller
  for (ctrlID = 'A' ; ctrlID <= 'F' ; ctrlID ++) {
    // go through each motor of this controller
    for (selIdx = '0' ; selIdx <= '1' ; selIdx ++) {
      // read and save joing position
      double pos = getJointPos(ctrlID, selIdx);
      lock_snapshot();
      phiSnapshot.state.joint[jointIdx++] = 0;
      unlock_snapshot();
    }
  }

  // gyro (return +/- percent of max reading)
  //
  // note: we actually accumulate the change in the state
  // because this runs at a faster rate than the transmission
  // so data would be lost

  float pitchDelta, yawDelta, rollDelta;
  HAL_gyroGetDeltas(&pitchDelta, &yawDelta, &rollDelta);

  lock_snapshot();

  phiSnapshot.state.gyro[0] += pitchDelta;
  phiSnapshot.state.gyro[1] += yawDelta;
  phiSnapshot.state.gyro[2] += rollDelta;

  unlock_snapshot();

  // temperature
  float t = HAL_gyroGetTemp();
  lock_snapshot();
  phiSnapshot.state.temp = t;
  unlock_snapshot();
}

-----

  usec_loopEnd = phiUpTime();
  
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
    usec_workEnd = phiUpTime();

    // sleep any leftover time

    INT32 usec_workTime = (INT32) (usec_workEnd - usec_loopStart);
    INT32 usec_sleepTime = (INT32) (HW_PUMP_LOOP_PERIOD_USEC - usec_workTime + usec_error);

    if (usec_sleepTime < 0) {
      // no time left over
      usec_sleepTime = 0;
    }

    usleep(usec_sleepTime);
    usec_loopEnd = phiUpTime();

    // compute error in loop time
    //
    // NOTE: this is disabled because it doesn't improve things much
    // but it DOES increase the standard deviation
    //
    // usec_error += (INT32) (HW_PUMP_LOOP_PERIOD_USEC - (usec_loopEnd - usec_loopStart));

    // DEBUG
    // printf("sleep time = %ld uS, error = %ld uS\n", usec_sleepTime, usec_error);
    // sleep(1);
    // usec_loopEnd = phiUpTime();

    // DEBUG - uncomment to disable loop adaptation
    // usec_error = 0;
*/ 
