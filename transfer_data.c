#include "testsdram.h"
//#include "globals.h"
#include "transfer_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hwlib.h"
#include <unistd.h>

void load_param(uint32_t* av_slave, uint32_t* u_ocram, uint32_t* w_ocram, uint32_t* data_ptr)
{
	uint32_t mask = 0b111111;
	uint32_t i, j;
	//first 2 words are conv2D parameters
	*(av_slave+4) = *(data_ptr);
	*(av_slave+5) = *(data_ptr+1);
	printf("Conv-2D parameters stored\n");
	//next words are Wz, Wr, Wh parameters
	for( i = 0; i < WOCRAM_SIZE; i++)
	{
		j = (uint32_t)(i/20*32 + i%20);
		*(w_ocram + j) = *(int32_t*)(data_ptr+i+2);
	}
	//rest are all to be stored in the UOCRAM (including dense layer' parameters)
	printf("Wz, Wr, Wh parameters stored\n");
	for( i = 0; i < UOCRAM_SIZE-19; i++)
	{
		j = (uint32_t)(i/20*32 + i%20); //shift line address by 5 (2^5 = 32) lower part is the address inside a line
		*(u_ocram + j) = *(int32_t*)(data_ptr+i+2+WOCRAM_SIZE);

	}
//	for( i = 0; i < UOCRAM_SIZE-17; i++)
//		if (i >= 256*20 && i % 20 == 0)
//		{
//			j = (uint32_t)(i/20*32 + i%20); //shift line address by 5 (2^5 = 32) lower part is the address inside a line
//			printf("uocram UH weights: 0x%x %d \n", (*(u_ocram + j)&(mask<<6))>>6, j);
//			usleep(1000000);
//		}
	printf("Uz, Ur, Uh, Bz, Br, Bh, Wlin, Blin parameters stored\n");
}

void ocram_init(uint32_t* u_ocram, uint32_t* w_ocram, uint32_t* x_ocram){
	printf("in ocram init\n");
	ocram_empty(u_ocram, UOCRAM_SIZE);
	printf("in ocram init\n");
	ocram_empty(w_ocram, WOCRAM_SIZE);
	printf("in ocram init\n");
	ocram_empty(x_ocram, XOCRAM_SIZE);
	printf("in ocram init\n");

}

void xocram_fill_RT(uint32_t* x_ocram, uint32_t* data_ptr){
	uint32_t i, j;
	for( i = 0; i < RTDATA_CHUNK_SIZE; i++)
	{
		j = (uint32_t)(i/20*32 + i%20);
		*(x_ocram + j) = *(int32_t*)(data_ptr+i);
	}
	//for (i =  0; i <40;  i++)
	//{
	//	j = (uint32_t)(i/20*32 + i%20);
	//	printf("FIRST TWO LINES OF XOCRAM: %x \n", *(data_ptr+i));
	//}
}
void read_xocram(uint32_t mode, uint32_t* ocram, int32_t* data){
	uint32_t i, j;
	for( i = 0+23*20; i < 20*27+23*20; i++)
	{
		j = (uint32_t)(i/20*32 + i%20);
		if (mode == 0){
			printf("xocram data %d at address %d and line-address %d \n",*(ocram + j), j, i/20);
		}
		else if (mode == 1){
			*(int32_t*)(data+i-23*20) = *(int32_t*)(ocram+j);
			//printf("xocram data %d at address %d and line-address %d \n",*(ocram + j), j, i/20);
		}
		//usleep(ALT_MICROSECS_IN_A_SEC/10);
	}
}

void ocram_empty(uint32_t* ocram, uint32_t RAM_SIZE){
	uint32_t i, j;
	printf("in xocram empty\n");
	for( i = 0; i < RAM_SIZE; i ++){
		j = (uint32_t)(i/20*32 + i%20);
		//printf("iter %d\n", i);
		*(ocram + j) = 0;
	}
}
void rearrange_conv2d_param(int32_t * word0, int32_t* word1){
	int32_t mask = 0b111111;
	int32_t wordconv0, wordconv1;

	wordconv0 = (*word0 & mask) |  ((*word0 & (mask<<12))>>6) | ((*word0 & (mask<<24))>>12)
				|  ((*word1 & (mask<<6))<<12) |  ((*word1 & (mask <<18))<<6);
	wordconv1 = ((*word1 & mask)<<12) |  ((*word1 & (mask<<12))<<6) | (*word1 & (mask<<24))
					|  ((*word0 & (mask<<6))>>6) |  ((*word0 & (mask <<18))>>12);

	printf("word0 & mask : %x \n",(*word0 & mask) );
	printf("word0 & mask<<6 : %x \n",(*word0 & (mask<<6))>>6);
	printf("word0 & mask<<12 : %x \n",(*word0 & (mask<<12))>>12) ;
	printf("word0 & mask<<18 : %x \n",(*word0 & (mask<<18))>>18);
	printf("word0 & mask<<24 : %x \n",(*word0 & (mask<<24))>>24);

	printf("wordconv0 : 0x%x\n",wordconv0);
	printf("wordconv1 : 0x%x\n",wordconv1);
	usleep(ALT_MICROSECS_IN_A_SEC);
	*word0 = wordconv0;
	*word1 = wordconv1;
}

void get_data_maxp(int32_t* data, int32_t* word_data)
{
	int32_t mask = 0b111111;
	uint32_t i,j = 0;
	uint32_t counter,line=0;
	for (line =  0; line <22; line++){
		for (counter = 0; counter<49; counter++){
			i = counter/5+line*20;
			j = counter%5;
			*(data+counter+line*49) = (*(word_data+i) & (mask << j*6))>>j*6;
			//printf("extracted val %d, word %d\n", *(data+counter+line*49), *(word_data+i));
		}
	}
}

void get_data_gru(int32_t* data, int32_t *word_data)
{
	int32_t mask = 0b111111;
	int32_t sgnextneg = 0b11111111111111111111111111000000;
	uint32_t k,i,j = 0;
	for (k = 0; k <500; k++){
		i = k/5;
		j = k%5;
		*(data + k) = (*(word_data+i) & (mask << j*6))>>j*6;
		if (*(data+k)>>5 == 1)
			*(data+k) = sgnextneg | *(data+k);
	}
}
