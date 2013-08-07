//
// Hardware abstraction layer
//
// Note: This is used so firmware can be tested without the
//       PHI hardware

#include <phi.h>

//
// HAL for PHI
//

HACK("Hacky cast below ... don't know why we need this ...")

HAL_FUNCS phiHal = {
  .pName =            "Phi HAL",
  .initPeripherals =  PHI_initPeripherals,
  .gyroInit =         PHI_gyroInit,
  .gyroGetData =      PHI_gyroGetData,
  .gyroGetTemp =      PHI_gyroGetTemp,
  .setMotorPower =    (halFunc_void) PHI_setMotorPower,
};

//
// HAL for generic Linux
//

HAL_FUNCS genericHal = {
  .pName =            "Generic HAL",
  .initPeripherals =  GENERIC_initPeripherals,
  .gyroInit =         GENERIC_gyroInit,
  .gyroGetData =      GENERIC_gyroGetData,
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

  // set up gyroscope
  if (!HAL_gyroInit()) {
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

BOOL  GENERIC_gyroInit() {
  return TRUE;
}

void  GENERIC_gyroGetData(float* pPitchDps, float* pYawDps, float* pRollDps) {
  static double lastSecs = 0;
  double currSecs = ((double) phi_upTime()) / 1e6;

  if (currSecs - lastSecs < 0.1) {
    *pPitchDps = 0;
    *pYawDps = 0;
    *pRollDps = 0;
    
  } else {
    *pPitchDps = 10;
    *pYawDps = 5;
    *pRollDps = -10;
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
