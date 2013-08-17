//
// PhiLink interface to PHI
//
// Note: this interface is used for realtime control.
//       Diagnostics uses the JSON interface.

#include "phi.h"

// loop state

typedef enum {
  RX_BEGIN,
  RX_RECEIVING,
  TX_BEGIN,
  TX_SENDING,
    
} COMM_STATE;

// internal

void setLinkState(PHILINK_STATE state);
void* phi_link_loop(void* arg);
void initStatePacket(PHI_STATE_PACKET *p);

BOOL startPhiLink(char* ipAddr, int port) {
  BOOL rc = TRUE;
  int sock;

  if (g_phiLinkState != LINK_OFF) {
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

  // disable the "Nagle" algorithm so the TCP stack doesn't
  // wait to bunch up packets

  int flag = 1;

  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)) == -1) {
    LOG_ERR("startPhiLink: couldn't set TCP_NODELAY");
    goto error_exit;
  }

  // spawn thread to run the link
  //
  // Note2: thread is started detached and we don't keep track
  // of the pthread_t, thread is "fire and forget"

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
  pthread_create(&thread, &threadAttr,  &phi_link_loop, pArgs);

  // release thread attr because we don't use it
  pthread_attr_destroy(&threadAttr);

quick_exit:

  return rc;

error_exit:

  LOG_ERR("startPhiLink: failed");
  rc = FALSE;
  goto quick_exit;
  
}

void* phi_link_loop(void* arg)
{
  int sock;
  struct sockaddr_in phiServer;

  // say link started
  setLinkState(LINK_STARTED);
  
  // arg is PHILINK_ARGS - copy and free

  PHILINK_ARGS* pArgs = (PHILINK_ARGS*) arg;

  sock = pArgs -> sock;
  
  memset(&phiServer, 0, sizeof(phiServer));
  phiServer.sin_family = AF_INET;
  phiServer.sin_addr.s_addr = pArgs -> ipAddr;
  phiServer.sin_port = pArgs -> port;

  PHI_FREE(pArgs);
  
  // say link connecting
  setLinkState(LINK_CONNECTING);

  // connect (blocking)

  if (connect(sock, (struct sockaddr *) &phiServer, sizeof(phiServer)) < 0) {
    LOG_ERR("startPhiLink: connect failed");
    goto error_exit;
  }

  //
  // Connected to Phi Core
  //

  // say connected
  setLinkState(LINK_CONNECTED);

  // DEBUG
  LOG_INFO("** Phi Link connected");

  // set socket to non-blocking
  fcntl(sock, F_SETFL, O_NONBLOCK);

  // loop forever

  UINT64 lastLoopTime = phi_upTime();
  UINT64 currTime;
  
  char rxBuff[sizeof(PHI_CMD_PACKET)];
  char txBuff[sizeof(PHI_STATE_PACKET)];
  
  size_t totRead = 0;
  size_t totSent = 0;
  
  ssize_t nRec = 0;
  ssize_t nSnd = 0;
  
  COMM_STATE commState = RX_BEGIN;

  while (TRUE) {

    //
    // RECEIVE PHI CORE CMDS
    //

    switch (commState) {

      case RX_BEGIN:
        totRead = 0;
        commState = RX_RECEIVING;
        break;

      case RX_RECEIVING:

        // receive async (phi core initiates exchanges)
        nRec = recv(sock, rxBuff + totRead, sizeof(rxBuff) - totRead, 0);

        if (nRec == 0) {
          // other end closed socket
          LOG_FATAL("Phi link socket closed?");
          
        } else if (nRec < 0) {

          if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            // no data yet -- not an error
          } else {
            // error occurred
            LOG_FATAL("Phi link unexpected read fail with errno = %d", errno);
          }

        } else {
          
          // got some data
          
          totRead += nRec;

          if (totRead >= sizeof(PHI_CMD_PACKET)) {
            // receive complete - transmit status
            commState = TX_BEGIN;
          }
        }

        break;

      case TX_BEGIN:

        // set up state packet for sending
        initStatePacket((PHI_STATE_PACKET *) txBuff);
        
        totSent = 0;
        commState = TX_SENDING;
        break;

      case TX_SENDING:

        // send async
        nSnd = send(sock, txBuff, sizeof(txBuff) - totSent, 0);

        if (nSnd < 0) {

          if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            
            // not ready to send - not an error

          } else {
            // error occurred
            LOG_FATAL("Phi link unexpected write fail with errno = %d", errno);
          }

        } else {
          // some data was sent

          totSent += nSnd;

          if (totSent >= sizeof(PHI_STATE_PACKET)) {
            // send complete - go back to receiving
            commState = RX_BEGIN;
          }
        }

        break;
    }

  } // while
  
quick_exit:

  return NULL;

error_exit:

  close(sock);
  goto quick_exit;
}

void setLinkState(PHILINK_STATE state) {
  // copy to global
  g_phiLinkState = state;

  // change LED
  
  switch (state) {
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

void initStatePacket(PHI_STATE_PACKET *p) {

  int i;

  // sign
  memcpy(p -> sign, STAP_SIGN, sizeof(p->sign));

  // image
  memset(p -> image, 0, sizeof(p -> image));

  // gyro (return +/- percent of max reading)
  
  float pitchDelta, yawDelta, rollDelta;
  HAL_gyroGetDeltas(&pitchDelta, &yawDelta, &rollDelta);

  p -> gyro[0] = pitchDelta;
  p -> gyro[1] = yawDelta;
  p -> gyro[2] = rollDelta;

  // joint (motor) positions
  for (i = 0 ; i < COUNTOF(p -> joint) ; i++) {
    p -> joint[i] = HAL_getMotorPosition(i);

  }

  // temp
  TODO("Temperature not implemented");
}





/*
  while (TRUE) {

    // receive cmds (blocking)

    totRead = 0;

    while (totRead < sizeof(PHI_CMD_PACKET)) {

      nRec = recv(sock, rxBuff + totRead, sizeof(rxBuff) - totRead, 0);

      if (nRec <= 0) {
        // error occurred
        WARN("add error check")
        break;

      } else {

        // got some data
        totRead += nRec;
      }
    }

    // set up state packet for sending
    initStatePacket((PHI_STATE_PACKET *) txBuff);

    // send state (blocking)

    totSent = 0;

    while (totSent < sizeof(PHI_STATE_PACKET)) {

      nSnd = send(sock, txBuff + totSent, sizeof(txBuff) - totSent, 0);

      if (nSnd <= 0) {
        // error occurred
        WARN("add error check")
        break;

      } else {

        // sent some data
        totSent += nSnd;
      }
    }

  } // while
*/