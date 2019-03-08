Panagiotis Anastasiadis   2134 paanastasiadis@uth.gr

Charalampos Patsianotakis 2116 cpatsianotakis@uth.gr

Tilemachos Tsiapras 	  2229 ttsiapras@uth.gr

#########################################################################################[Info]

IMPORTANT NOTE! 

Inside linux kernel (linux-3.14.62-dev), we added also the system call 
hello_syscall from eclass examples before we start the process of project 1.  

-----------------------------------------------------------------------------------------------

Compilation of find_roots_lib.c:

In this Directory
->Run gcc -c roots.c -o roots.o
->Run ar rcs libroots.a roots.o
->Run gcc -Wall -g find_roots_lib.c -o find_roots_lib libroots.a

Execution of find_roots_lib:

->Run ./find_roots_lib
->To check the results run dmesg or dmesg | tail

-----------------------------------------------------------------------------------------------
Compilation of module project1_iosched:

->Go to directory project1_module
->Run make
->sudo insmod project1-iosched.ko

Execution:

->To check current scheduler run: cat /sys/block/sda/queue/scheduler
->To enable our scheduler run: sudo bash -c 'echo teamXX > /sys/block/sda/queue/scheduler'
->To restore default scheduler run: sudo bash -c 'echo cfq > /sys/block/sda/queue/scheduler'

Removal:

->To remove the custom made module run: sudo rmmod project1_iosched

-----------------------------------------------------------------------------------------------
Compilation of module sysfs_module:

->Go to directory sysfs_module
->Run make
->sudo insmod sysfs_module.ko

Execution:

->Run cat /sys/kernel/teamXX/find_roots
->To check results run: dmesg | tail or dmesg

Removal:

->To remove the custom made module run: sudo rmmod sysfs_module

###############################################################################################
