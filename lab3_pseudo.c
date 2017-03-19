dd_scheduler(){
  dd_qid = open(DD_QUEUE,0);
  _msgpool_create();
  task_id task_running = 0;
  while(true){
    msg_ptr = msq_receive（dd_qid,0）;
    if(msg_ptr != null){
      if(create_task_msg){
        new task_list* tl;
        if(task_running == 0){
          active_list_head = tl;
          task_running = tl_tid;
          run tl;
        }else{
          compare tl deadline with running task;
          if(tl wins){
            pause running task;
            record the remaining time of running task;
            active_list_head = tl;
            run tl;
            insert the running task to active_list (with sorting);
          }else{
            insert the tl to active_list (with sorting);
          }
        }
      }else if(delete_task_msg){
        if(delete_item == active_list_head){
          pause delete_item;
          active_list_head = active_list_head->next;
          remove delete_item;
        }else{
          remove delete_item from active_list;
        }
      }else if(dd_return_active_list){
        send_back(active_list_head);
      }else if(dd_return_active_list){
        send_back(overdue_list_head);
      }else{
        printf(msg);
      }
    }
    if(task_running != 0){
      if(running_task time > deadline){
          pause running_task;
          active_list_head = active_list_head->next;
          push running_task to overdue_list;
          remove running_task;
      }
      if(running_task time == deadline){
          active_list_head = active_list_head->next;
          run active_list_head;
          remove running_task;       
      }
    }else{
      run active_list_head;
    }
  }
}

Creator_task(){
  clock_t start = clock();
  int i = 0;
  int j = 0;
  while(true){
    int rand = rand_generator(1~20);
    //create 10 periodic tasks
    if(i<10){
      //generate every 5 secs
      if((clock() - start) % 5 == 0){
        dd_create();
        i++;
      }
    }
    //create aperiodic
    if(j<5){
      if((clock()-start)%5 != 0 && (clock()-start)%rand == 0){
        dd_create();
        j++;
      }
    }
  }
}

