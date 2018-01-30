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

int Is_empty(p_stack ps)

void push(p_stack ps, int block_num, int count)

void pop(p_stack ps, int *block_num, int *count)

void traverse(p_stack ps)
