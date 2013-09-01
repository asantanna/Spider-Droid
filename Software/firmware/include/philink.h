
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

} PHILINK_STATE;

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
  INT8 motor    [CMDP_MOTOR_ELEM];
} PHI_CMD_PACKET;

#define STAP_SIGN               "SPV1"
#define STAP_NUM_IMAGE_BYTES    CAM_NUM_BYTES
#define STAP_NUM_GYRO_ELEM      3
#define STAP_NUM_JOINT_ELEM     NUM_MOTORS
#define STAP_TEMP_BYTES         1

typedef struct __attribute__ ((__packed__)) {
  char   sign   [NUM_SIGN_BYTES];
  BYTE   image  [STAP_NUM_IMAGE_BYTES];
  float  gyro   [STAP_NUM_GYRO_ELEM];
  UINT16 joint  [STAP_NUM_JOINT_ELEM];
  BYTE   temp   [STAP_TEMP_BYTES];
} PHI_STATE_PACKET;


    
