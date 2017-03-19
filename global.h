/*
 * global_variables.h
 *
 *  Created on: Feb 6, 2017
 *      Author: nguyen06
 */
#include <mqx.h>
#include <message.h>
#include <mutex.h>

#ifndef SOURCES_GLOBAL_VARIABLES_H_
#define SOURCES_GLOBAL_VARIABLES_H_

#define NUM_USER_TASKS 10

#define NUM_PERIODIC_TASKS 5
#define NUM_APERIODIC_TASKS 8


//Task IDs
#define DD_TASK 5
#define CREATOR_TASK 6

//Queue IDs
#define DD_QUEUE 7
#define CREATOR_QUEUE 8
#define TCREAT_QUEUE 9
#define DELETE_QUEUE 10
//#define INTERRUPT_QUEUE 11
//#define GET_DEADLINE_QUEUE 12
//#define WAIT_BLOCKED 0xF1

_queue_id dd_qid;
_queue_id tcreate_qid;
_queue_id delete_qid;
_queue_id get_deadline_qid;

//message structure
typedef struct{
	MESSAGE_HEADER_STRUCT HEADER;
	_task_id tid;
	int deadline;
	int creation_time;
	_mqx_uint priority;
}MESSAGE, *MSG_PTR;


//task structure
typedef struct t_list{
	uint32_t tid;
	uint32_t deadline;
	uint32_t creation_time;
	uint32_t execution_time;
	struct t_list *next_cell;
	struct t_list *previous_cell;
}task_list;

MSG_PTR dd_msg_ptr;
TIME_STRUCT start_t;


/*number of periodic tasks*/
int ptask_num = 0;

/*number of aperiodic tasks*/
int aptask_num = 0;

//check if there is a task running
bool is_running = false;

int tt = 0;
bool interrupt_occurs = false;

/*return the task_id who is currently running*/
_task_id running_task = 0;
int num_aperiod = 0;

int global_timeout = 0;

int sys_time = 0;
int total_idle_time = 0;
int total_running_time = 0;

_pool_id msg_pool;

#endif /* SOURCES_GLOBAL_VARIABLES_H_ */
