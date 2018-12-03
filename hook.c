#include <asm/unistd.h>
#include <asm/cacheflush.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <asm/pgtable_types.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <asm/cacheflush.h>
#include <linux/thread_info.h>

#define current (current_thread_info()->task)
MODULE_LICENSE("GPL");
MODULE_AUTHOR("PN");

void **system_call_table_addr;
/*linux original open syscall*/
asmlinkage int (*original_sys_open) (const char* filename, int flags, int mode);
/*linux original write syscall*/
asmlinkage int (*original_sys_write) (unsigned int fd, const char* buf, int count);
/*our pidtoname syscall*/
asmlinkage int (*sys_pidtoname)(int pid, char* buff, int len);
/*hook open syscall*/
asmlinkage int our_sys_open(const char* filename, int flags, int mode) {
    int pid = current->pid;
    char buff[100];
    int result = sys_pidtoname(pid, buff, 100);
    if (result!=-1 && result!=0){
        printk(KERN_INFO "(Our Open Syscall) Process name: %s", buff);
    }else{
        printk(KERN_INFO "(Our Open Syscall) Error");
    }
    printk(KERN_INFO "(Our Open Syscall) File name: %s", filename);
    return original_sys_open(filename, flags, mode);
}

asmlinkage int our_sys_write(unsigned int fd, const char* buf, int count) {
    int pid = current->pid;
    char buff[100];
    char filename[50];
    char fd_string[30];
    sprintf(fd_string,"/proc/self/fd/%d",fd);
    readlink(fd_string,filename,50);
    int result = sys_pidtoname(pid, buff, 100);
    if (result!=-1 && result!=0){
        printk(KERN_INFO "(Our Write Syscall) Process name: %s", buff);
    }else{
        printk(KERN_INFO "(Our Write Syscall) Error");
    }
    int bytes_write = original_sys_write(fd, buf, count);

    printk(KERN_INFO "(Our Write Syscall) File name: %s", filename);
    printk(KERN_INFO "(Our Write Syscall) Number of Written Bytes: %d", bytes_write);
    return bytes_write;
}
/*Make page writeable*/
int make_rw(unsigned long address){
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);
    if(pte->pte &~_PAGE_RW){
        pte->pte |=_PAGE_RW;
    }
    return 0;
}

/* Make the page write protected */
int make_ro(unsigned long address){
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);
    pte->pte = pte->pte &~_PAGE_RW;
    return 0;
}

static int __init entry_point(void){
    printk(KERN_INFO "Module Hook has loaded successfully...\n");
    /*MY sys_call_table address*/
    system_call_table_addr = (void*)0xffffffff81801480;
    /* Replace custom syscall with the correct system call name (write,open,etc) to hook*/
    original_sys_open = system_call_table_addr[__NR_open];
    original_sys_write = system_call_table_addr[__NR_write];
    sys_pidtoname = system_call_table_addr[__NR_pidtoname];
    /*Disable page protection*/
    make_rw((unsigned long)system_call_table_addr);
    /*Change syscall to our syscall function*/
    system_call_table_addr[__NR_open] = our_sys_open;
    system_call_table_addr[__NR_write] = our_sys_write;
    return 0;
}
static int __exit exit_point(void){
        printk(KERN_INFO "Module Hook has unloaded successfully...\n");
    /*Restore original system call */
    system_call_table_addr[__NR_open] = original_sys_open;
    system_call_table_addr[__NR_write] = original_sys_write;
    /*Renable page protection*/
    make_ro((unsigned long)system_call_table_addr);
    return 0;
}

module_init(entry_point);
module_exit(exit_point);
