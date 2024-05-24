/*
 * mmap.c
 *
 * Examining the virtual memory of processes.
 *
 * Operating Systems course, CSLab, ECE, NTUA
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

#include "help.h"

#define RED     "\033[31m"
#define RESET   "\033[0m"


char *heap_private_buf;
char *heap_shared_buf;

char *file_shared_buf;

uint64_t buffer_size;


/*
 * Child process' entry point.
 */
void child(void)
{
        uint64_t pa;

        /*
         * Step 7 - Child
         */
        if (0 != raise(SIGSTOP))
                die("raise(SIGSTOP)");
        /*
         * TODO: Write your code here to complete child's part of Step 7.
         */
        printf("child's memory map is the following:\n");
        show_maps();
      /*
         * Step 8 - Child
         */
        if (0 != raise(SIGSTOP))
                die("raise(SIGSTOP)");
        /*
         * TODO: Write your code here to complete child's part of Step 8.
         */
        uint64_t phys_addr_frst_page_child = get_physical_address((uint64_t)heap_private_buf);
        printf("The physical address of the buffer in main memory AT CHILD'S PROCCESS is : %ld\n",phys_addr_frst_page_child);
        printf("The VA info is the following : \n");
        show_va_info((uint64_t) heap_private_buf);

        /*
         * Step 9 - Child
         */
        if (0 != raise(SIGSTOP))
                die("raise(SIGSTOP)");
        /*
         * TODO: Write your code here to complete child's part of Step 9.
         */
        for(int i =0;i<(int)buffer_size;i++){
                        heap_private_buf[i] = 0;
                        }

        uint64_t phys_addr_frst_page_child_pvt = get_physical_address((uint64_t)heap_private_buf);
        printf("The physical address of the private_buffer in main memory AT CHILD'S PROCCESS is : %ld\n",phys_addr_frst_page_child_pvt);
        printf("The VA info is the following : \n");
        show_va_info((uint64_t) heap_private_buf);



        /*
         * Step 10 - Child
         */
        if (0 != raise(SIGSTOP))
                die("raise(SIGSTOP)");
        /*
         * TODO: Write your code here to complete child's part of Step 10.
         */
                //γεμίζω τον buffer με μηδενικά προκειμένου και η διεργασία παιδί και η διεργασία πατέρας
// να έχουν τον δικό τους χώρο φυσικής μνήμης καθως μέχρι αντί να αντιγραφούν αμέσως όλες οι σελίδες 
//μνήμης της διεργασίας γονέα για την διεργασία παιδί,
// οι σελίδες σημειώνονται ως κοινόχρηστες και προστατεύονται με τρόπο που καθιστά αδύνατη την εγγραφή
        for(int i=0; i<(int)buffer_size; i++) {
                heap_shared_buf[i]=0;
                }

        uint64_t phys_addr_frst_page_child_shared = get_physical_address((uint64_t)heap_shared_buf);
        printf("The physical address of the buffer in main memory AT CHILD'S PROCCESS is : %ld\n",phys_addr_frst_page_child_shared);
        printf("The VA info is the following : \n");
        show_va_info((uint64_t) heap_shared_buf);

     /*
       * Step 8 - Child
         */
        if (0 != raise(SIGSTOP))
                die("raise(SIGSTOP)");
        /*
         * TODO: Write your code here to complete child's part of Step 8.
         */
        uint64_t phys_addr_frst_page_child = get_physical_address((uint64_t)heap_private_buf);
        printf("The physical address of the buffer in main memory AT CHILD'S PROCCESS is : %ld\n",phys_addr_frst_page_child);
        printf("The VA info is the following : \n");
        show_va_info((uint64_t) heap_private_buf);

        /*
         * Step 9 - Child
         */
        if (0 != raise(SIGSTOP))
                die("raise(SIGSTOP)");
        /*
         * TODO: Write your code here to complete child's part of Step 9.
         */
        for(int i =0;i<(int)buffer_size;i++){
                        heap_private_buf[i] = 0;
                        }

        uint64_t phys_addr_frst_page_child_pvt = get_physical_address((uint64_t)heap_private_buf);
        printf("The physical address of the private_buffer in main memory AT CHILD'S PROCCESS is : %ld\n",phys_addr_frst_page_child_pvt);
        printf("The VA info is the following : \n");
        show_va_info((uint64_t) heap_private_buf);



        /*
         * Step 10 - Child
         */
        if (0 != raise(SIGSTOP))
                die("raise(SIGSTOP)");
        /*
         * TODO: Write your code here to complete child's part of Step 10.
         */
                //γεμίζω τον buffer με μηδενικά προκειμένου και η διεργασία παιδί και η διεργασία πατέρας
// να έχουν τον δικό τους χώρο φυσικής μνήμης καθως μέχρι αντί να αντιγραφούν αμέσως όλες οι σελίδες 
//μνήμης της διεργασίας γονέα για την διεργασία παιδί,
// οι σελίδες σημειώνονται ως κοινόχρηστες και προστατεύονται με τρόπο που καθιστά αδύνατη την εγγραφή
        for(int i=0; i<(int)buffer_size; i++) {
                heap_shared_buf[i]=0;
                }

        uint64_t phys_addr_frst_page_child_shared = get_physical_address((uint64_t)heap_shared_buf);
        printf("The physical address of the buffer in main memory AT CHILD'S PROCCESS is : %ld\n",phys_addr_frst_page_child_shared);
        printf("The VA info is the following : \n");
        show_va_info((uint64_t) heap_shared_buf);

        /*
        /*
      * Step 11 - Child
         */
        if (0 != raise(SIGSTOP))
                die("raise(SIGSTOP)");
        /*
         * TODO: Write your code here to complete child's part of Step 11.
         */
        mprotect(heap_shared_buf, buffer_size, PROT_READ);
        /*
        Η συνάρτηση mprotect() χρησιμοποιείται για την αλλαγή των δικαιωμάτων πρόσβασης σε μια περιοχή μνήμης. 
        Ορίζει τις προστασίες που θα ισχύουν για μια συγκεκριμένη περιοχή μνήμης στην οποία έχει ήδη γίνει 
        απεικόνιση ένας χώρος διευθύνσεων μέσω της συνάρτησης mmap()
        */
        printf("VM map of child's proccess is:\n");
        show_maps();
        show_va_info((uint64_t)heap_shared_buf);

        /*
         * Step 12 - Child
         */
        /*
         * TODO: Write your code here to complete child's part of Step 12.
         */
        //deallocate the buffers of child's proccess through munmap function which takes as arguments the address and buffer's size
        if (munmap(heap_shared_buf, buffer_size) == -1) {
                perror("munmap");
                exit(EXIT_FAILURE);
        }
        if (munmap(heap_private_buf, buffer_size) == -1) {
                perror("munmap");
                exit(EXIT_FAILURE);
        }if (munmap(file_shared_buf, buffer_size) == -1) {
                perror("munmap");
                exit(EXIT_FAILURE);
    }

}

/*
 * Parent process' entry point.
 */
void parent(pid_t child_pid)
{
        uint64_t pa;
        int status;

        /* Wait for the child to raise its first SIGSTOP. */
        if (-1 == waitpid(child_pid, &status, WUNTRACED))
                die("waitpid");

        /*
         * Step 7: Print parent's and child's maps. What do you see?
         * Step 7 - Parent
         */
 printf(RED "\nStep 7: Print parent's and child's map.\n" RESET);
        press_enter();

        /*
         * TODO: Write your code here to complete parent's part of Step 7.
         */
        printf("map of the parent's process is the following\n");
        show_maps();

        if (-1 == kill(child_pid, SIGCONT))
                die("kill");
        if (-1 == waitpid(child_pid, &status, WUNTRACED))
                die("waitpid");


        /*
         * Step 8: Get the physical memory address for heap_private_buf.
         * Step 8 - Parent
         */
        printf(RED "\nStep 8: Find the physical address of the private heap "
                "buffer (main) for both the parent and the child.\n" RESET);
        press_enter();

        /*
         * TODO: Write your code here to complete parent's part of Step 8.
         */
        uint64_t phys_addr_frst_page_parent = get_physical_address((uint64_t)heap_private_buf);
        printf("The physical address of the buffer in main memory IN PARENT'S PROCCESS is : %ld\n",phys_addr_frst_page_parent);
        printf("The VA info is the following : \n");
        show_va_info((uint64_t)heap_private_buf);

        if (-1 == kill(child_pid, SIGCONT))
                die("kill");
        if (-1 == waitpid(child_pid, &status, WUNTRACED))
                die("waitpid");


        /*
         * Step 9: Write to heap_private_buf. What happened?
         * Step 9 - Parent
         */
        printf(RED "\nStep 9: Write to the private buffer from the child and "
                "repeat step 8. What happened?\n" RESET);
        press_enter();

        /*
         * TODO: Write your code here to complete parent's part of Step 9.
         */
        uint64_t phys_addr_frst_page_parent_pvt = get_physical_address((uint64_t)heap_private_buf);
        printf("The physical address of the private_buffer in main memory AT PARENT'S PROCCESS is : %ld\n",phys_addr_frst_page_parent_pvt);
        printf("The VA info is the following : \n");
        show_va_info((uint64_t) heap_private_buf);
     printf(RED "\nStep 10: Write to the shared heap buffer (main) from "
                "child and get the physical address for both the parent and "
                "the child. What happened?\n" RESET);
        press_enter();

        /*
         * TODO: Write your code here to complete parent's part of Step 10.
         */
        for(int i=0; i<(int)buffer_size; i++) {
                heap_shared_buf[i]=0;
                }

uint64_t phys_addr_frst_page_parent_shared = get_physical_address((uint64_t)heap_shared_buf);
        printf("The physical address of the buffer in main memory AT PARENT'S PROCCESS is : %ld\n",phys_addr_frst_page_parent_shared);
        printf("The VA info is the following : \n");
        show_va_info((uint64_t) heap_shared_buf);
        if (-1 == kill(child_pid, SIGCONT))
                die("kill");
        if (-1 == waitpid(child_pid, &status, WUNTRACED))
                die("waitpid");


        /*
         * Step 11: Disable writing on the shared buffer for the child
         * (hint: mprotect(2)).
         * Step 11 - Parent
         */
        printf(RED "\nStep 11: Disable writing on the shared buffer for the "
                "child. Verify through the maps for the parent and the "
                "child.\n" RESET);
        press_enter();

        /*
         * TODO: Write your code here to complete parent's part of Step 11.
         */
        printf("VM map of parent's process is:\n");
        show_maps();
        show_va_info((uint64_t)heap_shared_buf);

        if (-1 == kill(child_pid, SIGCONT))
                die("kill");
        if (-1 == waitpid(child_pid, &status, 0))
                die("waitpid");


        /*
         * Step 12: Free all buffers for parent and child.
         * Step 12 - Parent
         */

        /*
         * TODO: Write your code here to complete parent's part of Step 12.
         */
//deallocate the buffers of parent's proccess through munmap function which takes as arguments the address and buffer's size
      if (munmap(heap_shared_buf,buffer_size) == -1) {
                perror("munmap");
                exit(EXIT_FAILURE);
        }
        if (munmap(heap_private_buf,buffer_size) == -1) {
                perror("munmap");
                exit(EXIT_FAILURE);
        }if (munmap(file_shared_buf,buffer_size) == -1) {
                perror("munmap");
                exit(EXIT_FAILURE);
    }
}

int main(void)
{
        pid_t mypid, p;
        int fd = -1;
        uint64_t pa;

        mypid = getpid();
        buffer_size = 1 * get_page_size();

        /*
         * Step 1: Print the virtual address space layout of this process.
         */
        printf(RED "\nStep 1: Print the virtual address space map of this "
                "process [%d].\n" RESET, mypid);
        press_enter();
        /*
         * TODO: Write your code here to complete Step 1.
         */ heap_private_buf = mmap(NULL,buffer_size,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,fd,0);

                if (heap_private_buf == MAP_FAILED) {
                           perror("mmap failed");
                           exit(EXIT_FAILURE);
                        }

                show_maps();
                show_va_info((uint64_t)heap_private_buf);



        /*
         * Step 2: Use mmap to allocate a buffer of 1 page and print the map
         * again. Store buffer in heap_private_buf.
         */
        printf(RED "\nStep 2: Use mmap(2) to allocate a private buffer of "
                "size equal to 1 page and print the VM map again.\n" RESET);
        press_enter();
        /*
         * TODO: Write your code here to complete Step 2.
         */
                //Δημιουργία απεικόνισης μνήμης
//το πρώτο όρισμα addr ειναι η αρχική διέυυθυνση του χάρτη .
//Το αρχικοποιούμε στην τιμή ΝULL προκειμένου ο πυρήνας να επιλέξει την(page-aligned)διέυθυνση στην οποία θα δημιουργηθεί η απεικόνιση,
//Επειτα ως length βάζω το buffer_size,στο int prot που περιγράφει την επιθυμητή προστασία μνήμης της απεικόνισης βάζω την ετικέτα PROT_READ|PROT_WRITE,
//μετά αρχικοποιώ τις σημαίες σε MAP_PRIVATE(καθορίζει τον τρόπο χαρτογράφησης ο οποίος ειναι private),
//έπειτα το περιγραφητή αρχείου fd και τέλος offset  = 0(προκειτα για την αρχική θέση από την οποία θα ξεκινησει η χαρτογράφηση)
                 heap_private_buf = mmap(NULL,buffer_size,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,fd,0);

                if (heap_private_buf == MAP_FAILED) {
                           perror("mmap failed");
                           exit(EXIT_FAILURE);
                        }

                show_maps();
                show_va_info((uint64_t)heap_private_buf);
                //Απελευθέρωση της απεικόνισης μνήμης
                /*
                if(munmap(heap_private_buf,buffer_size)== -1){
                        perror("munmap Failed");
                        exit(EXIT_FAILURE);
                }
                */
        /*
         * Step 3: Find the physical address of the first page of your buffer
         * in main memory. What do you see?
         */
        printf(RED "\nStep 3: Find and print the physical address of the "
                "buffer in main memory. What do you see?\n" RESET);
        press_enter();
        /*
         * TODO: Write your code here to complete Step 3.
         */
        //καλώ τηνν get_physical_address για να βρώ την φυσική διέυθυνση της πρώτης σελίδας του buffer
        //Ως όρισμα παίρνει την εικονικλη διέυθυνση μνήμης τύπου uint64_t για αυτό και μετατρέπω τον τύπο του heap_private_buf καταλλήλως
                uint64_t phys_addr_frst_page = get_physical_address((uint64_t)heap_private_buf);
                printf("The physical address of the buffer in main memory is : %ld\n",phys_addr_frst_page);


        /*
         * Step 4: Write zeros to the buffer and repeat Step 3.
         */
        printf(RED "\nStep 4: Initialize your buffer with zeros and repeat "
                "Step 3. What happened?\n" RESET);
        press_enter();
        /*
         * TODO: Write your code here to complete Step 4.
         */
         for(int i = 0;i<(int)buffer_size;i++){
                heap_private_buf[i] = 0;
                }

        uint64_t phys_addr_frst_page_zeros = get_physical_address((uint64_t)heap_private_buf);
                printf("The physical address of the buffer INITIALIZED WITH ZEROS in main memory is : %ld\n",phys_addr_frst_page_zeros);


        /*
         * Step 5: Use mmap(2) to map file.txt (memory-mapped files) and print
         * its content. Use file_shared_buf.
         */
        printf(RED "\nStep 5: Use mmap(2) to read and print file.txt. Print "
                "the new mapping information that has been created.\n" RESET);
        press_enter();
        /*
         * TODO: Write your code here to complete Step 5.
         */


                //τυπωση του περιεχομένου του αρχείου
                FILE *fp;
                char c;

                fp = fopen("file.txt","r");
                if(fp ==NULL){
                        printf("error while opening");
                        exit(0);}

                while((c = fgetc(fp))!=EOF)
                        printf("%c",c);
                fclose(fp);
                printf("\n");
        int fd_new = open("file.txt", O_RDONLY);//ανοίγουμε το αρχείο file.txt για διάβασμα και 
                                                //μέσω της open επιστρέφεται ο περιγραφητής του αρχείου
        if (fd_new == -1) {
                perror("open");

                }

        file_shared_buf = mmap(NULL,buffer_size,PROT_READ,MAP_SHARED,fd_new,0);

        if (file_shared_buf == MAP_FAILED) {
                perror("mmap failed");
                exit(EXIT_FAILURE);
               }

                show_maps();
                show_va_info((uint64_t)file_shared_buf);
        close(fd_new);

        /*
         * Step 6: Use mmap(2) to allocate a shared buffer of 1 page. Use
         * heap_shared_buf.
         */
        printf(RED "\nStep 6: Use mmap(2) to allocate a shared buffer of size "
                "equal to 1 page. Initialize the buffer and print the new "
                "mapping information that has been created.\n" RESET);
        press_enter();
        /*
         * TODO: Write your code here to complete Step 6         */

        heap_shared_buf=mmap(NULL, buffer_size, PROT_READ| PROT_WRITE,MAP_SHARED| MAP_ANONYMOUS,-1,0);
 if (heap_shared_buf == MAP_FAILED) {
                perror("mmap failed");
                exit(EXIT_FAILURE);
                }
        show_maps();
        show_va_info((uint64_t)heap_shared_buf);

        p = fork();
        if (p < 0)
                die("fork");
        if (p == 0) {
                child();
                return 0;
        }

        parent(p);

        if (-1 == close(fd))
                perror("close");
        return 0;
}
