
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
  char sign     [NUM_SIGN_BYTES];
  INT8 motors   [CMDP_MOTOR_ELEM];
} PHI_CMD_PACKET;

#define STAP_SIGN               "SPV1"
#define STAP_NUM_IMAGE_BYTES    CAM_NUM_BYTES
#define STAP_NUM_JOINT_ELEM     NUM_MOTORS
#define STAP_NUM_GYRO_ELEM      3
#define STAP_NUM_ACCEL_ELEM     3

typedef struct __attribute__ ((__packed__)) {
  char   sign   [NUM_SIGN_BYTES];
  UINT32 packetID;
  BYTE   image  [STAP_NUM_IMAGE_BYTES];
  float  joints [STAP_NUM_JOINT_ELEM];
  float  gyro   [STAP_NUM_GYRO_ELEM];
  float  accel  [STAP_NUM_ACCEL_ELEM];
  BYTE   temp;
  
} PHI_STATE_PACKET;


    
