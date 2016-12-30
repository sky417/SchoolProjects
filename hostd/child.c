//
//  child.c
//  EECE315_PA4_Q7
//
//  Created by Andrew Yoon on 12-04-04.
//  Copyright (c) 2012 UBC. All rights reserved.
//

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


void signal_suspend_handler(int signum);
void signal_proc_handler(int signum);
void signal_term_handler(int signum);
void signal_cont_handler(int signum);

int processTime;
int color;

int main(int argc, char* argv[]) {
    
    /* register signal handlers */
    signal(SIGTSTP, signal_suspend_handler);
    signal(SIGTERM, signal_term_handler);
    signal(SIGCONT, signal_cont_handler);
    signal(SIGUSR1, signal_proc_handler);
    
    /* random color */
    srand(time(NULL));
    color = rand() % 6 + 31;
    
    /* process time passed from hostd */
    processTime = atoi(argv[1]);

    printf("\033[%d;1mProcess %d has started, process time: %d\033[0m\n", color, getpid(), processTime);

    /* main loop for child process */
    while(1)
    {
        printf("\033[%d;1mprocess %d: %d seconds left\033[0m\n", color, getpid(), processTime);
        sleep(1);
    }
    
}


void signal_suspend_handler(int signum) {
    printf("\033[%d;1mPROCESS %d BEING SUSPENDED...AHHHHHH\033[0m\n", color, getpid());
    
    /* unblock signal */
    sigset_t mask; 
    signal(SIGTSTP, SIG_DFL);
    sigemptyset(&mask);  
    sigaddset(&mask, SIGTSTP);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    
    kill(getpid(), SIGTSTP);
    
    signal(SIGTSTP, signal_suspend_handler);
}

void signal_term_handler(int signum) {
    printf("\033[%d;1mProcess %d is being terminated....\033[0m\n", color, getpid());
    
    /* unblock signal */
    sigset_t mask; 
    signal(SIGTERM, SIG_DFL);
    sigemptyset(&mask);  
    sigaddset(&mask, SIGTERM);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    
    kill(getpid(), SIGTERM);
}

void signal_cont_handler(int signum) {
    printf("\033[%d;1mProcess %d is resumed!!\033[0m\n", color, getpid());
}

void signal_proc_handler(int signum) {
    processTime--;
}


