#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <sched.h>

int main ( void )
{

	int return_value;

	return_value = syscall ( 354 );

	printf("%d\n", return_value );

	return (0);
}