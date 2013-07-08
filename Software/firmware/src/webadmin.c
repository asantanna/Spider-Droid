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

//
// TODO: use PHI log mechanism
// TODO: fix forking
// TODO: fix return codes because no longer forking
// TODO: read through all code
// TODO: print thread id in "starting message"
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "phi.h"

#define NWEB_VERSION    23
#define BUFSIZE         8096
#define ERROR           42
#define LOG             44
#define FORBIDDEN       403
#define NOTFOUND        404

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
                              
void wa_logger(int type, const char *s1, const char *s2, int sock_or_val)
{
  int fd;
  char logbuffer[BUFSIZE];

  switch (type) {
    case ERROR:
      sprintf(logbuffer, "ERROR: %s:%s errno=%d, pid=%d", s1, s2, errno, getpid()); 
      break;

    case FORBIDDEN:
      write(sock_or_val, wa_response_forbidden, strlen(wa_response_forbidden));
      break;

    case NOTFOUND: 
      write(sock_or_val, wa_response_notfound, strlen(wa_response_notfound));
      sprintf(logbuffer, "NOT FOUND: %s:%s", s1, s2); 
      break;

    case LOG:
      sprintf(logbuffer, "INFO: %s:%s:%d", s1, s2, sock_or_val);
      break;
  }

  // No checks here, nothing can be done with a failure anyway

  if ((fd = open("phi_admin.log", O_CREAT | O_WRONLY | O_APPEND, 0644)) >= 0) {
    write(fd, logbuffer, strlen(logbuffer)); 
    write(fd, "\n", 1);      
    close(fd);
  }
}

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
    wa_logger(FORBIDDEN, "failed to read browser request", "", socketfd);
    return;
  }

  if (sock_numRead > 0 && sock_numRead < BUFSIZE) {
    // return code is valid chars
    buffer[sock_numRead]=0;
  }
  else {
    // error
    wa_logger(ERROR, "GET request too big", 0, 0);
    buffer[0]=0;
    return;
  }

  // remove CF and LF characters
  for (i=0 ; i<sock_numRead ; i++) {
    if (buffer[i] == '\r' || buffer[i] == '\n') {
      buffer[i]='*';
    }
  }

  wa_logger(LOG, "request", buffer, hit);

  if (strncmp(buffer, "GET ", 4) && strncmp(buffer, "get ", 4) ) {
    wa_logger(FORBIDDEN, "Only simple GET operation supported", buffer, socketfd);
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
      wa_logger(FORBIDDEN, "Parent directory (..) path names not supported", buffer, socketfd);
      return;
    }
  }

  // convert no filename to index file
  if ( !strncmp(&buffer[0], "GET /\0", 6) || !strncmp(&buffer[0], "get /\0", 6) ) {
    strcpy(buffer, "GET /index.html");
  }

  // work out the file type and check we support it

  buffLen = strlen(buffer);
  fstr = (char *)0;

  for (i=0 ; wa_extensions[i].ext != 0 ; i++) {
    len = strlen(wa_extensions[i].ext);
    if ( !strncmp(&buffer[buffLen-len], wa_extensions[i].ext, len)) {
      fstr = wa_extensions[i].filetype;
      break;
    }
  }
  
  if (fstr == 0) {
    wa_logger(FORBIDDEN, "file extension type not supported", buffer, socketfd);
    return;
  }

  // open the file for reading
  if ((pagefd = open(&buffer[5], O_RDONLY)) == -1) {
    wa_logger(NOTFOUND, "failed to open file", &buffer[5], socketfd);
    return;
  }

  wa_logger(LOG, "SEND", &buffer[5], hit);

  // lseek to the file end to find the length
  len = (long) lseek(pagefd, (off_t)0, SEEK_END);

  // lseek back to the file start ready for reading
  lseek(pagefd, (off_t)0, SEEK_SET);

  // send header plus blank line
  sprintf(buffer, wa_response_ok_header, PHI_VERSION, len, fstr);
  wa_logger(LOG, "Header", buffer, hit);
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
  wa_logger(LOG, "PHI Admin starting ", (char*) wwwRoot, getpid());

  // change dir to www doc root
  if (chdir(wwwRoot) == -1) { 
    wa_logger(ERROR, "can't change to directory %s\n", wwwRoot, -1);
    phi_abort_process(-1);
  }

  // setup the network socket
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    wa_logger(ERROR, "system call", "socket", 0);
    phi_abort_process(-1);
  }

  if (port < 0 || port > 60000) {
    wa_logger(ERROR, "Invalid port number (try 1 -> 60000)", "port given=", port);
    phi_abort_process(-1);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);

  if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <0) {
    wa_logger(ERROR, "system call", "bind", 0);
    phi_abort_process(-1);
  }


  if ( listen(listenfd, 64) <0) {
    wa_logger(ERROR, "system call", "listen", 0);
    phi_abort_process(-1);
  }

  // debug
  wa_logger(LOG, "PHI Admin going into accept loop", 0, 0);

  // go into infinite loop accepting calls

  for (hit = 1 ; ; hit++) {

    length = sizeof(cli_addr);
    if ((socketfd = accept(listenfd, (struct sockaddr *) &cli_addr, &length)) < 0) {
      wa_logger(ERROR, "system call", "accept", 0);
      phi_abort_process(-1);
    }

    // process web request
    wa_process_web_request(socketfd, hit);

    // allow socket to drain before closing
    sleep(1);

    // close the socket
    close(socketfd);
  }
}
