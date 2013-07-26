//
// PHI function declarations
//

//
// HELPERS
//

// clears mem, debug
void* phi_allocHelper(int size);

//
// MISC
//

UINT64 phi_upTime();

void phi_abortProcess(int rc);
void phi_abortWithMsg(const char* msg);

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
// MISC HARDWARE
//

char* phi_initPeripherals();

BOOL uartInit();
void uart_send(BYTE* pData, int dataLen);
int uart_receive(void* pBuff, int buffLen);

BOOL spiInit();
void spi_send(int spiIdx, BYTE* pTx, int txLen);
void spi_receive(int spiIdx, BYTE* pRx, int rxLen);
void spi_sendreceive(int spiIdx, BYTE* pRx, int rxLen, BYTE* pTx, int txLen);

BOOL gyroInit();
void gyroGetData(float* pPitchDps, float* pYawDps, float* pRollDps);
INT8 gyroGetTemp();


