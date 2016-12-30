//
//  resource.h
//  EECE315_PA4_Q7
//
//  Created by Andrew Yoon on 12-04-02.
//  Copyright (c) 2012 UBC. All rights reserved.
//

#include "process.h"

#ifndef EECE315_PA4_Q7_resource_h
#define EECE315_PA4_Q7_resource_h

#define MEMORY_SIZE 1024

/* resources are global, not being used initially */
int printers[2];
int modems;
int scanners;
int CDs[2];
int memory[MEMORY_SIZE];
struct CPU {
    long startTime;
    process currentProcess;
} CPU;


bool assignCPU(process p);
void releaseCPU();

bool checkResources(process p); 
bool assignResources(process p);
bool releaseResources(process p);

int remainingMemory();
void saveMemory();

#endif
