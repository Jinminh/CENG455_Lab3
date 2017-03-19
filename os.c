/* ###################################################################
**     Filename    : os_tasks.c
**     Project     : Project03
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-03-06, 16:35, # CodeGen: 1
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         ddScheduler - void ddScheduler(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file os_tasks.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         
/* MODULE os_tasks */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "global_variables.h"
#include "access_functions.h"
#include <inttypes.h>
#include <stdbool.h>
//#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Callback    : ddScheduler
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void ddScheduler(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
//	delay(100);
	puts("CREATION OF DD_SCHEDULER TASK\n");
	/*create active list and overdue list*/
	task_list* dd_active_list_head = NULL;
	task_list* dd_active_list_tail = NULL;
	task_list* dd_active_list_current = NULL;

	task_list* dd_overdue_list_head = NULL;
	task_list* dd_overdue_list_tail = NULL;
	task_list* dd_overdue_list_current = NULL;

	_mqx_uint old_priority;
	_mqx_uint new_priority = 13;
	_mqx_uint check_priority;
	// change priority of the running task to 16
	if(_task_get_priority(_task_get_id(), &old_priority) != MQX_OK)
		printf("\nCouldnt get dd_SCHEDULER task Priority\n");
	if(_task_set_priority(_task_get_id(), new_priority, &old_priority) != MQX_OK)
		printf("\nCouldnt SET dd_SCHEDULER task Priority\n");
	if(_task_get_priority(_task_get_id(), &check_priority) != MQX_OK)
		printf("\nCouldnt get dd_SCHEDULER task Priority\n");

	/*Create queue for dd_scheduler*/
	dd_qid = _msgq_open(DD_QUEUE, 0);
	if(dd_qid == 0){
		delay(200);
		printf("\nCould not open the handler isr queue\n");
		_task_block();
	}



	// create message pool
	msg_pool = _msgpool_create(sizeof(MESSAGE),NUM_USER_TASKS, 0, 0);

	delay(500);
	if (msg_pool == MSGPOOL_NULL_POOL_ID) {
		printf("\nCount not create a message pool\n");
		_task_block();
	}

	int prev_sec = -1;
	int time_out = 0;

#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */

	// converts timeout value from milliseconds to seconds
	  // _msgq_receive timeout parameter cannot be 0 or less than 0
	  if(global_timeout <= 0){
		  time_out = 10;
	  }else{
		  time_out = global_timeout*1000;
	  }
	  // receive message from task create
	  dd_msg_ptr = _msgq_receive(dd_qid, time_out);

//	  printf("GLOBAL TIMEOUT //// %d\n",global_timeout);
	  if(dd_msg_ptr != NULL){
		  // check if message indicates create new task
		  if(dd_msg_ptr->HEADER.SOURCE_QID == tcreate_qid){
			  //put the link list, and sort it
			  insert(&dd_active_list_head, &dd_active_list_tail, dd_msg_ptr->tid, dd_msg_ptr->deadline, dd_msg_ptr->creation_time);

			  // send msg back to ddt_create
			  dd_msg_ptr->HEADER.SOURCE_QID = dd_qid;
			  dd_msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, TCREAT_QUEUE);
			  _msgq_send(dd_msg_ptr);

			  // display the active tasks
			  printf("\nACTIVE LIST --------- ");
			  displayForward(dd_active_list_head);
		  }

		  //check if message indicates task delete
		  else if(dd_msg_ptr->HEADER.SOURCE_QID == delete_qid){
			  _task_id tid = dd_msg_ptr->tid;
			  // delete task from list
			  delete(&dd_active_list_head, &dd_active_list_tail, tid);
			  printf("\nACTIVE LIST --------- ");
			  displayForward(dd_active_list_head);
			  _msg_free(dd_msg_ptr);
		  }
		  // if task was preempted, schedule new task
		  if(running_task != dd_active_list_head->tid){
				schedule_task(dd_active_list_head->tid);
		  }

		  // update timeout
//		  delay(200);
//		  printf("\nbefore changes, dd_active_list_head->deadline: %d, global_timeout: %d\n\n", dd_active_list_head->deadline,global_timeout);

		  // update remaining execution time of running task
		  _time_get_elapsed(&start_t);
		  global_timeout = dd_active_list_head->deadline;
		  global_timeout -= start_t.SECONDS;

//		  delay(200);
//		  printf("\nAfter changes, dd_active_list_head->deadline: %d, global_timeout: %d\n\n", dd_active_list_head->deadline,global_timeout);

	  }else{
		  // this indicates a task is overdue
		  // check if active list is not empty.
		  if(dd_active_list_head != NULL){
			  _task_abort(dd_active_list_head->tid);
			  // add task to overdue list
			  insert(&dd_overdue_list_head, &dd_overdue_list_tail, dd_active_list_head->tid, dd_active_list_head->deadline, dd_active_list_head->creation_time);

			  printf("\nOVERDUE LIST --------");
			  displayForward(dd_overdue_list_head);
			  delete(&dd_active_list_head, &dd_active_list_tail, dd_active_list_head->tid);
			  printf("\nACTIVE LIST --------- ");
			  displayForward(dd_active_list_head);

			  // schedule the next task in the list
			  schedule_task(dd_active_list_head->tid);
			  // update the execution time of the new task
			  global_timeout = dd_active_list_head->deadline;
			  _time_get_elapsed(&start_t);
			  global_timeout -= start_t.SECONDS;
		  }
	  }


   
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}


void Task6_task(os_task_param_t task_init_data)
{
	/*Do something*/
	num_aperiod++;
	int task_num = num_aperiod;
	_time_get_elapsed(&start_t);
	uint32_t start_time = start_t.SECONDS;
	int prev_sec = -1;
	int remaining_execution_time = task_init_data;

#ifdef PEX_USE_RTOS
  while (remaining_execution_time > -1) {
#endif
    /* Write your code here ... */

	_time_get_elapsed(&start_t);

	if(start_t.SECONDS != prev_sec){
		remaining_execution_time--;
		prev_sec = start_t.SECONDS;
		printf("Aperiodic Task%d seconds>>>%d\n",task_num, start_t.SECONDS);
		total_running_time++;
	}


#ifdef PEX_USE_RTOS
  }
#endif
  dd_delete(_task_get_id(), start_time);
  printf("\nAPERIODIC TASK%d HAS ENDED\n", task_num);
}




void Creator_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */

	puts("CREATION OF TASK GENERATOR\n");

	int prev_sec = -1;

	_time_get_elapsed(&start_t);

	int execute_t = 0;
	int arrive_t = 0;
	int time_interval;
	srand(_time_get_nanoseconds());

	_mqx_uint old_priority;
	_mqx_uint new_priority = 10;

	// change priority of the running task to 16
	if(_task_get_priority(_task_get_id(), &old_priority) != MQX_OK)
		printf("\nCouldnt get task_CREATOR task Priority\n");
	if(_task_set_priority(_task_get_id(), new_priority, &old_priority) != MQX_OK)
		printf("\nCouldnt SET task_CREATOR task Priority\n");


#ifdef PEX_USE_RTOS
  while (1) {
#endif

	  /*Create periodic tasks*/
	  if(ptask_num < NUM_PERIODIC_TASKS){
		 _time_get_elapsed(&start_t);
		  /*Avoid create several tasks at a same time*/
		  if(start_t.SECONDS % 10 == 0 && start_t.SECONDS != prev_sec){
			  prev_sec = start_t.SECONDS;
			  if(ptask_num == 0)
			  	  dd_tcreate(TASK1_TASK,10,5);
			  else if(ptask_num == 1)
				  dd_tcreate(TASK2_TASK,15,10);
			  else if(ptask_num == 2)
				  dd_tcreate(TASK3_TASK,35,5);
			  else if(ptask_num == 3)
				  dd_tcreate(TASK4_TASK,48,15);
			  else if(ptask_num == 4)
				  dd_tcreate(TASK5_TASK,55,20);

			  ptask_num++;
		  }
	  }
	  /*Create aperiodic tasks*/
	  if(aptask_num < NUM_APERIODIC_TASKS){
			  /*get current time*/
			 _time_get_elapsed(&start_t);
			  time_interval = generateRandom(8, 15);
			  if(start_t.SECONDS % time_interval == 0 && start_t.SECONDS != prev_sec){
			  /*Avoid create several tasks at a same time*/
				  /*set the arrival time for next task */
				  printf("CREATING APERIODIC TASK%d \n",aptask_num+1);
				  prev_sec = start_t.SECONDS;
				  int deadline = prev_sec+generateRandom(15, 30);
				  int execution_time = generateRandom(8, 21);
				  dd_tcreate(TASK6_TASK, deadline, execution_time);
				  aptask_num++;
			  }
	  }

	if(ptask_num >= NUM_PERIODIC_TASKS && aptask_num >= NUM_APERIODIC_TASKS){
	  puts("Generator finished ------------------- \n");
	  _task_block();
	}

                 /* Example code (for task release) */
	OSA_TimeDelay(500);
    
#ifdef PEX_USE_RTOS   
  }
#endif

}


void Idle_task1(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
//	delay(100);
	puts("CREATION OF IDLE TASK\n");

	is_running = false;
	/*Do something*/

	_time_get_elapsed(&start_t);
	uint32_t start_time = start_t.SECONDS;
	int prev_sec = -1;

	_mqx_uint old_priority;
	_mqx_uint new_priority = 17;

	// change priority of the running task to 16
	if(_task_get_priority(_task_get_id(), &old_priority) != MQX_OK)
		printf("\nCouldnt get idle task Priority\n");
	if(_task_set_priority(_task_get_id(), new_priority, &old_priority) != MQX_OK)
		printf("\nCouldnt SET idle task Priority\n");


#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
	if(ptask_num >= NUM_PERIODIC_TASKS && aptask_num >= NUM_APERIODIC_TASKS)
		_task_block();

	_time_get_elapsed(&start_t);

	if(start_t.SECONDS != prev_sec){
		prev_sec = start_t.SECONDS;
		printf("IDLE --seconds>>>%d\n", start_t.SECONDS);
		total_idle_time++;
	}
    
#ifdef PEX_USE_RTOS   
  }
#endif
}


void Task5_task(os_task_param_t task_init_data)
{

	/*Do something*/
	_time_get_elapsed(&start_t);
	uint32_t start_time = start_t.SECONDS;
	int prev_sec = -1;
	int remaining_execution_time = task_init_data;

#ifdef PEX_USE_RTOS
  while (remaining_execution_time > -1) {
#endif
    /* Write your code here ... */

	_time_get_elapsed(&start_t);

	if(start_t.SECONDS != prev_sec){
		prev_sec = start_t.SECONDS;
		remaining_execution_time--;
		printf("Task5 seconds>>>%d\n", start_t.SECONDS);
		total_running_time++;
	}



#ifdef PEX_USE_RTOS
  }
#endif
  delay(100);
  puts("\nTASK5 ENDED");
  dd_delete(_task_get_id(), start_time);
}


void Task4_task(os_task_param_t task_init_data)
{
	/*receive msg for interrupt*/
	/*Do something*/
	_time_get_elapsed(&start_t);
	uint32_t start_time = start_t.SECONDS;
	int prev_sec = -1;
	int remaining_execution_time = task_init_data;

#ifdef PEX_USE_RTOS
  while (remaining_execution_time > -1) {
#endif
    /* Write your code here ... */

	_time_get_elapsed(&start_t);

	if(start_t.SECONDS != prev_sec){
		prev_sec = start_t.SECONDS;
		remaining_execution_time--;
		printf("Task4 seconds>>>%d\n", start_t.SECONDS);
		total_running_time++;
	}


#ifdef PEX_USE_RTOS
  }
#endif
  delay(100);
  puts("\nTASK4 ENDED");
  dd_delete(_task_get_id(), start_time);
}


void Task3_task(os_task_param_t task_init_data)
{
	/*receive msg for interrupt*/
	/*Do something*/
	_time_get_elapsed(&start_t);
	uint32_t start_time = start_t.SECONDS;
	int prev_sec = -1;
	int remaining_execution_time = task_init_data;

#ifdef PEX_USE_RTOS
  while (remaining_execution_time > -1) {
#endif
    /* Write your code here ... */

	_time_get_elapsed(&start_t);

	if(start_t.SECONDS != prev_sec){
		prev_sec = start_t.SECONDS;
		remaining_execution_time--;
		printf("Task3 seconds>>>%d\n", start_t.SECONDS);
		total_running_time++;
	}


#ifdef PEX_USE_RTOS
  }
#endif
  delay(100);
  puts("\nTASK3 ENDED");
  dd_delete(_task_get_id(), start_time);
}


void Task2_task(os_task_param_t task_init_data)
{
	/*Do something*/
	_time_get_elapsed(&start_t);
	uint32_t start_time = start_t.SECONDS;
	int prev_sec = -1;
	int remaining_execution_time = task_init_data;

#ifdef PEX_USE_RTOS
  while (remaining_execution_time > -1) {
#endif
    /* Write your code here ... */
	  _time_get_elapsed(&start_t);

	  	if(start_t.SECONDS != prev_sec){
	  		prev_sec = start_t.SECONDS;
	  		remaining_execution_time--;
//	  		delay(100);
	  		printf("TASK2 seconds>>>%d\n", start_t.SECONDS);
	  		total_running_time++;
	  	}



#ifdef PEX_USE_RTOS
  }
#endif
  delay(100);
  puts("\nTASK2 ENDED");
  dd_delete(_task_get_id(), start_time);
}


void Task1_task(os_task_param_t task_init_data)
{
	/*Do something*/
		running_task = _task_get_id();
		_time_get_elapsed(&start_t);
		uint32_t start_time = start_t.SECONDS;
		int prev_sec = -1;
		int remaining_execution_time = task_init_data;

	#ifdef PEX_USE_RTOS
	  while (remaining_execution_time > -1) {
	#endif
	    /* Write your code here ... */
		_time_get_elapsed(&start_t);

		if(start_t.SECONDS != prev_sec){
			prev_sec = start_t.SECONDS;
			remaining_execution_time--;
			printf("Task1 seconds>>>%d\n", start_t.SECONDS);
			total_running_time++;
		}


	#ifdef PEX_USE_RTOS
	  }
	#endif
	  delay(100);
	  puts("\nTASK1 ENDED");
	  dd_delete(_task_get_id(), start_time);

}


void Task_Moniter(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
	_mqx_uint old_priority;
	_mqx_uint new_priority = 19;

	// change priority of the running task to 16
	if(_task_get_priority(_task_get_id(), &old_priority) != MQX_OK)
		printf("\nCouldnt get task_CREATOR task Priority\n");
	if(_task_set_priority(_task_get_id(), new_priority, &old_priority) != MQX_OK)
		printf("\nCouldnt SET task_CREATOR task Priority\n");

	delay(100);
	printf("total utilization time: %d\n",total_running_time);
	delay(100);
	printf("total idle time: %d\n",total_idle_time);
	delay(100);
	printf("system time: %d\n",total_running_time+total_idle_time);

#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */


    OSA_TimeDelay(10);                 /* Example code (for task release) */




#ifdef PEX_USE_RTOS
  }
#endif
}



