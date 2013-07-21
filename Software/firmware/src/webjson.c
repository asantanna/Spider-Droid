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

// stringifying helpers

#define STR(s)                #s
#define STR_MACRO(m)          STR(m)          // two steps required to expand macro
#define Q(s)                  "\"" #s "\""

// max number of JSON tokens we support (should be way plenty)
#define MAX_JSON_TOKENS       256

// JSMN token helpers

#define TOK_TYPE(pTok)        (pTok)->type
#define TOK_NUM_CHILD(pTok)   (pTok)->size

#define TOK_EQ(pTok,s) \
    ((int) strlen(s) == (pTok)->end - (pTok)->start) \
    ? (strncmp(s, pJsonReq + (pTok)->start, strlen(s)) == 0) \
    : FALSE

//
// VALID COMMANDS
//

typedef struct _jsonCmdReply_t {
  char* pReply;
  struct _jsonCmdReply_t* pNext;

} PHI_JSON_CMD_REPLY_TYPE;

typedef PHI_JSON_CMD_REPLY_TYPE* (*jsonCmdHandler)(jsmntok_t** ppTok);

// Internal
jsonCmdHandler getJsonHandler(jsmntok_t** ppTok, char* pJsonReq);

// JSON handlers forward decls

#define JSON_HANDLER(h) PHI_JSON_CMD_REPLY_TYPE* json_##h (jsmntok_t** ppTok)

JSON_HANDLER(getInitState);
JSON_HANDLER(initPeripherals);
JSON_HANDLER(getVersion);
JSON_HANDLER(getHost);

JSON_HANDLER(debugJunk);
JSON_HANDLER(debugJunk2);

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
  CMD_ENTRY(getHost),
//  CMD_ENTRY(getUpTime),
//  CMD_ENTRY(getAccel),
//  CMD_ENTRY(setPower),
//  CMD_ENTRY(setBrake),
  CMD_ENTRY(debugJunk),
  CMD_ENTRY(debugJunk2),
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

char* phi_processJson(char *pJsonReq) {
  
  int i;
  jsmntok_t tokens[MAX_JSON_TOKENS];
  jsmn_parser parser;

  // individual command replies are put in a a linked list
  // temporarily and then concatenated into a monolithic
  // reply (pJsonReply) that is returned to the caller.
  //
  // Note: the caller must free pJsonReply when done with
  // it using phi_freeJsonReply()

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

    LOG_INFO("Received JSON command:");

    for (i = 0 ; i < 10 /* COUNTOF(tokens)*/ ; i++) {
      char val[2048];
      int len = tokens[i].end - tokens[i].start;
      int type = tokens[i].type;
      char *pType;
      switch (type) {
        case JSMN_PRIMITIVE: pType = "primitive"; break;
        case JSMN_OBJECT: pType = "object"; break;
        case JSMN_ARRAY: pType = "array"; break;
        case JSMN_STRING: pType = "string"; break;
        default: pType = "unknown" ; break;
      }
      memcpy(val, pJsonReq + tokens[i].start, len);
      val[len] = 0;
      LOG_INFO("  token %d: type=%s, val='%s', size=%d", i, pType, val, tokens[i].size);
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
        LOG_ERR("webjson: unknown cmd at index %d", i);
        goto err_exit;
    }

    // call handler
    PHI_JSON_CMD_REPLY_TYPE* pCmdReply = (*jsonHandler)(&pTok);

    // save reply
    
    if (pCmdReply != NULL) {
      pCmdReply -> pNext = NULL;
      if (pRepHead == NULL) {
        pRepHead = pCmdReply;
      } else {
        pRepCurr -> pNext = pCmdReply;
      }
      pRepCurr = pCmdReply;
    }
  }

  // calc size of reply to client

  int repSize = strlen(jsonReply_start) + strlen(jsonReply_end) + 1;

  for (pRepCurr = pRepHead; pRepCurr != NULL ; pRepCurr = pRepCurr -> pNext) {
    repSize += strlen(pRepCurr -> pReply);
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

quick_exit:

  while (pRepHead != NULL) {
    PHI_JSON_CMD_REPLY_TYPE* pNext = pRepHead -> pNext;
    // free part allocated with strdup
    free(pRepHead -> pReply);
    // free part allocated with PHI_ALLOC
    PHI_FREE(pRepHead);
    pRepHead = pNext;
  }

  return pJsonReply;

err_exit:

  if (pJsonReply != NULL) {
    phi_freeJsonReply(pJsonReply);
  }

  pJsonReply = PHI_ALLOC_N(strlen(jsonParseError));
    
  goto quick_exit;
  
}

void phi_freeJsonReply(char* pJsonReply) {
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
      // found handler - update caller pToken to next token after cmd : string
      // (ie the first "real" parameter for the command)
      *ppTok = pTok + 1;
      // return handler
      return pHandler -> handler;
    }
    pHandler ++;
  }

not_found:
  
  // not found (no need to update *ppTok because caller should abort)
  return NULL;
}

// ########################################
//          JSON COMMAND HANDLERS
// ########################################

#define JSON_TMP_BUFFSIZE     1024

#define JSON_HANDLER_PROLOG(cmd) \
  /* allocate buffer for building reply */ \
  char buff[JSON_TMP_BUFFSIZE]; \
  /* remember number of child tokens */ \
  int numChild = (*ppTok) -> size; \
  /* write common prolog into it */ \
  sprintf(buff, jsonReplyObject_start, #cmd)

#define JSON_HANDLER_EPILOG() \
  /* write common epilog into buff */ \
  strcat(buff, jsonReplyObject_end); \
  /* create reply struct */ \
  PHI_JSON_CMD_REPLY_TYPE* pRet = PHI_ALLOC(PHI_JSON_CMD_REPLY_TYPE); \
  pRet -> pReply = strdup(buff); \
  /* on entry, *ppTok points to the THIRD token within the cmd object */ \
  /* (if it exists) since we skip the common "cmd:string" part. */ \
  /* Advance *pTok to point to the next command object in the req array */ \
  (*ppTok) += numChild - 2; \
  /* return to caller */ \
  return pRet

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
//   req:   { cmd  : getHost }
//   reply: { name : string, ip = string }
//
//   req:   { cmd  : getUptime }
//   reply: { uSecs : num64 }
//
//   req:   { cmd : getAccel }
//   reply: { x : num,               // accel in X dir
//            y : num,               // accel in Y dir
//            z : num,               // accel in Z dir
//   }

JSON_HANDLER(getInitState) {
  JSON_HANDLER_PROLOG(getInitState);
  sprintf(buff + strlen(buff), Q(initPeriph) ":" Q(%s) "\n", g_initPeriph == TRUE ? "OK" : "NOT INIT");
  JSON_HANDLER_EPILOG();
}

JSON_HANDLER(initPeripherals) {
  JSON_HANDLER_PROLOG(initPeripherals);
  char* status = phi_initPeripherals();
  sprintf(buff + strlen(buff), Q(status) ":" Q(%s) "\n", status == NULL ? "OK" : status);
  JSON_HANDLER_EPILOG();
}

JSON_HANDLER(getVersion) {
  JSON_HANDLER_PROLOG(getVersion);
  sprintf(buff + strlen(buff), Q(version) ":" Q(%s) "\n", PHI_VERSION);
  JSON_HANDLER_EPILOG();
}

JSON_HANDLER(getHost) {
  JSON_HANDLER_PROLOG(getHost);
  sprintf(buff + strlen(buff), Q(name) ":" Q(%s) ",\n", "NOT_IMPL");
  sprintf(buff + strlen(buff), Q(ip) ":" Q(%lu.%lu.%lu.%lu) "\n",
    g_ipAddr & 0xff, (g_ipAddr >> 8) & 0xff,
    (g_ipAddr >> 16) & 0xff,(g_ipAddr >> 24) & 0xff);
  JSON_HANDLER_EPILOG();
}

//
// Motor control
//  
//  req:    { cmd : setPower, motorId : "ppj" , power : -100 to 100 (percent)
//  reply:  {}
//
//  req:    { cmd : setBrake, motorId : "ppj" , brake : on | off | zeroMeansBrake | zeroMeansCoast
//  reply:  {}
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
//      = t       thigh flexor joint (aka hip flexor)
//      = k       knee flexor joint
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
//


  

//
// JUNK commnds for quick tests
//


JSON_HANDLER(debugJunk) {
  JSON_HANDLER_PROLOG(debugJunk);
  char motorCmd[] = { MC_CMD_SIGN, MC_DEFAULT_DEVICE_NUM, MC_CMD_FWD_M0, 0x7F };
  uartSend(motorCmd, sizeof(motorCmd));
  sprintf(buff + strlen(buff), Q(status) ":" Q(%s) "\n", "OK");
  JSON_HANDLER_EPILOG();
}

JSON_HANDLER(debugJunk2) {
  JSON_HANDLER_PROLOG(debugJunk2);
  char motorCmd[] = { MC_CMD_SIGN, MC_DEFAULT_DEVICE_NUM, MC_CMD_FWD_M0, 0 };
  uartSend(motorCmd, sizeof(motorCmd));
  sprintf(buff + strlen(buff), Q(status) ":" Q(%s) "\n", "OK");
  JSON_HANDLER_EPILOG();
}
