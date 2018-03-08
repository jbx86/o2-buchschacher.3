#include "proj3.h"

#define MAXCHILD 100

int main(int argc, char *argv[]) {

	// Vars for flag parsing
	int opt;
	int hflag = 0;		// -h flag
	int x = 5;		// Maximum number of slave processes
	char *filename = NULL;	// Log file to be used
	int z = 20;		// Time in seconds when the master will terminat
	FILE *fp;		// log file pointer

	key_t key = KEY;	// IPC key
	int shmid;		// shared memory ID
	sim_time *shmclk;	// pointer to shared clock
	int msqid;		// message queue ID
	message_buf buf;	// message queue buffer
	size_t buf_length;

	int i;
	pid_t user_pid;
	int user_limit = 0;	// Max number of 'user' processes running at a time
	int user_count = 0;	// Number of 'user' processes currently running
	int user_total = 0;	// Total number of 'user' processes that have run

	time_t tstart = time(NULL);

//Parse command line options:

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
					exit(1);
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
					exit(1);
				}
				break;
		}
	}

	alarm(z);	//Set alarm to kill user after z seconds

	// Open log file
	if ((fp = fopen(filename, "w")) == NULL) {
		fprintf(stderr, "Could not open log file or no log file was provided\n");
		exit(1);
	}	

// IPC Setup:

	// Create memory segment
	if ((shmid = shmget(key, sizeof(sim_time), IPC_CREAT | 0666)) < 0) {
		perror("oss: shmget");
		exit(1);
	}

	// Attach to memory segment
	if ((shmclk = shmat(shmid, NULL, 0)) == (sim_time *) -1) {
		perror("oss: shmat");
		exit(1);
	}
	
	// Initialize clock at 0.0s
	shmclk->sec = 0;
	shmclk->nano = 0;

	// Create message queue
	if ((msqid = msgget(key, IPC_CREAT | 0666)) < 0) {
		perror("oss: msgget");
		exit(1);
	}

	// Send ready message
	buf.mtype = 2;
	sprintf(buf.mtext, "Ready");
	buf_length = strlen(buf.mtext) + 1;
	if (msgsnd(msqid, &buf, buf_length, 0) < 0) {
		perror("oss: msgsend");
        }

	// Generate initial children
	while ((user_count < x) && (user_total < MAXCHILD)) {
		user_total++;
		user_count++;
		 if((user_pid = fork()) == 0) {
			// Execute user process
			execlp("./user", "user", NULL);
		}
		else {
			// Write execution to log
			fprintf(fp, "Master: Creating new (%d) child pid %ld at my time %d.%09d\n", user_total, (long)user_pid, shmclk->sec, shmclk->nano);
			//sleep(1);
			printf("spawn\n");
		}
	}

	//Repeat as long as there are child processes running
	while (user_count > 0) {
		
		// Wait until message recieved from child terminating (type 1)
		if (msgrcv(msqid, &buf, MSGSZ, 1, 0) < 0) {
			perror("oss: msgrcv");
			exit(1);
		}

//----- Critical section -------------------------

		fprintf(fp, "Master: %s\n", buf.mtext);	// Write message to log file
		user_count--;				// Decrement current processes
		simadd(shmclk, 100);			// Increment clock

		// Spawn more children if MAXCHILD hasn't been reached yet
		if (user_total < MAXCHILD) {
			user_total++;
			user_count++;
			if((user_pid = fork()) == 0) {
				// Execute user process
				execlp("./user", "user", NULL);
			}
			else {
				// Write execution to log
				fprintf(fp, "Master: Creating new (%d) child pid %ld at my time %d.%09d\n", user_total, (long)user_pid, shmclk->sec, shmclk->nano);
			}
		}

		// Signal child waiting on queue to run (type 2)
		buf.mtype = 2;
		sprintf(buf.mtext, "next");
		buf_length = strlen(buf.mtext) + 1;
		if (msgsnd(msqid, &buf, buf_length, 0) < 0) {
			perror("oss: msgsend");
		}

//------------------------------------------------

	}

// IPC Cleanup:

	fclose(fp);
	shmdt(shmclk);
	shmctl(shmid, IPC_RMID, NULL);
	msgctl(msqid, IPC_RMID, NULL);

	exit(0);
}
