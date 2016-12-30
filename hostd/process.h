//
//  process.h
//  
//
//  Created by Andrew Yoon on 12-04-02.
//  Copyright (c) 2012 UBC. All rights reserved.
//

#ifndef _process_h
#define _process_h

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct {
    pid_t pid;
    int arrivalTime;
    int priority;
    int processorTime;
    int memory;
    int printers;
    int scanners;
    int modems;
    int CDs;
    
} process;

int suspendProcess(pid_t pid);
void resumeProcess(pid_t pid);
void killProcess(pid_t pid);
void decTimeProcess(pid_t pid);
void initProcessList(process* list, int length);

#endif