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
	key_t key = KEY;	//shared memory key
	simclock *myclock;	//pointer to shared clock

	// Message queue vars
	int msqid;
	message_buf sbuf;

	int i;
	int user_pid;
	int user_limit = 0;
	int user_count = 0;
	int user_total = 0;

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
	
	// Initialize clock at 0.0s
	myclock->sec = 0;
	myclock->nano = 0;


	/* Create message queue */
	if ((msqid = msgget(key, IPC_CREAT | 0666)) < 0) {
		fprintf(stderr, "Failed to create message queue\n");
		return 1;
	}



	/* message sending code */
	//message_buf sbuf;
	size_t buf_length;

	sbuf.mtype = 1;
	strcpy(sbuf.mtext, "SPAAAAAACE");
	buf_length = strlen(sbuf.mtext) + 1;

	if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
		fprintf(stderr, "Failed to send message\n");
	}
	else {
		fprintf(stderr, "Message: \"%s\" sent\n", sbuf.mtext);
	}


	// Continue until 2 seconds have passed in simulated system, 100 processes have been generated, or OSS has be running for maximum time allotted.
	while ((myclock->sec < 2) && (user_total < 100) && (difftime(time(NULL), tstart) < z)) {
		// Fork off the appropriate number of child processes
		if (user_count == x) {
			user_pid = wait(NULL);
			fprintf(fp, "Master: Child pid %3d is terminating at my time %2d.%2d\n", user_pid, myclock->sec, myclock->nano);
			user_count--;
		}

		user_count++;
		user_total++;

		user_pid = fork();
		if (user_pid == 0) {
			execlp("./user", "user", NULL);
		}
		else {
			fprintf(fp, "%3d Master: Creating new child pid %3d at my time %2d.%2d\n", user_total, user_pid, myclock->sec, myclock->nano);
		}
	}

	while (wait(NULL) != -1) {
	}

	shmdt(myclock);
	shmctl(shmid, IPC_RMID, NULL);

	exit(0);
}
