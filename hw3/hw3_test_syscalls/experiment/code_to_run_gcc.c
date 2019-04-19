#include <stdio.h>

int main ( void )
{

	int i , j , k;
	int a[100][100][100];

	for (i = 0; i < 100; i++)
	{
		for (j = 0; j < 100; j++)
		{
			for (k = 0; k < 100; k++)
			{
				if ( a[i][j][k] > 30 )
					a[i][j][k] += 2;
				else
					a[i][j][k] += 1;
			}
		}
	}


	return 0;
}