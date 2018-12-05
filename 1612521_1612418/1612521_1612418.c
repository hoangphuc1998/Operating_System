#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/string.h>
#include "1612521_1612418.h"
asmlinkage int pnametoid(char* process_name){
    struct task_struct *task;
    for_each_process(task){
        if(strcmp(task->comm,process_name) == 0){
            return (int)task_pid_nr(task);
        }
    }
    return -1;
}

asmlinkage int pidtoname(int pid, char* buff, int len){
    struct task_struct *task;
    for_each_process(task){
        if ((int)task_pid_nr(task)==pid){
            int l = strlen(task->comm);
            if (l>len-1){
                return 0;
            }else{
                strcpy(buff,task->comm);
                buff[l] = '\0';
                return l;
            }
        }
    }
    return -1;
}
