#include "receiver.h"


void usage(char *command)
{
    printf("usage:%s receiver_port\n", command);
}

void Socket(int *sock_id)
{
    if(((*sock_id) = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Create socket failed\n");
        exit(0);
    }
}

int Recv_one_packet_from(int sock_id, char *buf_ctp, int len, int flag, SA *src_addr, int *addr_len)
{
	int recv_len = 0;
	if((recv_len = recvfrom(sock_id, buf_ctp, len, flag, src_addr, addr_len)) <= 0)
	{
		perror("recvfrom failed !!!!!!!!!!!!");
	}
	else
		return 1;
}

void Fopen_for_write(FILE **fp, char *param)
{
    if((*fp = fopen(param, "w")) == NULL)
    {
        perror("Open file failed\n");
        exit(0);
    }
}

void Fopen_for_read(FILE **fp, char *param)
{
    if((*fp = fopen(param, "r")) == NULL)
    {
        perror("Open file failed\n");
        exit(0);
    }
}

void Fopen_for_write_not_cover_origin(FILE **fp, char *param)
{
    if((*fp = fopen(param, "a+")) == NULL)
    {
        perror("Open file failed\n");
        exit(0);
    }
}

size_t Fread(void *ptr, int size, int n, FILE *fp)
{
    int n_read = 0;
    if((n_read = fread(ptr, (size_t)size, (size_t)n, fp)) <= 0)
    {
        perror("fread falied");
        exit(0);
    }
    return n_read;
}

size_t Fwrite(void *ptr, int size, int n, FILE *fp)
{
    int n_write = 0;
    if((n_write = fwrite(ptr, (size_t)size, (size_t)n, fp)) <= 0)
    {
        perror("fwrite failed");
        exit(0);
    }
    return n_write;
}

void Bind(int sock_id, SA *ptr_addr, int len)
{
    if(bind(sock_id, ptr_addr, len) < 0)
    {
        perror("Bind socket failed\n");
        exit(0);
    }
}

////////////////////////////////////////////////////////////////////
void pre_treat(char *ptr_packet, de_symbol *_data)
{
	int i = 0;
	int k = 0;
	int mm = 0;
	int tmp = 0;

	_data->d = ptr_packet[LOCATION_D];

	for(i = LOCATION_PART; i < LOCATION_PART+MAX_BYTE_LIST; i++)
		for(mm = 0; mm < 8; mm++)
		{	
			if((tmp = ((unsigned char)ptr_packet[i]) & (0x1<<mm)) != 0)
			{	
//				_data->index[k] = (i-LOCATION_PART)*8 + mm + 1;  //index starts from 0,
				_data->index[k] = (i - 6) * 8 + mm + 1;
				k++;
			}
		}

	for(i = LOCATION_PART+MAX_BYTE_LIST, k = 0; i < LEN_EN_PACKET; i++, k++)
		_data->data[k] = ptr_packet[i];
}

/*************************************/
//methods of function decode

void get_amount(int *amount, int d)
{
	*amount = d;
}
/*
int get_id(de_symbol monkey)
{	
	int i, k;

	for(i = 0; i < MAX_INDEX_LIST; i++)
	{
		if (monkey.index[i] != 0)
			for(k = 0; k < 8; k++)
			{
				if((monkey.index[i] >> k) == (uchar)1)
					return (i*8 + k + 1);
			}
	}
	
	//error happened!!!!!!!!!
	printf("error: get_id !!!!!!!\n");
	exit(0);
}
*/

int get_id(de_symbol monkey)
{
	int i = 0;

	for(i = 0; i < MAX_INPUT_SYMBOL_AMOUNT; i++)
	{
		if(monkey.index[i] > 0)
			return monkey.index[i];
	}
	printf("monkey.d = %d\n", monkey.d);
	printf("error: get_id");
	exit(0);
}

//********************************************
int judge_repeat_rip(int id, int start, int end, int *array_id)
{
	int i = start;

	for(i = start; i < end; i++)
	{
		if(array_id[i] == id)
			return YES;
	}
	return NO;
}

int judge_repeat_buf(int num,struct de_sym _data,struct de_sym *buf_array)
{
	int i = 0;
	int k = 0;

	for(i = 0; i < num; i++)
	{
		if(_data.d == buf_array[i].d)
			for(k = 0; k < CODELINE; k++)
			{
				if(_data.data[k] != buf_array[i].data[k])
					break;
				else if(k == CODELINE - 1)
					return YES;
			}
	}
	return NO;
}
//********************************************

//********************************************
void insert_rip(int id, int addr_buf, int addr_rip, struct rip *ripple)
{
	ripple->id_rip[addr_rip] = id;
	ripple->addr_rip[addr_rip] = addr_buf;
}

void insert_buf(struct de_sym _data, struct de_sym *ptr)
{
	int i = 0;

	ptr->d = _data.d;
	for(i = 0; i < CODELINE; i++)
		ptr->data[i] = _data.data[i];
	for(i = 0; i < MAX_INDEX_LIST; i++)
		ptr->index[i] = _data.index[i];
}
//********************************************

void get_sym_from_rip(de_symbol **sym_from_rip, de_symbol *symbol_buf)
{
	*sym_from_rip = symbol_buf;
}

void sign_sucs(int id, char *sign)
{
	sign[(id-1)/8] |= (uchar)1<<((id-1) % 8);//start from 0 --->subscript
}

int search_self_neighbor(de_symbol aa, de_symbol bb)
{
	int i = 0;

	int _id	= get_id(aa);
	
	for(i = 0; i < MAX_INDEX_LIST; i++)
	{
		if(_id == bb.index[i])
			return 1;
	}
	return 0; // not found!!!
}

void update_buf(de_symbol *sym_from_rip, de_symbol *sym_from_buf)
{
	int i = 0;
	int tmp = 0;
	for(i = 0; i < CODELINE; i++)
		sym_from_buf->data[i] ^= sym_from_rip->data[i];
	sym_from_buf->d--; 

	tmp = get_id(*sym_from_rip);
	for(i = 0; i < MAX_INPUT_SYMBOL_AMOUNT; i++)
	{
		if(sym_from_buf->index[i] == tmp)
		{
			sym_from_buf->index[i] = 0;
			break;
		}
	}
}

int success_judge_recvry(char *sign)
{
	int i = 0;

	for(i = 0; i < MAX_INDEX_LIST; i++)
	{
		if(sign[i] != 0xff)
			return 0;
	}
	return 1;
}

//********************************************
//int decode(char *ptr_packet, char symbol_recover[][CODELINE])
int decode_for_one_packet(char *ptr_packet, int *_addr_buf, int *_addr_rip_insert
    , int *_addr_rip_procs, struct rip *ripple, de_symbol de_buf[]
    , char data_recovery[][CODELINE])
{	
	de_symbol _data_tmp;

	int i            = 0;
	int num_procs    = 0;
	int amount_insym = 0;
	int id_symbol    = 0;
	int sign_success = 0;
	
	de_symbol *sym_from_ripple;

/*	
//	static char sign[MAX_BYTE_LIST] = {'\0'}; 
	//sign of success:all bits are 1
	static int sign_reset       = 0;
	static int _addr_buf   	    = 0;
	static int _addr_rip_insert = 0;
	static int _addr_rip_procs  = 0;

//	static struct rip ripple = {{0},{0}};
	static struct rip ripple;
	static de_symbol de_buf[MAX_EN_SYMBOL_RECV];
//	memset(de_buf, 0, sizeof(de_symbol));
*/	
/************************************
*************************************/
//restart another procedure after one time of complishing 
//whole decodeing procedure
/*
	if(YES == sign_reset)
	{
		_addr_buf        = 0;
		_addr_rip_insert = 0;
		_addr_rip_procs  = 0;
		sign_reset       = 0;
		memset(&de_buf[0], 0, MAX_EN_SYMBOL_RECV * sizeof(de_symbol));
		memset(&ripple, 0, sizeof(struct rip));
	}
*/
/************************************
************************************/

	memset(&_data_tmp, 0, sizeof(de_symbol)); //reset to zero
	pre_treat(ptr_packet, &_data_tmp);
	get_amount(&amount_insym, _data_tmp.d);
	

	assert(amount_insym >= 1);  //check

	if(1 == amount_insym)
	{
		id_symbol = get_id(_data_tmp);
		//******************************
		//for test 
////		printf("input_symbol = %d\n", id_symbol);
		//******************************
		if(NO == judge_repeat_rip(id_symbol, 0, 
		   *_addr_rip_insert, ripple->id_rip))
		//should search all ripple set
		{
			//insert id and addr into ripple
////			printf("success of output: d of original = 1");
			insert_rip(id_symbol, *_addr_buf, *_addr_rip_insert, ripple);
			(*_addr_rip_insert)++;

			insert_buf(_data_tmp, &de_buf[(*_addr_buf)]); 
			(*_addr_buf)++;   
			//*****************************//
			//***check the final result****//
		    strncpy(data_recovery[id_symbol-1], _data_tmp.data, CODELINE);
			//*****************************//
		}
		//***********************
//		sign_sucs(id_symbol, sign);  //wrong location
		//***********************
	}
	else if(NO == judge_repeat_buf(*_addr_buf, _data_tmp, de_buf))
	{		
		insert_buf(_data_tmp, &de_buf[*_addr_buf]);
		(*_addr_buf)++;
		
		for(i = 0; i < (*_addr_rip_insert); i++)
		{
//			if(de_buf[ripple.addr_rip[i]].d > 0 && YES==search_self_neighbor
//				(de_buf[_addr_buf-1], de_buf[ripple.addr_rip[i]]))

			if(de_buf[ripple->addr_rip[i]].d > 0 && YES==search_self_neighbor
			              (de_buf[ripple->addr_rip[i]], de_buf[(*_addr_buf)-1]))
			{
				update_buf(&de_buf[ripple->addr_rip[i]], &de_buf[(*_addr_buf)-1]);
			}
			if(de_buf[(*_addr_buf)-1].d == 1)
			{
				if(NO == judge_repeat_rip(get_id(de_buf[(*_addr_buf)-1]), 0,
				  (*_addr_rip_insert), ripple->id_rip))
				{
					insert_rip(get_id(de_buf[(*_addr_buf)-1]), (*_addr_buf)-1,
						   (*_addr_rip_insert), ripple);
					(*_addr_rip_insert)++;
////            		printf("success of output: d of original_data != 1:abnormal:,hello, world id = %d\n", get_id(de_buf[_addr_buf-1]));
					//*******//
					//check the final result****/
					strncpy(data_recovery[get_id(de_buf[(*_addr_buf)-1])-1],de_buf[(*_addr_buf)-1].data,CODELINE);
					//******//
					num_procs++;
					break;
				}
			}
		}
	}
	
//	_num_buf_count
//	while(_addr_rip_procs < _addr_rip_insert && num_procs > 0)
	while((*_addr_rip_procs) < (*_addr_rip_insert))    
	{
	//	memset(sym_from_ripple, 0, sizeof(de_symbol));
		get_sym_from_rip(&sym_from_ripple,
		                &de_buf[ripple->addr_rip[(*_addr_rip_procs)]]);
		assert(sym_from_ripple != NULL);  //check
		(*_addr_rip_procs)++;
//		sign_sucs(id_symbol, sign);

	//*******************************************
//		sign_sucs((get_id(*sym_from_ripple)), sign); 
	//*******************************************

		for(i = 0, num_procs = 0; i < (*_addr_buf); i++)
		{
			if(de_buf[i].d != 0)
			{
				if(YES==search_self_neighbor(*sym_from_ripple, de_buf[i])) 
				//this encoding_symbol has it as a neighbor
				{
					if(de_buf[i].d != 1)
						update_buf(sym_from_ripple, &de_buf[i]); 
					//operand:exclusive or and update d
				}
				if(de_buf[i].d == 1)
				{
					if(NO == judge_repeat_rip(get_id(de_buf[i]), 0, 
					   (*_addr_rip_insert), ripple->id_rip)) 
					{
						insert_rip(get_id(de_buf[i]), i, 
								  (*_addr_rip_insert), ripple);
						(*_addr_rip_insert)++;
						//****************
						//check the final result
						strncpy(data_recovery[get_id(de_buf[i])-1], de_buf[i].data, CODELINE);
						//****************
////						printf("success of input: d of orignal_data != 1:normal: this_id of input symbol= %d\n", get_id(de_buf[i]));
						num_procs++;
					//	sign_sucs((get_id(*sym_from_ripple)), sign);
					}
					//attention: judge repeat
				}
			//	sign_sucs((get_id(*sym_from_ripple)), sign);
			}
		}
//		sign_success = success_judge_recvry(sign);

		if((*_addr_rip_procs) >= (*_addr_rip_insert) && 0 == num_procs)
			break;  //waiting for next packet to recover!!!!!!!
	}
	if(MAX_INPUT_SYMBOL_AMOUNT == (*_addr_rip_insert))
	{
//		sign_reset = 1;
		return SUCCESS_DECODE;
	}
	else
		return NOT_FINISH_DECODE;
//	return sign_success;
}
/************************************************/

/**********************/
//test for ptr_treat!!!!!!!!!!!!!
/*
int main()
{
//	de_symbol aa = {10, {'\0'}, {0}};
	int i = 0;

	packet[0] = 10;
	for(i = 2; i < 102; i++)
		packet[i] = 0x0f;
	for(i = 102; i < 302; i++)
		packet[i] = 'a';

	pre_treat(packet, &aa);
//	pre_treat(packet, ptr_desym);	
	return 0;
}
*/
/*************/

int Is_success_decode(char *array, int num_block)
{
	return (array[num_block] == 1);
}

int Is_all_decoded_sucs(char *array_state)
{
	int i = 0;
	
	for(i = 0; i < AMOUNT_TOTAL_BLOCK; i++)
		if(0 == array_state[i])	
			return 0;
	
	return 1;
}

/////////////////////////////////////////////////////////////////////////
//three type packet generator for packet SAC, block SAC and terminal flag
void feedbk_packet_sac_generator(char *buf, int num_block, int id_rip[])
{
	int i = 0;
  	
	buf[LOCATION_TYPE_PACKET] =     P_SAC;
	buf[LOCATION_NUM_BLOCK]   = num_block;
	
	for(i = 0; i < MAX_INPUT_SYMBOL_AMOUNT; i++)
		buf[2+(id_rip[i]-1)/8] |= (unsigned char)1 << ((id_rip[i]-1) % 8);
}

void feedbk_block_sac_generator(char *buf, int num_block, char *array_state)
{
	int i = 0;

	buf[LOCATION_TYPE_PACKET] =     B_SAC;
	buf[LOCATION_NUM_BLOCK]   = num_block;
	
	for(i = 0; i < AMOUNT_TOTAL_BLOCK; i++)
		if(1 == array_state[i])	
			buf[5+i/8] |= (unsigned char)1 << (i%8);
}

void feedbk_terminal_generator(char *buf)
{
	buf[LOCATION_TYPE_PACKET] = FLAG_ALL_FINISH;
}
/////////////////////////////////////////////////////////////////////////////

void set_state_sucs(char *array_state, int num_block)
{
	array_state[num_block] = 1;
}

