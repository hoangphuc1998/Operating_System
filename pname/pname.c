    #include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/string.h>
#include "pname.h"
asmlinkage int pnametoid(char* process_name){
    /*tasklist struct to use*/
    struct task_struct *task;
    /*tty struct*/
    struct tty_struct *my_tty;
    /*get current tty*/
    my_tty = get_current_tty();
    /*placeholder to print full string to tty*/
    char name[32];
    /*<sched.h> library method that iterates through list of processes from task_struct defined above*/
    for_each_process(task){
        /*compares the current process name (defined in task->comm) to the passed in name*/
        if(strcmp(task->comm,process_name) == 0){
            /*convert to string and put into name[]*/
            sprintf(name, "PID = %ld\n", (long)task_pid_nr(task));
            /*show result to user that called the syscall*/
                        (my_tty->driver->ops->write) (my_tty, name, strlen(name)+1);
            return (int)task_pid_nr(task);
        }
    }
    return -1;
}

asmlinkage int pidtoname(int pid, char* buff, int len){
    struct task_struct *task;
    struct tty_struct *tty;
    tty = get_current_tty();
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