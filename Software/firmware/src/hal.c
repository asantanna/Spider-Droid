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
  .gyroGetDeltas =    PHI_gyroGetDeltas,
  .gyroGetTemp =      PHI_gyroGetTemp,
  .getRawJointPos =   (halFunc_UINT16) PHI_getRawJointPos,
  .setMotorPower =    (halFunc_void) PHI_setMotorPower,
  .setControllerId =  (halFunc_void) PHI_setControllerId,
};

//
// HAL for generic Linux
//

HAL_FUNCS genericHal = {
  .pName =            "Generic HAL",
  .initPeripherals =  (halFunc_pChar) GENERIC_initPeripherals,
  .gyroGetDeltas =    GENERIC_gyroGetDeltas,
  .gyroGetTemp =      GENERIC_gyroGetTemp,
  .getRawJointPos =   (halFunc_UINT16) GENERIC_getRawJointPos,
  .setMotorPower =    (halFunc_void) GENERIC_setMotorPower,
  .setControllerId =  (halFunc_void) GENERIC_setControllerId,
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


//
// PHI HAL
//

char* PHI_initPeripherals() {

  // NULL means success
  char *rc = NULL;
  
  g_initPeriph = FALSE;

  // set up UART for communication with motor controllers
  if (!uart_init()) {
    rc = "PHI_InitPeripherals: UART init failed. Are you running on PHI?";
    goto quick_exit;
  }

  // set up SPI for communication with gyroscope
  if (!spi_init()) {
    rc = "PHI_InitPeripherals: SPI init failed";
    goto quick_exit;
  }

  // init gyroscope (FIFO enabled)
  if (!gyroInit(TRUE)) {
    rc = "PHI_InitPeripherals: gyroscope init failed";
    goto quick_exit;
  }

  // init motor controllers
  if (!initMotorCtrl()) {
    rc = "PHI_InitPeripherals: motor ctrl init failed";
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

#define GYRO_UPDATE_EVERY 0.01

void  GENERIC_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta) {

  static double lastSecs = 0;
  double currSecs = ((double) PHI_upTime()) / 1e6;
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
  double currSecs = ((double) PHI_upTime()) / 1e6;
  return (INT8) ( ((long) currSecs) % 100 );
}  

void GENERIC_setMotorPower(BYTE ctrlID, BYTE selIdx, BYTE power, BOOL bFwd) {
  // do nothing for now
}

UINT16 GENERIC_getRawJointPos(BYTE adcIdx) {
  double currSecs = ((double) PHI_upTime()) / 1e6;
  return (UINT16) ( ((long) ((currSecs * 512) + (adcIdx*100))) % 1024 );
}

void GENERIC_setControllerId(char oldId, char newId) {
}

