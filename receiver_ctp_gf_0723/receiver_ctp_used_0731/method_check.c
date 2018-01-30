#include "receiver.h"


void record_data_de_sucs(FILE *fp, char data_recovery[][CODELINE], int num_block)
{
	int i = 0;
	int k = 0;

	fprintf(fp, "the block %d is \n", num_block);

	for(i = 0; i < MAX_INPUT_SYMBOL_AMOUNT; i++)
    {
    	for(k = 0; k < CODELINE; k++)
   	    	fprintf(fp, "%c", data_recovery[i][k]);
        fprintf(fp, "\n\n");
	}
} 
