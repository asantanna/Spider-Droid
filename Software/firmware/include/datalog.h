
//
// Datalog Types
//

typedef struct {
  UINT64 time;
  float data;

} DATALOG_ELEM;

typedef struct {
  
  UINT32 sign;
  PHI_MUTEX_DECL(mutex);
  
  char* pName;
  char* pUnit;
  float epochSecs;
  int numElem;
  
  int currIdx;
  DATALOG_ELEM elem[];

} DATALOG;

