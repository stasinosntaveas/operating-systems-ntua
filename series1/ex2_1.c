#include <unistd.h>//for wait read write ,fork
#include <fcntl.h>//for open and close
#include <unistd.h>//for wait read write ,fork
#include <fcntl.h>//for open and close
#include <stdio.h>//standard c library
#include <stdlib.h>//for exit
#include <sys/wait.h>//for wait

int main(int argc, char* argv[]) {
    pid_t p = fork();
    if(p == -1){
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    if( p == 0){
        pid_t parent;
        parent = getppid();
        printf("hello world ,you are in the child's process and parent's pid is: %d\n",parent);
        exit(EXIT_SUCCESS); // Exit child process
    }
    if(p>0){
        pid_t my_pid;
        pid_t child = wait(NULL);//wait returns the child's pid in the parent process and the parent process stops until the child process terminates
        my_pid= getpid();
        printf("hello world, you are in the parent's process with pid %d and has a child with pid %d\n",my_pid,child);
        exit(EXIT_SUCCESS); // Exit parent process
    }

    return 0;
}
                                                                                                                                                                                                     