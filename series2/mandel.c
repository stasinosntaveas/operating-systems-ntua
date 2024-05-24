*
 * mandel.c
 *
 * A program to draw the Mandelbrot Set on a 256-color xterm.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <bits/pthreadtypes.h>
#include <semaphore.h>
#include <signal.h>
#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

/***************************
 * Compile-time parameters *
 ***************************/

/*
 * Output at the terminal is is x_chars wide by y_chars long
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

sem_t *semaphore; // Πίνακας σημαφόρων
int NTHREADS; // Αριθμός νημάτων

//ΣΕ ΠΕΡΙΠΤΩΣΗ ΠΟΥ ΠΑTΗΘΕΙ CTRL^C ΠΑΡΑΚΑΤΩ ΕΙΝΑΙ Ο
//O HANDLER 
void sigint_handler(int signum) {
    reset_xterm_color(1);
    exit(1);
}


void usage(char *argv0)
{
        fprintf(stderr, "Usage: %s thread_count array_size\n\n"
                "Exactly one argument required:\n"
                "    thread_count: The number of threads to create.\n",
                argv0);
        exit(1);
}

/*
 * This function computes a line of output
 * as an array of x_char color values.
 */
 void compute_mandel_line(int line, int color_val[])
{

        /*
         * x and y traverse the complex plane.
         */
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

// Κλείδωμα νημάτων
pthread_mutex_t pth;

// Η συνάρτηση που εκτελείται από κάθε νήμα
void *thread_function(void *arg) {
    int start_line = *(int *)arg; // Πάρε τον δείκτη από το όρισμα
    int color_val[x_chars];

    for (int i = start_line; i < y_chars; i += NTHREADS) {
        compute_mandel_line(i, color_val);

        if (sem_wait(&semaphore[start_line]) < 0) { // Περίμενε για το σήμα
            perror("Error in sem_wait");
            exit(1);
        }

        output_mandel_line(1, color_val);

        if (sem_post(&semaphore[(start_line + 1) % NTHREADS]) < 0) { // Δώσε σήμα στο επόμενο νήμα
            perror("Error in sem_post");
            exit(1);
        }
    }

    free(arg);
    return NULL;
}

int main(int argc, char *argv[]) {
    xstep = (xmax - xmin) / x_chars;
    ystep = (ymax - ymin) / y_chars;

        /*
         * draw the Mandelbrot Set, one line at a time.
         * Output is sent to file descriptor '1', i.e., standard output.
         */

    int i;
    if (argc != 2)
            usage(argv[0]);

    NTHREADS = atoi(argv[1]); // Πάρε τον αριθμό των νημάτων από το command line

   //ΣΕ ΠΕΡΙΠΤΩΣH ΠΟΥ ΚΛΗΘΕΙ ΔΙΑΚΟΠΗ ΤΟΥ ΠΡΟΓΡΑΜΜΑΤΟΣ ΑΠΟ ΤΟ ΠΛΗΚΤΡΟΛΟΓΙΟ ΜΕΣΩ ΤΗΣ ΕΝΤΟΛΗ CTRL^C
   //ΜΕΣΩ ΤΗΣ SIGNAL ΣΤΕΛΝΟΥΜΕ ΤΟ PC ΣΤΟΝ HANDLER
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }



    // Δέσμευση μνήμης για τον πίνακα των σημαφόρων
    semaphore = (sem_t *)malloc(NTHREADS * sizeof(sem_t));
    if (sem_init(&semaphore[0], 0, 1) < 0) { // Αρχικοποίηση του πρώτου σημαφόρου
        perror("Error in initializing first semaphore");
       return 1;
    }

    // Αρχικοποίηση των υπολοίπων σημαφόρων
    for (int i = 1; i < NTHREADS; i++) {
        if (sem_init(&semaphore[i], 0, 0) < 0) {
            perror("Error in initialization of semaphores");
            return 1;
        }
    }

    pthread_t array_threads[NTHREADS];
    for (i = 0; i < NTHREADS; i++) {
        int *a = malloc(sizeof(int));
        *a = i;
        if (pthread_create(&array_threads[i], NULL, &thread_function, a) != 0) { // Δημιουργία νέου νήματος
            perror("Failed thread creation");
        }
    }

    for (i = 0; i < NTHREADS; i++) {
        if (pthread_join(array_threads[i], NULL) != 0) { // Αναμονή τερματισμού των νημάτων το πρώτο όρισμα παίρνει τι id του νήματος και το δέυτερο την τιμή ΝULL
            perror("Failed thread termination");
        }
    }

    for (int i = 0; i < NTHREADS; i++) {
        if (sem_destroy(&semaphore[i]) < 0) { // Καταστροφή των σημαφόρων
            perror("Semaphores Failed to be destroyed");//αν η sem_destroy επιστρέψει αρνητική τιμή
            return 1;
        }
    }

    free(semaphore);//αποδέσμευση της μνήμης
    reset_xterm_color(1);
    return 0;
}
