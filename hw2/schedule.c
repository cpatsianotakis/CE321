/* schedule.c
 * This file contains the primary logic for the 
 * scheduler.
 */
#include "schedule.h"
#include "macros.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "privatestructs.h"


#define NEWTASKSLICE (NS_TO_JIFFIES(100000000))

/* Local Globals
 * rq - This is a pointer to the runqueue that the scheduler uses.
 * current - A pointer to the current running task.
 */
struct runqueue *rq;
struct task_struct *current;

/* External Globals
 * jiffies - A discrete unit of time used for scheduling.
 *			 There are HZ jiffies in a second, (HZ is 
 *			 declared in macros.h), and is usually
 *			 1 or 10 milliseconds.
 */
extern long long jiffies;
extern struct task_struct *idle;

/*-----------------Initilization/Shutdown Code-------------------*/
/* This code is not used by the scheduler, but by the virtual machine
 * to setup and destroy the scheduler cleanly.
 */
 
 /* initscheduler
  * Sets up and allocates memory for the scheduler, as well
  * as sets initial values. This function should also
  * set the initial effective priority for the "seed" task 
  * and enqueu it in the scheduler.
  * INPUT:
  * newrq - A pointer to an allocated rq to assign to your
  *			local rq.
  * seedTask - A pointer to a task to seed the scheduler and start
  * the simulation.
  */
void initschedule(struct runqueue *newrq, struct task_struct *seedTask)
{
	seedTask->next = seedTask->prev = seedTask;
	newrq->head = seedTask;
	newrq->nr_running++;
}

/* killschedule
 * This function should free any memory that 
 * was allocated when setting up the runqueu.
 * It SHOULD NOT free the runqueue itself.



 */
void killschedule()
{
	return;
}


void print_rq () {
	struct task_struct *curr;
	
	printf("Rq: \n");
	curr = rq->head;
	if (curr)
		printf("%p", curr);
	while(curr->next != rq->head) {
		curr = curr->next;
		printf(", %p", curr);
	};
	printf("\n");
}

/*-------------Scheduler Code Goes Below------------*/
/* This is the beginning of the actual scheduling logic */

/* schedule
 * Gets the next task in the queue
 */
void schedule()
{
	struct task_struct *chosen;
	unsigned int saved_exp_burst;
	
	printf("\nIn schedule\n");
	print_rq();
	
	current->need_reschedule = 0; /* Always make sure to reset that, in case *
								   * we entered the scheduler because current*
								   * had requested so by setting this flag   */


	printf("--------------------------------------------------------------------------\n");
	printf("Currently in CPU: %s\n",current->thread_info->processName );
	
	if (rq->nr_running == 1  ) {

		printf("--------------------------------------------------------------------------\n");
		context_switch(rq->head);
	
	}
	else if ( rq->nr_running == 2 ) {

		rq->head->next->cpu_owned = sched_clock();
	
		printf("--------------------------------------------------------------------------\n");
		context_switch(rq->head->next);

	}
	else {

		printf("Current: Saved Exp Burst = %d\n", current->exp_burst );

		saved_exp_burst = current->exp_burst;

		calculate_expBurst ( current );

		printf("Current: Temp Exp Burst = %d\n", current->exp_burst );

		chosen = find_minExpBurst( rq ); 
		
		printf("==> CHOSEN IS [%s], CHOSEN EXP_BURST IS [%u] <==\n", chosen->thread_info->processName, chosen->exp_burst);

		if ( current == chosen ) {
			
			current->exp_burst = saved_exp_burst;

			printf("--------------------------------------------------------------------------\n");

			context_switch(current);
		}

		else {

			current->burst = sched_clock() - current->cpu_owned;
			calculate_expBurst ( current );

			printf("%s IS GOING TO LEAVE CPU\n", current->thread_info->processName );
			printf("Calculating new burst == %d and exp_burst %d for %s \n", current->burst, current->exp_burst, current->thread_info->processName);
			printf("--------------------------------------------------------------------------\n");

			chosen->cpu_owned = sched_clock();
			context_switch(chosen);
		}

		
	}


}


/* sched_fork
 * Sets up schedule info for a newly forked task
 */
void sched_fork(struct task_struct *p)
{
	p->time_slice = 5;

/* ------ Here goes our code ---------------- */

	p->burst = 0;
	p->exp_burst = 0;
	p->cpu_owned = 0;

/* ---------End of our code ---------------- */

}

/* scheduler_tick
 * Updates information and priority
 * for the task that is currently running.
 */
void scheduler_tick(struct task_struct *p)
{
	
	if ( p == rq->head )
	{
		schedule();
		return;
	}

	p->time_slice--;
	if ( p->time_slice <= 0 )
	{
		p->time_slice = 5;
		schedule();
	}
	//schedule();

}

/* wake_up_new_task
 * Prepares information for a task
 * that is waking up for the first time
 * (being created).
 */
void wake_up_new_task(struct task_struct *p)
{	
	p->next = rq->head->next;
	p->prev = rq->head;
	p->next->prev = p;
	p->prev->next = p;
	
	rq->nr_running++;
}

/* activate_task
 * Activates a task that is being woken-up
 * from sleeping.
 */
void activate_task(struct task_struct *p)
{
	p->next = rq->head->next;
	p->prev = rq->head;
	p->next->prev = p;
	p->prev->next = p;
	
	rq->nr_running++;
}

/* deactivate_task
 * Removes a running task from the scheduler to
 * put it to sleep.
 */
void deactivate_task(struct task_struct *p)
{
	p->prev->next = p->next;
	p->next->prev = p->prev;
	p->next = p->prev = NULL; /* Make sure to set them to NULL *
							   * next is checked in cpu.c      */

	rq->nr_running--;
}


void calculate_expBurst ( struct task_struct *p ) 
{

	p->exp_burst =  ( p->burst + ( 0.5 * p->exp_burst ) ) / ( 1 + 0.5 ); 


}
struct task_struct *find_minExpBurst( struct runqueue *rq ) 
{

	unsigned long long minimum_exp_burst;
	struct task_struct *temp, *min_task;

	minimum_exp_burst = rq->head->next->exp_burst;
	min_task = rq->head->next;

	printf("Exp.Bursts...\n" );

	for ( temp = rq->head->next; temp != rq->head; temp = temp->next )
	{
		printf("%s, %d\n", temp->thread_info->processName, temp->exp_burst);
		
		if ( temp->exp_burst < minimum_exp_burst )
		{
			minimum_exp_burst = temp->exp_burst;
			min_task = temp;
		}
	}

	return ( min_task );
}

// unsigned long long find_maxWaitingInRQ( struct runqueue *rq ) 
// {

// 	unsigned long long max_waiting;
// 	struct task_struct temp;

// 	max_waiting = rq->head->last_in_runqueue;

// 	for ( temp = rq->head->next; temp != NULL; temp = temp->next )
// 	{
// 		if ( temp->last_in_runqueue < max_waiting )
// 		{
// 			max_waiting = temp->last_in_runqueue;
// 		}
// 	}

// 	return ( max_waiting );
// }


//Important!!!! Change the algorithm

// unsigned long long calculate_goodness ( struct runqueue *rq , struct task_struct *p, unsigned long long current_time  ) 
// {
// 	return (((1 + p->exp_burst) / (1 + find_minExpBurst(rq))) * ((1 + find_maxWaitingInRQ(rq)) / (1 + (current_time - p->last_in_runqueue))));
// }


