#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "macros.h"
#include "list.h"

struct thread_info;

/* ---------------- Do NOT Touch -------------- */
/* Sleep Types */
enum sleep_type
{
	SLEEP_NORMAL,
	SLEEP_NONINTERACTIVE,
	SLEEP_INTERACTIVE,
	SLEEP_INTERRUPTED
};

/* task_struct */
struct task_struct
{
	struct thread_info *thread_info;		/* Information about the thread */
	unsigned int time_slice;				/* Timeslice values */
	enum sleep_type sleep_type;				/* What type of sleep task is in */
	int need_reschedule;					/* Flag, set if task needs to
											   have schedule called */
	struct task_struct *prev, *next;	/* Used to link the task struct in the 
										   runqueue. Make sure to set them to
										   NULL when the process is not in the
										   runqueue */
/* ---------------- Do NOT Touch END-------------- */

/* ------ Here goes our code ---------------- */

	unsigned int burst;
	unsigned int exp_burst;
	unsigned long long cpu_owned;
	unsigned long long last_time_in_runqueue;

/* ---------End of our code ---------------- */



};

/* runqueue */
struct runqueue {
    unsigned long    nr_running;			/* number of runnable tasks */
	struct task_struct *head;
};

/*----------------------- System Calls ------------------------------*/
/* These calls are provided by the VM for your
 * convenience, and mimic system calls provided
 * normally by Linux
 */
void context_switch(struct task_struct *next); 
unsigned long long sched_clock();

/*------------------YOU MAY EDIT BELOW THIS LINE---------------------*/
/*------------------- User Defined Functions -------------------------*/
/*-------------These functions MUST be defined for the VM-------------*/
void initschedule(struct runqueue *newrq, struct task_struct *seedTask);
void killschedule();
void schedule();
void activate_task(struct task_struct *p);
void deactivate_task(struct task_struct *p);
void scheduler_tick(struct task_struct *p);
void sched_fork(struct task_struct *p);
void wake_up_new_task(struct task_struct *p);

/* ------ Here goes our code ---------------- */

struct task_struct *find_minExpBurst( struct runqueue *rq );
unsigned long long find_maxWaitingInRQ ( struct runqueue *rq );
unsigned long long calculate_goodness ( struct runqueue *rq , struct task_struct *p, unsigned long long current_time  );
void calculate_expBurst ( struct task_struct *p );



/* ---------End of our code ---------------- */


#endif
