
//
// An eventGate is an object similar to an Event in Win32.
// It is a simple mechanism like a gate.  When the gate is closed,
// all threads stop and wait at it.  When we open the gate, *all* threads
// are released.  We can additionally "pulse" the gate which behaves like open
// but the gate closes again before any threads get any execution time. 
//

#include "phi.h"

//
// Initialize eventGate
//

void eventGate_init(PHI_EVENT_GATE* pGate, BOOL bOpen) {

  // init mutex with default attrs
  pthread_mutex_init(&(pGate -> mutex), NULL);
  
  // init condition var with default attrs
  pthread_cond_init(&(pGate -> condVar), NULL);

  // set gate state
  pGate -> state = bOpen ? OPEN : CLOSED;
}

//
// Helper function for _open and _pulse.
//

static void eventGate_openHelper(PHI_EVENT_GATE* pGate, PHI_GATE_STATE newState) {
  
  // must lock mutex before touching state
  pthread_mutex_lock(&(pGate->mutex));
  
  // set state to req state
  pGate -> state = newState;

  // done with state - release mutex
  pthread_mutex_unlock(&(pGate->mutex));
  
  // release all waiters
  pthread_cond_broadcast(&(pGate->condVar));
}

//
// "Open" the eventGate ... waiters will NOT block when open
//

void eventGate_open(PHI_EVENT_GATE* pGate) {
  // release waiters and remain OPEN
  eventGate_openHelper(pGate, OPEN);
}

//
// "Pulse" the event gate, all waiters released but will be CLOSED again before
//  any waiters get to run.
//

void eventGate_pulse(PHI_EVENT_GATE* pGate) {
  // release waiters but stay CLOSED
  eventGate_openHelper(pGate, CLOSED);
}

//
// "Close" the eventGate ... waiters will block.
//

void eventGate_close(PHI_EVENT_GATE* pGate) {
  pthread_mutex_lock(&(pGate->mutex));
  pGate -> state = CLOSED;
  pthread_mutex_unlock(&(pGate->mutex));
}

//
// Wait on an eventGate.  If it is closed, caller will block until either
// somebody "opens" or "pulses" it or the timeout expires (if specified).
// If the gate is open, nothing happens.
//
// This function returns the same return values as pthread_cond_timedwait().
// If no timeouts occur, the function returns 0.  If a timeout occurs,
// the function will return ETIMEDOUT.
//

int eventGate_wait(PHI_EVENT_GATE* pGate, DWORD msTimeout) {

  int rc = 0;

  // must always lock mutex before checking/changing state or
  // calling any of the wait functions
  pthread_mutex_lock(&(pGate->mutex));

  if (pGate -> state == CLOSED) {

    // gate closed - block

    if (msTimeout == 0) {

      // wait with no timeout
      // note: call will ALWAYS atomically block AND release the mutex.
      // note 2: after waking, we own the mutex again.

      rc = pthread_cond_wait(&(pGate -> condVar), &(pGate -> mutex));

      // we have the mutex again here

    } else {

      // timeout specified - convert to abs time

      struct timespec wakeupTime;
      clock_gettime(CLOCK_REALTIME, &wakeupTime);
      offsetTimespecMs(&wakeupTime, msTimeout);
      
      // wait with timeout
      // note: call will ALWAYS atomically block AND release the mutex.
      // note 2: after waking, we own the mutex again.

      rc = pthread_cond_timedwait(&(pGate -> condVar), &(pGate -> mutex), &wakeupTime);
      
      // we have the mutex again here
    }
  }

  // we always have the mutex here
  pthread_mutex_unlock(&(pGate->mutex));
  
  return rc;
}


