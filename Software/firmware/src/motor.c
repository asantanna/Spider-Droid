
//
// Interface to spiderBot's motors
//

#include "phi.h"

MOTOR_DEF* attachMotor(BYTE motorID) {

  MOTOR_DEF* p = ALLOC(MOTOR_DEF);

  // init
  p -> motorID = motorID;

  // done
  return p;
}

