#include "sender.h"
#include "stack_state.h"

void init_stack(p_stack ps)
{
	if((ps->top = (p_node)malloc(sizeof(node))) == NULL)
	{
		printf("malloc failed!!!\n");
		exit(0);
	}
	else
	{
		ps->bottom = ps->top;
		ps->top->next = NULL;
	}
}

int Is_empty(p_stack ps)
{
	if(ps->top == ps->bottom)
		return YES;
	else
		return NO;
}

void push(p_stack ps, int block_num, int count, char type_encd, int type_packet)
{
	p_node p_new = NULL;

	if((p_new = (p_node)malloc(sizeof(node))) == NULL)
	{
		printf("push: p_new malloc failed!!!\n");
		exit(0);
	}
	p_new->block_num   =   block_num;
	p_new->count       =       count;
	p_new->type_encd   =   type_encd;
	p_new->type_packet = type_packet;
	p_new->next        =     ps->top;	

	ps->top            =       p_new;
}

void push_re_fec_for_unrecovered_symbol(p_stack stack_order
     , int num_block, int amount_limit , int array_packet_unrecovered[])
{
	int i = 0;
	
	for(i = 0; i < amount_limit; i++)
	{
		if(array_packet_unrecovered[i] > 0)
			push(stack_order, num_block, array_packet_unrecovered[i], FEC_D_ONE, DATA_RE_FEC);
		else
			push(stack_order, num_block, 0, FEC_D_NON_ONE, DATA_RE_FEC);
	}	
}

void push_for_normal(p_stack stack_order, int num_block, int amount_limit)
{
	while(amount_limit)
	{
		if(amount_limit <= MAX_INPUT_SYMBOL_AMOUNT)
		{
			push(stack_order, num_block, amount_limit, FEC_D_ONE, DATA_PRO_FEC);
		}
		else
			push(stack_order, num_block, 0, FEC_D_NON_ONE, DATA_PRO_FEC);
		
		amount_limit--;
	}
}

void pop(p_stack ps, int *block_num, int *count, char *type_encd, int *type_packet)
{
	p_node tmp = NULL;
	if(Is_empty(ps))
	{
		printf("stack is empty!!!\n");
		exit(0);
	}
	else
	{
		*block_num   =   ps->top->block_num;
		*count       =       ps->top->count;
		*type_encd   =   ps->top->type_encd;
		*type_packet = ps->top->type_packet;
	
		tmp     =       ps->top;
		ps->top = ps->top->next;

		free(tmp);
	}
}

void traverse(p_stack ps)
{
	p_node node_new = ps->top;
	while(node_new != ps->bottom)
	{
		printf("block_num = %d, count = %d\n", node_new->block_num, node_new->count);
		node_new = node_new->next;
	}
}

/*
int main()
{
	stack block_order_procs;

	int block_num, count = 0;

	init_stack(&block_order_procs);
	while(1)
	{
		scanf("%d %d", &block_num, &count);
		push(&block_order_procs, block_num, count);
		traverse(&block_order_procs);
	}	
	return 0;
}
*/
