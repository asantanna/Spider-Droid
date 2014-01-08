//
// PHI function declarations
//

//
// HELPERS
//

// clears mem, debug
void* phi_allocHelper(int size);

// hardware pump thread
void startHwPumpThread();

// set thread priority
BOOL phi_setRealtimePrio(pthread_t thread);

//
// non-ANSI
//

char* __itoa(int value, char* result, int base);

//
// WEB ADMIN
//

void phi_webadmin(int port, const char* wwwRoot);
char* phi_processJson(char *pJsonReq);
void phi_freeJsonReply(char* pJsonReply);

//
// LOGGING
//

int phi_logInit(char *filename);
void phi_logMsg(const char* tag, const char *fmt, ...);
void phi_logClose(void);

//
// Phi Link
//

BOOL startPhiLink(char* ipAddr, int port);
void phi_getStateSnapshot(PHI_STATE_PACKET *p);

//
// I/O Helpers
//

int setNonBlocking(int fd);

//
// UART
//

BOOL uartInit();
// void uart_lock();
// void uart_unlock();
void uart_send(BYTE* pData, int dataLen);
int uart_receive(void* pBuff, int buffLen);

//
// SPI
//

BOOL spiInit();
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
// MISC
//

char* initPeripherals();

void phi_abortProcess(int rc);
void phi_abortWithMsg(const char* msg);

UINT64 phi_upTime();
double phi_rand();
