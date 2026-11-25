/*  Write a C program that asks user to enter their username. If entered within 10 seconds of prompt, it should print a greeting message, e.g. Welcome $username. Otherwise, print “Time out. Exit the program” and terminate the program.
*/
#include <stdio.h>
#include <unistd.h>    /* standard unix functions, like alarm()          */
#include <signal.h>    /* signal name macros, and the signal() prototype */
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 100
char user[MAX_LENGTH];        /* buffer to read user name from the user */

/* define an alarm signal handler. */
void catch_alarm ();


int main()
{
    /* Set a signal handler for ALRM signals */
    int i=0;
    signal(SIGALRM, &catch_alarm);
    
    /* Prompt the user for input */
    printf("Username: ");
    
    /* Start a 10 seconds alarm */
    while(1){
        printf(" %d\n" ,i);
        sleep(1);
        i++;
    }
    /* Receive the user input */
   
    /* Remove the timer, now that we've got the user's input */
   
    
    printf("Greetings %s \n", user);
    
    return 0;
}


