#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include "../memory_stats.h"


#define ITERATIONS

int main( int argc, char *argv[])
{

	long alloc_samples [ITERATIONS];
	long free_samples [ITERATIONS];
	long alloc_mean;
	long alloc_temp;
	long free_mean;
	long free_temp;

	FILE *fd_result;
	pid_t child_pid;
	int algrorithm;

	unsigned int i;


	printf("============== MEMORY MANAGERS TESTER ==============\n\n\n",);
	
	printf ("Algorithm (0 for First - Next Fit / 1 for Best fit:\n");
	scanf ("%d", &algrorithm );

	while ( algrorithm != 0 && algrorithm != 1 )
	{
		printf ("Please.... Algorithm (0 for First - Next Fit / 1 for Best fit:\n");
		scanf ("%d", &algrorithm );
	}

	if ( algrorithm == 0 )
	{
		fd_result = fopen("first_fit_test_results.txt", "w+");
		if ( fd_result != NULL )
		{
			fprintf(fd_result, "FIRST FIT RESULTS:\n\n" );
		}
	}
	else
	{
		fd_result = fopen("best_fit_test_results.txt", "w+");
		if ( fd_result != NULL )
		{
			fprintf(fd_result, "BEST FIT RESULTS:\n\n" );
		}
	}

	child_pid = fork();
	if ( child_pid == 0 )
	{
		execlp ( "experiment/plateau", "plateau", NULL );
	}

	printf("Plateau (oropedio) results:\n" );
	free_temp = 0;
	alloc_temp = 0;
	for ( i = 0; i < ITERATIONS; i++)
	{
		alloc_samples[i] = slob_get_total_alloc_mem();
		free_samples[i] = slob_get_total_free_mem();
		alloc_temp += alloc_samples[i];
		free_temp += free_samples[i];
		printf("Iteration %d:\nAllocated: %ld\nFree %ld\n\n", i, alloc_samples[i], free_samples[i]);
		sleep(1);
	}

	kill (child_pid, SIGKILL);

	alloc_mean = alloc_temp / ITERATIONS;
	free_mean = free_temp / ITERATIONS;

	fprintf(fd_result, "Plateau (oropedio) results:\n" );
	fprintf(fd_result, "Mean Allocation: %lf\n", alloc_mean);
	fprintf(fd_result, "Mean Free:       %lf\n\n", free_mean);




	child_pid = fork();
	if ( child_pid == 0 )
	{
		execlp ( "experiment/rise", "rise", NULL );
	}

	printf("Rise (rampa) results:\n" );
	free_temp = 0;
	alloc_temp = 0;
	for ( i = 0; i < ITERATIONS; i++)
	{
		alloc_samples[i] = slob_get_total_alloc_mem();
		free_samples[i] = slob_get_total_free_mem();
		alloc_temp += alloc_samples[i];
		free_temp += free_samples[i];
		printf("Iteration %d:\nAllocated: %ld\nFree %ld\n\n", i, alloc_samples[i], free_samples[i]);
		sleep(1);
	}

	kill (child_pid, SIGKILL);

	alloc_mean = alloc_temp / ITERATIONS;
	free_mean = free_temp / ITERATIONS;

	fprintf(fd_result, "Rise (rampa) results:\n" );
	fprintf(fd_result, "Mean Allocation: %lf\n", alloc_mean);
	fprintf(fd_result, "Mean Free:       %lf\n\n", free_mean);



	child_pid = fork();
	if ( child_pid == 0 )
	{
		execlp ( "/usr/bin/gcc", "gcc", "-O3", "-Wall", "-g", "experiment/code_to_run_gcc.c", "-o", "experiment/code_to_run_gcc",  NULL );
	}

	printf("Gcc results:\n" );
	free_temp = 0;
	alloc_temp = 0;
	for ( i = 0; i < ITERATIONS; i++)
	{
		alloc_samples[i] = slob_get_total_alloc_mem();
		free_samples[i] = slob_get_total_free_mem();
		alloc_temp += alloc_samples[i];
		free_temp += free_samples[i];
		printf("Iteration %d:\nAllocated: %ld\nFree %ld\n\n", i, alloc_samples[i], free_samples[i]);
		sleep(1);
	}

	kill (child_pid, SIGKILL);

	alloc_mean = alloc_temp / ITERATIONS;
	free_mean = free_temp / ITERATIONS;

	fprintf(fd_result, "Compiler streaming results:\n" );
	fprintf(fd_result, "Mean Allocation: %lf\n", alloc_mean);
	fprintf(fd_result, "Mean Free:       %lf\n\n", free_mean);

	fclose ( fd_result );

	return 0;
}
