*
 * mandel.c
 *
 * A program to draw the Mandelbrot Set on a 256-color xterm.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <signal.h>
#include <sys/wait.h>
#include <semaphore.h>

#include "help.h"
/* TODO: Include necessary headers for mmap and munmap */

#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

/***************************
 * Compile-time parameters *
 ***************************/

/*
 * Output at the terminal is x_chars wide by y_chars long
 */
int y_chars = 50;
int x_chars = 90;

/*
 * The part of the complex plane to be drawn:
 * upper left corner is (xmin, ymax), lower right corner is (xmax, ymin)
 */
double xmin = -1.8, xmax = 1.0;
double ymin = -1.0, ymax = 1.0;

/*
 * Every character in the final output is
 * xstep x ystep units wide on the complex plane.
 */
double xstep;
double ystep;

int PPROCESSES; // Number of processes

sem_t *semaphores; // Array of semaphores for each line

// Handler to reset xterm color and exit on CTRL+C
void sigint_handler(int signum) {
    reset_xterm_color(1);
    exit(1);
}

/*
 * This function computes a line of output
 * as an array of x_char color values.
 */
void compute_mandel_line(int line, int color_val[])
{
    double x, y;
    int n;
    int val;

    /* Find out the y value corresponding to this line */
    y = ymax - ystep * line;

    /* and iterate for all points on this line */
    for (x = xmin, n = 0; n < x_chars; x += xstep, n++) {
        /* Compute the point's color value */
        val = mandel_iterations_at_point(x, y, MANDEL_MAX_ITERATION);
        if (val > 255)
            val = 255;

        /* And store it in the color_val[] array */
        val = xterm_color(val);
        color_val[n] = val;
    }
}

/*
 * This function outputs an array of x_char color values
 * to a 256-color xterm.
 */
void output_mandel_line(int fd, int color_val[])
{
    int i;
    char point = '@';
    char newline = '\n';

    for (i = 0; i < x_chars; i++) {
        /* Set the current color, then output the point */
        set_xterm_color(fd, color_val[i]);
        if (write(fd, &point, 1) != 1) {
            perror("compute_and_output_mandel_line: write point");
            exit(1);
        }
    }
    /* Now that the line is done, output a newline character */
    if (write(fd, &newline, 1) != 1) {
        perror("compute_and_output_mandel_line: write newline");
        exit(1);
    }
}

void process_function(int start_line, int (*shared_buffer)[x_chars]) {
    for (int i = start_line; i < y_chars; i += PPROCESSES) {
        int color_val[x_chars];
        compute_mandel_line(i, color_val);

        sem_wait(&semaphores[i]); // Περιμένει να  απόκτήσει το σημαφόρο ηια την γραμμή i  
        memcpy(shared_buffer[i], color_val, x_chars * sizeof(int)); //Αντιγράφει τις τιμές του πίνακα color_val στον αντίστοιχο πίνακα στην γραμμή i του shared_buffer
        sem_post(&semaphores[i]); // H διεργασία απελευθερώνει τον σημαφόρο για την γραμμή i
    }
}

/*
 * Create a shared memory area, usable by all descendants of the calling
 * process.
 */
void *create_shared_memory_area(unsigned int numbytes)
{
    int pages;
    void *addr;

    if (numbytes == 0) {
        fprintf(stderr, "%s: internal error: called for numbytes == 0\n", __func__);
        exit(1);
    }

    /*
     * Determine the number of pages needed, round up the requested number of
     * pages
     */
    pages = (numbytes - 1) / sysconf(_SC_PAGE_SIZE) + 1;

    /* Create a shared, anonymous mapping for this number of pages */
    addr = mmap(NULL, pages * sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap Failed");
        exit(EXIT_FAILURE);
    }
    return addr;
}

void destroy_shared_memory_area(void *addr, unsigned int numbytes) {
    int pages;

    if (numbytes == 0) {
        fprintf(stderr, "%s: internal error: called for numbytes == 0\n", __func__);
    exit(1);
    }

    /*
     * Determine the number of pages needed, round up the requested number of
     * pages
     */
    pages = (numbytes - 1) / sysconf(_SC_PAGE_SIZE) + 1;

    if (munmap(addr, pages * sysconf(_SC_PAGE_SIZE)) == -1) {
        perror("destroy_shared_memory_area: munmap failed");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_processes>\n", argv[0]);
        exit(1);
    }

    PPROCESSES = atoi(argv[1]);

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    xstep = (xmax - xmin) / x_chars;
    ystep = (ymax - ymin) / y_chars;

    int (*shared_buffer)[x_chars] = create_shared_memory_area(y_chars * x_chars * sizeof(int));

    // Allocate and initialize semaphores
    semaphores = create_shared_memory_area(y_chars * sizeof(sem_t));
    for (int i = 0; i < y_chars; i++) {
        if (sem_init(&semaphores[i], 1, 1) == -1) { // Initialize semaphores to 1 because we handle processes
            perror("sem_init");
            exit(1);
        }
    }

    pid_t *child_pids = malloc(PPROCESSES * sizeof(pid_t));
    if (child_pids == NULL) {
        perror("Error in allocating memory for child processes");
        exit(1);
    }

    for (int i = 0; i < PPROCESSES; i++) {
        child_pids[i] = fork();
        if (child_pids[i] < 0) {
            perror("Error in creating child process");
            exit(1);
               }
        if (child_pids[i] == 0) {
            process_function(i, shared_buffer);
            exit(0);
        }
    }

    int status;
    for (int i = 0; i < PPROCESSES; i++) {
        wait(&status);
    }

    for (int i = 0; i < y_chars; i++) {
        sem_wait(&semaphores[i]); // Wait to enter critical section
        output_mandel_line(1, shared_buffer[i]);
        sem_post(&semaphores[i]); // Signal exit from critical section
    }

    // Destroy semaphores
    for (int i = 0; i < y_chars; i++) {
        if (sem_destroy(&semaphores[i]) == -1) {
            perror("sem_destroy");
            exit(1);
        }
    }

    destroy_shared_memory_area(shared_buffer, y_chars * x_chars * sizeof(int));
    destroy_shared_memory_area(semaphores, y_chars * sizeof(sem_t));
    free(child_pids);
    reset_xterm_color(1);
    return 0;
}

