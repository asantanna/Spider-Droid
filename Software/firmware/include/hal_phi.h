
// PHI HAL

char*   HAL_PHI_initPeripherals();

void    HAL_PHI_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta);
float   HAL_PHI_gyroGetTemp();

void    HAL_PHI_setMotorPower(BYTE ctrlID, BYTE selIdx, BYTE power, BOOL bFwd);
void    HAL_PHI_setControllerId(BYTE oldId, BYTE newId);

float   HAL_PHI_getJointPos(BYTE jointIdx);
