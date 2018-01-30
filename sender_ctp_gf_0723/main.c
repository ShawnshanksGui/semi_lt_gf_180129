//*****author:shawnshanks fei
//*****date:start from 2017.07.11
//*****theme:sender.c of CTP protocol

//attention!!!!!!!!please
//prefix or suffix "amount"of identifier :  reprsent how many 
//prefix or suffix "num" of identifier   :  represent serial number or subscript of array

#include "sender.h"
#include "stack_state.h"

int main(int argc, char **argv)
{
	int i = 0;
	int k = 0;
	int x = 0;
	double plr_init   = 0.05; //initial packet loss rate
	double plr_feedbk =  0.0; // packet loss rate from feedback

///////////////////////////
//	int amount_pro_fec = 5;
//	int amount_re_fec  = 0;

	int flag_timer = 0;
	struct  timeval  start;
	struct  timeval  end;
	double timer = 0.0;
	double start_time=0.0;
	double end_time=0.0;


	int sock_id;
    struct sockaddr_in serv_addr;
    socklen_t serv_addr_len;

	int amount_in_symbol = MAX_INPUT_SYMBOL_AMOUNT;
	int amount_en_symbol[AMOUNT_TOTAL_BLOCK] = {0};
	int amount_total_block = AMOUNT_TOTAL_BLOCK;
	int type_packet     = 0;  //specify packet as proFEC+data  or refec+data?	
	int num_block       = 0;
	int amount_sent     = 0;	
	int sent_sucs       = 0;

	char type_encd    = 0;	

	int amount_re_fec    = 0;	
	int count_num_block = 0;

	int index_sym       = 0;
//	char en_packet[LEN_EN_PACKET] = {'\0'};

	double _cdf[MAX_INPUT_SYMBOL_AMOUNT + 1] =    {0.0};	

	char en_packet[LEN_EN_PACKET]         = {'\0'};	
	char buf_recv[1024]                    = {'\0'};
	char source_array[AMOUNT_TOTAL_BLOCK][MAX_INPUT_SYMBOL_AMOUNT][CODELINE] = {'\0'};

//process state stack
	stack stack_order_procs;
//	int limit_amount;
	int array_packet_unrecovered[AMOUNT_TOTAL_BLOCK][MAX_INPUT_SYMBOL_AMOUNT] = {0};

	int flag_re_fec [AMOUNT_TOTAL_BLOCK] = {0};

	int flag_normal_finish = 0;  //1: 0data+PRO_FEC of all block is finish sending 

	Socket(&sock_id);
	addr_of_reception_prepare(&serv_addr, argv[1], argv[2]);

//	compute_amount_pro_fec(&amount_pro_fec, plr_feedback);
//	get_cdf_final(_cdf, MAX_INPUT_SYMBOL_AMOUNT);
	for(i = 0; i < AMOUNT_TOTAL_BLOCK; i++)
		get_raw_source(source_array[i], 100000);

	plr_feedbk = 0.05; // inition of PLR packet loss rate
	
	num_block  = 0; //specify which block in order starting from 0

	srand((unsigned int)time(NULL));  //random seed!!!!!!!!

//non_block
	x=fcntl(sock_id,F_GETFL,0);
    fcntl(sock_id,F_SETFL,x | O_NONBLOCK);



	gettimeofday(&start,NULL);
    start_time=1000000*start.tv_sec + start.tv_usec;
//    printf("the current time is %ld us\n", start_time);

	init_stack(&stack_order_procs);
	while(1)
	{
		if((YES == Is_empty(&stack_order_procs) && 0 == flag_normal_finish) || YES == flag_re_fec[num_block])
		{
			if(YES == flag_re_fec[num_block])
			{
// invalidate it, don't send more re_FEC packet for this block
				flag_re_fec[num_block] = INVALID;
//according plr_feedback and num_block 
//,get amount of encoding sumbol, and push them into stack.
				get_amount_re_fec(&amount_re_fec, plr_feedbk, array_packet_unrecovered[num_block]);

				push_re_fec_for_unrecovered_symbol(&stack_order_procs
				, num_block, amount_re_fec, array_packet_unrecovered[num_block]);
			}
			else 
			{
				if(count_num_block >= AMOUNT_TOTAL_BLOCK)
					flag_normal_finish = YES;  //1: data+PRO_FEC of all block is finish sending 
				else
				{
					num_block = count_num_block;
					get_amount_pro_fec(&amount_en_symbol[num_block], plr_feedbk, MAX_INPUT_SYMBOL_AMOUNT);
					push_for_normal(&stack_order_procs, num_block, amount_en_symbol[num_block]);
				
					count_num_block++;
				}
			}
		}
		else
		{
//get num_block, index_symbol, type_encoding from stack of order.
			if(NO == Is_empty(&stack_order_procs))
			{
				pop(&stack_order_procs, &num_block, &index_sym, &type_encd, &type_packet);
		
				encode_and_packet_encapsult_semi_random(en_packet, type_packet, num_block
        	    , amount_total_block, source_array[num_block], amount_in_symbol, amount_en_symbol[num_block]
    	        , _cdf, plr_feedbk, type_encd, index_sym);
			
			 	if(!transmit_over_ch(plr_feedbk))
            	{
                	bzero(en_packet, sizeof(en_packet));
           	     	printf("packet %d is lost\n", index_sym);
            	    continue;
       	     	}
				Sendto(sock_id, en_packet, LEN_EN_PACKET, 0, (SA *)&serv_addr, sizeof(serv_addr));	

				printf("the block is %d \n", num_block);			
//				printf("the block is %d\n , enpacket is : \n", num_block);
/*
	          	for(k = 0; k < LEN_EN_PACKET; k++)  
               		printf("%c", en_packet[k]);
            	printf("\n\n");
*/
           		bzero(en_packet, sizeof(en_packet));
			}
		}
			
		if(0 < recvfrom(sock_id, buf_recv, sizeof(buf_recv), 0, (SA *)&serv_addr, &serv_addr_len))
		{
//		printf("%d\n\n", recvfrom(sock_id, buf_recv, sizeof(buf_recv), 0, (SA *)&serv_addr, &serv_addr_len));
			if(P_SAC == buf_recv[LOCATION_TYPE_PACKET])
			{
//				printf("***********receive one packet SAC !!!!!!\n\n");
				num_block = buf_recv[LOCATION_NUM_BLOCK];
				printf("***********receive one packet SAC ,for block: %d\n\n", num_block);

//				bzero(array_packet_unrecovered, sizeof(array_packet_unrecovered));
	//
				if(NO == flag_re_fec[num_block])
					flag_re_fec[num_block] = YES;
				resolve_and_get_unrecovered_sym(array_packet_unrecovered[num_block], buf_recv);
			
			}
			else if(FLAG_ALL_FINISH == buf_recv[LOCATION_TYPE_PACKET])
			{
///////////////////////////////////////////////////////////////////////
				gettimeofday(&end, NULL);
				end_time = 1000000 * end.tv_sec + end.tv_usec;
    			//printf("end_time = %ld us\n",end_time);
    			timer = end_time - start_time;
//    			printf("the current time is %ld us", end_time);
				printf("************all block is decoded successfully!!!!!!!!!!\n\n");
				printf("time spent on encoding %d blocks is %f us \n", AMOUNT_TOTAL_BLOCK, timer);
///////////////////////////////////////////////////////////////////////

				printf("**************decode all block successfully!!!!!!!!!!!!\n\n");
				break;
			}
			else 
				 printf("***********receive one BLOCK SAC !!!!!!\n\n");
			bzero(buf_recv, sizeof(buf_recv));
		}

	}
    return 0;
}















/*
	while(1)
	{	
//	    num_block  = 0; //specify which block in order starting from 0
//		compute_amount_pro_fec(&amount_pro_fec, plr_feedbk);
		amount_sent = 0;
		sent_sucs = 0;
		printf("block_num is %d\n", num_block);

		for(i = 0; i < amount_in_symbol+amount_pro_fec; i++)
		{
////////////////////////encode and packet encapsulation////////////////////////////////
			amount_en_symbol = amount_in_symbol + amount_pro_fec;

			encode_and_packet_encapsult_semi_random(en_packet, type_packet, num_block
			, amount_total_block, source_array[num_block], amount_in_symbol, amount_en_symbol
            , _cdf, amount_sent, amount_sent + 1);
		
			if(!transmit_over_ch(plr_feedbk))
			{
				amount_sent++;
				bzero(en_packet, sizeof(en_packet));
				printf("packet %d is lost\n", amount_sent);
				continue;
			}
			amount_sent++;
			sent_sucs++;  // amount of packet sent and received successfully

			Sendto(sock_id, en_packet, LEN_EN_PACKET, 0, (SA *)&serv_addr, sizeof(serv_addr));

			printf("enpacket is : ");
			for(k = 0; k < LEN_EN_PACKET; k++)	
				printf("%c", en_packet[k]);
			printf("\n");
			bzero(en_packet, sizeof(en_packet));
//
//
/////////////////////////receive feedback from receiver////////////////////////////////
			Recvfrom(sock_id, buf_ctp, sizeof(buf_ctp), 0, (SA *)&serv_addr, &serv_addr_len);
/////////////////////////resolve and judge the feedback///////////////////////////////
			if(FLAG_PACKET_SACK == buf_ctp[LOCATION_TYPE])
			{
//get and record which input symbols waiting for continuing redecoding and the amount of input symbol not decoded successfully
				get_uncovered_input_symbol(&num_block_unfinshed, &amount_residual, num_array, buf_ctp);
//compute amount of reactive FEC encoding symbol
				compute_amount_re_fec(&amount_re_fec);

//encode and get the reactive FEC encoding symbols and send all;
				for(i = 0; num_array[i] > 0; i++)  //number of input sym starting from 1
				{
					encode_and_packet_encapsult_semi_random(num_block_unfinished, buf_ctp[LOCATION_NUM_BLOCK], source_array[num_block], en_packet, amount_input_symbol, amount_encoding_symbol, _cdf, num_array[i], num_array[i]+1);	
					Sendto(sock_id, en_packet, len_input_symbol, 0, (SA *)&serv_addr, sizeof(sev_addr));
					bzero(en_packet, sizeof(en_packet));
				}
				for(i = 0; i < amount_re_fec - amount_residual; i++)
				{
					encode_and_packet_encapsult_primary(num_block_unfinished, buf_ctp[LOCATION_NUM_BLOCK], source_array[num_block], en_packet, amount_input_symbol, amount_encoding_symbol, _cdf, num_array[i], num_array[i]+1); 
                 
				   	Sendto(sock_id, en_packet, len_input_symbol, 0, (SA *)&serv_addr, sizeof(sev_addr));
					bzero(en_packet, sizeof(en_packet));
				}				
			}		
			else if(FLAG_BLOCK_SACK == buf_ctp[LOCATION_TYPE])
			{
				get_plr_from_feedbk(&plr_feedbk, buf_ctp[LOCATION_PLR]);
			}			 
//
//						
		}	
		if(num_block == 7)
			break;
		num_block++;	
	}
*/

