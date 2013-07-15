//
// JSON interface to PHI
//
// Note: this interface is used for diagnostics only.
//       Realtime commands use the faster phiLink interface.
//

// JSON format
//
// We run parser in "non-strict" mode which means:
//
// 1) no root object required (we use one though)
// 2) quotes not required
//
// Generic request format:
//
//   cmd_group = {
//     cmd = string
//     ... params ...
//   }
//
// See specific commands below for more info.
//

#include "phi.h"
#include "jsmn.h"

// max number of JSON tokens we support (should be way plenty)
#define MAX_JSON_TOKENS     100

void phi_processJson(char *pJson, char* pReplyBuff) {
  
  int i;
  jsmntok_t tokens[MAX_JSON_TOKENS];
  jsmn_parser parser;

  // init JSON parser
  jsmn_init(&parser);

  // parse
  int rc = jsmn_parse(&parser, pJson, tokens, 256);

  if (rc != 0) {
    // error parsing
    LOG_ERR("error parsing JSON request - rc=%d", rc);
    goto quick_exit;
  }

  // parse request

  // HACK for now just dump this stupid thing so we can
  // see what it does
  
  LOG_INFO("Received JSON command:");

  for (i = 0 ; i < 5 /* COUNTOF(tokens)*/ ; i++) {
    char val[128];
    int len = tokens[i].end - tokens[i].start + 1;
    memcpy(val, pJson + tokens[i].start, len);
    LOG_INFO("  token %d: type=%d, val='%s', size=%d",
             i, tokens[i].type, val, tokens[i].size);
  }

  sprintf(pReplyBuff,
         "\"reply\" : {\n"
         "  \"group\" : \"globalData\"\n"
         "  \"cmd\" : \"getVersion\"\n"
         "  \"version\" : \"%s\"\n"
         "}", PHI_VERSION);

quick_exit:

  return;
  
}

//
// Command group: globalData
//  
//   globalData : {
//      cmd : getVersion
//   }
//
//  reply : {
//    group : motorCtrl
//    cmd : getVersion
//    status : 0K | error code
// if cmd = getVersion
//    version : string
//  }

//
// Command group: motorCtrl
//  
//   motorCtrl : {
//      cmd : setPower | setBrake
//      motorId : "ppj"
// if cmd = setPower
//      power : -100% to 100%
// if cmd = setBrake
//      val : on | off | zeroMeansBrake
//   }
//                               
// where:
//
// motorId: 3 char code "ppj" with
//
//   pp = rf      right forward leg
//      = lf      left forward leg
//      = rr      right rear leg
//      = lr      left rear leg
//
//    j = r       hip rotator joint
//      = t       thigh joint
//      = k       knee joint
//
// if setPower:
//    power = -100% to 100%   negative means backwards, meaning of 0%
//                            depends on setBreak
// if setBrake:
//
//   val = on               put motor in braking mode now
//       = off              put motor in idle mode now
//       = zeroMeansBrake   power=0% means braking mode
//       = zeroMeansCoast   power=0% means coast (default)
//
// Returns:
//
//  reply : {
//    group : motorCtrl
//    cmd : setPower
//    status : 0K | error code  
//  }
//


//
// Command group: accelCtrl
//  
//   accelCtrl : {
//      cmd : getAccel
//   }
//
// Returns:
//
//   reply : {
//      group : accelCtrl
//      cmd : getAccel
//      status : 0K | error code  
//      x : num                       accel in X dir
//      y : num                       accel in Y dir
//      z : num                       accel in Z dir
//   }
//

  


