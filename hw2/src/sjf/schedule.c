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

#define ANSI_COLOR_GREEN     "\x1b[32m"
#define ANSI_COLOR_CYAN      "\x1b[36m"
#define ANSI_COLOR_RESET     "\x1b[0m"
#define ANSI_COLOR_MAGENTA   "\x1b[35m"
#define ANSI_COLOR_RED       "\x1b[31m"


#define NEWTASKSLICE (NS_TO_JIFFIES(100000000))

#define TIMESLICE 50

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
		printf("%s", curr->thread_info->processName);
	while(curr->next != rq->head) {
		curr = curr->next;
		printf(", %s", curr->thread_info->processName);
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
	unsigned int saved_burst;
	
	printf("In schedule\n");
	print_rq();
	
	current->need_reschedule = 0; /* Always make sure to reset that, in case *
								   * we entered the scheduler because current*
								   * had requested so by setting this flag   */


	printf( ANSI_COLOR_CYAN "==========================================================================\n");
	
	
	if (rq->nr_running == 1  ) {
		printf("Switch: "ANSI_COLOR_RED" %s "ANSI_COLOR_CYAN ,current->thread_info->processName );
		printf("==========>[" ANSI_COLOR_RED "%s " ANSI_COLOR_CYAN "]\n", rq->head->thread_info->processName );
		printf("--------------------------------------------------------------------------\n"ANSI_COLOR_RESET);
		context_switch(rq->head);
	
	}
	else if ( rq->nr_running == 2 ) {

		rq->head->next->cpu_owned = sched_clock();
		printf("Switch: "ANSI_COLOR_RED" %s "ANSI_COLOR_CYAN ,current->thread_info->processName );
		//printf("%s IS GOING TO LEAVE CPU\n", current->thread_info->processName );
		printf("==========>[" ANSI_COLOR_RED "%s " ANSI_COLOR_CYAN "]\n", rq->head->next->thread_info->processName );
		printf("--------------------------------------------------------------------------\n"ANSI_COLOR_RESET);
		
		context_switch(rq->head->next);


	}
	else {

		printf("Current: Saved Exp Burst = %dms\n", current->exp_burst / 1000000 );

		saved_exp_burst = current->exp_burst;
		saved_burst = current->burst;
		current->burst = sched_clock() - current->cpu_owned;

		calculate_expBurst ( current );

		printf("Current: Temp Exp Burst = %dms, Temp Burst = %dms\n", current->exp_burst / 1000000, current->burst / 1000000 );

		chosen = find_minExpBurst( rq ); 
		
		
		printf("Switch:"ANSI_COLOR_RED" %s"ANSI_COLOR_CYAN" ", current->thread_info->processName );
		printf("==========>["ANSI_COLOR_RED"%s"ANSI_COLOR_CYAN"], CHOSEN EXP_BURST IS [%ums] <==\n", chosen->thread_info->processName, chosen->exp_burst / 1000000);
		printf("--------------------------------------------------------------------------\n"ANSI_COLOR_RESET);

		if ( current == chosen ) {
			current->burst = saved_burst;
			current->exp_burst = saved_exp_burst;
			context_switch(current);
		}

		else {
			current->XE += current->burst;
			current->time_slice = TIMESLICE;
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
	p->time_slice = TIMESLICE;

/* ------ Here goes our code ---------------- */

	p->burst = 0;
	p->exp_burst = 0;
	p->cpu_owned = 0;
	p->XE = 0;
	p->XD = sched_clock();
	p->XA = 0;

/* ---------End of our code ---------------- */

}

/* scheduler_tick
 * Updates information and priority
 * for the task that is currently running.
 */
void scheduler_tick(struct task_struct *p)
{
	
	if (p == rq->head && rq->nr_running > 1)
	{
		schedule();
	}
	else if ( p != rq->head )
	{
		p->time_slice--;

		if ( p->time_slice <= 0 )
		{
			p->time_slice = TIMESLICE;
			printf( ANSI_COLOR_RED"Time Slice has expired!!!"ANSI_COLOR_RESET"\n" );
			schedule();
		}
	}


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
		printf(ANSI_COLOR_GREEN"%s"ANSI_COLOR_CYAN", %dms\n", temp->thread_info->processName, temp->exp_burst / 1000000);
		
		if ( temp->exp_burst < minimum_exp_burst )
		{
			minimum_exp_burst = temp->exp_burst;
			min_task = temp;
		}
	}

	return ( min_task );
}
