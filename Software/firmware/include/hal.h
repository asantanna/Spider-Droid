//
// Hardware abstraction layer
//

typedef void   (*halFunc_void)   ();
typedef int    (*halFunc_int)    ();
typedef char*  (*halFunc_pChar)  ();

typedef BOOL   (*halFunc_BOOL)   ();
typedef INT8   (*halFunc_INT8)   ();
typedef UINT8  (*halFunc_UINT8)  ();
typedef UINT16 (*halFunc_UINT16) ();

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
  halFunc_UINT16 getMotorPosition;
  halFunc_void   setMotorPower;
  halFunc_void   setControllerId;

} HAL_FUNCS;

// init HAL
void HAL_init();

//
// HAL - Hardware Abstraction Layer
//

#define HAL_name              (g_pHal   -> pName)
#define HAL_initPeripherals   (*(g_pHal -> initPeripherals))

#define HAL_gyroInit          (*(g_pHal -> gyroInit))
#define HAL_gyroGetDeltas     (*(g_pHal -> gyroGetDeltas))
#define HAL_gyroGetTemp       (*(g_pHal -> gyroGetTemp))

#define HAL_getMotorPosition  (*(g_pHal -> getMotorPosition))
#define HAL_setMotorPower     (*(g_pHal -> setMotorPower))
#define HAL_setControllerId   (*(g_pHal -> setControllerId))

// PHI HAL

char*   PHI_initPeripherals();
BOOL    PHI_gyroInit(BOOL bEnableFifo);
void    PHI_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta);
INT8    PHI_gyroGetTemp();
UINT16  PHI_getMotorPosition(int motorIdx);
void    PHI_setMotorPower(int motorIdx, BYTE power, BOOL bFwd);
void    PHI_setControllerId(int oldId, int newId);

// generic HAL

char*   GENERIC_initPeripherals();
BOOL    GENERIC_gyroInit(BOOL bEnableFifo);
void    GENERIC_gyroGetDeltas(float* pPitchDelta, float* pYawDelta, float* pRollDelta);
INT8    GENERIC_gyroGetTemp();
UINT16  GENERIC_getMotorPosition(int motorIdx);
void    GENERIC_setMotorPower(int motorIdx, BYTE power, BOOL bFwd);
void    GENERIC_setControllerId(int oldId, int newId);
