//
//  main.c
//  
//
//  Created by Andrew Yoon on 12-04-02.
//  Copyright (c) 2012 UBC. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include "process.h"
#include "resource.h"

#define LINE_SIZE 128
#define NUM_PARAMS 8
#define INPUTFILE "./p.txt"
#define DEFAULTCHILDPROCESS "./child"

#define MAX_PROCESS 100
#define D_QUEUE_SIZE 10
#define RT_QUEUE_SIZE 5
#define USER1_QUEUE_SIZE 10
#define USER2_QUEUE_SIZE 10
#define USER3_QUEUE_SIZE 10




int parseFile(char* filename, process* list);
long getTimeInMs();
long checkElapsedTime(long baseTime);

int main(int argc, char* argv[]) {
    
//    <arrival time>, <priority>, <processor time>, <Mbytes>, <#printers>, <#scanners>, <#modems>, <#CDs>
//    12, 0, 1, 64, 0, 0, 0, 0
//    12, 1, 2, 128, 1, 0, 0, 1
//    13, 3, 6, 128, 1, 0, 1, 2
	int startTime = getTimeInMs();
    bool isChild = false;
//    int childProcTime = 0;
    bool isFromTemp = false;
    char* filename = (char *)malloc(20 * sizeof(char));
    
    if (argc == 1) {
        /* filename not entered, use p.txt as default */
        filename = INPUTFILE;
    }
    else if (argc == 2) {
        filename = argv[1];
    }
    else {
        /* wrong number of arguments, exit */
        printf("Wrong number of arguments\n");
        printf("hostd: hostd <processlist>\n");
        exit(-1);
    }
    
    process processList[MAX_PROCESS];
    process dQueue[D_QUEUE_SIZE]; // dispatcher queue
    process rtQueue[RT_QUEUE_SIZE]; // real-time queue
    process userQueue_1[USER1_QUEUE_SIZE]; // user priority 1 queue
    process userQueue_2[USER2_QUEUE_SIZE]; // user priority 2 queue
    process userQueue_3[USER3_QUEUE_SIZE]; // user priority 3 queue
    process tempQueue; // temporary queue, where preempted process waits to get CPU back
    
    initProcessList(processList, MAX_PROCESS);
    initProcessList(dQueue, D_QUEUE_SIZE);
    initProcessList(rtQueue, RT_QUEUE_SIZE);
    initProcessList(userQueue_1, USER1_QUEUE_SIZE);
    initProcessList(userQueue_2, USER2_QUEUE_SIZE);
    initProcessList(userQueue_3, USER3_QUEUE_SIZE);
    tempQueue.pid = 0;
    releaseCPU();
    
    int tempTime; // time
    int dQCount = 0, rtQCount = 0, uQCount_1 = 0, uQCount_2 = 0, uQCount_3 = 0;

    /* parse the input file and put into processList */
    int processCount = parseFile(filename, processList);

    /* start processes and suspend immediately */
    for (int i=0; i < processCount; i++) {
        if (!(processList[i].pid = fork())) {
			/* child process action */
            
//            childProcTime = processList[i].processorTime; // assign the processor time
            isChild = true;
            char str[10];
            sprintf(str, "%d", processList[i].processorTime);
            execlp(DEFAULTCHILDPROCESS, DEFAULTCHILDPROCESS, str, (char*)0);
			break;
		}
		else {
			/* parent */
            /* suspend all children right after the creation
             * assuming that all processes passed into hostd are suspended */
			suspendProcess(processList[i].pid);            
		}
    }

    if (!isChild) {
        /* parent process only */   
//        long lastTest = 0;
        
        while (true) {
            /* TEST - called every 500ms */
//            if ((getTimeInMs()-lastTest)>=500) {
//                lastTest = getTimeInMs();
//                printf("%5ld ---- dq %d\trtq %d\tuqc1 %d\tuqc2 %d\tuqc3 %d\t", 
//                       checkElapsedTime(startTime), dQCount, rtQCount, uQCount_1, uQCount_2, uQCount_3);
//                printf("CPUproc %d\tprocTime %d\tprio %d\tmem: %d\n",
//                       CPU.currentProcess.pid, CPU.currentProcess.processorTime, CPU.currentProcess.priority,  
//                       remainingMemory());                        
//            }
            
            
            /* waiting for the processes to arrive */
            
            long elapsedTime = checkElapsedTime(startTime);
            if (elapsedTime > 60000) break; // quit program after 60s
            else {
                /* handle new process arrival */
                for (int i=0; i<processCount; i++) {
                    if (elapsedTime >= processList[i].arrivalTime * 1000) {
                        /* process i has now arrived, move into either DQ or RTQ */
//                        printf("------------------process %d (priority %d) arrived, moving to queue..\n", processList[i].pid, processList[i].priority);

                        if (processList[i].priority == 0) {
                            /* real-time process, check resources */
                            // TESTING: without checkResources
                            if (checkResources(processList[i])) {
                                /* resources available, move straight to RTQ */
//                                printf("++++++assigning resources to %d\n", processList[i].pid); 
                                assignResources(processList[i]);
                                saveMemory();
//                                printf("moving proc %d to RTQ[%d]\n", processList[i].pid, rtQCount);
                                rtQueue[rtQCount++] = processList[i];
                            }
                            else {
                                /* resources not available, shift everything in DQ back and put into DQ[0] */
                                for (int j=dQCount; j>0; j--) {
                                    dQueue[j] = dQueue[j-1];
                                }
//                                printf("putting process %d at DQ[%d]\n", processList[i].pid, 0);
                                dQueue[0] = processList[i];
                                dQCount++;
                            }
                        }
                        else {
                            /* user priority process, put it at the end of DQ */
//                            printf("putting process %d at DQ[%d]\n", processList[i].pid, dQCount);
                            dQueue[dQCount++] = processList[i];
                        }
                        
                        /* remove process i from the list, and shift everything */
                        for (int j=i; j<processCount-1; j++) {
                            processList[j] = processList[j+1];
                        }
                        processList[processCount].pid = 0;
                        
                        /* decrement the count */
                        processCount--;
                    }
                }
                
                
                /* handle DQ */
                while (dQCount > 0) {
                    
                    /* handle dQueue */
                    // TESTING: without checkResources
                    if (checkResources(dQueue[0])) {
                        
                        /* assign the resources and dispatch dQueue[0] to appropriate queue */
//                        printf("++++++assigning resources to %d\n", dQueue[0].pid); 
                        assignResources(dQueue[0]);
                        saveMemory();
                        
                        if (dQueue[0].priority == 0) {
                            /* user priority 1 */
//                            printf("moving DQ[0] to RTQ[%d]..\n", uQCount_1);
                            rtQueue[rtQCount++] = dQueue[0];
                        } else if (dQueue[0].priority == 1) {
                            /* user priority 1 */
//                            printf("moving DQ[0] to UQ1[%d]..\n", uQCount_1);
                            userQueue_1[uQCount_1++] = dQueue[0];
                        
                        } else if (dQueue[0].priority == 2) {
                            /* user priority 2 */
//                            printf("moving DQ[0] to UQ2[%d]..\n", uQCount_2);
                            userQueue_2[uQCount_2++] = dQueue[0];
                            
                        } else if (dQueue[0].priority == 3) {
                            /* user priority 3 */
//                            printf("moving DQ[0] to UQ3[%d]..\n", uQCount_3);
                            userQueue_3[uQCount_3++] = dQueue[0]; 
                        }
                    
                        for (int i=0; i<dQCount; i++) {
                            /* shift everything in dQueue */
                            dQueue[i] = dQueue[i+1];
                        }
                        dQCount--;
                    }
                    else break;
                }
                /* handle RTQ */
                if (rtQCount > 0) {
                    if (CPU.currentProcess.priority > 0) {
                        /* non real-time process running - current process gets preempted */
//                        printf("moving CPU process %d to tmpQ..\n", CPU.currentProcess.pid);
                        tempQueue = CPU.currentProcess;
                        tempTime = getTimeInMs() - CPU.startTime; // time left in ms
                        suspendProcess(CPU.currentProcess.pid);
                        releaseCPU();
                        
                        
//                        printf("assigning cpu to process %d from RTQ[0]\n", rtQueue[0].pid);
                        assignCPU(rtQueue[0]);
                        
                        for (int i=0; i<rtQCount; i++) {
                            /* shift everything in rtQueue */
                            rtQueue[i] = rtQueue[i+1];
                        }
                        rtQueue[rtQCount].pid = 0;
                        rtQCount--;
                    }
                }
                
                /* check CPU if something is running */
                if (((getTimeInMs() - CPU.startTime >= 1000) && (!isFromTemp)) || // regular proc
                    ((getTimeInMs() - CPU.startTime >= tempTime) && (isFromTemp))) // proc back from tmpQ
                {                    
                    /* if something is running, finish it */
                    if (CPU.currentProcess.pid > 0) {                 
                        /* decrement the time remaining */
                        CPU.currentProcess.processorTime--;
                        decTimeProcess(CPU.currentProcess.pid);

                        if (CPU.currentProcess.processorTime <= 0) {
                            /* current process is finished, kill the process and release theh resources */
                            releaseResources(CPU.currentProcess);
                            killProcess(CPU.currentProcess.pid);
//                            printf("**************** killing process %d... *dead*\n", CPU.currentProcess.pid);
                            releaseCPU();
                        }
                        else {
                            /* current process is not done yet, move back to Q */
                            if (CPU.currentProcess.priority == 0) {
                                /* 
                                 * real time process, check if another real time process is waiting
                                 * if not, keep running 
                                 */
                                if (rtQueue[0].pid) { 
                                    /* real time process running, move back to RTQ */
//                                    printf("moving process %d from CPU to RTQ..\n", CPU.currentProcess.pid);
                                    rtQueue[rtQCount++] = CPU.currentProcess;
                                    suspendProcess(CPU.currentProcess.pid);
                                    releaseCPU();
                                }
                                else {
                                    CPU.startTime = getTimeInMs();
                                    resumeProcess(CPU.currentProcess.pid);
                                }
                            } 
                            else if (CPU.currentProcess.priority == 1) {
                                /* priority 1, lower the priority and move to UQ2 */
                                CPU.currentProcess.priority++;
//                                printf("moving process %d from CPU to UQ2..\n", CPU.currentProcess.pid);
                                userQueue_2[uQCount_2++] = CPU.currentProcess;
                                suspendProcess(CPU.currentProcess.pid);
                                releaseCPU();
                            }
                            else if (CPU.currentProcess.priority == 2) {
                                /* priority 2, lower the priority and move to UQ3 */
                                CPU.currentProcess.priority++;
//                                printf("moving process %d from CPU to UQ3..\n", CPU.currentProcess.pid);
                                userQueue_3[uQCount_3++] = CPU.currentProcess;
                                suspendProcess(CPU.currentProcess.pid);
                                releaseCPU();
                            }
                            else if (CPU.currentProcess.priority == 3) {
                                /* priority 3, just move to UQ3 */
//                                printf("moving process %d from CPU to UQ3..\n", CPU.currentProcess.pid);
                                userQueue_3[uQCount_3++] = CPU.currentProcess;
                                suspendProcess(CPU.currentProcess.pid);
                                releaseCPU();
                            }
                        }
                        
                        if (isFromTemp) {
                            /* just handled proc back from tmpQ, reset the tempTime */
                            isFromTemp = false;
                            tempTime = 0;
                        }
                    }
                    
                    /* now check RTQ[0], tempQ, UQ1[0], UQ2[0], UQ3[0] 
                     * move to CPU if any of them exists, and go back to the top of while loop 
                     */
                    if (CPU.currentProcess.pid == 0) {
                        if (rtQCount > 0 ) {
//                            printf("assigning cpu to process %d from RTQ[0]\n", rtQueue[0].pid);
                            assignCPU(rtQueue[0]);
                            
                            for (int i=0; i<rtQCount; i++) {
                                /* shift everything in rtQueue */
                                rtQueue[i] = rtQueue[i+1];
                            }
                            rtQueue[rtQCount].pid = 0;
                            rtQCount--;
                            
                            CPU.startTime = getTimeInMs();
                            resumeProcess(CPU.currentProcess.pid);
                        }
                        else if (tempQueue.pid > 0) {
//                            printf("assigning cpu to process %d from tmpQ\n", tempQueue.pid);
                            assignCPU(tempQueue);
                            
                            tempQueue.pid = 0;
                            isFromTemp = true;
                            
                            CPU.startTime = getTimeInMs();
                            resumeProcess(CPU.currentProcess.pid);
                        }
                        else if (uQCount_1 > 0) {
//                            printf("assigning cpu to process %d from UQ1[0]\n", userQueue_1[0].pid);
                            assignCPU(userQueue_1[0]);
                            
                            for (int i=0; i<uQCount_1; i++) {
                                /* shift everything in userQueue_1 */
                                userQueue_1[i] = userQueue_1[i+1];
                            }
                            userQueue_1[uQCount_1].pid = 0;
                            uQCount_1--;
                            
                            CPU.startTime = getTimeInMs();
                            resumeProcess(CPU.currentProcess.pid);
                        }
                        else if (uQCount_2 > 0) {
//                            printf("assigning cpu to process %d from UQ2[0]\n", userQueue_2[0].pid);
                            assignCPU(userQueue_2[0]);
                            
                            for (int i=0; i<uQCount_2; i++) {
                                /* shift everything in userQueue_2 */
                                userQueue_2[i] = userQueue_2[i+1];
                            }
                            userQueue_2[uQCount_2].pid = 0;
                            uQCount_2--;
                            
                            CPU.startTime = getTimeInMs();
                            resumeProcess(CPU.currentProcess.pid);
                        }
                        else if (uQCount_3 > 0) {
//                            printf("assigning cpu to process %d from UQ3[0]\n", userQueue_3[0].pid);
                            assignCPU(userQueue_3[0]);
                            
                            for (int i=0; i<uQCount_3; i++) {
                                /* shift everything in userQueue_3 */
                                userQueue_3[i] = userQueue_3[i+1];
                            }
                            userQueue_3[uQCount_3].pid = 0;
                            uQCount_3--;
                            
                            CPU.startTime = getTimeInMs();
                            resumeProcess(CPU.currentProcess.pid);
                        }
                        else {
                            /* all Qs are empty, do nothing */
                        }
                        
                    }
                }
                /* go back to the top of while loop */
            }
        }
    }

    else {
        /* should not get here at all */
        exit(-1);
    }

	return 0;	
}

int parseFile(char* filename, process* list) {
    /* parse the given file and put the information into list */
    
    FILE* fid = fopen(filename, "r");
    int count = 0;
    
    while (!feof(fid)) {
        char* str = (char*)malloc(LINE_SIZE * sizeof(char));
        
        if (fgets(str, LINE_SIZE, fid) != NULL) {
            /* parse the line */
            sscanf(strtok(str, ", "), "%d", &list[count].arrivalTime);
            sscanf(strtok(NULL, ", "), "%d", &list[count].priority);
            sscanf(strtok(NULL, ", "), "%d", &list[count].processorTime);
            sscanf(strtok(NULL, ", "), "%d", &list[count].memory);
            sscanf(strtok(NULL, ", "), "%d", &list[count].printers);
            sscanf(strtok(NULL, ", "), "%d", &list[count].scanners);
            sscanf(strtok(NULL, ", "), "%d", &list[count].modems);
            sscanf(strtok(NULL, ", "), "%d", &list[count].CDs);
            count++;
        }
        free(str);
    }
    fclose(fid);
    
    return count;
}

// call this at the beginning of the program to initialize the given time
long getTimeInMs() {
	struct timeval time;  
	gettimeofday(&time, NULL); 
	return (time.tv_sec * 1000) +(time.tv_usec / 1000); 
}

// Check if the parent process's time is greater than 1 second 
long checkElapsedTime(long baseTime) {
	struct timeval time;  
    gettimeofday(&time, NULL);  
    
    return ((time.tv_sec * 1000) + (time.tv_usec / 1000)) - baseTime;
}









