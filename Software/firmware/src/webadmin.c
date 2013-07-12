//
// PHI web admin for diagnostic purposes
//
// Note: this file started off as the "nweb tiny webserver"
//       by Nigel Griffiths (nag@uk.ibm.com)
//
//       http://www.ibm.com/developerworks/systems/library/es-nweb/index.html
//
// It has been heavily modified - actually pretty much rewritten to use
// threads, handle HEAD/POST, remove hardwired content-lengths,
// implement JSON command requests, ad nauseam.
//

// Typical HTTP Request:
//
//			GET /path/file?CMD=3&FOO=3&MOO=192.168.1.1 HTTP/1.0
//			Content-Length: num  (optional if GET)
//			<blank line>
//			<data> (opional if "GET")
//
// Typical response:
//
//			HTTP/1.0 200 OK
//			Content-Length: num
//			<blank line>
//			<data>
//
// Note: Content-Length refers to the "data" portion only
//
// HTTP HEAD command: same as GET but return headers only (no data)
//
// HTTP POST command: similar to GET with following differences
//
//   a) request contains data for the server
//   b) headers specify size and type with "Content-Type:" and
//      "Content-Length:"
//   c) Parameters go in body with form submissions, it has the
//      same format as the part after the "?" in the GET command.
//

//
// TODO: fix forking
// TODO: fix return codes because no longer forking
// TODO: read through all code
// TODO: print thread id in "starting message"
// TODO: parse URL encoding ... it's easy and will save pain later
//

#include "phi.h"

#define BUFSIZE         8096      // max size of HTTP request
#define ERROR           42
#define LOG             44
#define FORBIDDEN       403
#define NOTFOUND        404

#define HTTP_METHOD_HEAD_STR		"HEAD"
#define HTTP_METHOD_GET_STR			"GET"
#define HTTP_METHOD_POST_STR		"POST"
#define HTTP_HDR_LEN						"Content-Length:"

#define MAX_TOKEN_SIZE          256

struct {
	char *pFileExt;
	char *pContentType;
} wa_extensions [] = {
	{"gif", "image/gif" },  
	{"jpg", "image/jpg" }, 
	{"jpeg", "image/jpeg"},
	{"png", "image/png" },  
	{"ico", "image/ico" },  
	{"zip", "image/zip" },  
	{"gz",  "image/gz"  },  
	{"tar", "image/tar" },  
	{"htm", "text/html" },  
	{"html", "text/html" },  
  {0, 0}
};

typedef struct {
  char* pMethod;
  char* pPath;
  char* pGetParams;
  char* pHdrContentLength;
  char* pBody;
} PHI_PARSED_HTTP;

char wa_response_forbidden_hdr[] =
  "HTTP/1.1 403 Forbidden\n"
  "Content-Length: %ld\n"
  "Connection: close\n"
  "Content-Type: text/html\n"
  "\n";

char wa_response_forbidden_body[] =
  "<html>\n"
  "<head>\n"
  "<title>403 Forbidden</title>\n"
  "</head>\n"
  "<body>\n"
  "<h1>Forbidden</h1>\n"
  "The requested URL, file type or operation is not allowed on this simple static file webserver.\n"
  "</body>\n"
  "</html>\n";

char wa_response_notfound_hdr[] =
  "HTTP/1.1 404 Not Found\n"
  "Content-Length: %ld\n"
  "Connection: close\n"
  "Content-Type: text/html\n"
  "\n";

char wa_response_notfound_body[] =
  "<html>\n"
  "<head>\n"
  "<title>404 Not Found</title>\n"
  "</head>\n"
  "<body>\n"
  "<h1>Not Found</h1>\n"
  "The requested URL was not found on this server.\n"
  "</body>\n"
  "</html>\n";

char wa_response_ok_hdr[] =
  "HTTP/1.1 200 OK\n"
  "Server: Phi Web Admin %s\n"
  "Content-Length: %ld\n"
  "Connection: close\n"
  "Content-Type: %s\n"
  "\n";

char wa_response_servererror_hdr[] =
 "HTTP/1.1 500 Internal Server Error\n"
 "Content-Length: %ld\n"
 "Connection: close\n"
 "Content-Type: text/html\n"
 "\n";

char wa_response_servererror_body[] =
 "<html>\n"
 "<head>\n"
 "<title>500 Internal Server Error</title>\n"
 "</head>\n"
 "<body>\n"
 "<h1>500 Internal Server Error</h1>\n"
 "Something went wrong (and it's probably your fault).\n"
 "</body>\n"
 "</html>\n";

#define SEND_ERROR_REPLY(replyType) \
    { \
    sprintf(buffer, wa_response_ ## replyType ## _hdr, strlen(wa_response_ ## replyType ## _body)); \
    write(socketfd, buffer, strlen(buffer)); \
    write(socketfd, wa_response_ ## replyType ## _body, strlen(buffer)); \
    }

// internal

void wa_process_web_request(int socketfd, int hit);
int wa_parseHttpRequest(char* pReq, PHI_PARSED_HTTP* pParsed);
char readToken(char** ppData, char** ppToken);
void readToEol(char** ppData, char** ppToken);
void readToSpace(char** ppData, char** ppToken);

// entry point of web admin - never returns

void phi_webadmin(int port, const char* wwwRoot)
{
  int i, pid, listenfd, socketfd, hit;
  socklen_t length;
  struct sockaddr_in cli_addr;
  struct sockaddr_in serv_addr;

  // init to zeros
  PHI_ZERO(cli_addr);
  PHI_ZERO(serv_addr);

  // debug
  LOG_INFO("PHI webadmin starting - wwwRoot='%s'", (char*) wwwRoot);

  // change dir to www doc root
  if (chdir(wwwRoot) == -1) { 
    LOG_FATAL("can't change to directory %s\n", wwwRoot);
  }

  // setup the network socket
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    LOG_FATAL("system call failed: socket");
  }

  if (port < 0 || port > 60000) {
    LOG_FATAL("Invalid port number (try 1 -> 60000)", "port given=", port);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);

  if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <0) {
    LOG_FATAL("system call failed: bind");
  }

  // get IP of (probable) interface we will get requests from

  UINT32 ipAddr = phi_getHostIP();

  printf("listening on address %lu.%lu.%lu.%lu on port %d\n",
         ipAddr & 0xff, (ipAddr >> 8) & 0xff,
         (ipAddr >> 16) & 0xff,(ipAddr >> 24) & 0xff,
         port);

  // listen (enable receive)

  if ( listen(listenfd, 64) <0) {
    LOG_FATAL("system call failed: listen");
  }

  // go into infinite loop accepting calls

  for (hit = 1 ; ; hit++) {

    length = sizeof(cli_addr);
    if ((socketfd = accept(listenfd, (struct sockaddr *) &cli_addr, &length)) < 0) {
      LOG_FATAL("system call failed: accept");
    }

// TODO: should spawn thread here
    
    // process web request
    // Note: function will close socketfd when done
    wa_process_web_request(socketfd, hit);
  }
}

void wa_process_web_request(int socketfd, int hit)
{
  int pagefd, pathLen;
  long sock_numRead, len;
  char* pContentType;
  char buffer[BUFSIZE+1];   // add space for zero term

  // read Web request in one go
  //
  // NOTE: this code assumes we get everything in one shot.
  // Not sure if this is OK.
  sock_numRead = read(socketfd, buffer, BUFSIZE);

  if (sock_numRead == 0 || sock_numRead == -1) {
  // read failure - give up
    SEND_ERROR_REPLY(forbidden);
    LOG_ERR("failed to read browser request");
    goto quick_exit;
  }

  if (sock_numRead > 0 && sock_numRead < BUFSIZE) {
    // read something - zero term
    buffer[sock_numRead] = 0;
  }
  else {
    // error
    SEND_ERROR_REPLY(forbidden);
    LOG_ERR("GET request too small/big - size=%d", sock_numRead);
    buffer[0] = 0;
    goto quick_exit;
  }

  LOG_INFO("webadmin: hit=%d, request follows:\n%s\n", hit, buffer);

  // parse web request

  PHI_PARSED_HTTP parsedHttp;
  
  if (wa_parseHttpRequest(buffer, &parsedHttp) == -1) {
    SEND_ERROR_REPLY(servererror);
    LOG_ERR("Parent directory (..) path names not supported");
    goto quick_exit;
  }

  // deal with blank path
  char* pPath = parsedHttp.pPath;
  
  if (pPath != NULL) {
    if (pPath[1] == 0) {
      // just a slash - use index.html instead
      pPath = "/index.html";
    }
  } else {
    // no path at all - error
    SEND_ERROR_REPLY(servererror);
    LOG_ERR("HTTP request has no path");
    goto quick_exit;
  }

  // check for illegal parent directory use
  int i;
  pathLen = strlen(pPath);
  
  for (i=0 ; i < pathLen ; i++) {
    if (pPath[i] == '.' && pPath[i+1] == '.') {
      SEND_ERROR_REPLY(forbidden);
      LOG_ERR("Parent directory (..) path names not supported");
      goto quick_exit;
    }
  }

  // work out the file type and check we support it
  pContentType = NULL;

  for (i=0 ; wa_extensions[i].pFileExt != 0 ; i++) {
    len = strlen(wa_extensions[i].pFileExt);
    if (!strncmp(&buffer[pathLen-len], wa_extensions[i].pFileExt, len)) {
      pContentType = wa_extensions[i].pContentType;
      break;
    }
  }
  
  if (pContentType == NULL) {
    SEND_ERROR_REPLY(forbidden);
    LOG_ERR("file extension type not supported (%s)", buffer);
    goto quick_exit;
  }

  // open the file for reading (skip leading slash so relative path)
  char* pFname = pPath + 1;
  
  if ((pagefd = open(pFname, O_RDONLY)) == -1) {
    SEND_ERROR_REPLY(notfound);
    LOG_ERR("webadmin failed to open file '%s'", pFname);
    return;
  }

  LOG_INFO("webadmin: page requested='%s'", pFname);

  // lseek to the file end to find the length
  len = lseek(pagefd, 0, SEEK_END);

  // lseek back to the file start ready for reading
  lseek(pagefd, 0, SEEK_SET);

  // send header plus blank line
  sprintf(buffer, wa_response_ok_hdr, PHI_VERSION, len, pContentType);
  LOG_INFO("webadmin: reply header='%s'", buffer);
  write(socketfd, buffer, strlen(buffer));

  // send file in 8KB blocks - last block may be smaller
  while (	(len = read(pagefd, buffer, BUFSIZE)) > 0 ) {
    write(socketfd, buffer, len);
  }

quick_exit:

  // allow socket to drain before closing
  sleep(1);

  // close the socket
  close(socketfd);
}

//
// This is based on my Arduino code in ALS-Libs but heavily modified.
//
// NOTE: This routine *writes* into *pReq to zero term strings.
//

int wa_parseHttpRequest(char* pReq, PHI_PARSED_HTTP* pParsed) {

  typedef enum {READ_METHOD, READ_PATH, READ_PARAMS, READ_HTTP_HEADERS, READ_BODY, DONE} state_t;

  int rc = -1;
  state_t state = READ_METHOD;
  char* pToken = NULL;
  char c;

  // clear parsed response
  PHI_ZERO(*pParsed);

  while (state != DONE) {

    switch (state) {
      case READ_METHOD:
        if (!(c = readToken(&pReq, &pToken))) goto quick_exit;
        pParsed -> pMethod = pToken;
        state = READ_PATH;
        break;

      case READ_PATH:
        // pToken will break at '?' or ws
        if (!(c = readToken(&pReq, &pToken))) goto quick_exit;
        pParsed -> pPath = pToken;
        if (c == '?') {
          // have params
          state = READ_PARAMS;
        } else {
          // no params - discard HTTP version
          readToEol(&pReq, &pToken);
          state = READ_HTTP_HEADERS;
        }
        break;

      case READ_PARAMS:
        // read all params
        readToSpace(&pReq, &pToken);
        pParsed -> pGetParams = pToken;
        // discard HTTP version
        readToEol(&pReq, &pToken);
        state = READ_HTTP_HEADERS;
        break;

      case READ_HTTP_HEADERS:
        // read header name
        if (!(c = readToken(&pReq, &pToken))) break;
        if (c == '\n') {
          //  blank line - done with headers, read body
          // TODO HACK: read body here - no need for another state
          // success
          state = DONE;
          rc = 0;
          break;
        }
        // read header value
        readToken(&pReq, &pToken);
        // check if it is one we know (ignore others)
        if (strcasecmp(pToken, HTTP_HDR_LEN) == 0) {
          // header is ContentLength
          pParsed -> pHdrContentLength = pToken;
        }
        break;
        
    } // switch
  } // while

quick_exit:
  
  return rc;
}

// reads up to a delimiter and returns zero-term string in token
// notes:
//		1) if too big, string is truncated
//		2) returns delimiter as return code (if error, then -1)
//

char readToken(char** ppData, char** ppToken) {

  BOOL bSkippingWs = TRUE;
  char* pToken = NULL;
  char* pData = *ppData;
  char c;

  while (TRUE) {

    if ((c = *pData++) == 0) {
      // end of data - error
      break;
    }

    if (c == '\r') {
      // ignore CR
      continue;
    }

    if (c == ' ') {
      // read a space
      if (bSkippingWs) {
        // ignore leading spaces
        continue;
      }
      // otherwise space terminates token
      break;

    } else {
      // not a space
      bSkippingWs = FALSE;
      if ((c == '=') || (c == '&') || (c == ':') || (c == '?') || (c == '\n')) {
        // these end token too
        break;
      }
    }

    if (pToken != NULL) {
      // remember start of token
      pToken = pData - 1;
    }
  }

  // terminate token in place (overwrite delimiter)
  *(pData-1) = 0;

  // done
  *ppData = pData;
  *ppToken = pToken;
  return c;
}

void readToSpace(char** ppData, char** ppToken) {

  char* pData = *ppData;
  char c;

  *ppToken = pData;

  while ((c = *pData++) != 0) {
    if (c == ' ') {
      // found space - done
      break;
    }
  }

  // zero term (overwrites space)
  *(pData-1) = 0;

  // done
  *ppData = pData;
  return;
}

void readToEol(char** ppData, char** ppToken) {

  char* pData = *ppData;
  char c;

  *ppToken = pData;

  while ((c = *pData++) != 0) {
    if (c == '\n') {
      // found NL - done
      break;
    }
  }

  // zero term (overwrites NL)
  *(pData-1) = 0;

  // done
  *ppData = pData;
  return;
}
