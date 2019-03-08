#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/sched.h>

static struct kobject *example_kobject;
volatile int roots = 0;

static ssize_t find_roots (struct kobject *kobj, struct kobj_attribute *attr, char *buf)
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

	return sprintf( buf, "%d\n", current->pid );	
 }

struct kobj_attribute foo_attribute = __ATTR (find_roots, 0660, find_roots, NULL);

static int __init mymodule_init ( void ) 
 {
 	int error = 0;

 	example_kobject = kobject_create_and_add ( "teamXX", kernel_kobj );

 	if ( !example_kobject )
 	 {
 		return -ENOMEM;
 	 }

 	 error = sysfs_create_file ( example_kobject, &foo_attribute.attr );

 	 if ( error ) 
 	  {
 	  	printk ( "failed to create the file find_roots in /sys/kernel/teamXX/find_roots \n" );
 	  }

 	  return error;
 }

static void __exit mymodule_exit ( void ) 
 {
	printk ( "Module uninitialized successfully \n" );
	kobject_put ( example_kobject );
 }

module_init ( mymodule_init );
module_exit ( mymodule_exit );

MODULE_LICENSE("GPL"); 
