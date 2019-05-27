/*
 * testsdram.h
 *
 *  Created on: Mar 27, 2019
 *      Author: snoeijs
 */

#ifndef TESTSDRAM_H_
#define TESTSDRAM_H_


#include "socal/socal.h"
void open_physical_memory_device();
void close_physical_memory_device();
void mmap_hps_peripherals();
void munmap_hps_peripherals();
void mmap_fpga_peripherals();
void munmap_fpga_peripherals();
void mmap_peripherals();
void munmap_peripherals();
void write_sram_fpga(uint32_t address, uint32_t data);
void read_sram_fpga(uint32_t address);
void write_accelerator(uint32_t address, uint32_t data);
uint32_t read_accelerator(uint32_t address);
uint32_t* get_uocram_base();
uint32_t* get_wocram_base();
uint32_t* get_xocram_base();
uint32_t* get_fpga_accelerator_base();
#endif /* TESTSDRAM_H_ */
