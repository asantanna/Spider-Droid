
//
// PhiLink defs
//

#define CMD_PACKET_SIGN     "CP_1"
#define STATE_PACKET_SIGN   "SP_1"

typedef enum {
    LINK_OFF,
    LINK_STARTED,
    LINK_CONNECTING,
    LINK_CONNECTED,
    LINK_ERROR,
    LINK_CLOSED

} PHILINK_STATE;

typedef enum {
  RX_IDLE,
  RX_RECEIVING,
  RX_CMD_COMPLETE
    
} RX_STATE;

typedef enum {
  TX_IDLE,
  TX_BEGIN,
  TX_SENDING,
  TX_COMPLETED
    
} TX_STATE;

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
#define CMDP_MOTOR_BYTES        NUM_MOTORS

typedef struct {
  char sign   [NUM_SIGN_BYTES];
  INT8 motor  [CMDP_MOTOR_BYTES];
  
} PHI_CMD_PACKET;

#define STAP_SIGN               "SPV1"
#define STAP_IMAGE_BYTES        CAM_NUM_BYTES
#define STAP_GYRO_BYTES         3
#define STAP_JOINT_BYTES        NUM_MOTORS
#define STAP_TEMP_BYTES         1

typedef struct {
  char sign   [NUM_SIGN_BYTES];
  BYTE image  [STAP_IMAGE_BYTES];
  BYTE gyro   [STAP_GYRO_BYTES];
  BYTE joint  [STAP_JOINT_BYTES];
  BYTE temp   [STAP_TEMP_BYTES];

} PHI_STATE_PACKET;


    
