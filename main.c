/* Client code */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>

#include "testsdram.h"
#include "parser.h"
#include "hwlib.h"
#include "transfer_data.h"
#include "alt_timers.h"
#include "alt_globaltmr.h"

#define PORT_NUMBER     5000
#define SERVER_ADDRESS  "169.254.37.95"
#define FILENAME   "/home/sahand/socketHPS/RT_NormQuantdata"
#define RT_DATA_CHUNK_SIZE 460 //23*20

int main(int argc, char **argv)
{
	printf("FPGA sdram test\n");
	open_physical_memory_device();
	printf("Physical memory device opened\n");
	mmap_peripherals();
	printf("mapped the peripherals\n");

	uint32_t n = 0;
	uint32_t* uocram = get_uocram_base();
	uint32_t* wocram = get_wocram_base();
	uint32_t* xocram = get_xocram_base();
	uint32_t* av_slave = get_fpga_accelerator_base();
	write_accelerator(0, 0);

	int32_t* words = malloc(NBWORDS * sizeof(int32_t));

	parse_weights("FINAL_signed_6b.txt", words);
	ocram_init(uocram, wocram, xocram);
	rearrange_conv2d_param(words, words+1);
	load_param(av_slave, uocram, wocram, words);
	printf("no problem before freeing memory\n");
	free(words);
	printf("no problem while freeing memory\n");

	int32_t DEBUG_data_words[540];
	int32_t DEBUG_data_maxp[1078];
	int32_t DEBUG_data_gru[500];
	uint8_t timesteps = 0;
	uint8_t hps_DEBUG_read = 0;
	uint8_t hps_write_new_batch = 0;
	int32_t sequences = 0;
	FILE* res_file;
	char file_size[256];
	char filename[25];
	char prt_step;
	char seqstr[3];
	char seq_nb[3];
	uint32_t res0, res1, res2;
	printf("no problem before new memory allocation\n");
	int32_t* xdata = malloc(RTDATA_CHUNK_SIZE * sizeof(int32_t));
	printf("no problem while allocating new memory\n");

	//FIRST algorithm iteration
	res_file = fopen("res_acc/Y.txt", "w");
	fprintf(res_file, "new_execution\n");
	fclose(res_file);
	int client_socket;
	ssize_t len;
	char message[50];
	struct sockaddr_in remote_addr;
	char buffer[BUFSIZ]; //defined in stdio.h
	int fileSize;
	FILE *received_file;
	int remain_data = 0;
	int buffsize;
	struct timeval st, et;
	/* Zeroing remote_addr struct */
	memset(&remote_addr, 0, sizeof(remote_addr));

	/* Construct remote_addr struct */
	remote_addr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_ADDRESS, &(remote_addr.sin_addr));
	remote_addr.sin_port = htons(PORT_NUMBER);
	int i;
	/*for (i=0; i <10; i++){
		gettimeofday(&st,NULL);
		gettimeofday(&et,NULL);
		int elapsed = ((et.tv_sec - st.tv_sec)*1000000) + (et.tv_usec - st.tv_usec);
		printf("Accelerator time per time step: %d us\n", elapsed);
	}*/
	/* Create client socket */
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1)
	{
			fprintf(stderr, "Error creating socket --> %s\n", strerror(errno));

			exit(EXIT_FAILURE);
	}

	/* Connect to the server */
	if (connect(client_socket, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
	{
			fprintf(stderr, "Error on connect --> %s\n", strerror(errno));

			exit(EXIT_FAILURE);
	}

	/* Receiving file size */
	for (sequences = 0; sequences < 800; sequences++)
	{
		printf("in main loop\n");
		recv(client_socket, file_size, sizeof(file_size), 0);
		fileSize = atoi(file_size);
		fprintf(stdout, "\nFile size : %d\n", fileSize);

		//received_file = fclose(fopen(FILENAME, "w"));
		received_file = fopen(FILENAME, "w");
		if (received_file == NULL)
		{
				fprintf(stderr, "Failed to open file --> %s\n", strerror(errno));

				exit(EXIT_FAILURE);
		}
		remain_data = fileSize;

		while ((remain_data > 0)&&((len = recv(client_socket, buffer, BUFSIZ, 0)) > 0))
		{
				fwrite(buffer, sizeof(char), len, received_file);
				remain_data -= len;
				//if (remain_data < BUFSIZ)
				//	buffsize = remain_data;
				//bzero(buffer,BUFSIZ);
				//if (len == 0 || len != BUFSIZ)
				//	break;
				fprintf(stdout, "Received %d bytes , %d bytes remaining\n", len, remain_data);
		}
		//code for accelerator here

		fclose(received_file);
		printf("start acc for sequence %d", sequences);
		for (timesteps=0; timesteps < 10; timesteps++){
			prt_step = timesteps +'0';
			//snprintf(seqstr, sizeof(seqstr),"%d", sequences);
			snprintf(filename,sizeof(filename), "RT_NormQuantdata", seqstr);
			printf("filename: %s\n", filename);
			parse_rtdata(filename, xdata, timesteps);
			xocram_fill_RT(xocram, xdata);
			printf("no problem parsing RT_datastream file memory\n");

			printf("iteration number %d\n", timesteps);

			write_accelerator(0, 3); // xocram B port in FPGA mode + trigger accelerator
			gettimeofday(&st,NULL);
			write_accelerator(0, 2); //  deassert trigger

			while(hps_DEBUG_read == 0){
				hps_DEBUG_read =  read_accelerator(1) >> 1;
			}
			gettimeofday(&et,NULL);
			int elapsed = ((et.tv_sec - st.tv_sec)*1000000) + (et.tv_usec - st.tv_usec);
			printf("Accelerator time per time step: %d us\n", elapsed);
			printf("frequency\n");
			hps_DEBUG_read = 0;
			write_accelerator(0, 0); //switch back to HPS mode
			read_xocram(1, xocram, DEBUG_data_words);
			get_data_maxp(DEBUG_data_maxp, DEBUG_data_words);
			get_data_gru(DEBUG_data_gru, DEBUG_data_words+20*22);

			if (sequences == 0){
				snprintf(filename, sizeof(filename), "res_acc/MAXP_t%c.txt", prt_step);
				res_file = fopen(filename, "w");
				printf("%s\n", filename);
				if (!res_file)
					printf("file never opened\n");
				else{
					printf("opened resfile\n");
					for (n=0; n<1078; n++){
						fprintf(res_file, "%f\n",((float)*(DEBUG_data_maxp+n))/16);
					}
				}
				fclose(res_file);
				snprintf(filename,sizeof(filename), "res_acc/GRU_t%c.txt", prt_step);
				res_file = fopen(filename, "w");
				if (!res_file)
						printf("file never opened\n");
					else{
						printf("opened resfile\n");
						for (n=0; n<400; n++){
							fprintf(res_file, "%f\n",((float)*(DEBUG_data_gru+n))/16);
					}
				}
				fclose(res_file);
				snprintf(filename,sizeof(filename), "res_acc/SR_t%c.txt", prt_step);
				res_file = fopen(filename, "w");
				if (!res_file)
						printf("file never opened\n");
					else{
						printf("opened resfile\n");
						for (n=400; n<500; n++){
							fprintf(res_file, "%f\n",((float)*(DEBUG_data_gru+n))/16);
					}
				}
				fclose(res_file);
			}

			if (timesteps == 9)
			{
				printf("last iter\n");
				write_accelerator(6,0);
				res0 = read_accelerator(6);
				write_accelerator(6,1);
				res1 = read_accelerator(6);
				write_accelerator(6,2);
				res2 = read_accelerator(6);
				res_file = fopen("res_acc/Y.txt", "a");
				if (!res_file)
					printf("file never opened\n");
				else{
					printf("opened resfile\n");
					fprintf(res_file, "%f ",((float)(res0))/2048);
					fprintf(res_file, "%f ",((float)(res1))/2048);
					fprintf(res_file, "%f\n",((float)(res2))/2048);

				}
				fclose(res_file);

			}

		}
		snprintf(message, sizeof(message),  "sequence processed, results: %d, %d, %d\n", res0, res1, res2);
		send(client_socket, message, sizeof(message),0);

	}
	free(xdata);
	munmap_peripherals();
	close_physical_memory_device();


	close(client_socket);

	return 0;
}
