//
// PhiLink interface to PHI
//
// Note: this interface is used for realtime control.
//       Diagnostics uses the JSON interface.

#include "phi.h"

/*
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
*/

void setLinkState(PHILINK_STATE state);
void* phi_link_loop(void* arg);


BOOL startPhiLink(char* ipAddr, int port) {
  BOOL rc = TRUE;
  int sock;

  if (g_phiLinkState != LINK_OFF) {
    // already started
    LOG_ERR("startPhiLink: already started, you monkey!");
    goto error_exit;
  }

  // create socket 

  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    LOG_ERR("startPhiLink: Failed to create socket");
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

  // set socket to non-blocking
  fcntl(sock, F_SETFL, O_NONBLOCK);

  // loop forever

  UINT64 lastLoopTime = phi_upTime();
  UINT64 currTime;
  
  char rxBuff[sizeof(PHI_CMD_PACKET)];
  char txBuff[sizeof(PHI_STATE_PACKET)];
  
  size_t totRead = 0;
  size_t totSent = 0;
  
  int rxNum = 0;
  int txNum = 0;
  
  ssize_t nRec = 0;
  ssize_t nSnd = 0;

  RX_STATE rxState = RX_IDLE;
  TX_STATE txState = TX_IDLE;

  while (TRUE) {
    
    //
    // RECEIVE PHI CORE CMDS
    //

    switch (rxState) {
      
      case RX_IDLE:
        totRead = 0;
        rxNum ++;
        rxState = RX_RECEIVING;
        break;

      case RX_RECEIVING:
        
        // receive async (phi core initiates exchanges)
        nRec = recv(sock, rxBuff + totRead, sizeof(rxBuff) - totRead, 0);
        
        if (nRec == 0) {
          // other end closed socket
          // Note: this is an error with PhiLink
          // TODO : do something error

        } else if (nRec < 0) {

          if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            // no data yet -- not an error
          } else {
            // error occurred
            // TODO : do something error            
          }

        } else {
          // got some data
          totRead += nRec;

          if (totRead >= sizeof(PHI_CMD_PACKET)) {
            rxState = RX_CMD_COMPLETE;
          }
        }

        break;

      case RX_CMD_COMPLETE:
        // execute commands
        // TODO - do nothing for now
        rxState = RX_IDLE;
        break;
    }

    //
    // TRANSMIT SENSOR STATE
    //

    switch (txState) {

      case TX_IDLE:
        if ((txNum < rxNum) && (totRead > 0)) {
          txState = TX_BEGIN;
        }
        break;

      case TX_BEGIN:
        totSent = 0;
        break;

      case TX_SENDING:

        // send async
        nSnd = send(sock, txBuff, sizeof(txBuff) - totSent, 0);

        if (nSnd < 0) {

          if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            // not ready to send - not an error

          } else {
            // error occurred
            // TODO - do stuff about the error
          }

        } else {
          // some data was sent

          totSent += nSnd;

          if (totSent >= sizeof(PHI_STATE_PACKET)) {
            txState = TX_COMPLETED;
            
          }
        }

        break;

      case TX_COMPLETED:
        txNum ++;
        txState = TX_IDLE;
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



