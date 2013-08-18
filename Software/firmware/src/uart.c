
//
// Interface to UART (used for motors)
//

#include "phi.h"

// UART 0 file descriptor
static int uartFile = -1;

// At bootup, RPI pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively

BOOL uartInit() {

  // open UART 0
  //
  // The flags (defined in fcntl.h):
  //
  // Access modes (use 1 of these):
  //		O_RDONLY - Open for reading only.
  //		O_RDWR - Open for reading and writing.
  //		O_WRONLY - Open for writing only.
  //
  //	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
  //											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
  //											immediately with a failure status if the output can't be written immediately.
  //
  //	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.

  // open UART 0 in non blocking read/write mode
  
  uartFile = open(UART_DRIVER_NAME, O_RDWR | O_NOCTTY | O_NDELAY);		

  if (uartFile == -1) {
    LOG_ERR("uartInit: can't open UART device driver");
    return FALSE;
  }

  // configure UART 0
  //
  // The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
  //	Baud rate -  B1200, B2400, B4800, B9600, B19200, B38400, B57600,
  //               B115200, B230400, B460800, B500000, B576000, B921600, B1000000,
  //               B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
  //	CSIZE- CS5, CS6, CS7, CS8
  //	CLOCAL - Ignore modem status lines
  //	CREAD - Enable receiver
  //	IGNPAR - Ignore characters with parity errors
  //	ICRNL - Map CR to NL on input
  //	PARENB - Parity enable
  //	PARODD - Odd parity (else even)

  struct termios options;
  tcgetattr(uartFile, &options);

  options.c_cflag = MC_DEF_BAUD | CS8 | CLOCAL;           // set baud rate, RX disabled
  options.c_iflag = IGNPAR;                               // ignore parity (RX disabled)
  options.c_oflag = 0;
  options.c_lflag = 0;
  tcflush(uartFile, TCIFLUSH);
  tcsetattr(uartFile, TCSANOW, &options);

  return TRUE;
}

void uart_send(BYTE* pData, int dataLen) {
  
  if (uartFile != -1)
  {
    int count = write(uartFile, pData, dataLen);
    if (count < 0)
    {
      phi_abortWithMsg("UART TX error");
    }
  }
}

// this function is not used in Phi because we never read motor
// controller status because the controller TX lines can't be
// wire-ANDed together. (Code also never tested ...)

int uart_receive(void* pBuff, int buffLen) {

  // Note: Since we use O_NDELAY, this function will exit if there are
  // no receive bytes waiting (non blocking read). If we want to hold
  // waiting for input, use a while loop when calling this or change
  // flags.

  int numRead = 0;

  if (uartFile != -1)
  {
    numRead = read(uartFile, pBuff, buffLen);
    
    if (numRead < 0)
    {
      // an error occured (will also occur if there are no bytes says
      // sample ... hmmm)
      //
      // Note: maybe these two tests are only meaningful when
      // in blocking mode?
    }
    else if (numRead == 0)
    {
      // no data waiting
    }
  }

  return numRead;
}
  