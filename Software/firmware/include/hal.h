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
  halFunc_void gyroGetData;
  halFunc_INT8 gyroGetTemp;

  // motor control
  halFunc_void setMotorPower;

} HAL_FUNCS;

// init
void HAL_init();

//
// HAL - Hardware Abstraction Layer
//

#define HAL_name              (g_pHal -> pName)
#define HAL_initPeripherals   (*(g_pHal -> initPeripherals))
#define HAL_gyroInit          (*(g_pHal -> gyroInit))
#define HAL_gyroGetData       (*(g_pHal -> gyroGetData))
#define HAL_gyroGetTemp       (*(g_pHal -> gyroGetTemp))
#define HAL_setMotorPower     (*(g_pHal -> setMotorPower))

// PHI HAL

char* PHI_initPeripherals();
BOOL  PHI_gyroInit();
void  PHI_gyroGetData(float* pPitchDps, float* pYawDps, float* pRollDps);
INT8  PHI_gyroGetTemp();
void  PHI_setMotorPower(int motorIdx, BYTE power, BOOL bFwd);

// generic HAL

char* GENERIC_initPeripherals();
BOOL  GENERIC_gyroInit();
void  GENERIC_gyroGetData(float* pPitchDps, float* pYawDps, float* pRollDps);
INT8  GENERIC_gyroGetTemp();
void  GENERIC_setMotorPower(int motorIdx, BYTE power, BOOL bFwd);
