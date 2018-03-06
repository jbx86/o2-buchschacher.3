#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <time.h>
#include "simclock.h"

int main(int argc, char *argv[]) {

	// Vars for flag parsing
	int opt;
	int hflag = 0;		// -h flag
	int x = 5;		// Maximum number of slave processes
	char *filename = NULL;	// Log file to be used
	int z = 20;		// Time in seconds when the master will terminat
	FILE *fp;		// File pointer

	// Vars for shared memory
	int shmid;		//shared memory ID
	key_t key = 5678;	//shared memory key
	simclock *myclock;	//pointer to shared clock

	int i;
	int pid;

	time_t tstart = time(NULL);

	//Parse command line options
	while ((opt = getopt(argc, argv, "hs:l:t:")) != -1) {
		switch (opt) {
			case 'h':
				// Set -h flag
				hflag = 1;
				break;
			case 's':
				// Set max number of slave processes
				x = atoi(optarg);
				if (x < 1) {
					fprintf(stderr, "Invalid value entered, x must be an integer greater than zero\n");
					return 1;
				}
				break;
			case 'l':
				// Set log file to be used
				filename = optarg;
				break;
			case 't':
				// Set time when master will terminate
				z = atoi(optarg);
				if (z < 1) {
					fprintf(stderr, "Invalid value entered, z must be an integer than zero\n");
					return 1;
				}
				break;
		}
	}

	// Open log file
	if ((fp = fopen(filename, "w")) == NULL) {
		fprintf(stderr, "Could not open log file or no log file was provided\n");
		return 1;
	}

	/* Create memory segment */
	if ((shmid = shmget(key, sizeof(simclock), IPC_CREAT | 0666)) < 0) {
		fprintf(stderr, "Failed to create shared memory segment\n");
		return 1;
	}

	/* Attach to memory segment */
	if ((myclock = shmat(shmid, NULL, 0)) == (simclock *) -1) {
		fprintf(stderr, "Shmat error");
		return 1;
	}
	
	// shmctl(shmid, IPC_RMID, NULL);
	myclock->sec = 0;
	myclock->nano = 0;
	printf("Parent: %d:%d at %p\n", myclock->sec, myclock->nano, myclock);

	pid = fork();
	if (pid == 0) {
		execlp("./user", "user", NULL);
	}
	else {
		printf("Parent %d spawned child %d\n", getpid(), pid);
	}


	exit(0);
/*
	// Fork off appropriate number of child processes
	for (i = 0; i < x; i++) {
		pid = fork();
		if (pid == 0) {
			//printf("Child %d at %d:%d\n", getpid(), sim_clock->sec, sim_clock->nano);
			execlp("./user", "user", sim_clock, NULL);
			return 1;
		}
		else {
			printf("Parent %d spawned process %d\n", getpid(), pid);
		}
	}

	while ((difftime(time(NULL), tstart) < z) && (sim_clock->sec < 2)) {
		//printf("Clock time passed:%2d, Simulation time passed:%2d:%2d\n", (int)difftime(time(NULL), tstart), sim_clock->sec, sim_clock->nano);
	}

	//shmdt(sim_clock);
	shmctl(shmid, IPC_RMID, NULL);

	//fprintf(stderr, "shared memory id:%d\n", shmid);
	// Fork off the appropriate number of child processes.
	// wait for child processes to send a message.
	// When sent a message, output the contents of that message to a file
	// Go into the critical section to add 100 to the clock
	// Fork off another child.
	// After 2 seconds have passed in simulated system, terminate all children and master.

	//
*/
	fclose(fp);
	exit(0);

}
