//
// Hardware abstraction layer
//

typedef void  (*halFunc_void)  ();
typedef int   (*halFunc_int)   ();
typedef BOOL  (*halFunc_BOOL)  ();
typedef INT8  (*halFunc_INT8)  ();
typedef char* (*halFunc_pChar) ();

typedef struct {

  // name
  const char* pName;

  // init
  halFunc_pChar initPeripherals;

  // gyroscope
  halFunc_BOOL gyroInit;
  halFunc_void gyroGetDeltas;
  halFunc_INT8 gyroGetTemp;

  // motor control
  halFunc_void setMotorPower;

} HAL_FUNCS;

// init HAL
void HAL_init();

//
// HAL - Hardware Abstraction Layer
//

#define HAL_name              (g_pHal -> pName)
#define HAL_initPeripherals   (*(g_pHal -> initPeripherals))
#define HAL_gyroInit          (*(g_pHal -> gyroInit))
#define HAL_gyroGetDeltas     (*(g_pHal -> gyroGetDeltas))
#define HAL_gyroGetTemp       (*(g_pHal -> gyroGetTemp))
#define HAL_setMotorPower     (*(g_pHal -> setMotorPower))

// PHI HAL

char* PHI_initPeripherals();
BOOL  PHI_gyroInit(BOOL bEnableFifo);
void  PHI_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta);
INT8  PHI_gyroGetTemp();
void  PHI_setMotorPower(int motorIdx, BYTE power, BOOL bFwd);

// generic HAL

char* GENERIC_initPeripherals();
BOOL  GENERIC_gyroInit(BOOL bEnableFifo);
void  GENERIC_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta);
INT8  GENERIC_gyroGetTemp();
void  GENERIC_setMotorPower(int motorIdx, BYTE power, BOOL bFwd);