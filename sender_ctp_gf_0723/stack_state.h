
typedef struct node{
		int 		block_num;
		int 			count;  // 
		char        type_encd;  // spceify packet as PRO_FEC or RE_FEC ???
		int       type_packet;	
		struct 	   node *next;
}node, *p_node;


typedef struct stack{
		p_node    top;
		p_node bottom;
}stack, *p_stack;


void init_stack(p_stack ps);

int Is_empty(p_stack ps);

void push(p_stack ps, int block_num, int count, char type_encd, int type_packet);

void push_re_fec_for_unrecovered_symbol(p_stack stack_order
     , int num_block, int amount_limit , int array_packet_unrecovered[]);

void push_for_normal(p_stack stack_order, int num_block, int amount_limit);

void pop(p_stack ps, int *block_num, int *count, char *type_encd, int *type_packet);

void traverse(p_stack ps);
