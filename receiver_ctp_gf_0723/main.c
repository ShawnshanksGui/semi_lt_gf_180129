#include "receiver.h"




int main(int argc, char **argv)
{
	int i = 0;
	int k = 0;
	int success_flag = 0;

	int flag_timer = 0;
	struct  timeval  start;
	struct  timeval  end;
	double timer = 0.0;
	double start_time=0.0;
	double end_time=0.0;


	FILE *fp;
    int sock_id = 0;
    socklen_t client_addr_len;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
	socklen_t serv_addr_len;
    client_addr_len = sizeof(client_addr);

//	char array_state_block[AMOUNT_TOTAL_BLOCK][BYTE_INDEX_BLOCK];  //BYTE_AMOUNT_BLOCK = (int)ceil(AMOUNT_TOTAL_BLOCK / 8.0) 
	char buf_ctp[LEN_EN_PACKET]          =   {'\0'};	
    char buf_packet_sac[LEN_P_SAC]  =   {'\0'};
	char buf_block_sac[LEN_B_SAC]   =   {'\0'};   
	char buf_terminal[1]            =   {'\0'};

//	int  num_current_block =   0;
	int  num_block         =   0;       //recurrent block
	int  num_last_block    =   0;		 //last block
	
	int count = 0;
	int flag_packet_sac    =   0;
	double plr_estimate    = 0.0; // plr: packet loss rate

	char array_state_block[AMOUNT_TOTAL_BLOCK] = {'\0'};
	char array_state_packet[AMOUNT_TOTAL_BLOCK][MAX_INPUT_SYMBOL_AMOUNT] = {'\0'};
	char data_recovery[MAX_INPUT_SYMBOL_AMOUNT][CODELINE] = {'\0'};

//five intermediate variables for all blocks in decode proccedure//
	int _addr_buf[AMOUNT_TOTAL_BLOCK]        = {0};
	int _addr_rip_insert[AMOUNT_TOTAL_BLOCK] = {0};
	int _addr_rip_procs[AMOUNT_TOTAL_BLOCK]  = {0};		
	struct rip ripple[AMOUNT_TOTAL_BLOCK];
	de_symbol de_buf[AMOUNT_TOTAL_BLOCK][MAX_EN_SYMBOL_RECV];
///////////////////////////////////////////////////////////////////

    if(argc != 3)
    {
        usage(argv[0]);
    }

//prepare for socket and bind
    Socket(&sock_id);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(sock_id, (SA *)&serv_addr, sizeof(serv_addr));
	
//    Fopen_for_write_not_cover_origin(&fp, "check.txt");

	for(;;)
	{
		bzero(buf_ctp, sizeof(buf_ctp));
		Recv_one_packet_from(sock_id, buf_ctp, sizeof(buf_ctp), 0, (SA *)&client_addr, &client_addr_len);
//		printf("receive a packet: block: %d\n", buf_ctp[LOCATION_NUM_BLOCK]);		
		if(0 == flag_timer) {
			gettimeofday(&start, NULL);
            start_time=1000000*start.tv_sec + start.tv_usec;
//            printf("the current time is %ld us\n", start_time);
			flag_timer = 1;
		}

		num_block = buf_ctp[LOCATION_NUM_BLOCK];
	
		if(DATA_PRO_FEC == buf_ctp[LOCATION_TYPE_PACKET])
		{
//			printf("receive a PROFEC-packet: block: %d\n", num_block);
		
			if(YES == Is_success_decode(array_state_block, num_block))
			{
				count = 0;   //prevent packet received not in order!!!
				continue;
			}
			else if(num_last_block != num_block) 
			{
				
//				if(NO == Is_success_decode(array_state_block, num_last_block))  //
					count++;
					if(count == 3)
					{
						if(NO == Is_success_decode(array_state_block, num_last_block))
						{
//
//							printf("send a packet SAC to source node for block: %d\n", num_last_block);
               		 		feedbk_packet_sac_generator(buf_packet_sac, num_last_block, ripple[num_last_block].id_rip);
               	    		sendto(sock_id, buf_packet_sac, LEN_P_SAC, 0, (SA *)&client_addr, sizeof(client_addr));
							bzero(buf_packet_sac, LEN_P_SAC);
						}
						count = 0;
						num_last_block = num_block;
					}
/*
				count++;
				if(count == 3)
				{
					flag_packet_sac = 1;
					printf("block: %d is NOT decoded sucessfully!!!!!\n\n", num_last_block);
				}	
				if(1 == flag_packet_sac)
				{
					if(0 == (count%3))
					{
						feedbk_packet_sac_generator(buf_packet_sac, num_last_block, ripple[num_last_block].id_rip);
						sendto(sock_id, buf_packet_sac, LEN_P_SAC, 0, (SA *)&client_addr, sizeof(client_addr));
//
						printf("****************send a packet SAC for block: %d*******************\n\n", num_last_block);					
						if(9 == count) // p_sac only transmit 3 times
						{
							flag_packet_sac = 0;
							count = 0;
							num_last_block = num_block;
						}
					}		
				}
*/
			}			
////////////////////////////////////////////////////////
			if(NO == Is_success_decode(array_state_block, num_block) && SUCCESS_ONE_BLOCK == 
			  decode_for_one_packet(buf_ctp, &_addr_buf[num_block], &_addr_rip_insert[num_block]
              , &_addr_rip_procs[num_block], &ripple[num_block], de_buf[num_block], data_recovery))
			{
//				printf("Block %d decoded successfully :\n\n", num_block);
//
/*
				Fopen_for_write_not_cover_origin(&fp, "check.txt");
				record_data_de_sucs(fp, data_recovery, num_block);				
				fclose(fp);
*/
//
				bzero(data_recovery, sizeof(data_recovery));
				
				set_state_sucs(array_state_block, num_block);				
//computing plr according to
   		      	feedbk_block_sac_generator(buf_block_sac, num_block, array_state_block);
				sendto(sock_id, buf_block_sac, LEN_B_SAC, 0, (SA *)&client_addr, sizeof(client_addr));
//
//				printf("send a BLOCK SAC to sender for block: %d \n\n", num_block);
				bzero(buf_block_sac, sizeof(buf_block_sac));
			}
		}

		else if(DATA_RE_FEC == buf_ctp[LOCATION_TYPE_PACKET])
		{
//			printf("receive RE_FEC packet, for block : %d!!!!!!!!\n\n", num_block);
		
			if(NO == Is_success_decode(array_state_block, num_block) && SUCCESS_ONE_BLOCK == 
			  decode_for_one_packet(buf_ctp, &_addr_buf[num_block], &_addr_rip_insert[num_block]
              , &_addr_rip_procs[num_block], &ripple[num_block], de_buf[num_block], data_recovery))
			{

//				printf("Block %d decoded successfully :\n\n", num_block);
//
/*
				Fopen_for_write_not_cover_origin(&fp, "check.txt"); 
                record_data_de_sucs(fp, data_recovery, num_block); 
                bzero(data_recovery, sizeof(data_recovery));
				fclose(fp);
*/
//	
				set_state_sucs(array_state_block, num_block);
               	feedbk_block_sac_generator(buf_block_sac, num_block, array_state_block);
				sendto(sock_id, buf_block_sac, LEN_B_SAC, 0, (SA *)&client_addr, sizeof(client_addr));
//			
//				printf("send a BLOCK SAC to sender for block: %d \n\n", num_block);
				bzero(buf_block_sac, sizeof(buf_block_sac));
			}
		}
		if(Is_all_decoded_sucs(array_state_block))
        {
        	gettimeofday(&end,NULL);
    		end_time = 1000000 * end.tv_sec + end.tv_usec;
    		//printf("end_time = %ld us\n",end_time);
    		timer = end_time - start_time;
//    		printf("the current time is %ld us", end_time);
			printf("************all block is decoded successfully!!!!!!!!!!\n\n");
			printf("time spent on decoding %d blocks is %f us\n", AMOUNT_TOTAL_BLOCK, timer);
        	feedbk_terminal_generator(buf_terminal);
        	sendto(sock_id, buf_terminal, sizeof(buf_terminal), 0, (SA *)&client_addr, sizeof(client_addr));      
			break; 
        }	
	}
//	fclose(fp);
	return 0;
}
