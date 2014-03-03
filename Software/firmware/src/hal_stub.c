
//
// STUB HAL
//

#include <phi.h>


char* HAL_STUB_initPeripherals() {
  // NULL means success
  g_initPeriph = TRUE;
  return NULL;
}

#define GYRO_UPDATE_EVERY 0.01f

void  HAL_STUB_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta) {

  static float lastSecs = 0;
  float currSecs = (float) (phiUpTime() / 1e6);
  float elapsed = currSecs - lastSecs;

  if ( elapsed < GYRO_UPDATE_EVERY) {
    *pPitchDelta = 0;
    *pYawDelta = 0;
    *pRollDelta = 0;

  } else {

    *pPitchDelta = 100.0f/360.0f * elapsed;    // 100 deg/s in canon [0,1]
    *pYawDelta   =  50.0f/360.0f * elapsed;    // 50 deg/s in canon [0,1]
    *pRollDelta  = -50.0f/360.0f * elapsed;    // -50 deg/s in canon [0,1]

    lastSecs = currSecs;
  }
}

float HAL_STUB_gyroGetTemp(){
  float currSecs = ((float) phiUpTime()) / 1e6f;
  float canonDeg = (currSecs / 100);
  canonDeg -= (int) canonDeg;
  return canonDeg;
}  

void HAL_STUB_setMotorPower(BYTE ctrlID, BYTE selIdx, BYTE power, BOOL bFwd) {
  // do nothing for now
}

//
// Generic getRawJointPos() - return fake values that advance with time
//
// note: ADC value is canon [0,1]
//

float HAL_STUB_getJointPos(BYTE adcIdx) {
  float currSecs = (phiUpTime() / 1e6);
  
  // advance 10 deg/second (in canon [0,1])
  float deg = currSecs * 10.0f/360.0f;
  
  // each index 20 deg ahead of prev
  deg += adcIdx * 20.0f/360.0f;
  
  // keep only fractional (canonical) part
  deg -= (int) deg;
  
  return deg;
}

void HAL_STUB_setControllerId(char oldId, char newId) {
}
