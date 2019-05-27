void load_param(uint32_t* av_slave, uint32_t* u_ocram, uint32_t* w_ocram, uint32_t* data_ptr);
void ocram_init(uint32_t* u_ocram, uint32_t* w_ocram, uint32_t* x_ocram);
void xocram_fill_RT(uint32_t* x_ocram, uint32_t* data_ptr);
void ocram_empty(uint32_t* ocram, uint32_t RAM_SIZE);
void rearrange_conv2d_param(int32_t * word0, int32_t* word1);
void read_xocram(uint32_t mode, uint32_t* ocram, int32_t* data);
void get_data_maxp(int32_t* data, int32_t* word_data);
void get_data_gru(int32_t* data, int32_t*word_data);
#define UOCRAM_SIZE 307*20
#define WOCRAM_SIZE 3234*20
#define XOCRAM_SIZE 72*20
#define RTDATA_CHUNK_SIZE 460 //2300 6bit words => 20*30 30bit words
