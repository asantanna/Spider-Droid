//
// PHI function declarations
//

//
// HELPERS
//

// clears mem, debug
void* allocHelper(int size);

// hardware pump thread
void startHwPumpThread();

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

BOOL startPhiLink(char* ipAddr, int port);
void getStateSnapshot(PHI_STATE_PACKET *p);

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

// gyro

void gyroGetDeltaDegrees(float* pPitchDelta, float* pYawDelta, float* pRollDelta);
INT8 gyroGetRawTemp();

//
// MISC
//

char* initPeripherals();

BOOL initMotorCtrl();
void stopMotor(BYTE ctrlID, BYTE selIdx);
void stopAllMotors();
void flushMotorCmds();

float getJointPos(BYTE adcIdx);

void abortProcess(int rc);
void abortWithMsg(const char* msg);

UINT64 phiUpTime();
double phiRand();
