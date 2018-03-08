#include "proj3.h"

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
	int user_pid;
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
	printf("oss: ready message sent\n");

// Critical Section:

	//Repeat until 100 children have been forked
	while (user_total < MAXCHILD) {
		printf("oss: in critical section\n");

		// Fork children until limit is reached
		while ((user_count < x) && (user_total < MAXCHILD)) {
			printf("oss: child spawned\n");
			user_count++;
			user_total++;
			if((user_pid = fork()) == 0) {
				// Execute user process
				execlp("./user", "user", NULL);
			}
			else {
				// Write execution to log
				fprintf(fp, "Master: Creating new (%d) child pid %d at my time %d.%09d\n", user_total,  user_pid, shmclk->sec, shmclk->nano);
			}
			int i = 0;
			while (i < 10) {
				i++;
			}
		}

		// Wait until message recieved from child terminating
		printf("oss: waiting for message\n");

		if (msgrcv(msqid, &buf, MSGSZ, 1, 0) < 0) {
			perror("oss: msgrcv");
			exit(1);
		}

//----- Critical section ---------------

		// Write message to buffer, decrement current processes, and increment clock
		printf("oss: message recieved\n");
		fprintf(fp, "Master: %s\n", buf.mtext);
		user_count--;
		simadd(shmclk, 100);

//--------------------------------------

		// Send message to queue
		buf.mtype = 2;
		sprintf(buf.mtext, "next");
		buf_length = strlen(buf.mtext) + 1;
		if (msgsnd(msqid, &buf, buf_length, 0) < 0) {
			perror("oss: msgsend");
		}

		printf("oss: message sent\n");

	}

	printf("oss: 10 processes have been run\n\n");

// IPC Cleanup:

	shmdt(shmclk);
	shmctl(shmid, IPC_RMID, NULL);
	msgctl(msqid, IPC_RMID, NULL);

	exit(0);
}
