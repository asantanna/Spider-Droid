//
// Hardware abstraction layer
//

typedef void   (*halFunc_void)   ();
typedef int    (*halFunc_int)    ();
typedef float  (*halFunc_float)  ();
typedef char*  (*halFunc_pChar)  ();

typedef struct {

  // name
  const char* pName;

  // init
  halFunc_pChar   initPeripherals;

  // gyroscope
  halFunc_void    gyroGetDeltas;
  halFunc_float   gyroGetTemp;

  // motor controllers
  halFunc_void    setMotorPower;
  halFunc_void    setControllerId;

  // ADCs
  halFunc_float   getJointPos;

} HAL_FUNCS;

// init HAL
void HAL_init();

//
// HAL - Hardware Abstraction Layer
//

#define HAL_name              (g_pHal   -> pName)
#define HAL_initPeripherals   (*(g_pHal -> initPeripherals))

#define HAL_gyroGetDeltas     (*(g_pHal -> gyroGetDeltas))
#define HAL_gyroGetTemp       (*(g_pHal -> gyroGetTemp))

#define HAL_setMotorPower     (*(g_pHal -> setMotorPower))
#define HAL_setControllerId   (*(g_pHal -> setControllerId))

#define HAL_getJointPos       (*(g_pHal -> getJointPos))
