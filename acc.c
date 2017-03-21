#include "access_functions.h"


_task_id dd_tcreate(uint32_t template_index, uint32_t deadline, uint32_t execution_time){
	_mqx_uint old_priority_ptr;
	_mqx_uint priority_ptr = 20;

	/*Create queue for dd_tcreate*/
	tcreate_qid = _msgq_open(TCREAT_QUEUE, 0);

	if(tcreate_qid == 0){
		printf("\nCould not open the tcreate queue\n");
		_task_block();
	}

	int task_info[2] = {execution_time, deadline};

	/*create tasks*/
	// auto block task at creation
	_task_id new_tid = _task_create_blocked(0, template_index, execution_time);

	MSG_PTR msg_ptr = (MSG_PTR)_msg_alloc(msg_pool);
	_time_get_elapsed(&start_t);

	// change priority of the task to 16
	_task_get_priority(new_tid, &old_priority_ptr);
	_task_set_priority(new_tid, priority_ptr, &old_priority_ptr);


	msg_ptr->HEADER.SOURCE_QID = tcreate_qid;
	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, DD_QUEUE);
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(int)*4 + sizeof(task_list);
	msg_ptr->tid = new_tid;
	msg_ptr->deadline = deadline;
	msg_ptr->creation_time = start_t.SECONDS;
	msg_ptr->priority = priority_ptr;


	_msgq_send(msg_ptr);


	msg_ptr = _msgq_receive(tcreate_qid, 0);


	// delete message pointer
	if(msg_ptr->tid == new_tid){
		_msgq_close(tcreate_qid);
		_msg_free(msg_ptr);
	}

	printf("TASK CREATED WITH DEADLINE: %d, EXECUTION TIME: %d ----------------\n",msg_ptr->deadline,execution_time);


	return new_tid;
}


uint32_t dd_delete(_task_id task_id, uint32_t creation_time){
	MSG_PTR msg_ptr = (MSG_PTR)_msg_alloc(msg_pool);
	is_running = false;
	running_task = 0;
	/*Create queue for delete_qid*/
	delete_qid = _msgq_open(DELETE_QUEUE, 0);
	if(delete_qid == 0){
		printf("\nCould not open the delete_qid queue\n");
		_task_block();
	}
	msg_ptr->HEADER.SOURCE_QID = delete_qid;
	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, DD_QUEUE);
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(int)*4+sizeof(task_list);
	msg_ptr->tid = task_id;
	msg_ptr->deadline = 0;
	msg_ptr->creation_time = creation_time;

	/*send msg to dd_scheduler with tid and creation time*/
	_msgq_send(msg_ptr);

	return task_id;
}

uint32_t dd_return_active_list(task_list **list){
	MSG_PTR msg_ptr = (MSG_PTR)_msg_alloc(msg_pool);
	dd_return_active_list_qid = _msgq_open(ACTIVE_QUEUE, 0);

	msg_ptr->HEADER.SOURCE_QID = dd_return_active_list_qid;
	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, DD_QUEUE);
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(int)*4+sizeof(task_list);

	_msgq_send(msg_ptr);

	msg_ptr = _msgq_receive(dd_return_active_list_qid, 0);

	if(msg_ptr != NULL){
//		puts("\n active>>>>>>i received!\n");
		*list = msg_ptr->t_list;
		_msgq_close(dd_return_active_list_qid);
		_msg_free(msg_ptr);
		return 1;
	}

	return 0;
}

uint32_t dd_return_overdue_list(task_list **list){
	MSG_PTR msg_ptr = (MSG_PTR)_msg_alloc(msg_pool);
	dd_return_overdue_list_qid = _msgq_open(OVERDUE_QUEUE, 0);

	msg_ptr->HEADER.SOURCE_QID = dd_return_overdue_list_qid;
	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, DD_QUEUE);
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(int)*4+sizeof(task_list);

	_msgq_send(msg_ptr);
	msg_ptr = _msgq_receive(dd_return_overdue_list_qid, 0);

	if(msg_ptr != NULL){
//		puts("\n active>>>>>>i received!\n");
		*list = msg_ptr->t_list;
		_msgq_close(dd_return_overdue_list_qid);
		_msg_free(msg_ptr);
		return 1;
	}
	return 0;
}

void dd_init(){
	return;

}

int generateRandom(int l, int u){
    int result = (rand() % (u - l)) + l;
    return result;
}


void delay(int val){
	for ( int c = 1 ; c <= val ; c++ )
		   for (int d = 1 ; d <= val ; d++ )
		   {}
}


/*linked list functions*/
void displayForward(task_list* head) {
   //start from the beginning

   task_list *ptr = head;

   //navigate till the end of the list
   delay(100);
   printf("\n[");

   while(ptr != NULL) {
	  delay(100);
      printf("(tid:%d, c:%d, d:%d) ",ptr->tid,ptr->creation_time, ptr->deadline);
      ptr = ptr->next_cell;
   }
   delay(100);
   printf("]\n\n");
}

int length(task_list **head){
	   int length = 0;
	   task_list *current;

	   for(current = *head; current != NULL; current = current->next_cell){
	      length++;
	   }

	   return length;
}


// adds to list and sorts the list
void insert(task_list** head, task_list** last, uint32_t tid, uint32_t deadline, uint32_t creation_time) {
   //create a link
   task_list *link = (task_list*) malloc(sizeof(task_list));
   link->tid = tid;
   link->deadline = deadline;
   link->creation_time = creation_time;
   link->next_cell = NULL;
   link->previous_cell = NULL;

   task_list* current = *head;

   /*if list is empty*/
   if(*head == NULL){
	   *last = link;
	   *head = link;
	   return;
   }

   int i = 0;

   // uses bubble sort to sort the list
   while(current->deadline <= deadline){
	   /*if it is last node*/
	   if(current->next_cell == NULL){
		   break;
	   }else{
		   current = current->next_cell;
	   }
   }

   delay(100);

   if(current == *last){
	   /*insert new node into the tail of the list*/
	   if((current)->deadline <= deadline){

		   link->next_cell = NULL;
		   *last = link;
		   link->previous_cell = current;
		   current->next_cell = link;
		   return;
	   }
	   else{
		   link->next_cell = current;
		   link->previous_cell = (current)->previous_cell;

		   if(current != *head)
			   (current)->previous_cell->next_cell = link;
		   else
			   *head = link;
		   (current)->previous_cell = link;
		   return;
	   }

   }else{
	   if(current == *head && (current)->deadline == deadline){
		   link->previous_cell = current;
		   link->next_cell = current->next_cell;
		   current->next_cell->previous_cell = link;
		   current->next_cell = link;
		   return;
	   }

	   link->next_cell = current;
	   if(current != *head){
		   link->previous_cell = current->previous_cell;
		   current->previous_cell->next_cell = link;
	   }
	   current->previous_cell= link;
	   if(current == *head){
		   *head = link;
	   }
	   return;
   }


}

//delete according to tid
void delete(task_list** head, task_list** last, uint32_t tid) {

	task_list* current = *head;
	task_list* previous = NULL;

	is_running = false;
	if(*head == NULL){
		return;
	}

	while((current->tid != tid)){
		if(current->next_cell == NULL){
			return;
		}else{
			previous = current;
			current = current->next_cell;
		}
	}


	if(current == *head){
		*head = (*head)->next_cell;
	}else{
		current->previous_cell->next_cell = current->next_cell;
	}

	if(current == *last){
		*last = current->previous_cell;
	}else{
		current->next_cell->previous_cell = current->previous_cell;
	}


}


void schedule_task(_task_id tid)
{

	_mqx_uint old_priority;
	_mqx_uint change_task_new_priority = 20;
	_mqx_uint shdled_priority = 15;

	// changes priority of preempted task
	if(is_running == true){
	// change priority of the running task to 20
		if(_task_get_priority(running_task, &old_priority) != MQX_OK)
			printf("\nCouldnt get running task Priority\n");
		if(_task_set_priority(running_task, change_task_new_priority, &old_priority) != MQX_OK)
			printf("\nCouldnt SET running task Priority\n");

		// save the currently running task
		running_task = tid;

		if(_task_get_priority(running_task, &old_priority) != MQX_OK)
			printf("\nCouldnt get task Priority\n");
		if(_task_set_priority(running_task, shdled_priority, &old_priority) != MQX_OK)
			printf("\nCouldnt SET new_task Priority\n");

		TD_STRUCT_PTR td_ptr = _task_get_td(tid);

		if (td_ptr != NULL){
			_task_ready(td_ptr);
		}
	}


	// check if no task was preempted
	if(is_running == false){
		running_task = tid;
		is_running = true;
		// change priority of the task to 15
		if(_task_get_priority(running_task, &old_priority) != MQX_OK)
			printf("\nCouldnt get task Priority\n");
		if(_task_set_priority(running_task, shdled_priority, &old_priority) != MQX_OK)
			printf("\nCouldnt SET new_task Priority\n");

		TD_STRUCT_PTR td_ptr = _task_get_td(tid);

		if (td_ptr != NULL){
			_task_ready(td_ptr);
		}
	}

}

void report_statistics(int test_id, int n_total_tasks){
	task_list* active_tasks_head_ptr = NULL;
	task_list* overdue_tasks_head_ptr = NULL;


    if(!dd_return_active_list(&active_tasks_head_ptr) || !dd_return_overdue_list(&overdue_tasks_head_ptr)){
        printf("error: failed to obtain the tasks list!\n\r");
        return 1;
    }

    int n_completed_tasks = 0;
    int n_failed_tasks = length(overdue_tasks_head_ptr);
    int n_running_tasks = length(active_tasks_head_ptr);



    n_completed_tasks = n_total_tasks-(n_failed_tasks+n_running_tasks);
    delay(100);
    printf("\n\n{ TASK GENERATOR TEST %d: %d failed, %d completed, %d still running. }\n\n", test_id, n_failed_tasks, n_completed_tasks, n_running_tasks);
}






