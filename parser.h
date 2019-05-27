#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include "socal/socal.h"

#define NBCHAR 24
#define NBPARAM_IN_LINE 100
#define NBPARAM_IN_WORD 5
#define NBPARAM 354013
#define CHARMAX NBCHAR*NBPARAM_IN_LINE
#define NBITS 6
#define NBWORDS 70803
#define NBDIGIT_RTDATA 10
#define RTDATA_CHUNK_SIZE 460
#define MAX_XDATA 1061.00122101221
//#define COMPACT_NBWORDS NBPARAM()
FILE* weights_file;
FILE* rtdata_file;
void parse_weights(char* file, int32_t* words);
void parse_rtdata(char* file, int32_t* words, int32_t chunk_number);
int32_t quantize_param(char* STR, uint32_t size);
int8_t process_string(char* STR, uint32_t size);
int32_t params2word(int32_t* param);
//void concat_words(int32_t** words, int32_t paddedw[NBWORDS]);
#endif
