
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
int main(){
    char name[32];
int pid = 0;
    puts("Enter process to find");
    scanf("%d",&pid);
    
    int status = syscall(331, pid, name, 32); //syscall number 320 and passing in the string.
	printf("Name of process: %s\n",name);
    printf("System call returned %d\n", status);
    return 0;
}

