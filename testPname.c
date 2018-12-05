
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
int main(){
    char name[32];
    puts("Enter process to find");
    scanf("%s",name);
    strtok(name, "\n");
    long int status = syscall(330, name); //syscall number 320 and passing in the string.
    printf("System call returned %ld\n", status);
    return 0;
}

