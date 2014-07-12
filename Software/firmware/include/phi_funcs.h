//
// PHI function declarations
//

//
// HELPERS
//

// clears mem, debug
void* allocHelper(int size);

// set thread priority
BOOL setRealtimePrio(pthread_t thread);

//
// non-ANSI
//

char* __itoa(int value, char* result, int base);

//
// WEB ADMIN
//

void webAdmin(int port, const char* wwwRoot);
char* processJson(char *pJsonReq);
void freeJsonReply(char* pJsonReply);

//
// LOGGING
//

int logInit(char *filename);
void logMsg(const char* tag, const char *fmt, ...);
void logClose(void);

//
// Phi Link
//

void startHwPump();

BOOL startPhiLink(char* ipAddr, int port);
void getStateSnapshot(PHI_STATE_PACKET *p);
void writeToCmdSnapshot(PHI_CMD_PACKET* p);
void setSnapshotMotorVal(char* motorName, float powerVal);

//
// I/O Helpers
//

int setNonBlocking(int fd);

//
// UART
//

BOOL uart_init();
// void uart_lock();
// void uart_unlock();
void uart_send(BYTE* pData, int dataLen);
int uart_receive(void* pBuff, int buffLen);
void uart_flush();
void uart_discardInput();
void uart_discardOutput();
void uart_discardAll();

//
// SPI
//

BOOL spi_init();
// void spi_lock();
// void spi_unlock();
void spi_send(int spiIdx, BYTE* pTx, int txLen);
void spi_receive(int spiIdx, BYTE* pRx, int rxLen);
void spi_sendreceive(int spiIdx, BYTE* pTx, int txLen, BYTE* pRx, int rxLen);
void spi_exchange(int spiIdx, BYTE* pTx, BYTE* pRx, int dataLen);
void spi_exchange_ADC(int adcIdx, BYTE* pTx, BYTE* pRx, int dataLen);

//
// I2C
//

//
// PHI Snapshot
//

void lock_snapshot();
void unlock_snapshot();

//
// Self tests
//

char* selfTest(int mode);

//
// Gyro
//

BOOL gyroInit(BOOL bEnableFifo);
void gyroGetDeltaDegrees(float* pPitchDelta, float* pYawDelta, float* pRollDelta);
INT8 gyroGetRawTemp();

//
// Data Logging
//

DATALOG* dlog_create(char* pName, char* pUnit, int numElem, float epochSecs);
void dlog_addElem_withTime(DATALOG* pLog, UINT64 time, float data);
void dlog_addElem(DATALOG* pLog, float data);
float dlog_avg(DATALOG* pLog, int depth);
float dlog_predict(DATALOG* pLog, UINT64 time);
void dlog_getStats(DATALOG* pLog, int depth,
   float* pMinVal, float* pMaxVal, float* pAvgVal, float* pStdVal);

//
// Peripherals
//

void globalInit();
void globalShutdown();

char* initPeripherals();

void setControllerId(BYTE oldId, BYTE newId);
BOOL initMotorCtrl();
void setMotorPower(BYTE ctrlID, BYTE selIdx, BYTE power, BOOL bFwd);
void stopMotor(BYTE ctrlID, BYTE selIdx);
void stopAllMotors();
void flushMotorCmds();

float getJointPos(BYTE jointIdx);
UINT16 getRawJointPos(BYTE jointIdx);
float getJointPosByMotorID(BYTE ctrlID, BYTE selIdx);

BOOL gpio_init();

//
// Misc
//

void cmdline(int argc, char** argv);

void abortProcess(int rc);
void abortWithMsg(const char* msg);

UINT64 phiUpTime();
float phiRand();

void addToTimespec(struct timespec* pT1, time_t numSecs, DWORD numNano);
void addTimespecs(struct timespec* pT1, struct timespec* pT2);
void offsetTimespecMs(struct timespec* pT1, DWORD mS);

UINT32 getHostIP();


