/***************************/
//author: Shawnshanks fei
//date:   2017.03.14
//theme:  header of lt_code source_Code
/***************************/

#ifndef ENCODE_CONSTRUCT 
#define ENCODE_CONSTRUCT 100

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "assert.h"

#include "errno.h"
#include <unistd.h>

#include <sys/time.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct sockaddr SA;

#define AMOUNT_TOTAL_BLOCK           1

#define BYTE_INDEX_BLOCK  AMOUNT_TOTAL_BLOCK/8

#define LOCATION_TYPE_PACKET         0
#define LOCATION_NUM_BLOCK           1
#define LOCATION_AMOUNT_TOTAL_BLOCK  2
#define LOCATION_IN_SYM_AMOUNT       3
#define LOCATION_EN_SYM_AMOUNT       4
#define LOCATION_D                   5

#ifdef  LOCATION_D
#define LOCATION_PART   LOCATION_D + 1
#endif

#define CODELINE                  1000 
#define MAX_INDEX_LIST              120
//#define MAX_BYTE_LIST 10 //index storaged as bit 
//#define MAX_INPUT_SYMBOL_NUM 80
//#define LEN_EN_PACKET  112

//about above, replaced by below
#ifdef MAX_INDEX_LIST 
//******************************
//#define MAX_BYTE_LIST (MAX_INDEX_LIST%8==0) ? MAX_INDEX_LIST/8 : MAX_INDEX_LIST/8+1
#define MAX_BYTE_LIST MAX_INDEX_LIST/8
#define MAX_INPUT_SYMBOL_AMOUNT MAX_INDEX_LIST
#define LEN_EN_PACKET LOCATION_PART+MAX_BYTE_LIST+CODELINE
//define lenth of packet of the packet sac
#define LEN_P_SAC    1+1+MAX_INDEX_LIST/8
//******************************
#endif

#ifdef AMOUNT_TOTAL_BLOCK     
//define lenth of packet of the blcok sac
#define LEN_B_SAC 1+4+AMOUNT_TOTAL_BLOCK/8                   
#endif

//data type
#define DATA_PRO_FEC           1
#define DATA_RE_FEC            2
#define P_SAC                  3
#define B_SAC                  4

//specify that all block is finish decoded successfully
#define FLAG_ALL_FINISH        5   

#define C_ROBUST_SLT_DISTRI  0.5
#define DELTA                0.5

#define YES                    1
#define NO                     0
#define SUCCESS_DECODE         1
#define FAILURE_DECODE         0
#define NOT_FINISH_DECODE      0

#define SUCCESS_ONE_BLOCK      1

#define PACKET_LOSS_RATE    0.05  //packet loss rate
//*******************************
//************decoder************
#define MAX_EN_SYMBOL_RECV   400
//*******************************

typedef unsigned char uchar;
typedef unsigned int  uint;

//*********************
//encoding symbol
typedef struct symbol{
    int d;
	char data[CODELINE];
	int index[MAX_INDEX_LIST]; 
}en_symbol;

//**********************
typedef struct de_sym{
	int d;
	char data[CODELINE];
	int index[MAX_INDEX_LIST];
}de_symbol;
//**********************
//**********************
typedef struct rip{
	int id_rip[MAX_INPUT_SYMBOL_AMOUNT   + 1000];
	int addr_rip[MAX_INPUT_SYMBOL_AMOUNT + 1000];   //reflect to addr of de_buf[]
}rip_s;
//**********************

void usage(char *command);

void Socket(int *sock_id);

void Fopen_for_write(FILE **fp, char *param);

void Fopen_for_read(FILE **fp, char *param);

size_t Fwrite(void *, int, int, FILE *);

size_t Fread( void *, int, int, FILE *);

void Fopen_for_write_not_cover_origin(FILE **fp, char *param);

void Bind(int sock_id, SA *ptr_addr, int len);

int Recv_one_packet_from(int sock_id, char *buf_ctp, int len, int flag, SA *src_addr, int *addr_len);


void record_data_de_sucs(FILE *fp, char data_recovery[][CODELINE], int num_block);

int Is_success_decode(char *array_state_block, int num_block);

int Is_all_decoded_sucs(char *);

/*******************************/
//get degree d
//***************************************
//transmit over channel
int transmit_over_ch();
//***************************************

//decode part
/****************************************/
void pre_treat(char *, de_symbol *);
void get_amount(int *, int);
int  get_id(de_symbol);
int  judge_repeat_rip(int, int, int , int *);
int  judge_repeat_buf(int, struct de_sym, struct de_sym *);
void insert_rip(int, int, int, struct rip *);
void insert_buf(struct de_sym, struct de_sym *);
void get_sym_from_rip(de_symbol **, de_symbol *);
void sign_sucs(int , char *);
int  search_self_neighbor(de_symbol, de_symbol);
void update_buf(de_symbol *, de_symbol *);
int  success_judge_recvry(char *);
//int  decode(char *, char symbol_recover[][CODELINE]);
int decode_for_one_packet(char *ptr_packet, int *_addr_buf, int *_addr_rip_insert
    , int *_addr_rip_procs, struct rip *ripple, de_symbol de_buf[]
    , char data_recovery[][CODELINE]);
/*****************************************/

void feedbk_packet_sac_generator(char *, int, int *);
void feedbk_block_sac_generator(char *, int, char *);
void feedbk_terminal_generator(char *);

void set_state_sucs(char *, int);

#endif


