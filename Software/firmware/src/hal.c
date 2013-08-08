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
  .initPeripherals =  (halFunc_pChar) PHI_initPeripherals,
  .gyroInit =         (halFunc_BOOL) PHI_gyroInit,
  .gyroGetDeltas =    PHI_gyroGetDeltas,
  .gyroGetTemp =      PHI_gyroGetTemp,
  .setMotorPower =    (halFunc_void) PHI_setMotorPower,
};

//
// HAL for generic Linux
//

HAL_FUNCS genericHal = {
  .pName =            "Generic HAL",
  .initPeripherals =  (halFunc_pChar) GENERIC_initPeripherals,
  .gyroInit =         (halFunc_BOOL) GENERIC_gyroInit,
  .gyroGetDeltas =    GENERIC_gyroGetDeltas,
  .gyroGetTemp =      GENERIC_gyroGetTemp,
  .setMotorPower =    (halFunc_void) GENERIC_setMotorPower,
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
  LOG_INFO("Loading HAL: \"%s\"\n",  HAL_name);
}


//
// PHI HAL
//

char* PHI_initPeripherals() {

  // NULL means success
  char *rc = NULL;
  g_initPeriph = FALSE;

  // set up UART for communication with motor controllers
  if (!uartInit()) {
    rc = "phi_InitPeripherals: UART init failed. Are you running on PHI?";
    goto quick_exit;
  }

  // set up SPI for communication with gyroscope
  if (!spiInit()) {
    rc = "phi_InitPeripherals: SPI init failed";
    goto quick_exit;
  }

  // set up gyroscope with FIFO
  if (!HAL_gyroInit(TRUE)) {
    rc = "phi_InitPeripherals: gyroscope init failed";
    goto quick_exit;
  }

  // success
  g_initPeriph = TRUE;

quick_exit:

  return rc;
}

//
// GENERIC HAL
//

char* GENERIC_initPeripherals() {
  // NULL means success
  g_initPeriph = TRUE;
  return NULL;
}

BOOL  GENERIC_gyroInit(BOOL bEnableFifo) {
  return TRUE;
}

#define GYRO_UPDATE_EVERY 0.1

void  GENERIC_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta) {

  static double lastSecs = 0;
  double currSecs = ((double) phi_upTime()) / 1e6;
  double elapsed = currSecs - lastSecs;

  if ( elapsed < GYRO_UPDATE_EVERY) {
    *pPitchDelta = 0;
    *pYawDelta = 0;
    *pRollDelta = 0;
    
  } else {
    
    *pPitchDelta = 100 * elapsed;    // 100 deg/s
    *pYawDelta   =  50 * elapsed;    // 50 deg/s
    *pRollDelta  = -50 * elapsed;    // -50 deg/s
    
    lastSecs = currSecs;
  }
}

INT8 GENERIC_gyroGetTemp(){
  double currSecs = ((double) phi_upTime()) / 1e6;
  return (INT8) ( ((long) currSecs) % 100 );
}  

void  GENERIC_setMotorPower(int motorIdx, BYTE power, BOOL bFwd) {
  // do nothing for now
}
