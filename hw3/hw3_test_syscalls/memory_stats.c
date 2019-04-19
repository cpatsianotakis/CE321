#include <sys/syscall.h>
#include <unistd.h>
#include "memory_stats.h"

long free_memory_stats(void){
	return(syscall(__NR_free_mem_stat));
}

long alloc_memory_stats(void){
	return(syscall(__NR_alloc_mem_stat));
}