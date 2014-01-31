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
// 1) no root object required (we use one tho)
// 2) quotes not required
//
// Generic request format (quotes left out when possible):
//
//   req : [
//     {
//        cmd = string,
//        ... params ...
//     },
//     {
//        cmd = string
//        ... params ...
//     }
//   ]
//
// Generic reply format (quotes present always for easier
// javascript processing):
//
//  "reply" : [
//    {
//      "error":"string"         // only present if error occurred
//      ... cmd specific ...
//    },
//    ...
//  ]
//
//
// See specific commands below for more info.
//

#include "phi.h"
#include "jsmn.h"

// max number of JSON tokens we support (should be way plenty)
#define MAX_JSON_TOKENS       256

// JSMN (JSON parser) helpers

#define PARSER_NUM_TOK(parser)  ((parser).toknext)

#define TOK_START(pTok)       (pJsonReq + (pTok)->start)
#define TOK_LEN(pTok)         ((pTok)->end - (pTok)->start)
#define TOK_TYPE(pTok)        ((pTok)->type)
#define TOK_NUM_CHILD(pTok)   ((pTok)->size)

#define TOK_EQ(pTok,s) \
    ((int) strlen(s) == TOK_LEN(pTok)) \
    ? (strncmp(s, TOK_START(pTok), strlen(s)) == 0) \
    : FALSE

//
// VALID COMMANDS
//

typedef struct _jsonCmdReply_t {
  char* pReply;
  struct _jsonCmdReply_t* pNext;

} PHI_JSON_CMD_REPLY_TYPE;

typedef PHI_JSON_CMD_REPLY_TYPE* (*jsonCmdHandler)(jsmntok_t** ppTok, char* pJsonReq);

// Internal
jsonCmdHandler getJsonHandler(jsmntok_t** ppTok, char* pJsonReq);

// JSON handlers forward decls

#define JSON_HANDLER(h) PHI_JSON_CMD_REPLY_TYPE* json_##h (jsmntok_t** ppTok, char* pJsonReq)

JSON_HANDLER(getInitState);
JSON_HANDLER(initPeripherals);
JSON_HANDLER(getVersion);
JSON_HANDLER(getIpAddr);
JSON_HANDLER(setPower);
JSON_HANDLER(getUname);
JSON_HANDLER(getSysInfo);
JSON_HANDLER(getGyroDeltas);
JSON_HANDLER(getGyroTemp);
JSON_HANDLER(getPhiUptime);
JSON_HANDLER(startPhiLink);
JSON_HANDLER(getLinkStatus);
JSON_HANDLER(setMCtlId);
JSON_HANDLER(selfTest);

// valid command list

typedef struct {
  char *pCmd;
  jsonCmdHandler handler;
  
} PHI_JSON_CMD_TYPE;

#define CMD_ENTRY(c) { STR(c) , json_ ## c }

PHI_JSON_CMD_TYPE validCmds[] = {
  CMD_ENTRY(getInitState),
  CMD_ENTRY(initPeripherals),
  CMD_ENTRY(getVersion),
  CMD_ENTRY(getIpAddr),
  CMD_ENTRY(getUname),
  CMD_ENTRY(getSysInfo),
  CMD_ENTRY(getPhiUptime),
  CMD_ENTRY(setPower),
  CMD_ENTRY(getGyroDeltas),
  CMD_ENTRY(getGyroTemp),
  CMD_ENTRY(startPhiLink),
  CMD_ENTRY(getLinkStatus),
  CMD_ENTRY(setMCtlId),
//  CMD_ENTRY(setBrake),
  CMD_ENTRY(selfTest),
  { 0, 0}
};

// reply PHI sends to JSON commands
char jsonReply_start[] = "{" Q(reply) ": [\n";
char jsonReply_end[]   = "] }";

// each individual json request returns the same prolog and epilog
char jsonReplyObject_start[] = "{\n" Q(cmd) ":" Q(%s) ",\n";
char jsonReplyObject_end[]   = "}";

// bad format JSON reply sent by parser
// (handlers do their own thing)
char jsonParseError[] = "{ " Q(error) ":" Q(PHI could not process your JSON request.  It''s probably your fault btw.) " }\n";


// This function processes a JSON request and returns
// a JSON reply.
// Note: the caller MUST free the reply after transmitting
// it to the client.

char* PHI_processJson(char *pJsonReq) {
  
  jsmntok_t tokens[MAX_JSON_TOKENS];
  jsmn_parser parser;

  // individual command replies are put in a a linked list
  // temporarily and then concatenated into a monolithic
  // reply (pJsonReply) that is returned to the caller.
  //
  // Note: the caller must free pJsonReply when done with
  // it using PHI_freeJsonReply()

  char* pJsonReply = NULL;
  PHI_JSON_CMD_REPLY_TYPE* pRepHead = NULL;
  PHI_JSON_CMD_REPLY_TYPE* pRepCurr = NULL;

  // init JSON parser
  jsmn_init(&parser);
  
  // parse
  int rc = jsmn_parse(&parser, pJsonReq, tokens, MAX_JSON_TOKENS);

  if (rc != 0) {
    // error parsing
    LOG_ERR("error parsing JSON request - rc=%d", rc);
    goto quick_exit;
  }

  // log request

  if (VERBOSE_LOG) {

    LOG_INFO("Parsed JSON request to %d tokens: ", PARSER_NUM_TOK(parser)) ;

    // print tokens
    
    int i;

    for (i = 0 ; i < PARSER_NUM_TOK(parser) ; i++) {
      char val[2048];
      jsmntok_t* pTok = &tokens[i];
      int len = TOK_LEN(pTok);
      int type = TOK_TYPE(pTok);
      char *pType;
      
      switch (type) {
        case JSMN_PRIMITIVE: pType = "primitive"; break;
        case JSMN_OBJECT: pType = "object"; break;
        case JSMN_ARRAY: pType = "array"; break;
        case JSMN_STRING: pType = "string"; break;
        default: pType = "unknown" ; break;
      }
      
      memcpy(val, TOK_START(pTok), len);
      val[len] = 0;
      LOG_INFO("  token %d: type=%s, val='%s', offset=%d, numChild=%d", i, pType, val, TOK_START(pTok) - pJsonReq, TOK_NUM_CHILD(pTok));
    }
  }

  //
  // Process JSON commands
  //
  
  jsmntok_t* pTok = tokens;

  if ((TOK_TYPE(pTok) != JSMN_PRIMITIVE) ||
    (!TOK_EQ(pTok, "req"))) {
    // first token is not "req"
    LOG_ERR("webjson: first token in request is not 'req'");
    goto err_exit;
  }

  // advance to array
  pTok ++;
  
  if ((TOK_TYPE(pTok) != JSMN_ARRAY)) {
    LOG_ERR("webjson: first value is not an array");
    goto err_exit;
  }

  int numCmds = TOK_NUM_CHILD(pTok);

  if (numCmds == 0) {
    LOG_ERR("webjson: req array is zero length");
    goto err_exit;
  }
  
  // advance to first command
  pTok ++;

  // process array

  int doneCount;
  
  for (doneCount = 0 ; doneCount < numCmds ; doneCount ++) {

    if (TOK_TYPE(pTok) != JSMN_OBJECT) {
      LOG_ERR("webjson: req array element is not an object");
      goto err_exit;
    }

    jsonCmdHandler jsonHandler = getJsonHandler(&pTok, pJsonReq);

    if (jsonHandler == NULL) {
      LOG_ERR("webjson: unknown cmd received: '%*s'", TOK_LEN(pTok), TOK_START(pTok));
      goto err_exit;
    }

    // call handler
    PHI_JSON_CMD_REPLY_TYPE* pCmdReply = (*jsonHandler)(&pTok, pJsonReq);

    LOG_INFO("pCmdReply=%lu, reply=%lu", pCmdReply, pCmdReply->pReply);

    // save reply

    if (pCmdReply != NULL) {
      pCmdReply -> pNext = NULL;
      if (pRepHead == NULL) {
        pRepHead = pCmdReply;
      } else {
        pRepCurr -> pNext = pCmdReply;
      }
      pRepCurr = pCmdReply;
    } else {
      LOG_FATAL("webjson: pCmdReply should never be NULL");
    }
  }

  // calc size of reply to client

  int repSize = strlen(jsonReply_start) + strlen(jsonReply_end) + 1;

  for (pRepCurr = pRepHead; pRepCurr != NULL ; pRepCurr = pRepCurr -> pNext) {
    // add len + a comma
    repSize += strlen(pRepCurr -> pReply) + 1;
  }

  // allocate it

  pJsonReply = (char*) PHI_ALLOC_N(repSize);

  if (!pJsonReply) {
    // out of mem
    LOG_ERR("webjson: alloc of reply failed - size=%d", repSize);
    goto err_exit;
  }

  //
  // Build reply from pieces
  //

  // common beginning
  strcpy(pJsonReply, jsonReply_start);

  // command replies
  for (pRepCurr = pRepHead; pRepCurr != NULL ; pRepCurr = pRepCurr -> pNext) {
    strcat(pJsonReply, pRepCurr -> pReply);
    strcat(pJsonReply, ",");
  }

  // common end (copy on top of extraneous last comma)
  strcpy(pJsonReply + strlen(pJsonReply) - 1, jsonReply_end);

  // make sure we didn't overrun
  if (strlen(pJsonReply) >= (size_t) repSize) {
    LOG_FATAL("length of reply (%d) is greater than allocation (%d)!", strlen(pJsonReply), repSize);
  }
  
  if (VERBOSE_LOG) {
    LOG_INFO("JSON reply:\n%s\n", pJsonReply);
  }

quick_exit:

  while (pRepHead != NULL) {
    PHI_JSON_CMD_REPLY_TYPE* pNext = pRepHead -> pNext;
    // debug
    LOG_INFO("freeing pCmdReply=%lu, reply=%lu", pRepHead, pRepHead->pReply);
    // free part allocated with strdup
    free(pRepHead -> pReply);
    // free part allocated with PHI_ALLOC
    PHI_FREE(pRepHead);
    pRepHead = pNext;
  }

  return pJsonReply;

err_exit:

  if (pJsonReply != NULL) {
    PHI_freeJsonReply(pJsonReply);
  }

  pJsonReply = PHI_ALLOC_N(strlen(jsonParseError));
    
  goto quick_exit;
  
}

void PHI_freeJsonReply(char* pJsonReply) {
  PHI_FREE(pJsonReply);
}

jsonCmdHandler getJsonHandler(jsmntok_t** ppTok, char* pJsonReq) {
  jsmntok_t* pTok = *ppTok;
  
  // pTok points to object, advance to first child token
  pTok++;
  
  // first child token should be "cmd : string"
  if ((TOK_TYPE(pTok) != JSMN_PRIMITIVE) ||
    (!TOK_EQ(pTok, "cmd"))) {
    LOG_ERR("webjson: first child of array element is not 'cmd'");
    goto not_found;
  }

  // advance to cmd name
  pTok ++;
  
  if (TOK_TYPE(pTok) != JSMN_PRIMITIVE) {
    LOG_ERR("webjson: cmd name is not a primitive");
    goto not_found;
  }
  
  PHI_JSON_CMD_TYPE* pHandler = validCmds;
  while (pHandler -> pCmd != NULL) {
    if (TOK_EQ(pTok, pHandler -> pCmd)) {
      // found handler
      return pHandler -> handler;
    }
    pHandler ++;
  }

not_found:
  
  // not found
  return NULL;
}

// ########################################
//          JSON COMMAND HANDLERS
// ########################################

#define JSON_TMP_BUFFSIZE     1024

#define JSON_HANDLER_PROLOG(cmd) \
  /* allocate buffer for building reply */ \
  char _buff[JSON_TMP_BUFFSIZE]; \
  /* remember number of child tokens */ \
  int _numChild = TOK_NUM_CHILD(*ppTok); \
  /* write common prolog into it */ \
  sprintf(_buff, jsonReplyObject_start, #cmd); \
  /* skip to first "real" parameter (skip function name) */ \
  (*ppTok) += 3; /* extra semi-colon ok */

#define JSON_HANDLER_EPILOG() \
  /* write common epilog into _buff */ \
  strcat(_buff, jsonReplyObject_end); \
  /* create reply struct */ \
  PHI_JSON_CMD_REPLY_TYPE* _pRet = PHI_ALLOC(PHI_JSON_CMD_REPLY_TYPE); \
  _pRet -> pReply = strdup(_buff); \
  /* after the prolog, *ppTok points to the THIRD token within the cmd object */ \
  /* (if it exists) since we skip the common "cmd:string" part. */ \
  /* Advance *pTok to point to the next command object in the req array */ \
  (*ppTok) += _numChild - 2; \
  /* return to caller */ \
  return _pRet; /* extra semi-colon ok */

//
// Global data
//  
//   req:   { cmd : getInitState }
//   reply: { initPeriph : "OK" | "NOT INIT" }
//
//   req:   { cmd : initPeripherals }
//   reply: { status : string }
//
//   req:   { cmd : getVersion }
//   reply: { version : string }
//
//   req:   { cmd  : getIpAddr }
//   reply: { ip = string }
//
//   req:   { cmd  : getPhiUptime }
//   reply: { mSecs : UINT32 }
//

JSON_HANDLER(getInitState) {
  JSON_HANDLER_PROLOG(getInitState);
  sprintf(_buff + strlen(_buff), Q(initPeriph) ":" Q(%s) "\n", g_initPeriph == TRUE ? "OK" : "NOT INIT");
  JSON_HANDLER_EPILOG();
}

JSON_HANDLER(initPeripherals) {
  JSON_HANDLER_PROLOG(initPeripherals);
  char* status = HAL_initPeripherals();
  sprintf(_buff + strlen(_buff), Q(status) ":" Q(%s) "\n", status == NULL ? "OK" : status);
  JSON_HANDLER_EPILOG();
}

JSON_HANDLER(getVersion) {
  JSON_HANDLER_PROLOG(getVersion);
  sprintf(_buff + strlen(_buff), Q(version) ":" Q(%s) "\n", PHI_VERSION);
  JSON_HANDLER_EPILOG();
}

JSON_HANDLER(getIpAddr) {
  JSON_HANDLER_PROLOG(
    getIpAddr);
  sprintf(_buff + strlen(_buff), Q(ip) ":" Q(%lu.%lu.%lu.%lu) "\n",
    g_ipAddr & 0xff, (g_ipAddr >> 8) & 0xff,
    (g_ipAddr >> 16) & 0xff,(g_ipAddr >> 24) & 0xff);
  JSON_HANDLER_EPILOG();
}

JSON_HANDLER(getUname) {
  JSON_HANDLER_PROLOG(getUname);
  sprintf(_buff + strlen(_buff), Q(sysName)  ":" Q(%s)  ",\n", g_uname.sysname);
  sprintf(_buff + strlen(_buff), Q(nodeName) ":" Q(%s)  ",\n", g_uname.nodename);
  sprintf(_buff + strlen(_buff), Q(release)  ":" Q(%s)  ",\n", g_uname.release);
  sprintf(_buff + strlen(_buff), Q(version)  ":" Q(%s)  ",\n", g_uname.version);
  sprintf(_buff + strlen(_buff), Q(machine)  ":" Q(%s)  " \n", g_uname.machine);
  JSON_HANDLER_EPILOG();
}

JSON_HANDLER(getSysInfo) {
  JSON_HANDLER_PROLOG(getSysInfo);
  struct sysinfo info;
  sysinfo(&info);
  sprintf(_buff + strlen(_buff), Q(upTime)      ":" Q(%ld)  ",\n", info.uptime);
  sprintf(_buff + strlen(_buff), Q(sysLoad_1m)  ":" Q(%lu)  ",\n", info.loads[0]);
  sprintf(_buff + strlen(_buff), Q(sysLoad_5m)  ":" Q(%lu)  ",\n", info.loads[1]);
  sprintf(_buff + strlen(_buff), Q(sysLoad_15m) ":" Q(%lu)  ",\n", info.loads[2]);
  sprintf(_buff + strlen(_buff), Q(totalRAM)    ":" Q(%lu)  ",\n", info.totalram);
  sprintf(_buff + strlen(_buff), Q(freeRAM)     ":" Q(%lu)  ",\n", info.freeram);
  sprintf(_buff + strlen(_buff), Q(sharedRAM)   ":" Q(%lu)  ",\n", info.sharedram);
  sprintf(_buff + strlen(_buff), Q(bufferRAM)   ":" Q(%lu)  ",\n", info.bufferram);
  sprintf(_buff + strlen(_buff), Q(totalSwap)   ":" Q(%lu)  ",\n", info.totalswap);
  sprintf(_buff + strlen(_buff), Q(freeSwap)    ":" Q(%lu)  ",\n", info.freeswap);
  sprintf(_buff + strlen(_buff), Q(numProcs)    ":" Q(%d)   ",\n", info.procs);
  sprintf(_buff + strlen(_buff), Q(totalHigh)   ":" Q(%lu)  ",\n", info.totalhigh);
  sprintf(_buff + strlen(_buff), Q(freeHigh)    ":" Q(%lu)  ",\n", info.freehigh);
  sprintf(_buff + strlen(_buff), Q(memUnit)     ":" Q(%d)   "\n" , info.mem_unit);
  JSON_HANDLER_EPILOG();
}

JSON_HANDLER(getPhiUptime) {
  JSON_HANDLER_PROLOG(getPhiUptime);
  sprintf(_buff + strlen(_buff), Q(mSecs) ":" Q(%lu) "\n", (UINT32) (PHI_upTime() / 1000));
  JSON_HANDLER_EPILOG();
}

//
// Motor control
//  
//  req:    { cmd : setPower, motorName : "[A-F][0-1]" , power : -100 to 100 (percent)  }
//  reply:  {}
//
//  req:    { cmd : setBrake, motorName : "[A-F][0-1]" , brake : on | off | zeroMeansBrake | zeroMeansCoast }
//  reply:  {}
//
//  req:    { cmd: setMCtlId, oldId: byte, newId: byte }
//
// where:
//
// motorId: see motor.c for more details
//
// if setPower:
//    power = -100 to 100   negative means backwards, meaning of 0%
//                          depends on setBreak
//
// if setBrake:
//
//   val = on               put motor in braking mode now
//       = off              put motor in idle mode now
//       = zeroMeansBrake   power=0% means braking mode
//       = zeroMeansCoast   power=0% means coast (default)
//
// if setMCtlId:
//
//   oldId = byte           current id of motor controller (default factory id = 9)
//   newId = byte           new id of motor controller
//
//

JSON_HANDLER(setPower) {
  JSON_HANDLER_PROLOG(setPower);
  
  jsmntok_t* pTok = *ppTok;
  char motorName[3] = {0, 0, 0};
  int powerVal = -1;
  BOOL bFwd = TRUE;
  int toksRead = 2;

  while (toksRead < _numChild) {

    if (TOK_TYPE(pTok) != JSMN_PRIMITIVE) {
      sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.setPower: param name is not a primitive.) );
      goto error_exit;
    }

    if (TOK_EQ(pTok, "motorName")) {
      // advance
      pTok ++;
      toksRead ++;

      // get motor name
      if (TOK_TYPE(pTok) != JSMN_PRIMITIVE) {
        sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.setPower: motorName value is not a primitive.) );
        goto error_exit;
      }

      if (TOK_LEN(pTok) != 2) {
        sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.setPower: motorName length is not 2.) );
        goto error_exit;
      }
      memcpy(motorName, TOK_START(pTok), sizeof(motorName));
    }
    else if (TOK_EQ(pTok, "power")) {
      // advance
      pTok ++;
      toksRead ++;

      // get power
      if (TOK_TYPE(pTok) != JSMN_PRIMITIVE) {
        sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.setPower: power value is not a primitive.) );
        goto error_exit;
      }

      int percent = atoi(TOK_START(pTok));

      if (percent < 0) {
        bFwd = FALSE;
        percent = -percent;
      }

      powerVal = (percent * 127) / 100;

    } else {
      // unknown property
      LOG_ERR("JSON.setPower: Unknown property '%s'", TOK_START(pTok));
      goto error_exit;
    }

    // advance
    pTok ++;
    toksRead ++;
  }

  if ((motorName[0] == 0) || (powerVal == -1)) {
    sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.setPower: parameters missing.) );
    goto error_exit;
  }

  LOG_INFO("JSON.setPower: setting motor %s to power=%u, dir=%s", motorName, (BYTE) powerVal, bFwd ? "FWD" : "BACK");

  HAL_setMotorPower(
    MOTOR_NAME_TO_CTRL_ID(motorName),
    MOTOR_NAME_TO_SEL_IDX(motorName),
    (BYTE) powerVal, bFwd);

quick_exit:

  JSON_HANDLER_EPILOG();

error_exit:

  LOG_ERR("JSON.setPower:  call failed");
  goto quick_exit;
}

JSON_HANDLER(setMCtlId) {
  JSON_HANDLER_PROLOG(setMCtlId);

  jsmntok_t* pTok = *ppTok;
  int oldId = MC_DEFAULT_DEVICE_NUM;
  int newId = -1;
  int toksRead = 2;

  while (toksRead < _numChild) {

    if (TOK_TYPE(pTok) != JSMN_PRIMITIVE) {
      sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.setMCtlId: param name is not a primitive.) );
      goto error_exit;
    }
    
    if (TOK_EQ(pTok, "oldId")) {

      // advance
      pTok ++;
      toksRead ++;

      oldId = atoi(TOK_START(pTok));
      
    } else if (TOK_EQ(pTok, "newId")) {

      // advance
      pTok ++;
      toksRead ++;

      newId = atoi(TOK_START(pTok));

    } else {

      // unknown property
      LOG_ERR("JSON.setMCtlId: Unknown property '%s'", TOK_START(pTok));
      goto error_exit;
    }

    // advance
    pTok ++;
    toksRead ++;
  }

  if (newId == -1) {
    sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.setMCtlId: newId parameter missing.) );
    goto error_exit;
  }

  LOG_INFO("JSON.setMCtlId: changing motor controller id '%d' to '%d'", oldId, newId);
  
  HAL_setControllerId(oldId, newId);

quick_exit:

  JSON_HANDLER_EPILOG();

error_exit:

  LOG_ERR("JSON.setMCtlId:  call failed");
  goto quick_exit;
}

//
// Gyroscope
//
//  req:    { cmd : getGyroDeltas }
//  reply:  { pitchDelta: +/- deg , yawDelta: +/- deg, rollDelta: +/- deg }
//
//  req:    { cmd : getGyroTemp }
//  reply:  { degreesC: -128 to 127 deg celsius (1s refresh rate) }

JSON_HANDLER(getGyroDeltas) {
  JSON_HANDLER_PROLOG(getGyroDeltas);
  float pitchDelta, yawDelta, rollDelta;
  HAL_gyroGetDeltas(&pitchDelta, &yawDelta, &rollDelta);
  sprintf(_buff + strlen(_buff), Q(pitchDelta) ":" Q(%.1f) ",\n", (double) pitchDelta);
  sprintf(_buff + strlen(_buff), Q(yawDelta) ":" Q(%.1f) ",\n", (double) yawDelta);
  sprintf(_buff + strlen(_buff), Q(rollDelta) ":" Q(%.1f) "\n", (double) rollDelta);
  JSON_HANDLER_EPILOG();
}

JSON_HANDLER(getGyroTemp) {
  JSON_HANDLER_PROLOG(getGyroTemp);
  sprintf(_buff + strlen(_buff), Q(degreesC) ":" Q(%d) "\n", HAL_gyroGetTemp());
  JSON_HANDLER_EPILOG();
}

//
// PHI link
//
//  req:    { cmd : getLinkStatus }
//  reply:  { state: string}
//
//  req:    { cmd : startPhiLink, server : ipAddr/hostname, [opt] port=num }
//  reply:  { }

JSON_HANDLER(getLinkStatus) {
  JSON_HANDLER_PROLOG(getLinkStatus);
  
  char* pState;
  switch (g_phiLinkStatus) {
    case LINK_OFF:
      pState="OFF";
      break;
    case LINK_STARTED:
      pState="STARTED";
      break;
    case LINK_CONNECTING:
      pState="CONNECTING";
      break;
    case LINK_CONNECTED:
      pState="CONNECTED";
      break;
    case LINK_ERROR:
      pState="ERROR";
      break;
    case LINK_CLOSED:
      pState="CLOSED";
      break;
    default:
      pState="UNKNOWN";
      break;
  }
  
  sprintf(_buff + strlen(_buff), Q(state) ":" Q(%s) "\n", pState);
  JSON_HANDLER_EPILOG();
}

JSON_HANDLER(startPhiLink) {
  JSON_HANDLER_PROLOG(startPhiLink);

  jsmntok_t* pTok = *ppTok;
  char hostName[257] = {0};
  int port = DEF_PHILINK_PORT;
  int toksRead = 2;

  while (toksRead < _numChild) {

    if (TOK_TYPE(pTok) != JSMN_PRIMITIVE) {
      sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.startPhiLink: param name is not a primitive.) );
      goto error_exit;
    }

    if (TOK_EQ(pTok, "serverName")) {
      // advance
      pTok ++;
      toksRead ++;

      // get server name (PhiCore)
      if (TOK_TYPE(pTok) != JSMN_PRIMITIVE) {
        sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.startPhiLink: server value is not a primitive.) );
        goto error_exit;
      }

      int len = TOK_LEN(pTok);
      memcpy(hostName, TOK_START(pTok), len);
      hostName[len] = 0;

    } else if (TOK_EQ(pTok, "serverPort")) {

      // advance
      pTok ++;
      toksRead ++;

      // get port number
      if (TOK_TYPE(pTok) != JSMN_PRIMITIVE) {
        sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.startPhiLink: power value is not a primitive.) );
        goto error_exit;
      }

      int port = atoi(TOK_START(pTok));

    } else {
      
      // unknown property
      LOG_ERR("JSON.startPhiLink: Unknown property '%s'", TOK_START(pTok));
      goto error_exit;
    }

    // advance
    pTok ++;
    toksRead ++;
  }

  if (hostName[0] == 0) {
    sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.startPhiLink: parameters missing.) );
    goto error_exit;
  }

  LOG_INFO("JSON.startPhiLink: starting link to host='%s:%d' ...", hostName, port);
  
  if (startPhiLink(hostName, port) == FALSE) {
    sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.startPhiLink: startPhiLink returned false.) );
    goto error_exit;
  }
  
quick_exit:

  JSON_HANDLER_EPILOG();

error_exit:

  LOG_ERR("JSON.startPhiLink:  call failed");
  goto quick_exit;
}

//
// Self Test Functions
//
//
//  req:    { cmd : selfTest, [opt] mode=full }
//  reply:  { verboseResult : string }
//

JSON_HANDLER(selfTest) {
  JSON_HANDLER_PROLOG(selfTest);

  jsmntok_t* pTok = *ppTok;
  int toksRead = 2;

  while (toksRead < _numChild) {

    if (TOK_TYPE(pTok) != JSMN_PRIMITIVE) {
      sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.selfTest: param name is not a primitive.) );
      goto error_exit;
    }

    if (TOK_EQ(pTok, "mode")) {
      // advance
      pTok ++;
      toksRead ++;

      // get mode
      if (TOK_TYPE(pTok) != JSMN_PRIMITIVE) {
        sprintf(_buff + strlen(_buff), Q(error) ":" Q(JSON.selfTest: mode value is not a primitive.) );
        goto error_exit;
      }

      // do nothing with mode for now
      // int len = TOK_LEN(pTok);

    } else {

      // unknown property
      LOG_ERR("JSON.selfTest: Unknown property '%s'", TOK_START(pTok));
      goto error_exit;
    }

    // advance
    pTok ++;
    toksRead ++;
  }

  // start self test
  char* pVerbose = selfTest(0);

quick_exit:

  sprintf(_buff + strlen(_buff), Q(verboseResult) ":" Q(%s) "\n", pVerbose);
  JSON_HANDLER_EPILOG();

error_exit:

  LOG_ERR("JSON.selfTest:  call failed");
  goto quick_exit;
  
}





