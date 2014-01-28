//
// PHI function declarations
//

//
// HELPERS
//

// clears mem, debug
void* PHI_allocHelper(int size);

// hardware pump thread
void startHwPumpThread();

// set thread priority
BOOL PHI_setRealtimePrio(pthread_t thread);

//
// non-ANSI
//

char* __itoa(int value, char* result, int base);

//
// WEB ADMIN
//

void PHI_webadmin(int port, const char* wwwRoot);
char* PHI_processJson(char *pJsonReq);
void PHI_freeJsonReply(char* pJsonReply);

//
// LOGGING
//

int PHI_logInit(char *filename);
void PHI_logMsg(const char* tag, const char *fmt, ...);
void PHI_logClose(void);

//
// Phi Link
//

BOOL startPhiLink(char* ipAddr, int port);
void PHI_getStateSnapshot(PHI_STATE_PACKET *p);

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
// PHI STATE
//

void lock_state();
void unlock_state();

//
// Self tests
//

char* selfTest(int mode);

//
// MISC
//

char* initPeripherals();

BOOL initMotorCtrl();
void stopMotor(BYTE ctrlID, BYTE selIdx);
void stopAllMotors();
void flushMotorCmds();

void PHI_abortProcess(int rc);
void PHI_abortWithMsg(const char* msg);

UINT64 PHI_upTime();
double PHI_rand();
