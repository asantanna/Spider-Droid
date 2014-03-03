
// STUB HAL

char*   HAL_STUB_initPeripherals();

void    HAL_STUB_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta);
float   HAL_STUB_gyroGetTemp();

void    HAL_STUB_setMotorPower(BYTE ctrlID, BYTE selIdx, BYTE power, BOOL bFwd);
void    HAL_STUB_setControllerId(char oldId, char newId);

float   HAL_STUB_getJointPos(BYTE adcIdx);
