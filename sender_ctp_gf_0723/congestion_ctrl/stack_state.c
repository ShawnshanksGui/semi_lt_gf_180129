#include "stdlib.h"
#include  "stdio.h"

#define YES 1
#define NO  0

typedef struct node{
		int 		block_num;
		int 			count;
		struct 	   node *next;
}node, *p_node;


typedef struct stack{
		p_node    top;
		p_node bottom;
}stack, *p_stack;


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

void push(p_stack ps, int block_num, int count)
{
	p_node p_new = NULL;

	if((p_new = (p_node)malloc(sizeof(node))) == NULL)
	{
		printf("push: p_new malloc failed!!!\n");
		exit(0);
	}
	p_new->block_num = block_num;
	p_new->count     = count;
	
	p_new->next = ps->top;	
	ps->top = p_new;
}

void pop(p_stack ps, int *block_num, int *count)
{
	p_node tmp = NULL;
	if(Is_empty(ps))
	{
		printf("stack is empty!!!\n");
		exit(0);
	}
	else
	{
		*block_num = ps->top->block_num;
		*count     = ps->top->count;

		tmp = ps->top;
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
