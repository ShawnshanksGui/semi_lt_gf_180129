/***************************/
//author: Shawnshanks fei
//date:   2017.07.15
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
#include "unistd.h"
#include "fcntl.h"

#include <sys/time.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct sockaddr SA;

#define AMOUNT_TOTAL_BLOCK           1

#define LOCATION_TYPE_PACKET         0
#define LOCATION_NUM_BLOCK           1
#define LOCATION_AMOUNT_TOTAL_BLOCK  2
#define LOCATION_IN_SYM_AMOUNT       3
#define LOCATION_EN_SYM_AMOUNT       4
#define LOCATION_D                   5

#ifdef  LOCATION_D
#define LOCATION_PART   LOCATION_D + 1
#endif

//apply to spcifing type of packet
#define DATA_PRO_FEC                 1
#define DATA_RE_FEC                  2 
#define P_SAC                        3
#define B_SAC                        4
//specify that all block is finish decoded successfully
#define FLAG_ALL_FINISH              5 


//apply to distinguishing type of procedure of encoding
#define FEC_D_ONE                    1
#define FEC_D_NON_ONE                2


#define CODELINE                  1000
#define MAX_INDEX_LIST              120
//#define MAX_BYTE_LIST 10 //index storaged as bit 
//#define MAX_INPUT_SYMBOL_NUM 80
//#define LEN_EN_PACKET  112

//about above, replaced by below
#ifdef MAX_INDEX_LIST 
/////////////////////////////////////////
//#define MAX_BYTE_LIST (MAX_INDEX_LIST%8==0) ? MAX_INDEX_LIST/8 : MAX_INDEX_LIST/8+1
#define MAX_BYTE_LIST MAX_INDEX_LIST/8
#define MAX_INPUT_SYMBOL_AMOUNT MAX_INDEX_LIST
#define LEN_EN_PACKET LOCATION_PART+MAX_BYTE_LIST+CODELINE
//******************************
//define lenth of packet of the packet sac
#define LEN_P_SAC    1+1+MAX_INDEX_LIST/8
#endif
//////////////////////////////////////

#ifdef AMOUNT_TOTAL_BLOCK     
//define lenth of packet of the blcok sac
#define LEN_B_SAC 1+4+AMOUNT_TOTAL_BLOCK/8                   
#endif

#define C_ROBUST_SLT_DISTRI  0.5
#define DELTA                0.5

#define INVALID                2
#define YES                    1
#define NO                     0
#define SUCCESS_DECODE         1
#define FAILURE_DECODE         0
#define NOT_FINISH_DECODE      0

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

void addr_of_reception_prepare(struct sockaddr_in * serv_addr, char *, char *);

void Sendto(int sock_id, char *en_packet, size_t len_en_packet
     , int flags, struct sockaddr *serv_addr, int addrlen);

void Recvfrom(int sock_id, char *buf_recv, int len_buf, int flag, struct sockaddr *serv_addr, int *addr_len);
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
void get_d_one(int *, double, char);
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
     , int in_sym_amount, int en_sym_amount, double *cdf, double _plr, char type_encd, int num_index);

void get_raw_source(char buf[][CODELINE], int);

void get_amount_re_fec(int *amount, double _plr, int array[]);

void get_amount_pro_fec(int *amount, double _plr, int in_sym_amount);

void resolve_and_get_unrecovered_sym(int *_array, char *buf_recv);

//transmit over channel
int transmit_over_ch(double _plr);
#endif


