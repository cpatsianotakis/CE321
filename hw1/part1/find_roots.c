
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE0 ( find_roots ) 
 {
 	struct task_struct *temp;

 	//print process id from the process that called find roots system call

 	printk("find_roots system call called by process %d\n", current->pid);

 	//start from current process and finish just before init process (pid == 1). 
 	//Print all the ids and names of processes in between.

 	for (temp = current; temp->pid != 1; temp = temp->parent ) 
 	 {
 	 	printk( "id: %d, name: %s\n", temp->pid, temp->comm );
 	 }

 	//print id and name of the last process (init, pid = 1)

 	printk( "id: %d, name: %s\n", temp->pid, temp->comm );

	return(0);
 }
