#include <unistd.h>//for wait read write ,fork
#include <fcntl.h>//for open and close
#include <stdio.h>//standard c library
#include <stdlib.h>//for exit
#include <sys/wait.h>//for wait
#include <string.h>

int main(int argc, char* argv[]) {
    int x = 21;//ορίζω και αναθέτω μία μεταβλητή στον γονέα
    int fdr, fdw; // file descriptors
    char cc, c2c = argv[3][0];/* character to search for (third parameter in command line) */
    int count = 0;//the counter starts from the zero
    /* open file for reading */
    if((fdr = open(argv[1],O_RDONLY)) == -1){//Opens the file in read-only mode,int open(const char *pathname, int flags, ...
    //              /* mode_t mode */ );.If there is error in the opening process returns 0

        perror("Failed open");
        printf("Problem opening file to read\n");
        exit(EXIT_FAILURE);//terminates the program
    }
    /* open file for writing the result */
    if((fdw = open(argv[2],O_WRONLY | O_CREAT )) == -1){
        //Creates and write in the creating file
        perror("Failed open");
        printf("Problem opening file to write\n");
        exit(EXIT_FAILURE);
    }
    pid_t p = fork();
    if(p == -1){
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    if( p == 0){
         while(read(fdr,&cc,1)>0)
    {
        if(cc == c2c){
            count++;
        }
    }

    // Write the result to the output file
    dprintf(fdw, "The character '%c' appears %d times in file %s.\n", c2c, count, argv[1]);//int dprintf(int fildes, const char *restrict format, ...);

    //to proeretiko erwthma gia to an yparksei lathos kata to kleisimo
    /* close the file for reading */
        x = 10;//αναθέτω διαγορετική τιμη στο παιδί
        pid_t parent;
        parent = getppid();
        printf("hello world ,you are in the child's process and parent's pid is: %d\n",parent);
        printf("variable x value:%d\n",x);//fpr a reason if we used undersum something wrong happened
        if(close(fdr) == -1)
    {
        perror("Close failed");
        exit(EXIT_FAILURE);
    }
    //Close the file for writing
    if(close(fdw) == -1)
    {
        perror("Close failed");
        exit(EXIT_FAILURE);
    }
        exit(EXIT_SUCCESS); // Exit child process
    }
    if(p>0){

        pid_t my_pid;
        pid_t child = wait(NULL);//wait returns the child's pid in the parent process and the parent process stops until the child process terminates
        my_pid= getpid();
        printf("hello world, you are in the parent's process with pid %d and has a child with pid %d\n",my_pid,child);
        printf("variable x value:%d\n",x);
        if(close(fdr) == -1)
    {
        perror("Close failed");
        exit(EXIT_FAILURE);
    }
    //Close the file for writing
    if(close(fdw) == -1)
    {
        perror("Close failed");
        exit(EXIT_FAILURE);
    }
        exit(EXIT_SUCCESS); // Exit parent process
    }

    return 0;
}
