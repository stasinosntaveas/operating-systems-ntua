#include <unistd.h>//for wait read write
#include <fcntl.h>//for open and close
#include <stdio.h>//standard c library
#include <stdlib.h>//for exit
//#include <sys/wait.h>//for wait

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <input_file> <output_file> <character>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int fdr, fdw; // file descriptors
    char cc, c2c = argv[3][0];/* character to search for (third parameter in command line) */
    int count = 0;//the counter starts from the zero
    /* open file for reading */
    if((fdr = open(argv[1],O_RDONLY)) == -1){//Opens the file in read-only mode,int open(const char *pathname, int flags, ...
    //              /* mode_t mode */ );.If there is error in the opening process returns -1
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
    //ssize_t read(int fd, void buf[.count], size_t count);
    //       read() attempts to read up to count bytes from file descriptor fd
    //   into the buffer starting at buf.
    // On success, the number of bytes read is returned (zero indicates
    //   end of file), and the file position is advanced by this number

      
    while(read(fdr,&cc,1)>0)
    {
        if(cc == c2c){
            count++;
        }
    }
    
    // Write the result to the output file
    dprintf(fdw, "The character '%c' appears %d times in file %s.\n", c2c, count, argv[1]);//int dprintf(int fildes, const char *restrict format, ...);
    // char buf[100]    
    // int len sprintf(buf,"The character '%c'appears '%d'times in file %s.\n",c2c,count,argv[1]);
    //if(write(fdw,buf,len)==-1)
    //{perror("error");
    //return 1;}
    //to proeretiko erwthma gia to an yparksei lathos kata to kleisimo
    /* close the file for reading */
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
    return 0;
}
              