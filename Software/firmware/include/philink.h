
//
// PhiLink defs
//

typedef enum {
    LINK_OFF,
    LINK_STARTED,
    LINK_CONNECTING,
    LINK_CONNECTED,
    LINK_ERROR,
    LINK_CLOSED

} PHILINK_STATUS;

typedef struct {
  int sock;
  in_addr_t ipAddr;
  uint16_t port;

} PHILINK_ARGS;

//
// Packets
//

#define NUM_SIGN_BYTES          4
#define CMDP_SIGN               "CPV1"

#define CMDP_MOTOR_ELEM         NUM_MOTORS

typedef struct  __attribute__ ((__packed__)) {
  
  char      sign     [NUM_SIGN_BYTES];
  UINT32    packetID;

  // values = [-127, 127] - couldn't decide if should make canonical [0,1]
  // prob should be canon but with separate fwd and back so:
  //    0,0 = coast
  //    x,x = brake
  //    x,y = power at x-y
  //
  
  INT8      motors   [CMDP_MOTOR_ELEM];
  
} PHI_CMD_PACKET;

#define STAP_SIGN               "SPV1"
#define STAP_NUM_IMAGE_BYTES    CAM_NUM_BYTES
#define STAP_NUM_JOINT_ELEM     NUM_MOTORS
#define STAP_NUM_GYRO_ELEM      3
#define STAP_NUM_ACCEL_ELEM     3

TODO("think about GYRO AND ACCEL ELEM");
// - should be split into pos and neg?
// - should return change
// - is gyro returning change? if so, change name

typedef struct __attribute__ ((__packed__)) {
  
  char      sign   [NUM_SIGN_BYTES];
  UINT32    packetID;
  BYTE      image  [STAP_NUM_IMAGE_BYTES];

  // everything below here is stored as a float
  // and encoded into the canonical range [0,1]
  
  float     joints [STAP_NUM_JOINT_ELEM];
  float     gyro   [STAP_NUM_GYRO_ELEM];
  float     accel  [STAP_NUM_ACCEL_ELEM];
  float     temp;
  
} PHI_STATE_PACKET;


    
