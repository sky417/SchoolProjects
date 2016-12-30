//
//  resource.c
//  EECE315_PA4_Q7
//
//  Created by Andrew Yoon on 12-04-02.
//  Copyright (c) 2012 UBC. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/time.h>
#include "resource.h"


bool checkResources(process p) {
	if (p.printers == 1) {
		if ((printers[0] != 0) && (printers[1] != 0)) {
			return false;
		}
	}
	else if (p.printers == 2) {
		if ( !(printers[0] == 0 || printers[1] == 0)) {
			return false;
		}
	}

	if (p.scanners != 0) {
		if (scanners != 0) {
			return false;
		}
	}

	if (p.modems != 0) {
		if (modems != 0) {
			return false;	
		}
	}
	
	if (p.CDs == 1) {
		if ((CDs[0] != 0) && (CDs[1] != 0)) {
			return false;
		}
	}
	else if (p.CDs  == 2) {
		if ( !(printers[0] == 0 || CDs[1] == 0)) {
			return false;
		}
	}

	//Memory allocation available?
	int count = 0;
	int max = 0;
	int i;

	if(p.priority == 0) { //for real-time processes
		for(i=0; i<64; i++) {
			if(memory[i] == 0) {
				count++;
			}
			else {
				if (count> max) {
					max = count;
				}
				count = 0;
			}
		}

		if(memory[63] == 0 && count>= max) {
			max = count;
		}

		if (p.memory > max) {
			return false;
		}

	}
	else { //for user-processes
		for(i=64; i<MEMORY_SIZE; i++) {
			if(memory[i] == 0) {
				count++;
			}
			else {
				if (count> max) {
					max = count;
				}
				count = 0;
			}
		}

		if(memory[1023] == 0 && count>= max) {
			max = count;
		}

		if (p.memory > max) {
			return false;
		}
	}

	return true;
}


bool assignResources(process p) {
	if (p.printers == 1) {
		
		if (printers[0] == 0)
			printers[0] = p.pid;
		else {
			printers[1] = p.pid;
		}
	} else if (p.printers == 2) {
		printers[0] = p.pid;
		printers[1] = p.pid;
	}


	if (p.scanners == 1) {
		scanners = p.pid;
	}

	if (p.modems == 1) {
		modems = p.pid;
	}

	if (p.CDs == 1) {
		if (CDs[0] == 0) 
			CDs[0] = p.pid;
		else
			CDs[1] = p.pid;
	} else if (p.CDs == 2) {
		CDs[0] = p.pid;
		CDs[1] = p.pid;
	}

	// Memory allocation
	int count = 0;
	int max = 0;
	int maxindex = 0;
	
	if (p.priority == 0) {	// For real time processes
		
		for(int i=0; i<64; i++) {
			if(memory[i] == 0) {
				count++;
				if (count> max) {
					max = count;
					maxindex = i-max+1;
					if (p.memory <= max)
						break;
				}
					
			}
			else {
				count = 0;
			}
		}



		for(int i=maxindex; i<p.memory; i++) {
			memory[i] = p.pid;
		}
	}
	else {		// for non-real time processes
		for(int i=64; i<MEMORY_SIZE; i++) {
			if(memory[i] == 0) {
				count++;
				if (count> max) {
					max = count;
					maxindex = i-max+1;
					if (p.memory <= max)
						break;
				}
			}
			else {
				
				count = 0;
			}
		}

		for(int i=maxindex; i<(p.memory+maxindex); i++) {
			memory[i] = p.pid;
		}
	}
	
	return true;
}


bool releaseResources(process p) {
	if (p.printers != 0) {
		if (printers[0] == p.pid)
			printers[0] = 0;
		if (printers[1] == p.pid)
			printers[1] = 0;
	} 

	if (p.CDs != 0) {
		if (CDs[0] == p.pid)
			CDs[0] = 0;
		if (CDs[1] == p.pid)
			CDs[1] = 0;
	}

	if (p.scanners != 0) {
		if (scanners == p.pid)
			scanners = 0;
	}

	if (p.modems != 0) {
		if (modems == p.pid) 
			modems = 0;
	}

	// Memory deallocation
	int i;
	for (i = 0; i<MEMORY_SIZE; i++) {
		if (memory[i] == p.pid) {
			memory[i] = 0;
		}
	}

    return true;
}


bool assignCPU(process p) {
    CPU.currentProcess = p;
//    copyProcess(&CPU.currentProcess, &p);
    struct timeval time;  
    gettimeofday(&time, NULL); 
    CPU.startTime = (time.tv_sec * 1000) +(time.tv_usec / 1000); 
    
	return true;
}

void releaseCPU() {
	CPU.currentProcess.pid = 0;
    CPU.startTime = 0;
}

int remainingMemory() {
    
//    FILE* log = fopen("./mem.txt", "a");
    int counter = 0;
    for (int i=0; i<MEMORY_SIZE; i++) {
        if (memory[i] == 0) counter++;
//        else fprintf(log, "[%d]: %d\t", i, memory[i]);
    }
//    fprintf(log, "\n\n\n\n\n\n\n");
    
//    fclose(log);
    return counter;
}

void saveMemory() {
    FILE* log = fopen("./mem.txt", "a");
    int counter = 0;
    for (int i=0; i<MEMORY_SIZE; i++) {
        if (memory[i] == 0) counter++;
        else fprintf(log, "[%d]: %d\t", i, memory[i]);
    }
    fprintf(log, "\n\n\n\n\n\n\n");
    
    fclose(log);
}


