/*
 * testsdram.c
 *
 *  Created on: Mar 27, 2019
 *      Author: snoeijs
 */

#include "testsdram.h"
#include <sys/mman.h>
#include "globals.h"
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "hps_0.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void open_physical_memory_device() {
    fd_dev_mem = open("/dev/mem", O_RDWR | O_SYNC);
    //printf("passed open\n");
    if(fd_dev_mem  == -1) {
        printf("ERROR: could not open \"/dev/mem\".\n");
        printf("    errno = %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void close_physical_memory_device() {
    close(fd_dev_mem);
}

void mmap_hps_peripherals() {
    hps_gpio = mmap(NULL, hps_gpio_span, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dev_mem, hps_gpio_ofst);
    if (hps_gpio == MAP_FAILED) {
        printf("Error: hps_gpio mmap() failed.\n");
        printf("    errno = %s\n", strerror(errno));
        close(fd_dev_mem);
        exit(EXIT_FAILURE);
    }
}

void munmap_hps_peripherals() {
    if (munmap(hps_gpio, hps_gpio_span) != 0) {
        printf("Error: hps_gpio munmap() failed\n");
        printf("    errno = %s\n", strerror(errno));
        close(fd_dev_mem);
        exit(EXIT_FAILURE);
    }

    hps_gpio = NULL;
}

void mmap_fpga_peripherals() {

    virtual_lw_base = mmap(NULL, ALT_LWFPGASLVS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd_dev_mem, ALT_LWFPGASLVS_BASE);
    printf("passed mapping of lw bridge\n");
    if (virtual_lw_base == MAP_FAILED) {
        printf("Error: h2f_lw_axi_master mmap() failed.\n");
        printf("    errno = %s\n", strerror(errno));
        close(fd_dev_mem);
       exit(EXIT_FAILURE);
    }
    //virtual_sharedram_base = mmap(NULL, SHARED_SDRAM_SPAN, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dev_mem, SHARED_SDRAM_BASE);
    //printf("passed mapping of SDRAM\n");
    //if(virtual_sharedram_base == MAP_FAILED) {
    //	printf("Error: h2f_axi_master mmap() failed.\n");
	//	printf("    errno = %s\n", strerror(errno));
	//	close(fd_dev_mem);
	//	exit(EXIT_FAILURE);
    //}

    virtual_hw_base = mmap(NULL, ALT_HWFPGASLVS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd_dev_mem, ALT_HWFPGASLVS_BASE);
    printf("passed mapping of hw bridge\n");
	if (virtual_lw_base == MAP_FAILED) {
		printf("Error: h2f_lw_axi_master mmap() failed.\n");
		printf("    errno = %s\n", strerror(errno));
		close(fd_dev_mem);
	   exit(EXIT_FAILURE);
	}

    fpga_accelerator = (uint32_t *) (virtual_lw_base + (( ACCELERATOR_0_AVALON_SLAVE_BASE) & (ALT_HWFPGASLVS_MASK))) ;
    uOCRAM = (uint32_t *) (virtual_hw_base + ((ACCELERATOR_0_U_OCRAM_BASE) & (ALT_HWFPGASLVS_MASK)));
    wOCRAM = (uint32_t *) (virtual_hw_base + ((ACCELERATOR_0_W_OCRAM_BASE) & (ALT_HWFPGASLVS_MASK)));
    xOCRAM = (uint32_t *) (virtual_hw_base + ((ACCELERATOR_0_X_OCRAM_BASE) & (ALT_HWFPGASLVS_MASK)));

}

void munmap_fpga_peripherals() {
    if (munmap(virtual_lw_base, ALT_LWFPGASLVS_SPAN) != 0) {
        printf("Error: h2f_lw_axi_master munmap() failed\n");
        printf("    errno = %s\n", strerror(errno));
        close(fd_dev_mem);
        exit(EXIT_FAILURE);
    }
    if (munmap(virtual_hw_base, ALT_HWFPGASLVS_SPAN) != 0) {
          printf("Error: h2f_lw_axi_master munmap() failed\n");
          printf("    errno = %s\n", strerror(errno));
          close(fd_dev_mem);
          exit(EXIT_FAILURE);
      }


    virtual_lw_base = NULL;
    virtual_hw_base = NULL;
    //virtual_sharedram_base = NULL;
}

void mmap_peripherals() {

    mmap_hps_peripherals();
    mmap_fpga_peripherals();
}

void munmap_peripherals() {
	fpga_accelerator = NULL;
	uOCRAM = NULL;
	wOCRAM = NULL;
	xOCRAM = NULL;
    munmap_hps_peripherals();
    munmap_fpga_peripherals();
}

uint32_t read_accelerator(uint32_t address){
	//printf("just before reading acccelerator's slave interface\n");
	uint32_t data = *(uint32_t*)(fpga_accelerator+address);
	//printf("reading 0x%x at address %d\n", data, address);
	return data;
}

void write_accelerator(uint32_t address, uint32_t data){
	//printf("just before writing to accelerator slave interface\n");
	*(uint32_t*) (fpga_accelerator+address) = data;
	//printf("writing %d at address %d\n", data, address);
}

uint32_t* get_uocram_base(){
	return uOCRAM;
}
uint32_t* get_wocram_base(){
	return wOCRAM;
}
uint32_t* get_xocram_base(){
	return xOCRAM;
}
uint32_t* get_fpga_accelerator_base(){
	return fpga_accelerator;
}

