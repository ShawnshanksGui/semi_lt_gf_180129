//*************************************************************
//!!!!!!!!!!!!!!!!!!attention please: !!!!!!!!!!!!!!!!!!!!!!!!!
//num   : means the serial number or the subscript of a array;   
//amount: means how many.       
//*************************************************************

#include "stack_state.h"
#include "math.h"

static int send_window;
static int _count_for_window;


int get_ece_from_ip_layer(struct )
{
	int flag_ece;
	......
	......
	return flag_ece;   // "1: ece marked,   0: ece not marked"
}

int judge_ece_marked()
{
	if(get_ece_from_ip_layer(...))
		return 1;
	else 
		return 0;
}
   
void alpha_update(double *alpha, double weitht, double frac_ece)
{
	double tmp = *alpha;

	*alpha = (1 - weight) * tmp + weight * frack_ece;
}

int cw_adjust(int window_old, int ece_count, int sac_count)
{
	static double alpha;
	double weight = 1.0/16.0;
	
	int window_new = 0;

	double fract_ece = ece_count / (double)sac_count;  // percentage of ece mark
	alpha_update(&alpha, weight, fract_ece_mark);
	
	window_new = ceil(window_old * (1 - alpha / 2));  //or floor();
	
	return window_new;
}

void signal_handler()
{
//	timer refresh?
	if(0 == _count_for_window)
		_count_for_window = send_window;
}

int main()
{
	send_window = 4;   //sendwindow initialize as 4;

	for(;;)
	{
//	_count_for_window = send_window;
	setitimer(SIGALARM, signal_handler);
	
//push when receive the feedback: last block is not decoded successfully;
//or stack is empty;
	if(YES == Is_empty(stack_order_procs) || YES == flag_re_fec)
	{
		if(1 == flag_re_fec)
		{
			compute_amount_pro_fec(&limit_amount, RE_FEC, num_block);
			while(limit_amount--) 	
			{
				push(&stack_order_procs, num_block, amount_limit);
			}
			falg_re_fec == NO;
		}
		else
		{
			compute_amount_pro_fec(&limit_amount, PRO_FEC, num_block); 
     		while(limit_amount--)
    	    {
            	push(&stack_order_procs, num_block, amount_limit);
            }
		}			
	}
		
	if(_count_for_window > 0)
	{
		pop(&num_block, &num_count, type_packet);  // get num_block, num_count from stack;
		

		encoding_and_packet_encapsult(num_block, en_packet, source_array);	
		send_dpdk(en_packet, ...);
		
		num_rand = (rand() % 100000) / (double)100000;
// this packet will be erased if num_rand < packet_loss_rate
		if(!transimit_over_ch(num_rand, packet_loss_rate))
		{
//			sent_total_amount++;

//			sent_real_for_window++;				
			amount_pkt_loss++;
			_count_for_window--;
			continue;
		}
//		sent_real++;
//		sent_total_amount++;

//_count_for_window control whether sender can send packet or only can  receive feedback
		_count_for_window--; //packet of sending window which can be sent minus 1 ,after send one packet

//*****************receive start
		if(recv_dpdk(buf_ctp, ...) > 0)  //already received one SAC feedback
		{
			sac_count++;
			if(judge_ece_marked() == 1)   // 
				ece_count++;
		}
		
		if(sac_count >= floor((send_window-amount_pkt_loss)/2))  // can deal with both odd and even 
		{
			if(0 == ece_count)
			{
				send_window = 2 * send_window;
			}
			else
			{
				send_window = cw_adjust(send_window, ece_count, sac_count);
				ece_count = 0;
				sac_count = 0;
				amount_pkt_loss = 0;
			}
		}
				
//        if(_count_for_window == 0)
//      {   
//			while(timer());   //keep not moving forward ,stay here unitl timer overtime;
//			_count_for_window = send_window;  // upodate _count_for_window
//       }
	}
	else if(0 == _count_for_window)   //current window has been all sent 
	{
	    if(recv_dpdk(buf_ctp, ...) > 0)  //already received one SAC feedback
        {
            sac_count++;
            if(judge_ece_marked() == 1)   // 
                ece_count++;
        }
    
        if(sac_count >= floor((send_window-amount_pkt_loss)/2))  // can deal with both odd and even 
        {
            if(0 == ece_count)
            {
                send_window = 2 * send_window;
            }
            else
            {
                send_window = cw_adjust(send_window, ece_count, sac_count);
                ece_count = 0;
                sac_count = 0;
                amount_pkt_loss = 0;
            }
        }	
	}

	return 0;
}
