#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#define P 15// Define the number of child processes



// Signal handler function for SIGINT (Ctrl+C)
void sigint_handler(int signum) {
    printf("\nTotal number of processes searching the file: %d\n", P);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char c2c = argv[3][0]; // The character to search for

    // Register the signal handler for SIGINT
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    // Creating the pipes
    int pipefd[P][2];
    for (int i = 0; i < P; ++i) {
        if (pipe(pipefd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Creating the child processes
    for (int i = 0; i < P; ++i) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Code for child process
            close(pipefd[i][0]); // Close the read end of the pipe
            // Get the file descriptor of the input file
            int file_fd = open(argv[1], O_RDONLY);
            if (file_fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            // Use fstat to get the size of the file
            struct stat st;
            if (fstat(file_fd, &st) == -1) {
                perror("fstat");
                exit(EXIT_FAILURE);
            }
            off_t file_size = st.st_size;
            off_t file_size_section = file_size / P;

            // Calculate the start and end positions for the child process
            off_t start = i * file_size_section;
            off_t end;
            if (i == P - 1) {
                end = file_size;
            } else {
            end = (i + 1) * file_size_section;
            }

            char ch;
            int count = 0;

            // Read from the input file
            lseek(file_fd, start, SEEK_SET);// SEEK_SET μετακίνηση από την αρχή του αρχείου.Μετακινούμε τον δείκτη κατά start bytes από την αρχή
            while (read(file_fd, &ch, 1) > 0 && lseek(file_fd, 0, SEEK_CUR) < end) {// SEEK_CUR μετακίνηση από την τρέχουσα θέση του δείκτη κατά μηδέν bytes
                if (ch == c2c) {
                    ++count;
                }
            }

            // Close the input file
            close(file_fd);

            // Send the count of occurrences of the character to the parent process
            if (write(pipefd[i][1], &count, sizeof(count)) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }

            // Close the write end of the pipe
            close(pipefd[i][1]);

            exit(EXIT_SUCCESS);
        }
    }

    // Read results from the child processes
    int total_count = 0;
    int count;
    ssize_t bytes_read;

    for (int i = 0; i < P; ++i) {
        sleep(1);
        close(pipefd[i][1]); // Close the write end of the pipe
        read(pipefd[i][0], &count, sizeof(count));
        total_count += count;
    }

    // Open the output file
    int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Write the total count to the output file
    dprintf(output_fd, "Total occurrences of '%c' in the file: %d \n", c2c, total_count);

    // Closthe output file
    close(output_fd);

    // Wait for the child processes to complete
    for (int i = 0; i < P; ++i) {
        wait(NULL);
    }

    return 0;
}
