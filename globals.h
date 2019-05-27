#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "socal/socal.h"
#include "socal/hps.h"
#include <stdbool.h>
#include <stdlib.h>

#define ALT_LWFPGASLVS_BASE ( 0xff200000 )
#define ALT_LWFPGASLVS_SPAN ( 0x200000 )
#define ALT_LWFPGASLVS_MASK ( ALT_LWFPGASLVS_SPAN-1 )
/* define heavyweight bridges */

#define ALT_HWFPGASLVS_BASE ( 0xc0000000 )
#define ALT_HWFPGASLVS_SPAN ( 0x20000000 )
#define ALT_HWFPGASLVS_MASK ( ALT_HWFPGASLVS_SPAN-1 )
//#define SHARED_SDRAM_SPAN ( 0x04000000 )
//#define SHARED_SDRAM_MASK ( SHARED_SDRAM_SPAN -1 )


//#define uOCRAM_BASE ( 0xc0080000 )
//#define uOCRAM_SPAN ( 0x00009600 )

//#define wOCRAM_BASE ( 0xc0000000 )
//#define wOCRAM_SPAN ( 0x0004b000 )

//#define xOCRAM_BASE ( 0xc0090000 )
//#define xOCRAM_SPAN ( 0x000012c0 )

// physical memory file descriptor
int fd_dev_mem = 0;

// memory-mapped peripherals
void   *virtual_lw_base =NULL;
void   *virtual_hw_base =NULL;
//void  *virtual_hw_base =NULL;
//void *virtual_sharedram_base = NULL;

//void *virtual_uOCRAM_base = NULL;
//void *virtual_wOCRAM_base = NULL;
//void *virtual_xOCRAM_base = NULL;

uint32_t   *hps_gpio     = NULL;

size_t hps_gpio_span = ALT_GPIO1_UB_ADDR - ALT_GPIO1_LB_ADDR + 1;
size_t hps_gpio_ofst = ALT_GPIO1_OFST;

uint32_t *fpga_accelerator = NULL; // lightweight bridge slave
//uint32_t *shared_sdram = NULL;
uint32_t *uOCRAM = NULL;
uint32_t *wOCRAM = NULL;
uint32_t *xOCRAM = NULL;

#endif
