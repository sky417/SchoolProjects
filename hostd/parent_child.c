#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "process.h"

int main(int argc, char* argv[]) {

	pid_t pid[5];
	bool isChild = false;
	int status;
	
	for (int i=0; i<5; i++) {
		if (!(pid[i] = fork())) {
			/* child */
			isChild = true;
			break;
		}
		else {
			/* parent */
			printf("suspending process %d...\n", pid[i]);
//			kill(pid[i], SIGSTOP); // suspend child
//			waitpid(pid[i], &status, WUNTRACED);
            suspendProcess(pid[i]);
		}
	}
	
	if (isChild) {
		/* children action */
		printf("process %d is resumed, bam! \n", getpid());
	}
	else {
		/* parent action */
		for(int i=0; i<5; i++) {
			printf("Now resuming process %d....\n", pid[i]);
//			kill(pid[i], SIGCONT);
            resumeProcess(pid[i]);
			sleep(2);
		}
		
	}
	
	return 0;	
}
