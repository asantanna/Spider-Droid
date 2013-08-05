//
// PhiLink interface to PHI
//
// Note: this interface is used for realtime control.
//       Diagnostics uses the JSON interface.

#include "phi.h"

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

  // loop forever

  UINT64 lastLoopTime = phi_upTime();
  UINT64 currTime;
  
  char rxBuff[sizeof(PHI_CMD_PACKET)];
  char txBuff[sizeof(PHI_STATE_PACKET)];
  
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
        // TODO : do something error            
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
        // TODO : do something error            
        break;

      } else {

        // sent some data
        totSent += nSnd;
      }
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

  // sign
  memcpy(p -> sign, STAP_SIGN, sizeof(p->sign));

  // image
  memset(p -> image, 0, sizeof(p -> image));

  // gyro (return +/- percent of max reading)
  
  float pitchDps, yawDps, rollDps;
  gyroGetData(&pitchDps, &yawDps, &rollDps);

  p -> gyro[0] = (INT16) (pitchDps / 250.0F);
  p -> gyro[1] = (INT16) (yawDps / 250.0F);
  p -> gyro[2] = (INT16) (rollDps / 250.0F);
}





