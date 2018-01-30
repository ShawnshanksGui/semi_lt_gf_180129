#include "sender.h"

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

void 
addr_of_reception_prepare(struct sockaddr_in *ptr_addr, char *argv_a, char *argv_b)
{
	memset(ptr_addr, 0, sizeof(*ptr_addr));
	ptr_addr->sin_family = AF_INET;
	ptr_addr->sin_port = htons(atoi(argv_b));
	inet_pton(AF_INET, argv_a, &(ptr_addr->sin_addr));
//    *len_addr = sizeof(ptr_addr);
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

/****************************************************/
double get_tau(int i, int k)
{
	double tau;
	double R;
	double temp;

//	temp = floor((double)k/R);
	R = C_ROBUST_SLT_DISTRI*log((double)k/(double)DELTA)*sqrt(k);
	temp = floor((double)k/R);

	if(i >= 1 && i <  (int)temp)
		tau = R/((double)i * (double)k);
	else if(i == (int)temp)
//		tau = R*log(R/DELTA)*k;
		tau = R * log(R/DELTA) / (double)k;
	else if(i > (int)temp && i <= k)
		tau = 0.0;
	else
	{
		printf("get_tau error happened!!!!!!!!");
		exit(0);
	}
	return tau;
}

double get_beta(int k)
{
	int i;
//	double sum_ideal_solit;
	double beta;

	for(i = 1; i <= k; i++)
	{
		beta += (get_ideal_slt_distri(i, k) + get_tau(i, k));
	 }
	return beta;
}

double get_ideal_slt_distri(int i, int k)
{
	if(1 == i)
		return 1.0/(double)k;
	else if(i > 1 && i <= k)
		return 1.0/(double)(i*(i-1));
	else
	{
		printf("error: get ideal slt-distribution\n");
		exit(0);
	}
}


void get_robust_slt_distri_pro(double *ptr_distri, int k)
{
	int i = 0;
	double beta = 0.0;

	beta = get_beta(k);
	assert(beta != 0.0);

	for(i = 1; i <= k; i++)
	{
		//reduce computing compplexity//
	//	beta = get_beta(k, i); //deserve improvement
		//***************************//
		ptr_distri[i] = (get_ideal_slt_distri(i, k) + get_tau(i, k)) / beta;
	}
}

void get_cdf(double *ptr_cdf, double *ptr_distri_pro, int k)
{
	int i = 0;
	ptr_cdf[0] = 0.0;

	for(i = 1; i < k; i++)
	{
		ptr_cdf[i] += ptr_distri_pro[i] + ptr_cdf[i-1];
	}
	ptr_cdf[k] = 1.0; //the final is 100% !!!
}

void interval_judge(int *ptr_d, double value, double *cdf_distri, int k)
{
	int i;
	for(i = 1; i <= k; i++)
	{
		if(value < cdf_distri[i] && value > cdf_distri[i-1])
		{
			*ptr_d = i;
			break;
		}
	}
}

void get_cdf_final(double *cdf_distri, int k)
{
	double robust_distri_pro[k+1];
//	double cdf_distri[k+1];
//	double value;
	get_robust_slt_distri_pro(robust_distri_pro, k);	
	get_cdf(cdf_distri, robust_distri_pro, k);
//	value = (double)((random() % 100000))/100000.0;
//	interval_judge(ptr_d, value, cdf_distri, k);
}

void get_d(int *ptr_d, double *cdf_distri, int k)
{
	double value;
//	srand((int)time(NULL));
	value = rand()/((double)RAND_MAX);
//	value = ((double)(random() % 10000)) / 10000.0;
	interval_judge(ptr_d, value, cdf_distri, k);
}

//void get_d_one(int *ptr_d, int num_sent)
void get_d_one(int *ptr_d, double _plr, char type_packet)
{
//	static int num_sent = 0;
//	static int num_flag  = 1 / PACKET_LOSS_RATE;
	
	int num_high_degree = floor(1/_plr);
	
//	if(num_sent < MAX_INPUT_SYMBOL_AMOUNT)
	if(type_packet == FEC_D_ONE)	 // degree = 1
		*ptr_d = 1;
	else if(type_packet == FEC_D_NON_ONE)  // degree >= 1
		*ptr_d = num_high_degree; 
	
//	num_sent++;
}
/****************************************************/

/****************************************************/
//get_index
void get_index(int *index, int d, int k)
{
	int i = 0;
	int j = 0;
	int num = 0;
	int flag_success = 0;  //flag of preventing from repeating select

//	srand((int)time(0));

	for(i = 0; i < d; i ++)
	{
		while(!flag_success)
		{
			flag_success = 1;
//			num = random()%k + 1;
//modified by gf,data:2017.04.30,
//correct the method of random number
			num = rand() % k + 1;
//**************************************			
			for(j = 0; j < i; j++)
			{
				if(num == index[j])
					 flag_success = 0;
			}
		}
		index[i] = num;
		flag_success = 0;
	}
}

void get_index_one(int *index, int d, int k, int count)
{
//	static int index_mem[MAX_EN_SYMBOL_RECV] = {0};
//	static int count = 1;
	int num          = 0;
	int i            = 0;
	int j            = 0;
	int flag_success = 0;

//	srand((int)time(0));
	if(d == 1)
	{
		/*
		while(!flag_success)
		{
			flag_success = 1;
			num = random()%k + 1;
			for(i = 0;i < count; i++)
			if(num == index_mem[i])
					flag_success = 0;
		}
		index[0] = num;
		index_mem[count++] = num;
		*/
		index[0] = count;
//		count++;
	}
	else
	{
		for(i = 0; i < d; i++)
		{
			while(!flag_success)
			{
				flag_success = 1;
//				num = random()%k + 1;
//modified by gf,
//data:2017.04.30,
//correct the method of random number.
				num = rand() % k + 1;
//**************************************
				for(j = 0; j < i; j++)
				{
					if(num == index[j])
					{
						flag_success = 0;
					}
				}
			}	
			index[i] = num;	
			flag_success = 0;
		}
	}
}

void get_data(char symbol_raw[][CODELINE], struct symbol *ptr)
{
	int i = 0;
	int j = 0;

	for(i = 0; i < ptr->d; i++)
	{
		for(j = 0; j < CODELINE; j++)
			ptr->data[j] ^= symbol_raw[(ptr->index[i])-1][j]; //module 2 + operator
	}
}

void set_packet_sent(char *en_packet, int type_packet, int num_block, int amount_total_block
     , int in_sym_amount, int en_sym_amount, char *en_symbol, int *en_list, int d)
{
//	int num_byte_list = 0;
	int i = 0;
//	num_byte_list = ceil(d/8);
	en_packet[LOCATION_TYPE_PACKET]        =        type_packet;
	en_packet[LOCATION_NUM_BLOCK]          =          num_block;
	en_packet[LOCATION_AMOUNT_TOTAL_BLOCK] = amount_total_block;
	en_packet[LOCATION_IN_SYM_AMOUNT]      =      in_sym_amount;
	en_packet[LOCATION_EN_SYM_AMOUNT]      =      en_sym_amount;
	en_packet[LOCATION_D]                  =                  d;
	
	for(i = 0; i < d; i++)
		en_packet[6+((en_list[i]-1)/8)] |= (unsigned char)1 
				<< ((en_list[i]-1)%8);

	for(i = 0; i < CODELINE; i++)
	{
		en_packet[6 + MAX_BYTE_LIST + i] = en_symbol[i];
	}
}


//void encode_and_packet_encapsult_semi_random(char *en_packet, int type_packet
//     , int num_block, int amount_total_block, char symbol_raw[][CODELINE]
//     , int in_sym_amount, int en_sym_amount, double *cdf, int num_sent, int num_index)
void encode_and_packet_encapsult_semi_random(char *en_packet, int type_packet
     , int num_block, int amount_total_block, char symbol_raw[][CODELINE]
     , int in_sym_amount, int en_sym_amount, double *cdf, double _plr, char type_encd, int num_index)
//void encode_lt(char symbol_raw[][CODELINE],char *en_packet,int k, double *cdf)
{
	struct symbol *ptr_struct_ensym;
	ptr_struct_ensym = (struct symbol*)malloc(sizeof(struct symbol));
//get_d_one(&(ptr_struct_ensym->d)); //modifing data :170701
	get_d_one(&(ptr_struct_ensym->d), _plr, type_encd);

//get_index_one(ptr_struct_ensym->index, ptr_struct_ensym->d, k); //modifing data :170701
	get_index_one(ptr_struct_ensym->index, ptr_struct_ensym->d, in_sym_amount, num_index);
//	get_index_one(ptr_struct_ensym->index, ptr_struct_ensym->d, k);
	get_data(symbol_raw, ptr_struct_ensym);
	
    set_packet_sent(en_packet, type_packet, num_block, amount_total_block
    , in_sym_amount, en_sym_amount, ptr_struct_ensym->data
    , ptr_struct_ensym->index, ptr_struct_ensym->d);
}


void encode_and_packet_encapsult_primary(char *en_packet, int type_packet
     , int num_block, int amount_total_block, char symbol_raw[][CODELINE]
     , int in_sym_amount, int en_sym_amount, double *cdf)
{
    struct symbol *ptr_struct_ensym;
    ptr_struct_ensym = (struct symbol*)malloc(sizeof(struct symbol));
	//get_d_one(&(ptr_struct_ensym->d)); //modifing data :170701
	get_d(&(ptr_struct_ensym->d), cdf, in_sym_amount);
	//get_index_one(ptr_struct_ensym->index, ptr_struct_ensym->d, k); //modifing data :170701
	get_index(ptr_struct_ensym->index, ptr_struct_ensym->d, in_sym_amount);
	//  get_index_one(ptr_struct_ensym->index, ptr_struct_ensym->d, k);
    get_data(symbol_raw, ptr_struct_ensym);
  	set_packet_sent(en_packet, type_packet, num_block, amount_total_block
    , in_sym_amount, en_sym_amount, ptr_struct_ensym->data
    , ptr_struct_ensym->index, ptr_struct_ensym->d);
}

void get_raw_source(char buf[][CODELINE], int symbol_num)
{
	FILE *fp         = NULL;
	int readlen      = 0;
	int i            = 0;
	char *ptr_char   = NULL;
	int _input_limit = 0;;  
	
//	Fopen_for_read(&fp, "hello.txt");
	ptr_char     = buf[i];
	_input_limit = MAX_INPUT_SYMBOL_AMOUNT;

	Fopen_for_read(&fp, "hello.txt");
	while((readlen = fread(ptr_char, sizeof(char), CODELINE, fp)) > 0)
	{	
		i++;
		if(i >= _input_limit || i >= symbol_num)
		{
			break;
		}
		ptr_char = buf[i];
	}
}

void get_amount_re_fec(int *amount, double _plr, int array[])
{
	int i = 0;
	int tmp = 0;

	while(array[i++] > 0)
		tmp++;

	*amount = tmp + (int)ceil(tmp*8*_plr); // waiting for
	printf("\n****************_amount_unrecovered_symbol%d , tmp = REFEC = %d********************\n\n", tmp, *amount); 
}

void get_amount_pro_fec(int *amount, double _plr, int in_sym_amount)
{
	*amount = ceil(in_sym_amount * 2 * _plr) + in_sym_amount;
}

void Sendto(int sock_id, char *en_packet, size_t len_en_packet
	 , int flags, struct sockaddr *serv_addr, int addrlen)
{
	if(sendto(sock_id, en_packet, len_en_packet, flags, (SA *)&(*serv_addr), addrlen) < 0)
	{
		perror("sending failed!!!!!!!!!!!!\n");
		exit(0);		
	}
}

void Recvfrom(int sock_id, char *buf_recv, int len_buf, int flag
    , struct sockaddr *serv_addr, int *addr_len)
{
	if(recvfrom(sock_id, buf_recv, len_buf, flag, serv_addr, addr_len) <= 0)
	{
		perror("recvfrom faiiled!!!!!!!!!!!!!\n");
		exit(0);
	}
}

//get index of symbol which is unrecovered from packet SAC!!!!
void resolve_and_get_unrecovered_sym(int *_array, char *buf_recv)
{
	int i = 0;
	int k = 0;
	printf("\npacket_sac: unrecovered symbol is");
	for(i = 0, k = 0; i < MAX_INPUT_SYMBOL_AMOUNT; i++)
	{
		if(0 == (buf_recv[2+i/8] & ((unsigned char)1 << (i%8))))
		{
			_array[k] = i+1;
			k++;
			printf("%d  ", i+1);		
		}
	}
	printf("\n\n");
}

int transmit_over_ch(double _plr)
{
    double tmp = 0.0;

    tmp = (rand()%100000)/100000.0;
    if(tmp > _plr)
        return 1;
    else
        return 0;
}
