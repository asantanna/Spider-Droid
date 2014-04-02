
#include "phi.h"

#define NEED_ARGS(n) if (i+(n) >= argc) { printf("not enough args for '%s'\n", p); goto quick_exit;}

void usage();

void cmdline(int argc, char** argv) {
  
  if (argc == 1) {
    // no args - just start up normally
    return;
  }

  printf("\n*** Doing immediate commands only ***\n\n");

  // scan thru command line

  int i;
  
  for (i = 1 ; i < argc ; i++) {
    
    char* p = argv[i];
    
    if (!strcmp(p, "-read-joint")) {

      // read joint ADC

      NEED_ARGS(1);
      int jointNum = atoi(argv[++i]);
      printf("%d: Read joint(%d) = %g\n", i, jointNum, (double) getJointPos(jointNum));

    } else if (!strcmp(p, "-set-motor-power")) {

      // set motor power

      NEED_ARGS(2);

      int powerVal = -1;
      BOOL bFwd = TRUE;
      char* motorName  = argv[++i];
      char* percentStr = argv[++i];
      int percent = atoi(percentStr);

      if (percent < 0) {
        bFwd = FALSE;
        percent = -percent;
      }

      powerVal = (percent * 127) / 100;

      printf("%d: Setting motor '%s' to %s%% (%s%d)\n",
        i, motorName, percentStr, bFwd ? "F":"B", powerVal);

      HAL_setMotorPower(
        MOTOR_NAME_TO_CTRL_ID(motorName),
        MOTOR_NAME_TO_SEL_IDX(motorName),
        (BYTE) powerVal, bFwd);

    } else if (!strcmp(p, "-read-gyro")) {

      // read gyroscope
      
      float pitchDelta, yawDelta, rollDelta;
      
      HAL_gyroGetDeltas(&pitchDelta, &yawDelta, &rollDelta);
      
      printf("%d: Get gyro deltas = (P%g, Y%g, R%g)\n",
        i, (double) pitchDelta, (double) yawDelta, (double) rollDelta);


    } else if (!strcmp(p, "-read-accel")) {

      // read accelerometer

    } else if (!strcmp(p, "-sleep")) {

      // sleep a bit

      NEED_ARGS(1);
      int secs = atoi(argv[++i]);
      printf("%d: Sleeping for %d secs\n", i, secs);
      sleep(secs);
      
    } else if (!strcmp(p, "-goto")) {

      // goto command (reset argument index)
      
      NEED_ARGS(1);
      int argNum = atoi(argv[++i]);
      if ((argNum < 1) || (argNum >= argc) || (*argv[argNum] != '-')) {
        printf("bad argument to -goto command = %d\n", argNum);
        goto quick_exit;
      }

      // set argument index to specified value
      
      printf("%d: Jump to argument %d\n", i, argNum-1);
      i = argNum - 1;
      continue;

    } else {

      // unknown 
      printf("%d: unknown param '%s'\n", i, p);
      usage();
      goto quick_exit;
    }
  }

  printf("\n");

quick_exit:
  
  // exit
  abortProcess(0);
}

void usage() {

  printf(

    "\n"
    "Usage:  phi [<params>]\n"
    "\n"
    "<no params>                            Startup PHI normally\n"
    "-read-joint <num>                      Read joint ADC, num = [0,11]\n"
    "-set-motor-power <motor> <pwr>         Set motor power, motor = [A-F][0-1], pwr=[-1,1]\n"
    "-read-gyro                             Read gyroscope\n"
    "-read-accel                            Read accelerometer\n"
    "-sleep <secs>                          Sleep for <secs> seconds\n"
    "-goto <num>                            Go back to processing arg <num>\n"
    "\n"
    
    );
}


