#include <unistd.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc ,char *argv[]){
   if(argc != 4){
        printf("Give the correct number of arguments\n");
        }

   int pid = fork();

   if(pid==-1){
       printf("fork failed");
       return 1;
   }

   if(pid>0)
       wait(NULL);

   if(pid==0){
       printf("Child process is executing...");
       char *args[] = {"./a1.1-C",argv[1],argv[2],argv[3]};
       //Sthn thesh tou /home/oslab/code/char‐count mpainei to executable  pou tha ektelesoume
       //Using the execv the pid don't change but all the other stuff like heap variables stack are changed.
       //When the exec is called the previous file is replaced and a new file is executed
       execv(args[0], args);
       // Αν η execv αποτύχει, τυπώνουμε ένα μήνυμα σφάλματος
        perror("execv");
   }
   return 0;
}
/*
Η συνάρτηση execv() στο λειτουργικό σύστημα Unix επιτρέπει σε μια διεργασία να εκτελέσει ένα άλλο πρόγραμμα. Η συνάρτηση execv() αντικαθιστά την τρέχουσα διεργασία με το πρόγραμμα που καλείται.

Η σύνταξη της συνάρτησης execv() είναι η εξής:
int execv(const char *path, char *const argv[]);
Το path είναι η διαδρομή προς το εκτελέσιμο πρόγραμμα που θέλετε να εκτελέσετε, ενώ το argv είναι ένας πίνακας συμβολοσειρών που παρέχει τα ορίσματα για το πρόγραμμα. Ο τελευταίος πίνακας συμβολοσειρών πρέπει να είναι NULL, ώστε η συνάρτηση να γνωρίζει ότι έχουν τελειώσει τα ορίσματα.

*/
