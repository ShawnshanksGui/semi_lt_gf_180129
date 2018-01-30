/***************************/
//author: Shawnshanks fei
//date:   2017.03.14
//theme:  header of lt_code source_Code
/***************************/

#ifndef SENDER
#define SENDER 100

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

#define AMOUNT_TOTAL_BLOCK          10

#define LOCATION_TYPE_PACKET         0
#define LOCATION_NUM_BLOCK           1
#define LOCATION_AMOUNT_TOTAL_BLOCK  2
#define LOCATION_IN_SYM_AMOUNT       3
#define LOCATION_EN_SYM_AMOUNT       4
#define LOCATION_D                   5

#ifdef  LOCATION_D
#define LOCATION_PART   LOCATION_D + 1
#endif

#define CODELINE                   100
#define MAX_INDEX_LIST              40
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
//******************************
#endif

#define C_ROBUST_SLT_DISTRI  0.5
#define DELTA                0.5

#define YES                    1
#define NO                     0
#define SUCCESS_DECODE         1
#define FAILURE_DECODE         0
#define NOT_FINISH_DECODE      0

#define PACKET_LOSS_RATE    0.05  //packet loss rate
//*******************************
//************decoder************
#define MAX_EN_SYMBOL_RECV 16000
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
	int id_rip[MAX_INPUT_SYMBOL_AMOUNT   + 10];
	int addr_rip[MAX_INPUT_SYMBOL_AMOUNT+ 10];   //reflect to addr of de_buf[]
}rip_s;
//**********************

void usage(char *command);

void Socket(int *sock_id);

void Fopen_for_write(FILE **fp, char *param);

void Fopen_for_read(FILE **fp, char *param);

size_t Fwrite(void *, int, int, FILE *);

size_t Fread( void *, int, int, FILE *);

void Bind(int sock_id, SA *ptr_addr, int len);

void Sendto(int sock_id, char *en_packet, size_t len_en_packet
     , int flags, struct sockaddr *serv_addr, int addrlen);
/////////////////////////////////////////////////
//get degree d
double get_tau(int, int);

double get_beta(int);

double get_ideal_slt_distri(int, int);

void get_robust_slt_distri_pro(double *, int);

void get_cdf_distri(double *, double *, int);

void interval_judge(int *, double, double *, int);

void get_cdf_final(double *, int);

void get_d(int *, double *, int);
void get_d_one(int *, int);
////////////////////////////////////////////////


//get_index
void get_index(int *, int, int);
void get_index_one(int *, int, int, int);

//exclu-or 
void get_data(char symbol_raw[][CODELINE], struct symbol *);

//void set_packet_sent(char *, char *, int *, int);
void set_packet_sent(char *en_packet, int type_packet, int num_block, int amount_total_block
     , int in_sym_amount, int en_sym_amount, char *en_symbol, int *en_list, int d);

//void encode_lt_primary(char symbol_raw[][CODELINE], char *, int, double *);
void encode_and_packet_encapsult_primary(char *en_packet, int type_packet
     , int num_block, int amount_total_block, char symbol_raw[][CODELINE]
     , int in_sym_amount, int en_sym_amount, double *cdf);

//void encode_lt_semi_random(char symbol_raw[][CODELINE], char *, int, double *, int, int);
void encode_and_packet_encapsult_semi_random(char *en_packet, int type_packet
     , int num_block, int amount_total_block, char symbol_raw[][CODELINE]
     , int in_sym_amount, int en_sym_amount, double *cdf, int num_sent, int num_index);

void get_raw_source(char buf[][CODELINE], int);

//transmit over channel
int transmit_over_ch();
#endif


