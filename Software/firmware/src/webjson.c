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
#include "jsmh.h"

// max number of JSON tokens we support (should be way plenty)
#define MAX_JSON_TOKENS     100

void processJson(char *pJson) {

  // init JSON parser
  jsmntok_t tokens[MAX_JSON_TOKENS];
  struct jsmn_parser parser;
  jsmn_init_parser(&parser);

  // we only support 

  int jsonRc = jsmn_parse(&parser, js, tokens, 256);

  if (jsonRc != 0) {
    // error parsing
    LOG_ERR("error parsing JSON request - rc=%d", jsonRc);
    goto quick_exit;
  }

/*
  If something goes wrong, you will get an error returned by jsmn_parse(). Return value will be one of these:
      •JSMN_SUCCESS - everything went fine. String was parsed
      •JSMN_ERROR_INVAL - bad token, JSON string is corrupted
      •JSMN_ERROR_NOMEM - not enough tokens, JSON string is too large
      •JSMN_ERROR_PART - JSON string is too short, expecting more JSON data
*/  
  
}

//
// Command group: motorCtrl
//
//   motorCtrl = {
//     motorId = 3 "ppj"        // motor identifier
//   }
//                               
// where:
//
// motorId: 3 char code "ppj" with
//
//   pp = "rf"   right forward leg
//      = "lf"   left forward leg
//      = "rr"   right rear leg
//      = "lr"   left rear leg
//
//    j = "r"    hip rotator joint
//      = "t"    thigh joint
//      = "k"    knee joint
//     
