//
//  process.c
//  
//
//  Created by Andrew Yoon on 12-04-02.
//  Copyright (c) 2012 UBC. All rights reserved.
//

#include "process.h"

int suspendProcess(pid_t pid) {
	int status;
	kill(pid, SIGTSTP);
	waitpid(pid, &status, WUNTRACED);
	
	return status;
}

void resumeProcess(pid_t pid) {
	kill(pid, SIGCONT);
	
	return;
}

void killProcess(pid_t pid) {
    kill(pid, SIGTERM);
    
    return;
}

void decTimeProcess(pid_t pid) {
    kill(pid, SIGUSR1);
    
    return;
}

void initProcessList(process* list, int length) {
    process zero = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (int i=0; i<length; i++) {
        list[i] = zero;
    }
}

