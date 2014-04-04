
//
// PHI event gates. An eventGate is an object similar to an Event in Win32.
// See event_gate.h for more info.
//

typedef enum {OPEN, CLOSED} PHI_GATE_STATE;

typedef struct {
  pthread_mutex_t mutex;
  pthread_cond_t condVar;
  PHI_GATE_STATE state;

} PHI_EVENT_GATE;


void eventGate_init(PHI_EVENT_GATE* pGate, BOOL bOpen);
void eventGate_open(PHI_EVENT_GATE* pGate);
void eventGate_pulse(PHI_EVENT_GATE* pGate);
void eventGate_close(PHI_EVENT_GATE* pGate);
int eventGate_wait(PHI_EVENT_GATE* pGate, DWORD msTimeout);

