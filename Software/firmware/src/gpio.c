//
// Raspberry GPIO access routines
//
// Based on:
//    How to access GPIO registers from C-code on the Raspberry-Pi
//    Example program
//    15-January-2012
//    Dom and Gert
//    Revised: 15-Feb-2013
//
// See library source at Spider-Droid/Software/third-party/RaspberryPi-GPIO
// to see how to do weirder things like pull-ups, alt functions, etc.
//

#include "phi.h"

#include <sys/mman.h>

// Access from ARM Running Linux

#define GPIO_BASE             GPFSEL0
#define PAGE_SIZE             (4*1024)
#define BLOCK_SIZE            (4*1024)

BOOL gpio_init()
{
  // open driver /dev/mem
  
  int mem_fd;

  if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
    LOG_ERR("can't open /dev/mem");
    return FALSE;
  }

  // memory map GPIO
  
  void *gpio_map;
  
  gpio_map = mmap(
    NULL,                     // Any adddress in our space will do
    BLOCK_SIZE,               // Map length
    PROT_READ|PROT_WRITE,     // Enable reading & writting to mapped memory
    MAP_SHARED,               // Shared with other processes
    mem_fd,                   // File to map
    GPIO_BASE                 // Offset to GPIO peripheral
    );
  
  // no need to keep mem_fd open after mmap
  close(mem_fd);

  if (gpio_map == MAP_FAILED) {
    // fail - errno also set
    LOG_ERR("mmap error %d - errno=%d", (int) gpio_map, errno);
    return FALSE;
  }

  // Always use volatile pointer!
  g_pGpio = (volatile DWORD *) gpio_map;

  // success
  return TRUE;
}

/*


void examples()
{
  int g,rep;

  // Set up gpi pointer for direct register access
  setup_io();

  // Switch GPIO 7..11 to output mode

  //
  // You are about to change the GPIO settings of your computer.
  // Mess this up and it will stop working!
  // It might be a good idea to 'sync' before running this program
  // so at least you still have your code changes written to the SD-card!
  //

  // Set GPIO pins 7-11 to output
  for (g=7; g<=11; g++)
  {
    INP_GPIO(g); // must use INP_GPIO before we can use OUT_GPIO
    OUT_GPIO(g);
  }

  for (rep=0; rep<10; rep++)
  {
    for (g=7; g<=11; g++)
    {
      GPIO_SET = 1<<g;
      sleep(1);
    }
    for (g=7; g<=11; g++)
    {
      GPIO_CLR = 1<<g;
      sleep(1);
    }
  }

  return 0;

} // main

*/
