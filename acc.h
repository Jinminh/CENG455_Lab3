/*
 * access_functions.h
 *
 *  Created on: Mar 6, 2017
 *      Author: nguyen06
 */
#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "global_variables.h"


#ifndef SOURCES_ACCESS_FUNCTIONS_H_
#define SOURCES_ACCESS_FUNCTIONS_H_

_task_id dd_tcreate(uint32_t template_index, uint32_t deadline, uint32_t execution_time);
uint32_t dd_delete(uint32_t task_id, uint32_t creation_time);
uint32_t dd_return_active_list(task_list **list);
uint32_t dd_return_overdue_list(task_list **list);
void dd_init();
void delay(int val);
int generateRandom(int l, int u);
void report_statistics(int test_id, int n_total_tasks);

/*linked list functions*/
bool isEmpty(task_list *head);
int length(task_list** head);
void displayForward(task_list *head);
void insert(task_list** head, task_list** last, uint32_t tid, uint32_t deadline, uint32_t creation_time);
void delete(task_list** head, task_list** last, uint32_t tid);
void schedule_task(_task_id tid);

#endif /* SOURCES_ACCESS_FUNCTIONS_H_ */
