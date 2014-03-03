//
// Hardware abstraction layer
//
// Note: This is used so firmware can be tested without the
//       PHI hardware

#include <phi.h>

//
// HAL for PHI
//

// apparently GCC doesn't like unspecified params so we need to cast
// the function pointers below

HAL_FUNCS phiHal = {
  .pName =            "Phi HAL",
  .initPeripherals =  (halFunc_pChar)     HAL_PHI_initPeripherals,
  .gyroGetDeltas =                        HAL_PHI_gyroGetDeltas,
  .gyroGetTemp =                          HAL_PHI_gyroGetTemp,
  .getJointPos =      (halFunc_float)     HAL_PHI_getJointPos,
  .setMotorPower =    (halFunc_void)      HAL_PHI_setMotorPower,
  .setControllerId =  (halFunc_void)      HAL_PHI_setControllerId,
};

//
// HAL for generic Linux
//

HAL_FUNCS genericHal = {
  .pName =            "Stub HAL",
  .initPeripherals =  (halFunc_pChar)     HAL_STUB_initPeripherals,
  .gyroGetDeltas =                        HAL_STUB_gyroGetDeltas,
  .gyroGetTemp =                          HAL_STUB_gyroGetTemp,
  .getJointPos =      (halFunc_float)     HAL_STUB_getJointPos,
  .setMotorPower =    (halFunc_void)      HAL_STUB_setMotorPower,
  .setControllerId =  (halFunc_void)      HAL_STUB_setControllerId,
};

void HAL_init() {

  BOOL isPhi = strcasestr(g_uname.nodename, "phi");
  BOOL isUbuntu = strcasestr(g_uname.nodename, "ubuntu");
  
  // determine what host we are running on and set HAL pointer
  // to correct function table

  if (isPhi && !isUbuntu) {
    // really sure this is PHI
    g_pHal = &phiHal;
    
  } else {
    // use generic
    g_pHal = &genericHal;
  }

  printf("Loading HAL: %s\n",  HAL_name);
  LOG_INFO("Loading HAL: \"%s\"",  HAL_name);
}


