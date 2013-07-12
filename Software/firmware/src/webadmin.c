//
// PHI web admin for diagnostic purposes
//
// Note: this file started off as the "nweb tiny webserver"
//       by Nigel Griffiths (nag@uk.ibm.com)
//
//       http://www.ibm.com/developerworks/systems/library/es-nweb/index.html
//
// Modified to:
//
// 1) run as a single process - no forking.  Only accepts one
// connection at a time which is enough for admin purposes and
// simplifies access to PHI internals
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
// HTTP HEAD command: same as GET but return headers only (no data)
//
// HTTP POST command: similar to GET with following differences
//
//      a) request contains data for the server
//      b) headers specify size and type with "Content-Type:" and
//         "Content-Length:"
//      c) Parameters go in body with form submissions, it has the
//         same format as the part after the "?" in the GET command.
//

//
// TODO: use PHI log mechanism
// TODO: fix forking
// TODO: fix return codes because no longer forking
// TODO: read through all code
// TODO: print thread id in "starting message"
// TODO: parse URL encoding ... it's easy and will save pain later
//

#include "phi.h"

#define NWEB_VERSION    23
#define BUFSIZE         8096
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
	char *ext;
	char *filetype;
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

char wa_response_forbidden[] =
  "HTTP/1.1 403 Forbidden\n"
  "Content-Length: 185\n"
  "Connection: close\n"
  "Content-Type: text/html\n"
  "\n"
  "<html>\n"
  "<head>\n"
  "<title>403 Forbidden</title>\n"
  "</head>\n"
  "<body>\n"
  "<h1>Forbidden</h1>\n"
  "The requested URL, file type or operation is not allowed on this simple static file webserver.\n"
  "</body>\n"
  "</html>\n";

char wa_response_notfound[] =
  "HTTP/1.1 404 Not Found\n"
  "Content-Length: 136\n"
  "Connection: close\n"
  "Content-Type: text/html\n"
  "\n"
  "<html>\n"
  "<head>\n"
  "<title>404 Not Found</title>\n"
  "</head>\n"
  "<body>\n"
  "<h1>Not Found</h1>\n"
  "The requested URL was not found on this server.\n"
  "</body>\n"
  "</html>\n";

char wa_response_ok_header[] =
  "HTTP/1.1 200 OK\n"
  "Server: Phi Web Admin %s\n"
  "Content-Length: %ld\n"
  "Connection: close\n"
  "Content-Type: %s\n"
  "\n";

void wa_process_web_request(int socketfd, int hit)
{
  int j, pagefd, buffLen;
  long i, sock_numRead, len;
  char* fstr;
  char buffer[BUFSIZE+1];

  // read Web request in one go
  sock_numRead = read(socketfd, buffer, BUFSIZE);

  // read failure stop now
  if (sock_numRead == 0 || sock_numRead == -1) {
    write(socketfd, wa_response_forbidden, strlen(wa_response_forbidden));
    LOG_ERR("failed to read browser request");
    return;
  }

  if (sock_numRead > 0 && sock_numRead < BUFSIZE) {
    // return code is valid chars
    buffer[sock_numRead]=0;
  }
  else {
    // error
    LOG_ERR("GET request too big - size=%d", sock_numRead);
    buffer[0]=0;
    return;
  }

  // remove CF and LF characters
  for (i=0 ; i<sock_numRead ; i++) {
    if (buffer[i] == '\r' || buffer[i] == '\n') {
      buffer[i]='*';
    }
  }

  LOG_INFO("webadmin: hit=%d, request follows:\n%s\n", hit, buffer);

  if (strncasecmp(buffer, "GET ", 4)) {
    write(socketfd, wa_response_forbidden, strlen(wa_response_forbidden));
    LOG_ERR("Only simple GET operation supported");
    return;
  }

  // null terminate after the second space to ignore extra stuff
  for (i=4 ; i<BUFSIZE ; i++) {
    if (buffer[i] == ' ') {
      // string is "GET URL " +lots of other stuff
      buffer[i] = 0;
      break;
    }
  }

  // check for illegal parent directory use
  for (j=0 ; j<i-1 ; j++) {
    if (buffer[j] == '.' && buffer[j+1] == '.') {
      write(socketfd, wa_response_forbidden, strlen(wa_response_forbidden));
      LOG_ERR("Parent directory (..) path names not supported");
      return;
    }
  }

  // convert no filename to index file
  if (!strncasecmp(&buffer[0], "GET /\0", 6)) {
    strcpy(buffer, "GET /index.html");
  }

  // work out the file type and check we support it

  buffLen = strlen(buffer);
  fstr = (char *)0;

  for (i=0 ; wa_extensions[i].ext != 0 ; i++) {
    len = strlen(wa_extensions[i].ext);
    if (!strncmp(&buffer[buffLen-len], wa_extensions[i].ext, len)) {
      fstr = wa_extensions[i].filetype;
      break;
    }
  }
  
  if (fstr == 0) {
    write(socketfd, wa_response_forbidden, strlen(wa_response_forbidden));
    LOG_ERR("file extension type not supported (%s)", buffer);
    return;
  }

  // open the file for reading
  if ((pagefd = open(&buffer[5], O_RDONLY)) == -1) {
    write(socketfd, wa_response_notfound, strlen(wa_response_notfound));
    LOG_ERR("webadmin failed to open file '%s'", &buffer[5]);
    return;
  }

  LOG_INFO("webadmin: page requested='%s'", &buffer[5]);

  // lseek to the file end to find the length
  len = (long) lseek(pagefd, (off_t)0, SEEK_END);

  // lseek back to the file start ready for reading
  lseek(pagefd, (off_t)0, SEEK_SET);

  // send header plus blank line
  sprintf(buffer, wa_response_ok_header, PHI_VERSION, len, fstr);
  LOG_INFO("webadmin: Header='%s'", buffer);
  write(socketfd, buffer, strlen(buffer));

  // send file in 8KB blocks - last block may be smaller
  while (	(len = read(pagefd, buffer, BUFSIZE)) > 0 ) {
    write(socketfd, buffer, len);
  }
}

// entry point of web admin - never returns

void phi_webadmin(int port, const char* wwwRoot)
{
  int i, pid, listenfd, socketfd, hit;
  socklen_t length;
  struct sockaddr_in cli_addr;
  struct sockaddr_in serv_addr;

  // init to zeros
  memset(&cli_addr, 0, sizeof(cli_addr));
  memset(&serv_addr, 0, sizeof(serv_addr));

  // debug
  LOG_INFO("PHI webadmin starting - wwwRoot='%s'", (char*) wwwRoot);

  // change dir to www doc root
  if (chdir(wwwRoot) == -1) { 
    LOG_ERR("can't change to directory %s\n", wwwRoot);
    phi_abortProcess(-1);
  }

  // setup the network socket
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    LOG_ERR("system call failed: socket");
    phi_abortProcess(-1);
  }

  if (port < 0 || port > 60000) {
    LOG_ERR("Invalid port number (try 1 -> 60000)", "port given=", port);
    phi_abortProcess(-1);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);

  if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <0) {
    LOG_ERR("system call failed: bind");
    phi_abortProcess(-1);
  }

  // debug

  UINT32 ipAddr = phi_getHostIP();

  printf("listening on address %lu.%lu.%lu.%lu on port %d\n",
         ipAddr & 0xff, (ipAddr >> 8) & 0xff,
         (ipAddr >> 16) & 0xff,(ipAddr >> 24) & 0xff,
         port);

  // listen (enable receive)
  
  if ( listen(listenfd, 64) <0) {
    LOG_ERR("system call failed: listen");
    phi_abortProcess(-1);
  }

  // go into infinite loop accepting calls

  for (hit = 1 ; ; hit++) {

    length = sizeof(cli_addr);
    if ((socketfd = accept(listenfd, (struct sockaddr *) &cli_addr, &length)) < 0) {
      LOG_ERR("system call failed: accept");
      phi_abortProcess(-1);
    }

    // process web request
    wa_process_web_request(socketfd, hit);

    // allow socket to drain before closing
    sleep(1);

    // close the socket
    close(socketfd);
  }
}

//
// This is based on Arduino code in ALS-Libs.  Hence the use of statics
// and other limitations due to the small amount of memory avail on the
// Arduino.
//
// Limitations of parseRequest:
//
// 1) Maximum string is MAX_TOKEN_SIZE
// 2) "%" escapes not processed

typedef struct {
  char filePath[MAX_TOKEN_SIZE+1];
} PHI_PARSED_HTTP;

void wa_parseHttpRequest(char* pReq, PHI_PARSED_HTTP* pParsed) {

  enum state_t {READ_METHOD, READ_PATH, READ_PARAMS, READ_HTTP_HEADERS, DONE};
  state_t state = READ_METHOD;
  m_bAbort = false;
  m_status = OK;
  char c;

  // add space for term zero
  char token[MAX_TOKEN_SIZE+1];

  // clear parsed response
  memset(pParsed, 0, sizeof(*pParsed));

  // free strings in params from last time
  freeParams();

  while ((m_bAbort == false) &&
         (state != DONE)) {

    // HACK TODO remove this if
    if (TRUE) {

      switch (state) {
        case READ_METHOD:
          readToken(client, token);
          if (strcasecmp(token, HTTP_METHOD_HEAD_STR) == 0) {
            m_httpMethod = ALSWU_HTTP_HEAD;
          } else if (strcasecmp(token, HTTP_METHOD_GET_STR) == 0) {
            m_httpMethod = ALSWU_HTTP_GET;
          } else if (strcasecmp(token, HTTP_METHOD_POST_STR) == 0) {
            m_httpMethod = ALSWU_HTTP_POST;
          } else {
            // unknown
            m_bAbort = true;
            m_status = BAD_METHOD;
          }
          state = SKIP_PATH;
          break;

        case READ_PATH:
          // token will break at '?' or ws
          c = readToken(client, token);
          // save
          strncpy(pParsed -> filePath, token, MAX_TOKEN_SIZE);
          if (c == '?') {
            // have params
            state = READ_PARAMS;
          } else {
            // no params - discard HTTP version
            readToEol(client);
            state = READ_HTTP_HEADERS;
          }
          break;

        case READ_PARAMS:
          // read param
          c = readParam(client, token);
          if (c != '&') {
            // no more params - discard HTTP version
            readToEol(client);
            state = READ_HTTP_HEADERS;
          }
          // stay in same state
          break;

        case READ_HTTP_HEADERS:
          // read header name
          c = readToken(client, token);
          if (c == '\n') {
            // blank line - done
            state = DONE;
            break;
          }
          if (strcasecmp(token, HTTP_HDR_LEN) == 0) {
            // header is ContentLength:
            // read value
            readToken(client, token);
            m_bodyLen = atoi(token);
          } else {
            // don't know this header - discard
            readToken(client, token);
          }
          break;
      }
    } else {
      // TODO Think about this and fix
      
      // if client not available (no chars to read), delay minimally in
      // case the delay() function actually does something more than
      // delay - this is probably not necessary
      delay(1);
    }
  }
}

// reads up to a delimiter and returns zero-term string in token
// notes:
//		1) if too big, string is truncated
//		3) set m_bAbort if error
//		4) returns delimiter as return code (if error, then -1)
//

char ALS_WebUtils::readToken(WiFiClient client, char* token, bool bBreakOnDot) {

  int idx = 0;
  bool bSkippingWs = true;
  char c;

  // just in case
  *token = 0;

  while (true) {

    if ((c = waitRead(client)) == -1) {
      // error - status already set
      return -1;
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
      bSkippingWs = false;
      if ((c == '=') || (c == '&') || (c == ':') || (c == '?') || (c == '\n')) {
        // these end token too
        break;
      } else if (bBreakOnDot && (c == '.')) {
        // it's a '.' and we were asked to break on it (reading IP addr)
        break;
      }
    }

    // save char in token
    token[idx] = c;

    // advance to next if not too much
    if (idx < MAX_TOKEN_SIZE) {
      // this can increment to MAX_TOKEN_SIZE but buffer has
      // one extra char which will overwrite the char with a zero
      idx++;
    }
  }

  // add term zero
  token[idx] = 0;

  // done
  return c;
}

void ALS_WebUtils::readToEol(WiFiClient client) {

  char c;

  while (true) {

    if ((c = waitRead(client)) == -1) {
      // error - status already set
      break;
    }

    if (c == '\n') {
      // done
      break;
    }
  }

  // done
  return;
}

char ALS_WebUtils::readParam(WiFiClient client, char* token) {

  char c = readToken(client, token);
  uint32_t ip = 0;

  if (c != '=') {
    // equal sign missing
    m_bAbort = true;
    m_status = BAD_PARAM_SPEC;
    return -1;
  }

  int idx = atoi(token+1);
  switch (*token) {
    case PARAM_INT:
      c = readToken(client, token);
      if (idx < ALSWU_MAX_INT_PARAMS) {
        m_paramInt[idx] = atoi(token);
      }
      break;

    case PARAM_FLOAT:
      c = readToken(client, token);
      if (idx < ALSWU_MAX_FLOAT_PARAMS) {
        m_paramFloat[idx] = atof(token);
      }
      break;

    case PARAM_IP:
      for (int i = 0 ; i < 4 ; i++) {
        // read token but ask to break on '.'
        c = readToken(client, token, true);
        ip = (ip << 8) + strtoul(token, NULL, 10);
        if (i < 3) {
          if (c != '.') {
            // missing period
            m_bAbort = true;
            m_status = BAD_IP_ADDR;
            return -1;
          }
        }
      }

      if (idx < ALSWU_MAX_IP_PARAMS) {
        m_paramIP[idx] = ip;
      }
      break;

    case PARAM_STR:
      c = readToken(client, token);
      if (idx < ALSWU_MAX_STR_PARAMS) {
        m_paramStr[idx] = strdup(token);
      }
      break;

    default:
      // unknown spec type
      m_bAbort = true;
      m_status = BAD_PARAM_SPEC;
      return -1;
  }

  // done
  return c;
}

char ALS_WebUtils::waitRead(WiFiClient client) {

  int b;

  while(client.connected()) {
    if (client.available()) {
      // available() makes state machine tick on Hydrogen
      if ((b = client.read()) != -1) {
        // have char
        return (char) b;
      }
      // TODO: add timeout or will just disconnect?
    } // available()
  } // connected()

  // client disconnected
  m_bAbort = true;
  m_status = CLIENT_DISCONNECTED;
  return -1;
}
