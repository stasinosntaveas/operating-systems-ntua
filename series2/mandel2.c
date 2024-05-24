#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <bits/pthreadtypes.h>
#include <semaphore.h>

#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

/***************************
 * Compile-time parameters *
 ***************************/

int y_chars = 50;
int x_chars = 90;

double xmin = -1.8, xmax = 1.0;
double ymin = -1.0, ymax = 1.0;

double xstep;
double ystep;

sem_t *semaphore; // Πίνακας σημαφόρων
int NTHREADS; // Αριθμός νημάτων

pthread_mutex_t mutex1,mutex2;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;//δημιουργούμε την condition variable που θα χρησιμοποιήσουμε

int current_thread = 0;

/*
 * Η συνάρτηση αυτή υπολογίζει μια γραμμή της εικόνας Mandelbrot ως έναν πίνακα
 *     χρωμάτων που αντιστοιχεί σε κάθε σημείο της γραμμής.
 */

 void compute_mandel_line(int line, int color_val[])
{
    double x, y;
    int n;
    int val;

    y = ymax - ystep * line;

    for (x = xmin, n = 0; n < x_chars; x += xstep, n++) {
        val = mandel_iterations_at_point(x, y, MANDEL_MAX_ITERATION);
        if (val > 255)
            val = 255;
        val = xterm_color(val);
        color_val[n] = val;
    }
}
/*
 * Η συνάρτηση αυτή εκτυπώνει μια γραμμή χρωμάτων στο τερματικό.
 */
void output_mandel_line(int fd, int color_val[])
{
    int i;

    char point = '@';
    char newline = '\n';

    for (i = 0; i < x_chars; i++) {
        set_xterm_color(fd, color_val[i]);
        if (write(fd, &point, 1) != 1) {
            perror("compute_and_output_mandel_line: write point");
            exit(1);
        }
    }

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
        pthread_mutex_lock(&mutex1);
        while (current_thread != start_line%NTHREADS){
            pthread_cond_wait(&cond, &mutex1);
        }
        pthread_mutex_lock(&mutex2);
        output_mandel_line(1, color_val);
        current_thread = (current_thread + 1) % NTHREADS;
        for(int i = 0 ; i<NTHREADS-1;i++){
        pthread_cond_signal(&cond);
        }
        pthread_mutex_unlock(&mutex1);
        pthread_mutex_unlock(&mutex2);

    }
    free(arg);
    return NULL;


}
int main(int argc, char *argv[]) {
    xstep = (xmax - xmin) / x_chars;
    ystep = (ymax - ymin) / y_chars;

    int i;
    NTHREADS = atoi(argv[1]); // Πάρε τον αριθμό των νημάτων από το command line



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
    reset_xterm_color(1);
    return 0;
}
