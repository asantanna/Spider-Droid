//
// PhiLink interface to PHI
//
// Note: this interface is used for realtime control.
//       Diagnostics uses the JSON interface.
//
// PhiLink is started by issuing a JSON "startPhiLink" command through
// the diagnostics interface.  Phi then connects to the server with the
// address specified.  The reason it is done this way instead of simply
// listening for connections like the diagnostic interface is that this
// method allows future changes to a UDP based link without major
// modifications since the "startPhiLink" command provides a target address.

#include "phi.h"

TODO("think about GYRO AND ACCEL ELEM");
// - should be split into pos and neg?
// - should return change
// - is gyro returning change? if so, change name

// internal

void setLinkStatus(PHILINK_STATUS status);
void* phiLink_loop(void* arg);

BOOL startPhiLink(char* ipAddr, int port) {
  BOOL rc = TRUE;
  int sock;

  if (g_phiLinkStatus != LINK_OFF) {
    // already started
    LOG_ERR("startPhiLink: already started, you monkey!");
    goto error_exit;
  }

  // DEBUG
  LOG_INFO("** Starting Phi Link");

  // create socket 

  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    LOG_ERR("startPhiLink: Failed to create socket");
    goto error_exit;
  }

  // disable the "Nagle" algorithm so the TCP stack
  // doesn't wait to bunch up packets

  int flag = 1;

  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)) == -1) {
    LOG_ERR("startPhiLink: couldn't set TCP_NODELAY");
    goto error_exit;
  }

  //
  // Spawn thread to run the PhiLink
  //
  // Note2: thread is started detached and we don't keep track
  // of the pthread_t, thread is "fire and forget"
  //

  pthread_t thread;
  pthread_attr_t threadAttr;

  // init default attrs
  PHI_ZERO(threadAttr);
  pthread_attr_init(&threadAttr);

  // request detached thread
  pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

  // alloc args
  PHILINK_ARGS* pArgs = PHI_ALLOC(PHILINK_ARGS);
  pArgs -> sock = sock;
  pArgs -> ipAddr = inet_addr(ipAddr);
  pArgs -> port = htons(port);

  // create thread
  pthread_create(&thread, &threadAttr,  &phiLink_loop, pArgs);

  // release thread attr because we don't use it
  pthread_attr_destroy(&threadAttr);

  // increase priority to make philink more even

  if (setRealtimePrio(thread) == FALSE) {
      // not fatal
    LOG_ERR("phiLink set_realtime_priority() failed!");
  }
  
  // sleep a bit to allow thread to run (1/4 sec)
  usleep(1e6/4);

quick_exit:

  return rc;

error_exit:

  LOG_ERR("startPhiLink: failed");
  rc = FALSE;
  goto quick_exit;
  
}

static UINT64 phiLink_loopStart_save = 0;

void* phiLink_loop(void* arg)
{
  int sock;
  struct sockaddr_in phiServer;

  // say link started
  setLinkStatus(LINK_STARTED);
  
  // arg is PHILINK_ARGS - copy and free

  PHILINK_ARGS* pArgs = (PHILINK_ARGS*) arg;

  sock = pArgs -> sock;
  
  memset(&phiServer, 0, sizeof(phiServer));
  phiServer.sin_family = AF_INET;
  phiServer.sin_addr.s_addr = pArgs -> ipAddr;
  phiServer.sin_port = pArgs -> port;

  PHI_FREE(pArgs);
  
  // say link connecting
  setLinkStatus(LINK_CONNECTING);

  // connect (blocking)

  if (connect(sock, (struct sockaddr *) &phiServer, sizeof(phiServer)) < 0) {
    LOG_ERR("startPhiLink: connect failed");
    goto error_exit;
  }

  //
  // Connected to Phi Core
  //

  // say connected
  setLinkStatus(LINK_CONNECTED);

  // DEBUG
  LOG_INFO("** Phi Link connected");

  //
  // PHILINK comm loop (loop forever)
  //
  
  static char rxBuff[sizeof(PHI_CMD_PACKET)];
  static char txBuff[sizeof(PHI_STATE_PACKET)];
  
  size_t totRead = 0;
  size_t totSent = 0;
  
  ssize_t nRec = 0;
  ssize_t nSnd = 0;

  while (TRUE) {

    // receive cmds (blocking)

    totRead = 0;

    while (totRead < sizeof(PHI_CMD_PACKET)) {

      nRec = recv(sock, rxBuff + totRead, sizeof(rxBuff) - totRead, 0);

      if (nRec <= 0) {
        // error occurred
        perror("Phi link: recv() fail: ");
        LOG_FATAL("Phi link: recv() fail, errno = %d", errno);
        break;

      } else {

        // got some data
        totRead += nRec;
      }
    }
    
    // new loop start
    // Note: loop is considered to start once a cmd packet is received
    UINT64 usec_loopStart = phiUpTime();

    // write received commands to command snapshot
    // note: this will trigger the event gate for update
    writeToCmdSnapshot((PHI_CMD_PACKET *) rxBuff);

    // get state snapshot to create reply
    getStateSnapshot((PHI_STATE_PACKET *) txBuff);

    // send state (blocking)

    totSent = 0;

    while (totSent < sizeof(PHI_STATE_PACKET)) {

      nSnd = send(sock, txBuff + totSent, sizeof(txBuff) - totSent, 0);

      if (nSnd <= 0) {
        // error occurred
        LOG_FATAL("Phi link: send() fail, errno = %d", errno);
        break;

      } else {

        // sent some data
        totSent += nSnd;
      }
    }
    
    // log loop period

    if (phiLink_loopStart_save != 0) {
      dlog_addElem(g_pDlog_phiLink_period, usec_loopStart-phiLink_loopStart_save);
    }

    phiLink_loopStart_save = usec_loopStart;

  } // while
  
quick_exit:

  return NULL;

error_exit:

  close(sock);
  goto quick_exit;
}

void setLinkStatus(PHILINK_STATUS status) {
  // copy to global
  g_phiLinkStatus = status;

  // change LED
  // TODO
  
  switch (status) {
    case LINK_OFF:
      break;
    case LINK_STARTED:
      break;
    case LINK_CONNECTING:
      break;
    case LINK_CONNECTED:
      break;
    case LINK_ERROR:
      break;
    case LINK_CLOSED:
      break;
  }
}


