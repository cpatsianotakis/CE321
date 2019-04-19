#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>

SYSCALL_DEFINE0( free_mem_stat ) 
{
	long mem_average = 0;
	int i;

	for (i = 0; i < 50; ++i)
	{
		mem_average = memory_free[i] + mem_average;
	}

	return (mem_average / 50);

}
