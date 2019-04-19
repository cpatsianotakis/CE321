#include<stdio.h>

#define MAX 10000

int main()
{

	int i;
	char **table;

	table = malloc ( MAX * sizeof (char*));

	for ( i = 0; i < MAX; i++ )
	{
		table[i] = malloc ( sizeof(char) );
		table[i] = 1;
	}

	for ( i = 0; i < MAX; i++ )
	{
		table[i]++;
	}

	for ( i = 0; i < MAX; i++ )
	{
		table[i]++;
	}

	for ( i = 0; i < MAX; i++ )
	{
		table[i]++;
	}

	for ( i = 0; i < MAX; i++ )
	{
		table[i]++;
	}

	for ( i = 0; i < MAX; i++ )
	{
		table[i]++;
	}

	for ( i = 0; i < MAX; i++ )
	{
		table[i]++;
	}

	for ( i = 0; i < MAX; i++ )
	{
		table[i]++;
	}

	for ( i = 0; i < MAX; i++ )
	{
		table[i]++;
	}

	for ( i = 0; i < MAX; i++ )
	{
		free (table[i]);
	}
	free (table);

}