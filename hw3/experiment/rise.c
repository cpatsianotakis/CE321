#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define MAX 100000

int main( int argc, char *argv[] ){

	int i;
	char **table;


	table = malloc ( MAX * sizeof (char*));
	for ( i = 0; i < MAX; i++)
	{
		table[i] = malloc ( sizeof(char) );
		table[i] = 1;
	}

	for ( i = 0; i < MAX; i++)
	{
		free (table[i]);
	}
	free (table);



	
	return 0;
}