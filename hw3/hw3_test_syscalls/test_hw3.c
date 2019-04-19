#include <stdio.h>
#include <stdlib.h>
#include "memory_stats.h"
int main (void)
{
	//char * buffer0, buffer1, buffer2, buffer3, buffer4, buffer5, buffer6, buffer7;

        // buffer0 = (char*) malloc (9000000);
        // buffer1 = (char*) malloc (345555);
        // buffer2 = (char*) malloc (1234);
        // buffer3 = (char*) malloc (144445554);
        // buffer4 = (char*) malloc (55630);
        // buffer5 = (char*) malloc (10064666665);
        // buffer6 = (char*) malloc (105555555550);
        // buffer7 = (char*) malloc (1000000000);

	printf("Amount Free: %ld bytes\n", free_memory_stats());
        printf("Amount Claimed: %ld bytes\n",alloc_memory_stats());

        return 0;
}
