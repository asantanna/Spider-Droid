//
// Routines to self test PHI hardware
//

//
// TEST JOINTS
//
// 1) check that each joint motor can move
// 2) find max and min joint angles
//
// Note: motor power is [0,127] plus a direction flag
//

#include <phi.h>


#define JTEST_MIN_POWER         10
#define JTEST_MAX_POWER         60
#define JTEST_POWER_STEPS       10

#define JTEST_WAIT_STEP_MS      10
#define JTEST_WAIT_MAX_MULT     25

#define JTEST_JOINT_CHG_DEG     5

//
// STATICS
//

BYTE minPowerNeeded = 0;
INT32 minWaitNeeded = 0;

//
// PROTOS
//

void testAllJoints();
BOOL testJointMovement(BYTE ctrlID, BYTE selIdx);
BOOL testJointRange(BYTE ctrlID, BYTE selIdx);
double moveJointToStop(BYTE ctrlID, BYTE selIdx, BOOL bFwd);

//
// CODE
//

// perform full hardware self test

char* selfTest(int mode) {

  // test all joints
  testAllJoints();

  // return verbose result
  return "PASS ... :-)";
}

// test all joints (movement and range)

void testAllJoints() {

  char ctrlID;
  char selIdx;

  //
  // Test that all joints can move
  //

  // stop all motors
  stopAllMotors();

  // wait for transmission to complete
  flushMotorCmds();

  // go through each controller
  for (ctrlID = 'A' ; ctrlID <= 'F' ; ctrlID ++) {
    // go through each motor of this controller
    for (selIdx = '0' ; selIdx <= '1' ; selIdx ++) {
      // test this joint
      if (testJointMovement(ctrlID, selIdx) == FALSE) {
        LOG_FATAL("testJointMovement() for motor %c%c failed", ctrlID, selIdx);
      }
    }
  }

  //
  // Determine range of all joints
  //

  // stop all motors (should be stopped already)
  stopAllMotors();

  // wait for transmission to complete
  flushMotorCmds();

  // go through each controller
  for (ctrlID = 'A' ; ctrlID <= 'F' ; ctrlID ++) {
    // go through each motor of this controller
    for (selIdx = '0' ; selIdx <= '1' ; selIdx ++) {
      // test this joint
      if (testJointRange(ctrlID, selIdx) == FALSE) {
        LOG_FATAL("testJointRange() for motor %c%c failed", ctrlID, selIdx);
      }
    }
  }
}

BOOL testJointMovement(BYTE ctrlID, BYTE selIdx) {

  int waitMult;
  INT32 usecWait;
  double orig_jointPos;
  double jointPos;
  BOOL dirFwd;
  BYTE power;

  // get current joint pos
  orig_jointPos =  PHI_getJointPos(ctrlID, selIdx);

  // try ever increasing power until we detect movement
  for (power = JTEST_MIN_POWER ; power <= JTEST_MAX_POWER ; power += JTEST_POWER_STEPS) {
    // alternate directions in case we are at max range
    for (dirFwd = FALSE ; dirFwd <= TRUE ; dirFwd++) {
      // try current direction with this power
      PHI_setMotorPower(ctrlID, selIdx, power, dirFwd);
      for (waitMult = 1 ; waitMult <= 25 ; waitMult ++) {
        // sleep a bit
        usecWait = waitMult * JTEST_WAIT_STEP_MS * 1000;
        usleep(usecWait);
        // get joint pos
        jointPos =  PHI_getJointPos(ctrlID, selIdx);
        // check if moved enough
        double diff = abs(jointPos - orig_jointPos);
        if (diff >= JTEST_JOINT_CHG_DEG) {
          // joint moved enough
          goto ok_exit;
        }
      }
    }
  }

  // ERROR - joint didn't move
  
  stopMotor(ctrlID, selIdx);
  
  LOG_INFO("Joint %c%c movement test FAILED", ctrlID, selIdx);
  
  return FALSE;

ok_exit:
  
  // SUCCESS - joint moved

  stopMotor(ctrlID, selIdx);
  
  LOG_INFO("Joint %c%c movement test SUCCESS - power=%d, dir=%s, wait=%d ",
    ctrlID, selIdx, power, dirFwd ? "fwd" : "rev", usecWait);

  // accumulate min wait/power needed
  minPowerNeeded = MAX(minPowerNeeded, power);
  minWaitNeeded  = MAX(minWaitNeeded,  usecWait);

  return TRUE;
}

BOOL testJointRange(BYTE ctrlID, BYTE selIdx) {
  
  double maxFwdPos;
  double maxRevPos;

  // find max forward pos
  maxFwdPos = moveJointToStop(ctrlID, selIdx, TRUE);
  
  // find max rev pos
  maxRevPos = moveJointToStop(ctrlID, selIdx, FALSE);

  LOG_INFO("Joint %c%c range: min=%g, max=%d",
    ctrlID, selIdx, maxRevPos, maxFwdPos);

  WARN("Save this range info somewhere");
  WARN("Use range info to prevent joint damage");

  // success
  return TRUE;
}

double moveJointToStop(BYTE ctrlID, BYTE selIdx, BOOL bFwd) {

  double prevJointPos;
  double jointPos;

  // get current joint pos
  prevJointPos =  PHI_getJointPos(ctrlID, selIdx);

  // turn on motor at minimum necessary power for movement
  PHI_setMotorPower(ctrlID, selIdx, minPowerNeeded, bFwd);

  while(TRUE) {
    
    // wait
    usleep(minWaitNeeded);
    
    // check if moved
    jointPos =  PHI_getJointPos(ctrlID, selIdx);

    if (bFwd) {
      // fwd
      if (jointPos <= prevJointPos) {
        // did not move forward - end of range
        break;
      }
    } else {
      // rev
      if (jointPos >= prevJointPos) {
        // did not move rev - end of range
        break;
      }
    }
    
    // moved - remember position and wait again
    prevJointPos = jointPos;
  }

  // return stop position
  return prevJointPos;
}

